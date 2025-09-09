//===- ScalarizeRecurrentStore.cpp - Affine scalar-replacement pattern ----===//
//
// This pattern detects a recurrent load/add/store to the same memref element
// inside an inner affine.for and turns it into a scalar accumulator using
// iter_args, performing a single store after the loop.
//
// Example before:
//   affine.store %c0, %A[i,j]
//   affine.for %k = 0 to N {
//     %old = affine.load %A[i,j]
//     %x = ... ; %contrib = ...
//     %new = arith.addi %old, %contrib
//     affine.store %new, %A[i,j]
//   }
//
// After:
//   %acc = affine.for %k = 0 to N iter_args(%acc0 = %c0) -> (i32) {
//     %acc1 = arith.addi %acc0, %contrib
//     affine.yield %acc1 : i32
//   }
//   affine.store %acc, %A[i,j]
//
//===----------------------------------------------------------------------===//
#include "compigra/Transforms/AffineScalarizeRecurrentStore.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/IR/AffineValueMap.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;

namespace {

/// Return true if `v` is defined outside `region`.
static bool isLoopInvariant(Value v, Region &region) {
  if (v.isa<BlockArgument>())
    return v.cast<BlockArgument>().getOwner()->getParent() != &region;
  Operation *def = v.getDefiningOp();
  return !def || def->getParentRegion() != &region;
}

/// Check memref+indices equality (same memref and pairwise same SSA Values).
static bool sameLocation(Value memrefA, Value memrefB, ValueRange idxA,
                         ValueRange idxB) {
  if (memrefA != memrefB || idxA.size() != idxB.size())
    return false;
  for (auto it : llvm::zip(idxA, idxB))
    if (std::get<0>(it) != std::get<1>(it))
      return false;
  return true;
}

/// Pattern works on the *outer* AffineForOp and looks for:
///   [affine.store init, M[idx]]
///   inner = affine.for ... { load M[idx]; addi(load, contrib); store M[idx]; }
struct ScalarizeRecurrentStorePattern
    : public OpRewritePattern<affine::AffineForOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(affine::AffineForOp outer,
                                PatternRewriter &rewriter) const override {
    // We want: inside `outer`, a store of an "init" value followed immediately
    // by an inner affine.for.
    auto &outerOps = outer.getBody()->getOperations();
    if (outerOps.size() < 2)
      return failure();

    // Find a leading init store and an immediately following inner for.
    affine::AffineStoreOp initStore = nullptr;
    auto it = outerOps.begin();
    for (auto opit = outerOps.begin(); opit != outerOps.end(); ++opit) {
      if (isa<affine::AffineStoreOp>(*opit)) {
        initStore = dyn_cast<affine::AffineStoreOp>(*opit);
        it = opit;
        break;
      }
    }
    // auto initStore = dyn_cast<affine::AffineStoreOp>(&*it);
    if (!initStore)
      return failure();
    ++it;
    auto inner = dyn_cast<affine::AffineForOp>(&*it);
    if (!inner)
      return failure();

    // Only handle i32 for CGRA.
    auto valTy = initStore.getValue().getType();
    if (!valTy.isa<IntegerType>() || valTy.cast<IntegerType>().getWidth() != 32)
      return failure();

    // The element we’re reducing to.
    Value targetMemref = initStore.getMemRef();
    SmallVector<Value> targetIdx(initStore.getIndices().begin(),
                                 initStore.getIndices().end());

    // The indices must be invariant w.r.t. the inner loop.
    for (Value idx : targetIdx)
      if (!isLoopInvariant(idx, inner.getRegion()))
        return failure();

    // Inside the inner loop body, look for exactly one store to the same
    // location, driven by an addi(old, contrib), where `old` is a load from the
    // same location.
    affine::AffineStoreOp innerStore = nullptr;
    affine::AffineLoadOp innerLoad = nullptr;
    arith::AddIOp addi = nullptr;

    for (Operation &op : inner.getBody()->getOperations()) {
      if (auto st = dyn_cast<affine::AffineStoreOp>(op)) {
        if (sameLocation(st.getMemRef(), targetMemref, st.getIndices(),
                         targetIdx)) {
          if (innerStore)
            return failure(); // multiple stores — skip
          innerStore = st;
        }
      }
    }
    if (!innerStore)
      return failure();

    // Find the addi producing the stored value.
    addi = innerStore.getValue().getDefiningOp<arith::AddIOp>();
    if (!addi)
      return failure();

    // Verify one operand of addi is a load from same location.
    auto tryLoadA = addi.getLhs().getDefiningOp<affine::AffineLoadOp>();
    auto tryLoadB = addi.getRhs().getDefiningOp<affine::AffineLoadOp>();
    Value contrib;
    if (tryLoadA && sameLocation(tryLoadA.getMemRef(), targetMemref,
                                 tryLoadA.getIndices(), targetIdx)) {
      innerLoad = tryLoadA;
      contrib = addi.getRhs();
    } else if (tryLoadB && sameLocation(tryLoadB.getMemRef(), targetMemref,
                                        tryLoadB.getIndices(), targetIdx)) {
      innerLoad = tryLoadB;
      contrib = addi.getLhs();
    } else {
      return failure();
    }

    // Make sure the inner IV doesn’t affect the target indices (already
    // checked) and that no other writes to the same location occur in the inner
    // loop. We already ensured single store; also ensure the memref isn’t
    // aliased via another store with same memref+indices (conservatively check
    // exact match).
    for (Operation &op : inner.getBody()->getOperations()) {
      if (&op == innerStore.getOperation())
        continue;
      if (auto st = dyn_cast<affine::AffineStoreOp>(op)) {
        if (sameLocation(st.getMemRef(), targetMemref, st.getIndices(),
                         targetIdx))
          return failure();
      }
    }

    // Rewrite:
    // 1) Create a new inner loop with iter_args = init value; we re-clone all
    //    inner loop operations except:
    //      - replace the recurrent load with the iter_arg
    //      - replace the recurrent addi with add(iter_arg, contrib)
    //      - drop the recurrent store
    //    yield the updated accumulator.
    rewriter.setInsertionPoint(inner);
    auto loc = inner.getLoc();

    // Use the same bounds/step.
    auto newInner = rewriter.create<affine::AffineForOp>(
        loc, inner.getLowerBoundOperands(), inner.getLowerBoundMap(),
        inner.getUpperBoundOperands(), inner.getUpperBoundMap(),
        inner.getStep(), /*iterArgs=*/ValueRange{initStore.getValue()});

    // Map block arguments: [outer captured values ...] handled via IRMapping.
    IRMapping mapper;
    // Map the inner loop IV and iter-arg.
    // New inner has a block with args: [iter-arg] appended after IV in
    // AffineFor? AffineForOp body: first arg is IV, then iter_args as block
    // args.
    Block &oldBody = *inner.getBody();
    Block &newBody = *newInner.getBody();

    mapper.map(oldBody.getArgument(0), newBody.getArgument(0)); // IV
    Value accIter = newBody.getArgument(1); // iter arg (i32)

    if (newBody.getOperations().empty() ||
        !isa<affine::AffineYieldOp>(newBody.getOperations().back())) {
      rewriter.setInsertionPointToEnd(&newBody);
      // Seed yield with the iter-arg (same arity as iter_args)
      rewriter.create<affine::AffineYieldOp>(loc, ValueRange{accIter});
    }

    rewriter.setInsertionPointToStart(&newBody);

    // Clone all ops, but rewrite the specific pieces.
    for (Operation &op : oldBody.without_terminator()) {
      if (op.isAncestor(innerLoad.getOperation()) ||
          op.isAncestor(innerStore.getOperation()) ||
          op.isAncestor(addi.getOperation())) {
        // rebuild the “acc update” ourselves before yield.
        continue;
      }
      // Clone any op that doesn't touch the recurrent location.
      rewriter.clone(op, mapper);
    }

    // Rebuild the contribution value in the new loop (it may depend on cloned
    // ops). Map the old `contrib` through `mapper`.
    Value mappedContrib = mapper.lookupOrNull(contrib);
    if (!mappedContrib)
      mappedContrib = contrib; // covers contrib that are invariant/captured

    // Create new accumulator update: accNext = accIter + mappedContrib
    rewriter.setInsertionPointToEnd(&newBody);
    // Replace the old yield with a new one.
    // Insert before the existing yield
    auto yieldOp = cast<affine::AffineYieldOp>(newBody.getTerminator());
    rewriter.setInsertionPoint(yieldOp);
    // Build the accumulator update
    Value accNext = rewriter.create<arith::AddIOp>(loc, accIter, mappedContrib);
    rewriter.replaceOpWithNewOp<affine::AffineYieldOp>(newBody.getTerminator(),
                                                       ValueRange{accNext});
    // Insert a single store after the new inner loop.
    rewriter.setInsertionPointAfter(newInner);
    Value finalAcc = newInner.getResult(0);
    rewriter.create<affine::AffineStoreOp>(loc, finalAcc, targetMemref,
                                           targetIdx);

    // Erase the old inner loop and the init store.
    rewriter.eraseOp(inner);
    rewriter.eraseOp(initStore);

    return success();
  }
};

} // end anonymous namespace

namespace {
struct AffineScalarizeStorePass
    : public compigra::impl::AffineScalarizeStoreBase<
          AffineScalarizeStorePass> {
  explicit AffineScalarizeStorePass() {}

  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    patterns.add<ScalarizeRecurrentStorePattern>(&getContext());
    if (failed(
            applyPatternsAndFoldGreedily(getOperation(), std::move(patterns))))
      signalPassFailure();
    // print current modOp operation
    auto modOp = getOperation();
  }
};

} // namespace

namespace compigra {
std::unique_ptr<mlir::Pass> createAffineScalarizeStore() {
  return std::make_unique<AffineScalarizeStorePass>();
};
} // namespace compigra
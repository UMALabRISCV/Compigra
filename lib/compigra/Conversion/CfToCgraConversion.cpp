//===- CfToCgraConversion.cpp - Convert Cf to Cgra ops   --------*- C++ -*-===//
//
// Copmigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the --convert-llvm-to-cgra pass, which converts the operations not
// supported in CGRA in llvm dialects to customized cgra dialect.
//
//===----------------------------------------------------------------------===//

#include "compigra/Conversion/CfToCgraConversion.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/MLIRContext.h"

// memory interface support
#define JSON_HAS_FILESYSTEM 0
#include "nlohmann/json.hpp"
#include <fstream>

// Debugging support
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
using namespace compigra;
using json = nlohmann::json;

namespace {
/// Get the (true) destination block of the cgra branch operation, if block is
/// a innermost block, return the last block of the successors.
static Block *getCgraBranchDstBlock(Block *block) {
  auto *nextNode = block->getNextNode();

  auto *sucNode = block->getSuccessors().front();
  return nextNode == sucNode ? block->getSuccessors().back() : sucNode;
}

/// Remove unused operations
static LogicalResult removeUnusedOps(func::FuncOp funcOp) {
  SmallVector<Operation *> eraseOps;
  for (auto &op : funcOp.getOps()) {
    if (op.getBlock()->getTerminator() == &op || isa<cgra::SwiOp>(op) ||
        isa<cgra::BlasGemmOp>(op))
      continue;

    if (op.use_empty()) {
      eraseOps.push_back(&op);

      // Backtrack the definition Op if its only user has been erased
      SmallVector<Operation *> toProcess = eraseOps;
      while (!toProcess.empty()) {
        Operation *op = toProcess.back();
        toProcess.pop_back();

        for (Value operand : op->getOperands()) {
          Operation *predOp = operand.getDefiningOp();
          if (predOp && predOp->hasOneUse()) {
            // Erase an operation once
            if (std::find(eraseOps.begin(), eraseOps.end(), predOp) ==
                eraseOps.end())
              eraseOps.push_back(predOp);
            toProcess.push_back(predOp);
          }
        }
      }
    }
  }

  for (Operation *op : eraseOps)
    op->erase();

  return success();
}

static LogicalResult raiseConstOpToTop(func::FuncOp funcOp) {
  for (auto cstOp :
       llvm::make_early_inc_range(funcOp.getOps<arith::ConstantOp>())) {
    cstOp->moveBefore(&funcOp.getBlocks().front().front());
  }
  return success();
}

/// Check whether the block should be forced to jump to the next block.
/// cf.cond_br to cgra.cond_br adaptation is composed of single
/// conditional + branch. If the negative successor is the next node, the
/// scheduling guarantee it by PC+1. Otherwise, a new basic block is inserted
/// after with an unconditional branch to the negative successor.
static bool forceJumpToNextBlock(Block *block) {
  auto *nextNode = block->getNextNode();
  for (auto *succ : block->getSuccessors())
    if (succ != nextNode)
      return false;
  return true;
}

static arith::CmpIPredicate reverseCmpFlag(arith::CmpIPredicate predicate) {
  switch (predicate) {
  case arith::CmpIPredicate::eq:
    predicate = arith::CmpIPredicate::ne;
    break;
  case arith::CmpIPredicate::ne:
    predicate = arith::CmpIPredicate::eq;
    break;
  case arith::CmpIPredicate::slt:
    predicate = arith::CmpIPredicate::sge;
    break;
  case arith::CmpIPredicate::sgt:
    predicate = arith::CmpIPredicate::sle;
    break;
  case arith::CmpIPredicate::sge:
    predicate = arith::CmpIPredicate::slt;
    break;
  case arith::CmpIPredicate::sle:
    predicate = arith::CmpIPredicate::sgt;
    break;
  case arith::CmpIPredicate::ult:
    predicate = arith::CmpIPredicate::uge;
    break;
  case arith::CmpIPredicate::ugt:
    predicate = arith::CmpIPredicate::ule;
    break;
  case arith::CmpIPredicate::uge:
    predicate = arith::CmpIPredicate::ult;
    break;
  case arith::CmpIPredicate::ule:
    predicate = arith::CmpIPredicate::ugt;
    break;
  }
  return predicate;
}

/// CGRA branch only support bne, beq, blt, bge. All predicates must be
/// converted to eq, ne, lt, ge, where in some cases the comparison operands are
/// swapped.
static cgra::CondBrPredicate getCgraBrPredicate(arith::CmpIPredicate pred,
                                                Value &val1, Value &val2) {
  switch (pred) {
  case arith::CmpIPredicate::eq:
    return cgra::CondBrPredicate::eq;
  case arith::CmpIPredicate::ne:
    return cgra::CondBrPredicate::ne;
  case arith::CmpIPredicate::slt:
  case arith::CmpIPredicate::ult:
    return cgra::CondBrPredicate::lt;
  case arith::CmpIPredicate::sgt:
  case arith::CmpIPredicate::ugt:
    std::swap(val1, val2);
    return cgra::CondBrPredicate::lt;
  case arith::CmpIPredicate::sge:
  case arith::CmpIPredicate::uge:
    return cgra::CondBrPredicate::ge;
  case arith::CmpIPredicate::sle:
  case arith::CmpIPredicate::ule:
    std::swap(val1, val2);
    return cgra::CondBrPredicate::ge;
  }
}

/// Lower arith::SelectOp to cgra::BzfaOp
struct ArithSelectOpConversion : OpConversionPattern<arith::SelectOp> {
  using OpConversionPattern<arith::SelectOp>::OpConversionPattern;

  LogicalResult
  matchAndRewrite(arith::SelectOp selectOp, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto trueVal = selectOp.getTrueValue();
    auto falseVal = selectOp.getFalseValue();
    auto cond = selectOp.getCondition();

    // replace arith.select with cgra.bzfa
    rewriter.replaceOpWithNewOp<cgra::BzfaOp>(
        selectOp, selectOp->getResult(0).getType(), cond,
        SmallVector<Value>({falseVal, trueVal}));

    return success();
  }
};

/// Lower cf::CondBranchOp to cgra::CondBranchOp
struct CfCondBrOpConversion : OpConversionPattern<cf::CondBranchOp> {
  using OpConversionPattern<cf::CondBranchOp>::OpConversionPattern;

  LogicalResult
  matchAndRewrite(cf::CondBranchOp condBrOp, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    // create a new block for the default branch
    Block *condBrBlock = getCgraBranchDstBlock(condBrOp->getBlock());
    bool forceJump = forceJumpToNextBlock(condBrOp->getBlock());
    Block *falseBlk = condBrBlock == condBrOp.getTrueDest()
                          ? condBrOp.getFalseDest()
                          : condBrOp.getTrueDest();
    if (forceJump) {
      rewriter.setInsertionPoint(condBrOp);
      falseBlk = rewriter.createBlock(condBrOp->getBlock()->getNextNode());
    }

    // get predicate from the condition
    arith::CmpIOp cmpOp =
        condBrOp.getCondition().getDefiningOp<arith::CmpIOp>();
    arith::CmpIPredicate predicate = cmpOp.getPredicate();

    Value cmpOpr0 = cmpOp.getOperand(0);
    Value cmpOpr1 = cmpOp.getOperand(1);
    if (cmpOp.getOperand(0).getType().isa<IndexType>()) {
      rewriter.setInsertionPoint(condBrOp);
      arith::IndexCastOp castOp0 = rewriter.create<arith::IndexCastOp>(
          cmpOp->getLoc(), rewriter.getIntegerType(32), cmpOp.getOperand(0));
      arith::IndexCastOp castOp1 = rewriter.create<arith::IndexCastOp>(
          cmpOp->getLoc(), rewriter.getIntegerType(32), cmpOp.getOperand(1));
      cmpOpr0 = castOp0.getResult();
      cmpOpr1 = castOp1.getResult();
    }

    bool switchSuccs = condBrBlock == condBrOp.getFalseDest();
    if (switchSuccs)
      predicate = reverseCmpFlag(predicate);

    auto condBrArgs = switchSuccs ? condBrOp.getFalseDestOperands()
                                  : condBrOp.getTrueDestOperands();
    auto jumpArgs = switchSuccs ? condBrOp.getTrueDestOperands()
                                : condBrOp.getFalseDestOperands();
    Block *jumpBlock =
        switchSuccs ? condBrOp.getTrueDest() : condBrOp.getFalseDest();

    // replace cf.condbr with cgra.condbr
    cgra::CondBrPredicate cgraPred =
        getCgraBrPredicate(predicate, cmpOpr0, cmpOpr1);
    if (forceJump) {
      auto newCondBr = rewriter.replaceOpWithNewOp<cgra::ConditionalBranchOp>(
          condBrOp, cgraPred, cmpOpr0, cmpOpr1, condBrBlock, condBrArgs,
          falseBlk, SmallVector<Value>());
      rewriter.setInsertionPointToStart(falseBlk);
      auto defaultBr = rewriter.create<cf::BranchOp>(newCondBr->getLoc(),
                                                     jumpArgs, jumpBlock);
      defaultBr->moveAfter(&falseBlk->getOperations().front());
    } else {
      rewriter.replaceOpWithNewOp<cgra::ConditionalBranchOp>(
          condBrOp, cgraPred, cmpOpr0, cmpOpr1, condBrBlock, condBrArgs,
          falseBlk, jumpArgs);
    }

    return success();
  }
};

/// Rewrite arith::CmpIOp to corresponding operation in cgra dialect
struct ArithCmpIOpConversion : OpConversionPattern<arith::CmpIOp> {
  using OpConversionPattern<arith::CmpIOp>::OpConversionPattern;

  LogicalResult
  matchAndRewrite(arith::CmpIOp cmpOp, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto predicate = cmpOp.getPredicate();

    // Use substraction to compare the two operands
    rewriter.setInsertionPoint(cmpOp);
    arith::SubIOp subOp = nullptr;
    // Reverse the operands order for greater than or equal to and greater
    if (predicate == arith::CmpIPredicate::uge ||
        predicate == arith::CmpIPredicate::sge ||
        predicate == arith::CmpIPredicate::ugt ||
        predicate == arith::CmpIPredicate::sgt) {
      subOp = rewriter.create<arith::SubIOp>(
          cmpOp.getLoc(), cmpOp.getOperand(1), cmpOp.getOperand(0));
    } else
      subOp = rewriter.create<arith::SubIOp>(
          cmpOp.getLoc(), cmpOp.getOperand(0), cmpOp.getOperand(1));

    // insert additional bzfa operation to conclude equal case of the
    // comparison.
    auto selectFlag = subOp.getResult();
    if (predicate == arith::CmpIPredicate::uge ||
        predicate == arith::CmpIPredicate::uge ||
        predicate == arith::CmpIPredicate::ule ||
        predicate == arith::CmpIPredicate::ule) {
      // create constant -1 to indicate the equal case
      auto resType = subOp.getResult().getType();
      arith::ConstantIntOp constOp = rewriter.create<arith::ConstantIntOp>(
          cmpOp.getLoc(), -1, resType.getIntOrFloatBitWidth());

      // create bzfa operation to include the equal case
      cgra::BzfaOp bzfaOp = rewriter.create<cgra::BzfaOp>(
          cmpOp.getLoc(), subOp.getResult().getType(), subOp.getResult(),
          SmallVector<Value>({constOp.getResult(), subOp.getResult()}));
      selectFlag = bzfaOp.getResult();
    }

    // Replace the select operation with bsfa/bzfa operation.
    SmallVector<Operation *> selOps;
    for (auto user : cmpOp->getUsers()) {
      if (auto selOp = dyn_cast_or_null<arith::SelectOp>(user))
        selOps.push_back(selOp);
    }
    for (auto selOp : selOps) {
      rewriter.setInsertionPoint(selOp);
      if (predicate == arith::CmpIPredicate::eq) {
        rewriter.replaceOpWithNewOp<cgra::BzfaOp>(
            selOp, selOp->getResult(0).getType(), selectFlag,
            SmallVector<Value>({selOp->getOperand(1), selOp->getOperand(2)}));
      } else if (predicate == arith::CmpIPredicate::ne) {
        rewriter.replaceOpWithNewOp<cgra::BzfaOp>(
            selOp, selOp->getResult(0).getType(), selectFlag,
            SmallVector<Value>({selOp->getOperand(2), selOp->getOperand(1)}));
      } else {
        rewriter.replaceOpWithNewOp<cgra::BsfaOp>(
            selOp, selOp->getResult(0).getType(), selectFlag,
            SmallVector<Value>({selOp->getOperand(1), selOp->getOperand(2)}));
      }
    }

    // check whether the cmpOp has non-select and non-branch users
    bool existNonSelAndBrUser = false;
    for (auto user : cmpOp->getUsers()) {
      if (!isa<cf::CondBranchOp, arith::SelectOp>(user)) {
        existNonSelAndBrUser = true;
        break;
      }
    }
    if (existNonSelAndBrUser) {
      // insert additional bsfa operation to conclude the comparison
      arith::ConstantIntOp constOp0 =
          rewriter.create<arith::ConstantIntOp>(cmpOp.getLoc(), 0, 1);
      arith::ConstantIntOp constOp1 =
          rewriter.create<arith::ConstantIntOp>(cmpOp.getLoc(), 1, 1);
      rewriter.setInsertionPoint(cmpOp);
      Operation *binSelOp = nullptr;
      if (predicate == arith::CmpIPredicate::eq) {
        // insert bzfa %selFlag, 1, 0;
        binSelOp = rewriter.create<cgra::BzfaOp>(
            cmpOp.getLoc(), rewriter.getI1Type(), selectFlag,
            SmallVector<Value>({constOp1.getResult(), constOp0.getResult()}));
      } else if (predicate == arith::CmpIPredicate::ne) {
        // insert bzfa %selFlag, 0, 1;
        binSelOp = rewriter.create<cgra::BzfaOp>(
            cmpOp.getLoc(), rewriter.getI1Type(), selectFlag,
            SmallVector<Value>({constOp0.getResult(), constOp1.getResult()}));
      } else {
        // insert bsfa %selFlag, 1, 0;
        binSelOp = rewriter.create<cgra::BsfaOp>(
            cmpOp.getLoc(), rewriter.getI1Type(), selectFlag,
            SmallVector<Value>({constOp1.getResult(), constOp0.getResult()}));
      }
      rewriter.replaceOp(cmpOp, binSelOp->getResult(0));
    } else {
      rewriter.eraseOp(cmpOp);
    }
    return success();
  }
};

template <typename MemRefOp>
Operation *computeOffSet(MemRefOp memOp, Operation *baseAddr,
                         SmallVector<Operation *> strideVals,
                         ConversionPatternRewriter &rewriter) {
  Operation *offSet = nullptr;

  // Pre-compute cumulative strides as constant values
  SmallVector<Operation *> cumulativeStrides;
  for (size_t dim = 0; dim < strideVals.size(); ++dim) {
    int64_t strideProduct = 1;
    for (size_t i = dim; i < strideVals.size(); ++i) {
      // Extract constant value from the operation
      auto constOp = dyn_cast<arith::ConstantOp>(strideVals[i]);
      int64_t val = constOp.getValue().cast<IntegerAttr>().getInt();
      strideProduct *= val;
    }
    // Create a single constant operation for the cumulative stride
    auto cumulativeStride = rewriter.create<arith::ConstantOp>(
        memOp.getLoc(), rewriter.getI32Type(),
        rewriter.getI32IntegerAttr(strideProduct));
    cumulativeStrides.push_back(cumulativeStride);
  }

  for (auto [dim, indice] : llvm::enumerate(memOp.getIndices())) {
    // if indice is a constant 0, skip
    if (auto constantOp =
            dyn_cast_or_null<arith::ConstantIndexOp>(indice.getDefiningOp())) {
      if (constantOp.value() == 0) {
        continue;
      }
    }

    auto castOp = rewriter.create<arith::IndexCastOp>(
        memOp.getLoc(), rewriter.getIntegerType(32), indice);

    if (dim == memOp.getIndices().size() - 1) {
      if (offSet)
        offSet = rewriter.create<arith::AddIOp>(
            memOp.getLoc(), rewriter.getI32Type(), offSet->getResult(0),
            castOp.getResult());
      else
        offSet = castOp;
      break;
    }

    auto dimStride = rewriter.create<arith::MulIOp>(
        memOp.getLoc(), rewriter.getI32Type(), castOp.getResult(),
        cumulativeStrides[dim]->getResult(0));
    if (offSet)
      offSet = rewriter.create<arith::AddIOp>(
          memOp.getLoc(), rewriter.getI32Type(), offSet->getResult(0),
          dimStride->getResult(0));
    else
      offSet = dimStride;
  }

  return offSet;
}

static int preComputeOffset(Operation *offSetOp, Operation *byteOp) {

  if (arith::IndexCastOp cstOffset =
          dyn_cast_or_null<arith::IndexCastOp>(offSetOp)) {
    // directly compute the offset
    auto srcOffset = dyn_cast_or_null<arith::ConstantIndexOp>(
        cstOffset.getOperand().getDefiningOp());
    if (!srcOffset)
      return -1;

    long byteWidth = 4;
    if (auto cstByte = dyn_cast_or_null<arith::ConstantIntOp>((byteOp)))
      byteWidth = cstByte.value();
    return byteWidth * srcOffset.value();
  }
  return -1;
}

// Rewrite memref.get_global to constant operation
struct MemRefGetGlobalOpConversion : OpConversionPattern<memref::GetGlobalOp> {
  MemRefGetGlobalOpConversion(MLIRContext *ctx,
                              DenseMap<llvm::StringRef, Operation *> baseAddrs)
      : OpConversionPattern<memref::GetGlobalOp>(ctx), baseAddrs(baseAddrs) {}

  LogicalResult
  matchAndRewrite(memref::GetGlobalOp getGlobalOp, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    auto varName = getGlobalOp.getName();
    rewriter.replaceOp(getGlobalOp, baseAddrs.at(varName));
    return success();
  }

  DenseMap<llvm::StringRef, Operation *> baseAddrs;
};

/// Lower memref.load/ memref.store to cgra.lwi/cgra.swi
template <typename MemRefOp>
struct MemRefRWOpConversion : OpConversionPattern<MemRefOp> {
  MemRefRWOpConversion(
      MLIRContext *ctx, DenseMap<int, Operation *> &baseAddrs,
      DenseMap<llvm::StringRef, Operation *> &globalConstAddrs,
      DenseMap<Operation *, Operation *> &allocAddrs,
      DenseMap<Operation *, SmallVector<Operation *>> &strideValMap)
      : OpConversionPattern<MemRefOp>(ctx), baseAddrs(baseAddrs),
        globalConstAddrs(globalConstAddrs), allocAddrs(allocAddrs),
        strideValMap(strideValMap) {}

  LogicalResult
  matchAndRewrite(MemRefOp op, typename MemRefOp::Adaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    Operation *byteOp = baseAddrs.at(-1);
    Location loc = op.getLoc();

    // compute the base address of op
    Value ref = op.getMemRef();
    Operation *baseOp;
    if (auto arg = dyn_cast_or_null<BlockArgument>(ref)) {
      unsigned argIndex = arg.getArgNumber();
      baseOp = baseAddrs.at(argIndex);
    } else if (isa<memref::GetGlobalOp>(ref.getDefiningOp())) {
      auto getOp = dyn_cast<memref::GetGlobalOp>(ref.getDefiningOp());
      baseOp = globalConstAddrs.at(getOp.getName());
    } else if (isa<memref::AllocOp>(ref.getDefiningOp())) {
      baseOp = allocAddrs.at(ref.getDefiningOp());
    } else {
      return failure();
    }

    // get the index of the memory reference
    rewriter.setInsertionPoint(op);
    auto offSetOp =
        computeOffSet<MemRefOp>(op, baseOp, strideValMap.at(baseOp), rewriter);

    Operation *addrOp = nullptr;
    if (offSetOp) {
      Operation *byteOffset;
      int byteWidth = preComputeOffset(offSetOp, byteOp);
      if (byteWidth >= 0) {
        byteOffset = rewriter.create<arith::ConstantIntOp>(loc, byteWidth, 32);
      } else {
        byteOffset = rewriter.create<arith::MulIOp>(loc, rewriter.getI32Type(),
                                                    offSetOp->getResult(0),
                                                    byteOp->getResult(0));
      }
      addrOp = rewriter.create<arith::AddIOp>(loc, rewriter.getI32Type(),
                                              baseOp->getResult(0),
                                              byteOffset->getResult(0));
    } else {
      addrOp = baseOp;
    }

    if constexpr (std::is_same_v<MemRefOp, memref::LoadOp>) {
      rewriter.replaceOpWithNewOp<cgra::LwiOp>(op, op.getResult().getType(),
                                               addrOp->getResult(0));
    } else if constexpr (std::is_same_v<MemRefOp, memref::StoreOp>) {
      rewriter.replaceOpWithNewOp<cgra::SwiOp>(op, op.getValue(),
                                               addrOp->getResult(0));
    }

    return success();
  }

  DenseMap<int, Operation *> baseAddrs;
  DenseMap<llvm::StringRef, Operation *> globalConstAddrs;
  DenseMap<Operation *, SmallVector<Operation *>> strideValMap;
  DenseMap<Operation *, Operation *> allocAddrs;
};

} // namespace

static LogicalResult
assignMemoryToArg(mlir::Type typeAttr, unsigned &lastPtr,
                  std::vector<int> &memAlloc,
                  std::vector<std::vector<int>> &memRefDims) {
  if (!typeAttr.isa<MemRefType>())
    return failure();
  auto memrefType = typeAttr.cast<MemRefType>();
  memRefDims.push_back(std::vector<int>());
  // arg.getOperation()

  // Allocate memory based on the default size
  memAlloc.push_back(lastPtr);
  int memRefSize = 1;
  for (int i = memrefType.getRank() - 1; i >= 0; i--) {
    memRefSize *= memrefType.getDimSize(i);
    memRefDims.back().insert(memRefDims.back().begin(), memRefSize);
  }
  lastPtr += memRefSize * 4;

  return success();
}

LogicalResult
allocateMemory(ModuleOp &modOp, DenseMap<int, Operation *> &constAddr,
               DenseMap<llvm::StringRef, Operation *> &globalConstAddrs,
               DenseMap<Operation *, Operation *> &allocAddrs,
               DenseMap<Operation *, SmallVector<Operation *>> &offValMap,
               OpBuilder &builder, Pass::ListOption<int> &startAddr) {
  if (modOp.getOps<func::FuncOp>().empty())
    return success();

  auto funcOp = *modOp.getOps<func::FuncOp>().begin();

  unsigned lastPtr = 0;
  if (!startAddr.empty()) {
    lastPtr = startAddr[0];
  }

  // assign memory for function arguments
  unsigned directLoadArgNum = 0;
  builder.setInsertionPointToStart(&funcOp.getBlocks().front());
  for (auto [ind, arg] : llvm::enumerate(funcOp.getArguments())) {
    if (!arg.getType().isa<MemRefType>() &&
        arg.getType() != builder.getI32Type())
      return failure();

    if (arg.getType() == builder.getI32Type()) {
      directLoadArgNum++;
      // insert lwd operation for the integer argument
      auto lwdOp =
          builder.create<cgra::LwdOp>(funcOp.getLoc(), builder.getI32Type());
      lwdOp->setAttr("Value",
                     builder.getStringAttr("arg" + std::to_string(ind)));
      constAddr[ind] = lwdOp;
      // replace the argument with the lwd operation result
      arg.replaceAllUsesWith(lwdOp.getResult());
      continue;
    }

    auto memrefType = arg.getType().cast<MemRefType>();
    if (memrefType.getDimSize(0) <= 0)
      return failure();

    // allocate memory for array argument
    Operation *baseOp = nullptr;
    if (startAddr.empty()) {
      baseOp =
          builder.create<cgra::LwdOp>(funcOp.getLoc(), builder.getI32Type());
    } else {
      baseOp = builder.create<arith::ConstantIntOp>(funcOp.getLoc(), lastPtr,
                                                    builder.getI32Type());
    }
    constAddr[ind] = baseOp;
    baseOp->setAttr("BaseAddr",
                    builder.getStringAttr("arg" + std::to_string(ind)));
    SmallVector<Operation *> dimOps;
    int memRefSize = 1;
    for (int i = 1; i < memrefType.getRank(); i++) {
      auto curDim = memrefType.getDimSize(i);
      memRefSize *= curDim;
      auto dimOp = builder.create<arith::ConstantIntOp>(funcOp.getLoc(), curDim,
                                                        builder.getI32Type());
      dimOp->setAttr("arg", builder.getIntegerAttr(builder.getI32Type(), ind));
      dimOp->setAttr("DimProd",
                     builder.getIntegerAttr(builder.getI32Type(), i));
      dimOps.push_back(dimOp);
    }
    offValMap[baseOp] = dimOps;
    lastPtr += memRefSize * 4;
  }
  // create a constant operation to initialize the offset
  auto offset = builder.create<arith::ConstantIntOp>(funcOp.getLoc(), 4,
                                                     builder.getI32Type());
  constAddr[-1] = offset;

  // assign memory for alloc operations
  unsigned baseAddr = 0x10158;
  for (auto [ind, op] : llvm::enumerate(funcOp.getOps<memref::AllocOp>())) {
    auto baseOp = builder.create<arith::ConstantIntOp>(
        funcOp.getLoc(), baseAddr, builder.getI32Type());
    baseOp->setAttr("BaseAddr",
                    builder.getStringAttr("alloc" + std::to_string(ind)));
    allocAddrs[op] = baseOp;
    SmallVector<Operation *> dimOps;
    int memRefSize = 1;
    auto memrefType = op.getType().cast<MemRefType>();
    for (int i = 1; i < memrefType.getRank(); i++) {
      auto curDim = memrefType.getDimSize(i);
      memRefSize *= curDim;
      auto dimOp = builder.create<arith::ConstantIntOp>(funcOp.getLoc(), curDim,
                                                        builder.getI32Type());
      dimOp->setAttr("alloc",
                     builder.getIntegerAttr(builder.getI32Type(), ind));
      dimOp->setAttr("DimProd",
                     builder.getIntegerAttr(builder.getI32Type(), i));
      dimOps.push_back(dimOp);
    }
    offValMap[baseOp] = dimOps;
    baseAddr += memRefSize * 4;
  }

  std::vector<int> memAlloc;
  std::vector<std::vector<int>> memRefDims;
  std::map<int, memref::GetGlobalOp> globalArgs;
  // assign memory for global arguments
  for (auto [ind, arg] : llvm::enumerate(funcOp.getOps<memref::GetGlobalOp>())) {
    globalArgs[ind] = arg;
    assignMemoryToArg(arg.getType(), lastPtr, memAlloc, memRefDims);
  }
  for (unsigned i = 0; i < memAlloc.size(); i++) {
    Operation *baseOp = builder.create<arith::ConstantIntOp>(
        funcOp.getLoc(), memAlloc[i], builder.getI32Type());
    baseOp->setAttr("BaseAddr",
                    builder.getStringAttr("global" + std::to_string(i)));

    SmallVector<Operation *> dimOps;
    for (unsigned j = 1; j < memRefDims[i].size(); j++) {
      auto dimOp = builder.create<arith::ConstantIntOp>(
          funcOp.getLoc(), memRefDims[i][j], builder.getI32Type());
      dimOp->setAttr("global", builder.getIntegerAttr(builder.getI32Type(), i));
      dimOp->setAttr("DimProd",
                     builder.getIntegerAttr(builder.getI32Type(), j));
      dimOps.push_back(dimOp);
    }
    offValMap[baseOp] = dimOps;
    globalConstAddrs[globalArgs[i].getName()] = baseOp;
  }

  return success();
}

void populateCfToCgraConversionPatterns(
    RewritePatternSet &patterns, DenseMap<int, Operation *> &constAddr,
    DenseMap<llvm::StringRef, Operation *> &globalConstAddrs,
    DenseMap<Operation *, Operation *> &allocAddrs,
    DenseMap<Operation *, SmallVector<Operation *>> &offValMap) {
  patterns.add<CfCondBrOpConversion>(patterns.getContext());
  patterns.add<ArithCmpIOpConversion>(patterns.getContext());
  patterns.add<ArithSelectOpConversion>(patterns.getContext());
  patterns.add<MemRefRWOpConversion<memref::LoadOp>,
               MemRefRWOpConversion<memref::StoreOp>>(
      patterns.getContext(), constAddr, globalConstAddrs, allocAddrs,
      offValMap);
  patterns.add<MemRefGetGlobalOpConversion>(patterns.getContext(),
                                            globalConstAddrs);
}

// Function to compute the linear offset of a subview operation
// This creates arithmetic operations to calculate: offset = sum(index[i] *
// stride[i]) + base_offset Parameters:
//   - builder: OpBuilder to create arithmetic operations
//   - loc: Location for the operations
//   - subviewOp: The SubViewOp to compute offset for
// Returns:
//   - Value representing the computed linear offset
Value computeSubviewOffset(OpBuilder &builder, Location loc,
                           memref::SubViewOp subviewOp) {
  Value sourceMemref = subviewOp.getSource();
  MemRefType sourceType = sourceMemref.getType().cast<MemRefType>();
  ArrayRef<int64_t> sourceShape = sourceType.getShape();

  // Get the offsets from the subview operation
  SmallVector<OpFoldResult> offsets = subviewOp.getMixedOffsets();

  builder.setInsertionPointAfter(subviewOp);

  // Precompute strides for each dimension
  SmallVector<int64_t> strides(sourceShape.size(), 1);
  for (int i = sourceShape.size() - 2; i >= 0; --i) {
    if (sourceShape[i + 1] == ShapedType::kDynamic || strides[i + 1] == 0)
      return nullptr; // Can't precompute if dynamic
    strides[i] = strides[i + 1] * sourceShape[i + 1];
  }

  int64_t totalConstOffset = 0;
  Value totalOffset = nullptr;

  for (size_t i = 0; i < offsets.size(); ++i) {
    int64_t stride = strides[i];
    if (auto attr = offsets[i].dyn_cast<Attribute>()) {
      int64_t staticOffset = attr.cast<IntegerAttr>().getInt();
      if (staticOffset != 0)
        totalConstOffset += staticOffset * stride;
    } else {
      Value offsetValue = offsets[i].get<Value>();
      // Only generate IR if offset is not a constant zero
      if (auto constOp = offsetValue.getDefiningOp<arith::ConstantOp>()) {
        if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
          if (intAttr.getInt() == 0)
            continue;
        }
      }
      Value strideVal = builder.create<arith::ConstantOp>(
          loc, builder.getIndexType(), builder.getIndexAttr(stride));
      Value contribution =
          builder.create<arith::MulIOp>(loc, offsetValue, strideVal);
      if (totalOffset)
        totalOffset =
            builder.create<arith::AddIOp>(loc, totalOffset, contribution);
      else
        totalOffset = contribution;
    }
  }

  // If all offsets are constant, just return a constant
  if (!totalOffset) {
    return builder.create<arith::ConstantOp>(
        loc, builder.getIndexType(), builder.getIndexAttr(totalConstOffset));
  }
  // If there is a constant offset, add it
  if (totalConstOffset != 0) {
    Value constOffset = builder.create<arith::ConstantOp>(
        loc, builder.getIndexType(), builder.getIndexAttr(totalConstOffset));
    totalOffset = builder.create<arith::AddIOp>(loc, totalOffset, constOffset);
  }
  return totalOffset;
}

static LogicalResult
getBLASArguments(cgra::BlasGemmOp op, SmallVector<Value> &newOperands,
                 DenseMap<Operation *, Operation *> allocAddrs,
                 OpBuilder &builder, func::FuncOp funcOp) {
  for (auto operand : op.getOperands()) {
    // check whether index or integer type
    if (operand.getType().isa<IndexType>() ||
        operand.getType().isa<IntegerType>()) {
      newOperands.push_back(operand);
      continue;
    }

    auto memrefType = dyn_cast_or_null<MemRefType>(operand.getType());
    if (!memrefType) {
      newOperands.push_back(operand);
      continue;
    }

    if (memrefType.getRank() < 2)
      return failure();

    if (memrefType.getRank() == 2) {
      if (auto blockArg = dyn_cast_or_null<BlockArgument>(operand)) {
        builder.setInsertionPoint(op);
        auto baseOp =
            builder.create<cgra::LwdOp>(op.getLoc(), builder.getI32Type());
        baseOp->setAttr("BaseAddr",
                        builder.getStringAttr(
                            "arg" + std::to_string(blockArg.getArgNumber())));

        newOperands.push_back(baseOp->getResult(0));
        continue;
      } else if (auto getOp = dyn_cast_or_null<memref::AllocOp>(
                     operand.getDefiningOp())) {
        newOperands.push_back(allocAddrs.at(getOp)->getResult(0));
        continue;
      } else {
        return failure();
      }
    }

    // beyond 2D memref, check whether is produced by a subview operation, and
    // first (N-2) dimension are all 1.
    auto prodOp = dyn_cast_or_null<memref::SubViewOp>(operand.getDefiningOp());
    if (!prodOp)
      return failure();

    ArrayRef<int64_t> shape = memrefType.getShape();
    int64_t rank = shape.size();
    for (int i = 0; i < rank - 2; i++) {
      if (shape[i] != 1)
        return failure();
    }

    // compute the offset of the subview operation
    auto offSet = computeSubviewOffset(builder, op.getLoc(), prodOp);
    if (isa<IndexType>(offSet.getType())) {
      auto castOp = builder.create<arith::IndexCastOp>(
          op.getLoc(), builder.getI32Type(), offSet);
      offSet = castOp.getResult();
    }
    // offset = offset * 4
    auto byteOp = builder.create<arith::ConstantIntOp>(op.getLoc(), 4,
                                                       builder.getI32Type());
    auto offMulOp = builder.create<arith::MulIOp>(
        op.getLoc(), builder.getI32Type(), offSet, byteOp->getResult(0));
    auto blockArg = dyn_cast_or_null<BlockArgument>(prodOp.getSource());
    if (!blockArg)
      return failure();
    builder.setInsertionPointToStart(&funcOp.getBlocks().front());
    auto baseOp =
        builder.create<cgra::LwdOp>(op.getLoc(), builder.getI32Type());
    baseOp->setAttr(
        "BaseAddr",
        builder.getStringAttr("arg" + std::to_string(blockArg.getArgNumber())));
    builder.setInsertionPoint(op);
    auto startAddr = builder.create<arith::AddIOp>(
        op.getLoc(), builder.getI32Type(), baseOp->getResult(0),
        offMulOp.getResult());

    newOperands.push_back(startAddr->getResult(0));
  }

  // create save operation to store the operands to the memory
  for (auto [ind, operand] : llvm::enumerate(newOperands)) {
    if (operand.getParentBlock() == op->getBlock())
      builder.setInsertionPoint(op);
    else
      builder.setInsertionPoint(operand.getParentBlock()->getTerminator());
    auto addrOp = builder.create<arith::ConstantIntOp>(
        op.getLoc(), 0xFE00 + ind * 4, builder.getI32Type());
    builder.create<cgra::SwiOp>(op.getLoc(), operand, addrOp->getResult(0));
  }
  return success();
}

static void connectPredecessorToInitBlk(SmallVector<mlir::Block *> predecessors,
                                        Block *initBlk, Block *sucBlk) {
  for (auto *pred : predecessors) {
    // Get the terminator of the predecessor block
    Operation *terminator = pred->getTerminator();

    // Handle cf.br (unconditional branch)
    if (auto brOp = dyn_cast<mlir::cf::BranchOp>(terminator)) {
      // Change destination from sucBlk to initBlk
      brOp.setDest(initBlk);
    }
    // Handle cf.cond_br (conditional branch)
    else if (auto condBrOp = dyn_cast<mlir::cf::CondBranchOp>(terminator)) {
      // Check which successor is sucBlk and update accordingly
      if (condBrOp.getTrueDest() == sucBlk) {
        condBrOp->setSuccessor(initBlk, 0);
      }
      if (condBrOp.getFalseDest() == sucBlk) {
        condBrOp->setSuccessor(initBlk, 1);
      }
    }
    // Handle blas incremental PC jump
    else if (auto blasAsmOp = dyn_cast<cgra::BlasGemmAsmOp>(terminator)) {
      // Change destination from sucBlk to initBlk
      blasAsmOp->setSuccessor(initBlk, 0);
    }
  }
}

static LogicalResult
transformkernelBLAS(func::FuncOp funcOp,
                    DenseMap<Operation *, Operation *> allocAddrs,
                    OpBuilder &builder) {
  // if find a blas gemm operation, create a new block for it
  SmallVector<cgra::BlasGemmOp> blasOps;
  for (auto [ind, op] : llvm::enumerate(funcOp.getOps<cgra::BlasGemmOp>())) {
    // revise the operands of op
    // get the subview operation
    SmallVector<memref::SubViewOp> subviewOps;
    for (auto operand : op.getOperands()) {
      auto subviewOp =
          dyn_cast_or_null<memref::SubViewOp>(operand.getDefiningOp());
      if (subviewOp)
        subviewOps.push_back(subviewOp);
    }

    SmallVector<Value> newOperands;
    if (failed(getBLASArguments(op, newOperands, allocAddrs, builder, funcOp)))
      return failure();

    // set attributes for the blas arguments
    for (auto [oprId, operand] : llvm::enumerate(newOperands)) {
      auto defOp = operand.getDefiningOp();
      defOp->setAttr("blas", builder.getStringAttr(std::to_string(ind)));
      defOp->setAttr("blas_arg", builder.getStringAttr(std::to_string(oprId)));
    }

    // Update the operation with new operands
    op->setOperands(newOperands);
    // remove subview operation if it is not used anymore
    for (auto subviewOp : subviewOps) {
      if (subviewOp->use_empty())
        subviewOp.erase();
    }

    blasOps.push_back(op);
  }
  if (blasOps.empty())
    return success();

  // create sequential CFG for the blas gemm operation
  for (auto [ind, blasOp] : llvm::enumerate(blasOps)) {

    auto finiBlk = blasOp->getBlock();
    builder.setInsertionPoint(blasOp);
    auto initArg = finiBlk->getArguments();
    auto initBlk = builder.createBlock(finiBlk);

    // if finiBlk has predecessor blocks, change it to initBlk
    SmallVector<Block *> predecessors(finiBlk->getPredecessors());
    connectPredecessorToInitBlk(predecessors, initBlk, finiBlk);

    // store operations in the init blocks

    // replace of finiBlk arguments with initBlk arguments and remove it
    for (auto [ind, arg] : llvm::enumerate(finiBlk->getArguments())) {
      initBlk->addArgument(arg.getType(), blasOp->getLoc());
      arg.replaceAllUsesWith(initBlk->getArgument(ind));
    }
    while (!finiBlk->args_empty()) {
      finiBlk->eraseArgument(0);
    }

    auto blasBlk = builder.createBlock(finiBlk);
    // add a jump operation from predBlk to blasBlk
    builder.setInsertionPointToEnd(initBlk);
    builder.create<cf::BranchOp>(blasOp->getLoc(), blasBlk);

    builder.setInsertionPointToStart(blasBlk);
    auto asmBLASOp =
        builder.create<cgra::BlasGemmAsmOp>(blasOp.getLoc(), finiBlk);
    asmBLASOp->setAttrs(blasOp->getAttrs());
    asmBLASOp->setAttr("blas", builder.getStringAttr(std::to_string(ind)));

    for (auto &op : llvm::make_early_inc_range(finiBlk->getOperations())) {
      if (&op == blasOp)
        break;
      op.moveBefore(initBlk->getTerminator());
    }
    // remove the original blas operation
    blasOp.erase();
  }

  return success();
}

void CfToCgraConversionPass::runOnOperation() {
  ModuleOp modOp = dyn_cast<ModuleOp>(getOperation());
  OpBuilder builder(modOp);
  DenseMap<llvm::StringRef, Operation *> globalConstAddrs;
  DenseMap<int, Operation *> constAddrs;
  DenseMap<Operation *, Operation *> allocAddrs;

  // Map to store the stride information for each memory reference, where
  // the key is the constant value of the base address.
  // For a multidimensional array (e.g., c[M][N][K]), the stored value
  // is a vector of products of the inner dimensions (e.g., [N*K, K]).
  DenseMap<Operation *, SmallVector<Operation *>> offValMap;
  if (failed(allocateMemory(modOp, constAddrs, globalConstAddrs, allocAddrs,
                            offValMap, builder, startAddr)))
    return signalPassFailure();

  if (failed(transformkernelBLAS(*modOp.getOps<func::FuncOp>().begin(),
                                 allocAddrs, builder)))
    return signalPassFailure();

  ConversionTarget target(getContext());

  target.addIllegalOp<arith::CmpIOp>();
  target.addIllegalOp<memref::LoadOp>();
  target.addIllegalOp<cf::CondBranchOp>();
  target.addIllegalOp<memref::StoreOp>();
  target.addIllegalOp<memref::GetGlobalOp>();
  target.addIllegalOp<arith::SelectOp>();

  target.addLegalOp<cgra::ConditionalBranchOp>();
  target.addLegalOp<cgra::LwiOp>();
  target.addLegalOp<cgra::SwiOp>();
  target.markUnknownOpDynamicallyLegal([](Operation *) { return true; });

  RewritePatternSet patterns(&getContext());
  populateCfToCgraConversionPatterns(patterns, constAddrs, globalConstAddrs,
                                     allocAddrs, offValMap);
  if (failed(
          applyPartialConversion(getOperation(), target, std::move(patterns))))
    signalPassFailure();

  // raise the constant operation to the top level
  auto funcOps = modOp.getOps<func::FuncOp>();
  if (!funcOps.empty()) {
    auto funcOp = *funcOps.begin();
    if (failed(removeUnusedOps(funcOp)) || failed(raiseConstOpToTop(funcOp)))
      signalPassFailure();
  }
}

namespace compigra {
std::unique_ptr<mlir::Pass> createCfToCgraConversion() {
  return std::make_unique<CfToCgraConversionPass>();
}
} // namespace compigra
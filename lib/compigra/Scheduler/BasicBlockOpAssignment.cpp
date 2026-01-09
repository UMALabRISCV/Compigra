//===- BasicBlockOpAssignment.cpp - Implements the class/functions to place
// operations of a basic block *- C++-* ----------------------------------===//
//
// Compigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements class for BasicBlockILPModel functions.
//
//===----------------------------------------------------------------------===//

#include "compigra/Scheduler/BasicBlockOpAssignment.h"
#include "compigra/CgraOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include <fstream>
#include <numeric>
#include <queue>

using namespace mlir;
using namespace compigra;

// Global output directory for log files
static std::string g_logOutputDir = ".";

void compigra::setLogOutputDir(const std::string &dir) {
  g_logOutputDir = dir;
}

// Function to log messages to a file
void logMessage(const std::string &message, bool overwrite, bool debug) {
  if (!debug)
    return;
  
  // Build path for log file in logs subdirectory
  llvm::SmallString<256> logPath(g_logOutputDir);
  llvm::sys::path::append(logPath, "logs");
  llvm::sys::fs::create_directories(logPath);
  llvm::sys::path::append(logPath, "compigra_mapping.log");
  std::string logFilePath = std::string(logPath.str());
  
  // if overwrite is true, clear the log file
  if (overwrite) {
    std::ofstream logFile(logFilePath,
                          std::ios::out | std::ios::trunc);
    logFile.close();
  }

  std::ofstream logFile(logFilePath, std::ios::out | std::ios::app);
  if (!logFile.is_open()) {
    llvm::errs() << "ERROR: Unable to open log file for writing.\n";
    return;
  }
  logFile << message << std::endl;
  logFile.close();
}

template <typename T>
static SetVector<T> getSubSet(SetVector<T> vec1, SetVector<T> vec2) {
  SetVector<T> result = vec1;
  for (auto it = result.begin(); it != result.end();) {
    if (vec2.count(*it) == 0)
      it = result.erase(it);
    else
      ++it;
  }
  return result;
}

template <typename T>
SetVector<T> compigra::getInterSection(SetVector<T> &vec1, SetVector<T> &vec2) {
  SetVector<T> result;
  for (auto it = vec1.begin(); it != vec1.end();) {
    if (vec2.count(*it) > 0)
      result.insert(*it);
    ++it;
  }
  return result;
}

static bool isTerminatorOp(Operation *op) {
  return op->getBlock()->getTerminator() == op;
}

static unsigned getNonCstOpSize(Block *block) {
  unsigned constantOpSize =
      std::distance(block->getOps<arith::ConstantOp>().begin(),
                    block->getOps<arith::ConstantOp>().end());
  unsigned scheduledOpSize = block->getOperations().size() - constantOpSize;
  return scheduledOpSize;
}

/// Insert a value to a set if it is not a constant. The constant value is not
/// considered as a live value.
static void insertNonConst(Value val, SetVector<Value> &vec) {
  if (dyn_cast_or_null<arith::ConstantOp>(val.getDefiningOp()) ||
      dyn_cast_or_null<arith::ConstantIntOp>(val.getDefiningOp()) ||
      dyn_cast_or_null<arith::ConstantFloatOp>(val.getDefiningOp()))
    return;
  vec.insert(val);
}

/// successor blocks. If the liveIn value is a block argument (phi node), add
/// the corresponding value in the predecessor block.
static void updateLiveOutBySuccessorLiveIn(Value val, Block *blk,
                                           SetVector<Value> &liveOut) {
  if (auto arg = dyn_cast_or_null<BlockArgument>(val)) {
    Block *argBlk = arg.getOwner();

    auto termOp = blk->getTerminator();
    if (auto branchOp = dyn_cast_or_null<cf::BranchOp>(termOp)) {
      if (argBlk == branchOp.getSuccessor()) {
        unsigned argIndex = arg.getArgNumber();
        liveOut.insert(branchOp.getOperand(argIndex));
        return;
      }
    } else if (auto branchOp =
                   dyn_cast_or_null<cgra::ConditionalBranchOp>(termOp)) {
      if (argBlk == branchOp.getSuccessor(0)) {
        unsigned argIndex = arg.getArgNumber();
        liveOut.insert(branchOp.getTrueOperand(argIndex));
        return;
      } else if (argBlk == branchOp.getSuccessor(1)) {
        unsigned argIndex = arg.getArgNumber();
        liveOut.insert(branchOp.getFalseOperand(argIndex));
        return;
      }
    }
  }

  liveOut.insert(val);
}

bool isCstZero(Operation *op) {
  auto cstOp = dyn_cast_or_null<arith::ConstantOp>(op);
  if (cstOp == nullptr)
    return false;

  auto attr = cstOp.getValue();
  if ((attr.isa<mlir::IntegerAttr>() &&
       attr.cast<mlir::IntegerAttr>().getValue().isZero()) ||
      (attr.isa<mlir::FloatAttr>() &&
       attr.cast<mlir::FloatAttr>().getValue().isZero()))
    return true;
  return false;
}

Value getRouteSrcOp(Operation *op) {
  // op = add srcOp, zero, get SrcOp value until the value is not in
  // spilledVals first check whether op match the pattern;

  if (isa<arith::AddIOp>(op) || isa<arith::AddFOp>(op)) {
    // the first operand is constant, return the op it self
    if (op->getOperand(0).getDefiningOp() &&
        isa<arith::ConstantOp>(op->getOperand(0).getDefiningOp()))
      return op->getResult(0);
    if (isCstZero(op->getOperand(1).getDefiningOp()))
      return op->getOperand(0);
  }

  return op->getResult(0);
}

Value getRootValue(Value val) {
  auto defOp = val.getDefiningOp();
  if (!defOp)
    return val;

  auto srcVal = getRouteSrcOp(defOp);
  if (srcVal == val)
    return val;
  // recursively get the root value
  return getRootValue(srcVal);
}

static SmallVector<Operation *>
getSpilledOps(Value val, SetVector<Value> spilledVals,
              std::map<int, SmallVector<Operation *>> spillOps) {
  SmallVector<Operation *> ops;
  auto rootVal = getRootValue(val);
  if (spilledVals.count(rootVal) > 0) {
    // find corresponding route op in spillOps
    auto index = std::distance(
        spilledVals.begin(),
        std::find_if(spilledVals.begin(), spilledVals.end(),
                     [&](const Value &v) { return v == rootVal; }));
    ops = spillOps[index];
  }
  return ops;
}

void computeLiveValue(Region &region,
                      std::map<Block *, SetVector<Value>> &liveIns,
                      std::map<Block *, SetVector<Value>> &liveOuts) {
  // compute def and use for each block
  std::map<Block *, SetVector<Value>> defMap;
  std::map<Block *, SetVector<Value>> useMap;

  for (auto &block : region) {
    SetVector<Value> def;
    SetVector<Value> use;
    // push all block arguments to use
    for (auto arg : block.getArguments()) {
      // the entry block argument is IN/OUT of the function
      if (!block.isEntryBlock())
        insertNonConst(arg, use);
    }

    for (auto &op : block.getOperations()) {
      if (isa<cgra::BlasGemmOp>(op)) {
        // skip BlasGemmOp, it directly interfaces with memory
        continue;
      }
      for (auto res : op.getResults())
        insertNonConst(res, def);

      for (auto opr : op.getOperands())
        // branch argument is not a use
        insertNonConst(opr, use);
    }
    defMap[&block] = def;
    useMap[&block] = use;
  }

  // calculate (use - def)
  std::map<Block *, SetVector<Value>> outBBUse;
  for (auto &block : region) {
    SetVector<Value> outUse;
    for (auto V : useMap[&block]) {
      if (!defMap[&block].count(V)) {
        outUse.insert(V);
      }
    }
    outBBUse[&block] = outUse;
  }

  // clear liveIn and liveOut
  liveIns.clear();
  liveOuts.clear();

  // compute liveIn and liveOut for each block
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &block : region) {
      SetVector<Value> liveIn = outBBUse[&block];
      SetVector<Value> liveOut = liveOuts[&block];

      // liveIn = outBBUse + (liveOut - def)
      for (auto val : liveOut)
        if (!defMap[&block].count(val))
          insertNonConst(val, liveIn);

      for (auto succ : block.getSuccessors()) {
        // add to succesor's liveOut
        for (auto val : liveIns[succ])
          updateLiveOutBySuccessorLiveIn(val, &block, liveOut);
      }
      if (liveIn != liveIns[&block] || liveOut != liveOuts[&block]) {
        liveIns[&block] = liveIn;
        liveOuts[&block] = liveOut;
        changed = true;
      }
    }
  }
}

static int getLwiAddress(cgra::LwiOp lwiOp) {
  // get the address value
  auto addrValue = lwiOp.getOperand();
  if (auto constOp = addrValue.getDefiningOp<arith::ConstantOp>()) {
    return constOp.getValue().cast<IntegerAttr>().getInt();
  }
  return -1;
}

static bool isAddrConstOp(arith::ConstantOp constOp) {
  for (auto &use : constOp->getUses()) {
    if (isa<cgra::LwiOp>(use.getOwner()))
      return true;
    // Only address can be used as Imm field of swi operation
    if (use.getOperandNumber() == 1 && isa<cgra::SwiOp>(use.getOwner()))
      return true;
  }

  return false;
}

void computeLiveValueWithLargeCst(
    Region &region, std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts) {
  // compute def and use for each block
  std::map<Block *, SetVector<Value>> defMap;
  std::map<Block *, SetVector<Value>> useMap;

  auto insertValsExceptValidCst = [](Value val, SetVector<Value> &vec) {
    // if val is produced by a constant operation and in the valid range [-4097,
    // 4096], not insert it to the set
    if (auto constOp =
            dyn_cast_or_null<arith::ConstantOp>(val.getDefiningOp())) {
      if (isAddrConstOp(constOp))
        return;
      int64_t value = 0;
      auto attr = constOp->getAttr("value");
      if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
        value = intAttr.getInt();
      } else if (auto floatAttr = attr.dyn_cast<FloatAttr>()) {
        value = static_cast<int64_t>(floatAttr.getValueAsDouble());
      }
      if (value >= -4097 && value <= 4096)
        return;
    }
    vec.insert(val);
  };

  for (auto &block : region) {
    SetVector<Value> def;
    SetVector<Value> use;
    // push all block arguments to use
    for (auto arg : block.getArguments()) {
      // the entry block argument is IN/OUT of the function
      if (!block.isEntryBlock())
        insertValsExceptValidCst(arg, use);
    }

    for (auto &op : block.getOperations()) {
      if (isa<cgra::BlasGemmOp>(op)) {
        // skip BlasGemmOp, it directly interfaces with memory
        continue;
      }
      for (auto res : op.getResults())
        insertValsExceptValidCst(res, def);

      for (auto opr : op.getOperands())
        // branch argument is not a use
        insertValsExceptValidCst(opr, use);
    }
    defMap[&block] = def;
    useMap[&block] = use;
  }

  // calculate (use - def)
  std::map<Block *, SetVector<Value>> outBBUse;
  for (auto &block : region) {
    SetVector<Value> outUse;
    for (auto V : useMap[&block]) {
      if (!defMap[&block].count(V)) {
        outUse.insert(V);
      }
    }
    outBBUse[&block] = outUse;
  }

  // clear liveIn and liveOut
  liveIns.clear();
  liveOuts.clear();

  // compute liveIn and liveOut for each block
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &block : region) {
      SetVector<Value> liveIn = outBBUse[&block];
      SetVector<Value> liveOut = liveOuts[&block];

      // liveIn = outBBUse + (liveOut - def)
      for (auto val : liveOut)
        if (!defMap[&block].count(val))
          insertValsExceptValidCst(val, liveIn);

      for (auto succ : block.getSuccessors()) {
        // add to succesor's liveOut
        for (auto val : liveIns[succ])
          updateLiveOutBySuccessorLiveIn(val, &block, liveOut);
      }
      if (liveIn != liveIns[&block] || liveOut != liveOuts[&block]) {
        liveIns[&block] = liveIn;
        liveOuts[&block] = liveOut;
        changed = true;
      }
    }
  }
}

// static SmallVector<Operation *, 4>
// getNextLayerOps(Block *block, SetVector<Value> &liveIn,
//                 std::set<Operation *> &visitedOps) {
//   SmallVector<Operation *, 4> schedulingOps;
//   auto termOp = block->getTerminator();

//   for (auto &op : block->getOperations()) {
//     if (isa<arith::ConstantOp>(op) || visitedOps.count(&op) ||
//         isTerminatorOp(&op)) {
//       continue;
//     }

//     // operation can be scheduled if all of its operands exist
//     bool canSchedule = true;
//     for (auto operand : op.getOperands()) {
//       // check whether the operand is a liveIn value or belongs to the
//       // scheduled operations
//       bool isLiveIn =
//           std::find(liveIn.begin(), liveIn.end(), operand) != liveIn.end();
//       bool producedByScheduledOp =
//           std::find(visitedOps.begin(), visitedOps.end(),
//                     operand.getDefiningOp()) != visitedOps.end();
//       bool isConstant = operand.getDefiningOp() &&
//                         isa<arith::ConstantOp>(operand.getDefiningOp());
//       if (!isLiveIn && !producedByScheduledOp && !isConstant) {
//         canSchedule = false;
//         break;
//       }
//     }

//     // if the operation can be scheduled, add it to the scheduled operations
//     bool loadCstAddr = isa<cgra::LwiOp>(op) &&
//                        op.getOperand(0).getDefiningOp() &&
//                        isa<arith::ConstantOp>(op.getOperand(0).getDefiningOp());
//     if (canSchedule || loadCstAddr)
//       schedulingOps.push_back(&op);
//   }
//   return schedulingOps;
// }

static bool isLive(Value val, Block *curBlk, SetVector<Value> liveOut,
                   SetVector<Operation *> scheduledOps) {
  // check whether the value is used by the scheduledOp
  if (liveOut.contains(val))
    return true;

  for (auto user : val.getUsers()) {
    if (user->getBlock() != curBlk)
      continue;
    if (!scheduledOps.contains(user)) {
      return true;
    }
  }
  return false;
}

static bool isLiveExcept(Value val, Block *curBlk, Operation *user,
                         SetVector<Value> liveOut,
                         SetVector<Operation *> scheduledOps) {
  // check whether the value is used by the scheduledOp
  scheduledOps.insert(user);
  return isLive(val, curBlk, liveOut, scheduledOps);
}

bool compigra::usedByBranch(OpOperand &use) {
  auto user = use.getOwner();
  return isa<cf::BranchOp>(user) ||
         (isa<cgra::ConditionalBranchOp>(user) && use.getOperandNumber() > 1);
}

static SmallVector<Operation *, 4>
getPreviousLayerOps(Block *block, SetVector<Value> &liveout,
                    SetVector<Operation *> &ancestors) {
  SmallVector<Operation *, 4> visitedOps;

  for (auto op : llvm::make_early_inc_range(ancestors)) {
    // swi op and terminator op can be scheduled at the last stage
    if (op->getResults().empty()) {
      visitedOps.push_back(op);
      continue;
    }

    // if the operation produce live-out or its produced value is not used by
    // the ancestors, add it to the scheduling operations
    auto result = op->getResult(0);
    // bool canSchedule =
    //     std::find(liveout.begin(), liveout.end(), result) != liveout.end() ||
    //     result.use_empty();
    bool canSchedule = true;

    // any users does not belong to the ancestors
    for (auto &use : result.getUses()) {
      auto user = use.getOwner();
      if (ancestors.count(user) && !usedByBranch(use)) {
        canSchedule = false;
        break;
      }
    }

    if (canSchedule) {
      visitedOps.push_back(op);
    }
  }
  return visitedOps;
}

static std::optional<Value> generateNewPhiVal(Operation *op,
                                              SetVector<Value> liveIn) {
  if (op->getNumResults() == 0)
    return std::nullopt;

  auto res = op->getResult(0);
  SetVector<Value> relatedVal;
  getAllPhiRelatedValues(res, relatedVal);
  for (auto val : relatedVal) {
    // if the value is in the liveIn set, return it
    if (liveIn.count(val) > 0) {
      return val;
    }
  }
  return std::nullopt;
}

static bool canGenerateNewPhiVal(Operation *op, SetVector<Value> liveIn,
                                 Block *block,
                                 std::set<Operation *> &visitedOps) {
  auto phiVal = generateNewPhiVal(op, liveIn);
  if (phiVal.has_value()) {
    auto arg = phiVal.value();
    // all users in the block of arg should be scheduled before the op
    bool hasNonScheduledUser = false;
    for (auto &use : arg.getUses()) {
      auto user = use.getOwner();
      if (user->getBlock() != block || usedByBranch(use) || user == op)
        continue;
      if (visitedOps.count(user) == 0) {
        hasNonScheduledUser = true;
        break;
      }
    }

    if (hasNonScheduledUser)
      return false;
  }
  return true;
}

static bool usedByBranchForNextCC(Operation *user, unsigned operandNum) {
  return isa<cf::BranchOp>(user) ||
         (isa<cgra::ConditionalBranchOp>(user) && operandNum > 1);
}

static SmallVector<Operation *, 4>
getScheduleOps(int nonCstOpSize, Block *block,
               SetVector<Operation *> scheduledOps,
               const SetVector<Value> &liveIn,
               const std::map<int, std::vector<Operation *>> &memoryAccessOps,
               ScheduleStrategy strategy = ScheduleStrategy::ASAP) {
  SmallVector<Operation *, 4> schedulingOps;
  auto termOp = block->getTerminator();

  for (auto &op : block->getOperations()) {
    if (isa<arith::ConstantOp>(op) || scheduledOps.count(&op)) {
      continue;
    }

    // if the operation can be scheduled, add it to the scheduled operations
    bool loadCstAddr = isa<cgra::LwiOp>(op) &&
                       op.getOperand(0).getDefiningOp() &&
                       isa<arith::ConstantOp>(op.getOperand(0).getDefiningOp());
    if (loadCstAddr) {
      auto addrPtr = getLwiAddress(dyn_cast<cgra::LwiOp>(op));
      bool scheduleLoad = true;
      if (memoryAccessOps.count(addrPtr) > 0) {
        auto seq = memoryAccessOps.at(addrPtr);
        auto it = std::find(seq.begin(), seq.end(), &op);
        for (auto seqIt = seq.begin(); seqIt != it; ++seqIt) {
          // if seqIt is SwiOp and not scheduled, op cannot be scheduled
          if (isa<cgra::SwiOp>(*seqIt) && !scheduledOps.contains(*seqIt)) {
            scheduleLoad = false;
            break;
          }
        }
      }
      if (scheduleLoad)
        schedulingOps.push_back(&op);
      continue;
    }

    // whether the new phi value can be generated
    std::set<Operation *> visitedOps{scheduledOps.begin(), scheduledOps.end()};
    if (!canGenerateNewPhiVal(&op, liveIn, block, visitedOps)) {
      continue;
    }

    // operation can be scheduled if all of its operands exist
    bool canSchedule = true;
    for (auto &operand : op.getOpOperands()) {
      // check whether the operand is a liveIn value or belongs to the
      // scheduled operations
      if (usedByBranchForNextCC(&op, operand.getOperandNumber()))
        continue;
      auto operandVal = operand.get();
      bool isLiveIn =
          std::find(liveIn.begin(), liveIn.end(), operandVal) != liveIn.end();
      bool producedByScheduledOp =
          std::find(scheduledOps.begin(), scheduledOps.end(),
                    operandVal.getDefiningOp()) != scheduledOps.end();
      bool isConstant = operandVal.getDefiningOp() &&
                        isa<arith::ConstantOp>(operandVal.getDefiningOp());
      if (!isLiveIn && !producedByScheduledOp && !isConstant) {
        canSchedule = false;
        break;
      }
    }

    if (canSchedule || loadCstAddr)
      schedulingOps.push_back(&op);
  }

  // if scheduledOps + schedulingOps not cover all operations, erase the termOp
  if (scheduledOps.size() + schedulingOps.size() < nonCstOpSize) {
    schedulingOps.erase(
        std::remove_if(schedulingOps.begin(), schedulingOps.end(),
                       [&](Operation *op) { return op == termOp; }),
        schedulingOps.end());
  }
  return schedulingOps;
}

/// Compute the schedule priority of the operations in the block. The earliest
/// schedule time is traversed through the block liveIn, and the latest
/// schedule time is traversed through the block liveOut. This function
/// returns a map of the operations and their schedule priority [earliest,
/// latest].
static std::map<Operation *, std::pair<int, int>> getSchedulePriority(
    int opSize, Block *block, SetVector<Value> &liveIn,
    SetVector<Value> &liveOut,
    const std::map<int, std::vector<Operation *>> &memoryAccessOps,
    bool DebugMode = false) {
  std::map<Operation *, std::pair<int, int>> schedulePriority;

  SetVector<Operation *> visitedOps;
  // the live-in height is zero
  int earliest = 0;
  // scheduledOp size = block->getOperations().size() - constantOp size - 1
  int totalOpSize = getNonCstOpSize(block) - 1;

  while (visitedOps.size() < totalOpSize && earliest < 100) {
    // get all operations that its operands are in the liveIn set or belong to
    // the scheduledOps
    earliest++;
    SmallVector<Operation *, 4> schedulingOps =
        getScheduleOps(opSize, block, visitedOps, liveIn, memoryAccessOps);

    if (schedulingOps.empty()) {
      // print non scheduled ops
      for (auto &op : block->getOperations()) {
        if (!isa<arith::ConstantOp>(op) && visitedOps.count(&op) == 0)
          llvm::errs() << op << "\n";
      }
      break;
    }
    for (auto op : schedulingOps) {
      schedulePriority[op] = {earliest, INT_MAX};
      visitedOps.insert(op);
    }
    // if (visitedOps.size() < totalOpSize)
    //   earliest++;
  }

  int latest = earliest;
  // accomandate the terminator op
  auto termOp = block->getTerminator();
  bool delayOneCC = false;
  // delay One clock cycle of the terminator execution if its first two operands
  // earliest should be the same as the last scheduled operation
  if (isa<cgra::ConditionalBranchOp>(termOp)) {
    delayOneCC =
        llvm::any_of(termOp->getOperands().take_front(2), [&](Value arg) {
          return arg.getDefiningOp() &&
                 schedulePriority.count(arg.getDefiningOp()) &&
                 schedulePriority[arg.getDefiningOp()].first == latest;
        });
  }
  latest += delayOneCC;
  schedulePriority[termOp] = {latest, latest};
  visitedOps.insert(termOp);

  while (!visitedOps.empty()) {
    // get all operations that its operands are in the liveOut set or belong
    // to the scheduledOps
    SmallVector<Operation *, 4> schedulingOps =
        getPreviousLayerOps(block, liveOut, visitedOps);

    for (auto op : schedulingOps) {
      schedulePriority[op].second = latest;
      visitedOps.remove(op);
    }
    latest--;
  }

  if (DebugMode) {
    std::string message;
    llvm::raw_string_ostream rso(message);
    rso << "Schedule Priority:\n";
    for (auto [op, priority] : schedulePriority) {
      rso << *op << " [" << priority.first << " " << priority.second << "]";
      rso << "\n";
    }
    logMessage(rso.str(), false, DebugMode);
  }

  return schedulePriority;
}

static bool readyToSchedule(Operation *op, Block *blk,
                            SetVector<Operation *> scheduledOps) {
  for (auto &opr : op->getOpOperands()) {
    if (usedByBranchForNextCC(op, opr.getOperandNumber()))
      continue;
    auto operand = opr.get();

    if (auto arg = dyn_cast_or_null<BlockArgument>(operand))
      continue;

    auto defOp = operand.getDefiningOp();
    // check whether the operand is a liveIn value or belongs to the
    // scheduled operations
    bool isConstant = isa<arith::ConstantOp>(defOp);
    if (isConstant)
      continue;

    // if the operand is from other block, it is considered as liveIn
    if (defOp->getBlock() != blk)
      continue;

    // check whether defOp is scheduled
    if (!scheduledOps.contains(defOp))
      return false;
  }
  return true;
}

int compigra::getDistance(int pe1, int pe2, int row, int col) {
  int x1 = pe1 / 4;
  int y1 = pe1 % 4;

  int x2 = pe2 / 4;
  int y2 = pe2 % 4;

  int dx = std::min(abs(x1 - x2), row - abs(x1 - x2));
  int dy = std::min(abs(y1 - y2), col - abs(y1 - y2));

  // dx = dx == 1 ? 0 : dx;
  // dy = dy == 1 ? 0 : dy;

  return dx + dy;
}

// Builds a child tree of operations starting from a given value.
// This function traverses the users of a given value (`val`) and constructs a
// tree of operations (`childTree`) that are reachable through the user chain
// within the same block (`blk`).
void compigra::buildChildTree(Value val, SetVector<Operation *> &childTree,
                              Block *blk) {
  std::queue<Operation *> userQueue;
  for (auto &use : val.getUses()) {
    auto user = use.getOwner();
    if (!usedByBranch(use))
      childTree.insert(user);

    if (user->getBlock() == blk)
      userQueue.push(user);
  }

  while (!userQueue.empty()) {
    auto currentUser = userQueue.front();
    userQueue.pop();

    for (auto &useChild : currentUser->getUses()) {
      auto userChild = useChild.getOwner();

      bool isInstantChild = !isa<cf::BranchOp>(userChild) &&
                            !(isa<cgra::ConditionalBranchOp>(userChild) &&
                              useChild.getOperandNumber() > 1);
      if (isInstantChild)
        childTree.insert(userChild);
      if (userChild->getBlock() == blk)
        userQueue.push(userChild);
    }
  }
}

// static double getSpatialAffinityCost(
//     Value val1, ScheduleUnit unit1, Value val2, ScheduleUnit unit2,
//     const std::map<Operation *, std::pair<int, int>> heightMap,
//     std::vector<ValuePlacement> &finiGraph, GridAttribute grid, Block *blk) {

//   auto pe1 = unit1.pe;
//   auto pe2 = unit2.pe;

//   int maxHeight = 0;
//   for (const auto &entry : heightMap)
//     maxHeight = std::max(maxHeight, entry.second.second);

//   SetVector<Operation *> childTree1;
//   SetVector<Operation *> childTree2;
//   buildChildTree(val1, childTree1, blk);

//   if (!childTree1.contains(val2.getDefiningOp()))
//     buildChildTree(val2, childTree2, blk);

//   auto children = getInterSection<Operation *>(childTree1, childTree2);

//   double affinity = 0;
//   double maxRouting = grid.nRow / 2 + grid.nCol / 2;

//   double bias = 0;
//   for (auto child : children) {
//     auto childHeight =
//         heightMap.count(child) ? heightMap.at(child).first : maxHeight + 1;
//     auto val1Height =
//         (val1.getDefiningOp() && heightMap.count(val1.getDefiningOp()))
//             ? heightMap.at(val1.getDefiningOp()).first
//             : 0;
//     auto val2Height =
//         (val1.getDefiningOp() && heightMap.count(val1.getDefiningOp()))
//             ? heightMap.at(val2.getDefiningOp()).first
//             : 0;

//     int depth1 = childHeight - val1Height;
//     int depth2 = childHeight - val2Height;
//     int depth = (depth1 + depth2) / 2;

//     affinity += std::pow(2, -depth);

//     // if child is liveout,the affinity must include the spatial distance to
//     the
//     // liveout space
//     auto liveOutIt = std::find_if(
//         finiGraph.begin(), finiGraph.end(), [&](const ValuePlacement &place)
//         {
//           return child->getNumResults() > 0 && place.val ==
//           child->getResult(0);
//         });
//     if (liveOutIt != finiGraph.end()) {
//       auto childPE = liveOutIt->pe;
//       // evaluate the pe1 and pe2's distance to the childPE
//       int childDistance1 = getDistance(pe1, childPE, grid.nRow, grid.nCol);
//       int childDistance2 = getDistance(pe2, childPE, grid.nRow, grid.nCol);
//       bias = std::pow(2, -depth) * (childDistance1 + childDistance2) / 2.0 /
//              maxRouting;
//     }
//   }

//   return affinity * getDistance(pe1, pe2, grid.nRow, grid.nCol) / maxRouting
//   +
//          bias;
// }

// get the torus routing PEs
static std::vector<unsigned>
getTorusRoutingPEs(unsigned pe, GridAttribute &attr, bool includeSelf = true) {
  std::vector<unsigned> routingPEs;
  if (includeSelf)
    routingPEs.push_back(pe);
  int nRow = attr.nRow;
  int nCol = attr.nCol;

  int row = pe / nCol;
  int col = pe % nCol;

  int left_col = (col - 1 + nCol) % nCol;
  int right_col = (col + 1) % nCol;
  int up_row = (row - 1 + nRow) % nRow;
  int bottom_row = (row + 1) % nRow;

  routingPEs.push_back(row * nCol + left_col);
  routingPEs.push_back(row * nCol + right_col);
  routingPEs.push_back(up_row * nCol + col);
  routingPEs.push_back(bottom_row * nCol + col);

  return routingPEs;
}

bool isOccupied(std::vector<ValuePlacement> curGraph,
                std::vector<ValuePlacement> finiGraph, unsigned pe) {
  // if find any curGraph that is occupied by the PE, return true
  for (const auto &place : curGraph) {
    bool taken = place.pe == pe && (place.regAttr == RegAttr::EX);
    auto val = place.val;
    // Check if the value exists in finiGraph with regAttr=EX|IE
    auto it = std::find_if(finiGraph.begin(), finiGraph.end(),
                           [&](const ValuePlacement &finiPlace) {
                             return finiPlace.val == val &&
                                    finiPlace.pe == pe &&
                                    finiPlace.regAttr == RegAttr::EX;
                           });

    return taken || it != finiGraph.end();
  }
  return false;
};

ValuePlacement getOccupiedValue(std::vector<ValuePlacement> curGraph,
                                std::vector<ValuePlacement> finiGraph,
                                SetVector<mlir::Operation *> scheduledOps,
                                Block *curBlk, unsigned pe) {
  for (auto place : curGraph) {
    bool taken = (place.pe == pe) && (place.regAttr == RegAttr::EX);
    // check whether the value is still in use
    bool inUse = false;
    for (auto user : place.val.getUsers()) {
      if (user->getBlock() != curBlk || isa<cf::BranchOp>(user) ||
          (isa<cgra::ConditionalBranchOp>(user) &&
           user->getOperand(0) != place.val &&
           user->getOperand(1) != place.val))
        continue;
      if (scheduledOps.count(user) == 0)
        inUse = true;
    }
    if (taken && inUse) {
      return place;
    }
    auto val = place.val;
    // Check if the value exists in finiGraph with regAttr=EX|IE
    auto it = std::find_if(finiGraph.begin(), finiGraph.end(),
                           [&](const ValuePlacement &finiPlace) {
                             return finiPlace.val == val &&
                                    finiPlace.pe == pe &&
                                    (finiPlace.regAttr == RegAttr::EX);
                           });
    if (it != finiGraph.end()) {
      return (*it);
    }
  }
  return ValuePlacement{};
}

// static double getAccessCost(
//     Block *curBlk, std::vector<ValuePlacement> curGraph,
//     std::map<Operation *, ScheduleUnit> scheduleResult,
//     SetVector<Operation *> scheduledOps, SetVector<Value> liveOut,
//     std::vector<ValuePlacement> finiGraph, GridAttribute &attr, size_t opNum,
//     const std::map<mlir::Operation *, std::pair<int, int>> schedulePriority,
//     int height = 0) {
//   double cost = 0;

//   for (auto i = 0; i < attr.nRow * attr.nCol; i++) {
//     // get the aggregation of the value placement in current PE
//     SmallVector<ValuePlacement> liveVals;
//     bool blockedPE = isOccupied(curGraph, finiGraph, i);
//     for (auto valPlace : curGraph) {
//       if (valPlace.pe != i || valPlace.regAttr != RegAttr::IN)
//         continue;
//       liveVals.push_back(valPlace);
//     }

//     for (auto val : liveVals) {
//       // get the non-scheduled users of the value
//       SetVector<Operation *> nonScheduledUsers;
//       for (auto &use : val.val.getUses()) {
//         auto user = use.getOwner();
//         if (user->getBlock() != curBlk || usedByBranch(use))
//           continue;
//         if (scheduledOps.count(user) == 0 && scheduleResult.count(user) == 0)
//           nonScheduledUsers.insert(user);
//       }

//       // the access cost of value in internal register is
//       2^(schedulePriority.) double accessCost = 0; for (auto user :
//       nonScheduledUsers) {
//         int scheduleHeight = (schedulePriority.at(user).first +
//                               schedulePriority.at(user).second) /
//                              2;
//         double coef = blockedPE ? 2 : 1;
//         // if the value is not co-consumed by other operations, the
//         coefficient
//         // is 0.1;
//         for (auto opr : user->getOperands()) {
//           if (opr.getDefiningOp() &&
//               isa<arith::ConstantOp>(opr.getDefiningOp())) {
//             coef = 0.1;
//           }
//         }
//         double userCost = std::pow(2, height - scheduleHeight);

//         accessCost += coef * userCost;
//       }
//       cost += accessCost;
//     }
//   }

//   return cost;
// }

// static double getSuccessCost(
//     int height, std::map<Operation *, ScheduleUnit> scheduleResult,
//     const std::map<Operation *, std::pair<int, int>> schedulePriority,
//     SmallVector<mlir::Operation *, 4> totalOps) {
//   double cost = 0;
//   double normRatio = 0;
//   for (auto op : totalOps) {
//     auto earliest = schedulePriority.at(op).first;
//     auto latest = schedulePriority.at(op).second;
//     double weight = std::pow(2, earliest - latest);
//     if (scheduleResult.count(op) != 0) {
//       cost += weight;
//     }
//     normRatio += weight;
//   }
//   double failAll = cost == 0 ? 1e2 : 0;
//   return normRatio == 0 ? failAll : failAll + (normRatio - cost) / normRatio;
// }

// SmallVector<Operation *, 4> getRouteOpStep1(Value val) {
//   SmallVector<Operation *, 4> routeOps;
//   for (auto user : val.getUsers()) {
//     if ((isa<arith::AddIOp>(user) || isa<arith::AddFOp>(user)) &&
//         user->getOperand(0) == val && user->getOperand(1).getDefiningOp() &&
//         isCstZero(user->getOperand(1).getDefiningOp())) {
//       routeOps.push_back(user);
//     }
//   }
//   return routeOps;
// }

// static double getLiveOutAffinityCost(Operation *op, ScheduleUnit opUnit,
//                                      std::vector<ValuePlacement> finiGraph,
//                                      GridAttribute grid, int height) {
//   double cost = 0;
//   if (op->getNumResults() == 0)
//     return cost;

//   // if the op has a consumer who only takes its value in the finiGraph,
//   // evaluate the affinity cost to the liveout
//   for (auto singleUser : op->getResult(0).getUsers()) {
//     if (singleUser->getNumResults() == 0)
//       continue;
//     // check whether the singleUser only consumes op's value
//     int producerCount = 1;
//     for (auto opr : singleUser->getOperands()) {
//       if (opr.getDefiningOp() == op)
//         continue;
//       if (isa<BlockArgument>(opr) ||
//           !isa<arith::ConstantOp>(opr.getDefiningOp())) {
//         producerCount++;
//         break;
//       }
//     }
//     if (producerCount > 1)
//       continue;

//     // evaluate the op's pe distance to the liveout PE
//     auto liveOutIt = std::find_if(
//         finiGraph.begin(), finiGraph.end(), [&](const ValuePlacement &place)
//         {
//           return place.val == singleUser->getResult(0);
//         });
//     if (liveOutIt == finiGraph.end())
//       continue;
//     // if the liveout is not in the finiGraph, return 0
//     int childDistance =
//         getDistance(opUnit.pe, liveOutIt->pe, grid.nRow, grid.nCol);
//     cost += 0.5 * childDistance / (grid.nRow / 2 + grid.nCol / 2);
//   }

//   // if op is a route liveout operation, evaluate its PE from the liveout
//   // requirement
//   // initialize a queue to store the route operations
//   std::queue<Operation *> routeQueue;
//   routeQueue.push(op);
//   int liveOutPE = -1;
//   while (!routeQueue.empty()) {
//     // search whether there are route operations to generate the liveout
//     auto curOp = routeQueue.front();
//     routeQueue.pop();
//     // check whether this is in liveout
//     auto liveOutIt = std::find_if(finiGraph.begin(), finiGraph.end(),
//                                   [&](const ValuePlacement &place) {
//                                     return place.val == curOp->getResult(0);
//                                   });
//     if (liveOutIt != finiGraph.end()) {
//       liveOutPE = liveOutIt->pe;
//       break;
//     }

//     auto mov = getRouteOpStep1(curOp->getResult(0));
//     // insert mov to the queue
//     for (auto m : mov)
//       routeQueue.push(m->getResult(0).getDefiningOp());
//   }

//   if (liveOutPE >= 0) {
//     auto childPE = opUnit.pe;
//     // evaluate the current PE distance to the liveout PE
//     int childDistance = getDistance(liveOutPE, childPE, grid.nRow,
//     grid.nCol); double affinity = std::pow(2, height - opUnit.time); double
//     maxRouting = grid.nRow / 2 + grid.nCol / 2; cost = affinity *
//     childDistance / maxRouting;
//   }
//   return cost;
// }

// static double getSpatialAffinityTotalCost(
//     Block *curBlk, std::map<Operation *, ScheduleUnit> scheduleResult,
//     std::vector<ValuePlacement> curGraph, std::vector<ValuePlacement>
//     finiGraph, std::map<Operation *, std::pair<int, int>> heightMap,
//     GridAttribute grid, SetVector<Value> liveOut, SetVector<Operation *>
//     scheduledOps, int height) {
//   double cost = 0;
//   int count = 0;
//   for (auto op : scheduleResult) {
//     if (op.first->getNumResults() == 0)
//       continue;

//     auto liveOutCost =
//         getLiveOutAffinityCost(op.first, op.second, finiGraph, grid, height);
//     if (liveOutCost > 0) {
//       cost += liveOutCost;
//       count++;
//     }

//     // check the affinity cost with livein values
//     for (auto place : curGraph) {
//       if (place.val == op.first->getResult(0) ||
//           !isLive(place.val, curBlk, liveOut, scheduledOps))
//         continue;

//       auto affCost = getSpatialAffinityCost(
//           place.val, {0, (int)place.pe, -1}, op.first->getResult(0),
//           op.second, heightMap, finiGraph, grid, op.first->getBlock());
//       if (affCost > 0)
//         count++;

//       cost += affCost;
//     }
//   }

//   return count == 0 ? 0 : cost / count;
// }

static SetVector<unsigned> getAvailablePEs(std::map<int, PERegUse> &freeReg,
                                           RegAttr reg) {
  SetVector<unsigned> peList;
  for (auto [pe, use] : freeReg) {
    if (reg == RegAttr::IE && use.inNum > 0 && use.exAvail) {
      peList.insert(pe);
      continue;
    }
    if (reg == RegAttr::IN && use.inNum > 0) {
      peList.insert(pe);
      continue;
    }
    if (reg == RegAttr::EX && use.exAvail) {
      peList.insert(pe);
      continue;
    }
  }
  return peList;
}

static std::map<int, PERegUse>
getResourceGraph(std::vector<ValuePlacement> curGraph, GridAttribute &attr) {
  // get available slots
  std::map<int, PERegUse> freeReg;
  for (int i = 0; i < attr.nRow * attr.nCol; i++)
    freeReg[i] = {(int)attr.maxReg, true};

  // remove resources occupied by values in initGraph
  for (auto place : curGraph) {
    auto pe = place.pe;
    auto reg = place.regAttr;
    // check whether
    // if regAttr is IN, remove the resource from the freeReg
    if (reg == RegAttr::IN || reg == RegAttr::IE)
      freeReg[pe].inNum--;
    // if regAttr is EX, remove the resource from the freeReg
    if (reg == RegAttr::EX || reg == RegAttr::IE)
      freeReg[pe].exAvail = false;
  }
  return freeReg;
}

void BasicBlockOpAssignment::updateEmbeddingGraph(
    std::vector<ValuePlacement> &curGraph,
    SetVector<mlir::Operation *> tmpScheduledOps,
    std::map<mlir::Operation *, std::pair<unsigned, compigra::RegAttr>>
        tmpResult,
    SetVector<Value> &liveout) {
  SetVector<Operation *> totalScheduledOp = scheduledOps;
  totalScheduledOp.insert(tmpScheduledOps.begin(), tmpScheduledOps.end());
  // op takes place of pe, invalidate the Rout of the pe
  for (auto op : tmpScheduledOps) {
    if (op->getNumResults() == 0)
      continue;

    auto pe = tmpResult[op].first;
    RegAttr regAttr = tmpResult[op].second;

    for (auto it = curGraph.begin(); it != curGraph.end();) {
      if (it->pe != pe) {
        ++it;
        continue;
      }

      if (it->regAttr == RegAttr::EX) {
        // remove it from the curGraph
        it = curGraph.erase(it);
        continue;
      }

      if (it->regAttr == RegAttr::IE) {
        it->regAttr = RegAttr::IN;
      }
      ++it;
    }
    // add operation to curGraph
    if (op->getNumResults() > 0)
      curGraph.push_back({op->getResult(0), pe, regAttr});
  }

  // update internal register
  for (auto p = 0; p < attr.nCol * attr.nRow; p++) {
    for (auto it = curGraph.begin(); it != curGraph.end();) {
      if (it->pe != p || it->regAttr != RegAttr::IN) {
        ++it;
        continue;
      }
      // check whether the value should be kept anymore
      auto val = it->val;
      if (!isLive(val, curBlock, liveout, totalScheduledOp)) {
        it = curGraph.erase(it);
      } else {
        ++it;
      }
    }
  }
}

static std::map<int, PERegUse> getAvailableResourceGraph(
    const std::vector<ValuePlacement> curGraph, GridAttribute &attr,
    Operation *scheduleOp, SetVector<Value> liveout,
    const std::vector<ValuePlacement> finiGraph,
    std::map<Operation *, std::pair<unsigned, RegAttr>> tmpResult = {},
    SetVector<Operation *> scheduledOps = {}) {
  // get available slots
  SetVector<Operation *> layerScheduledOps;
  std::vector<bool> used(attr.nRow * attr.nCol, false);
  for (auto [op, unit] : tmpResult) {
    layerScheduledOps.insert(op);
    used[unit.first] = true;
  }

  // remove resources occupied by liveout values
  for (auto valP : curGraph) {
    auto val = valP.val;
    auto regAttr = valP.regAttr;
    // if find val should be external live in finiGraph
    if (std::find_if(finiGraph.begin(), finiGraph.end(),
                     [&](const ValuePlacement &place) {
                       return place.val == val &&
                              (place.regAttr == RegAttr::EX);
                     }) != finiGraph.end()) {
      used[valP.pe] = true;
    }
  }

  std::map<int, PERegUse> freeReg;
  for (int i = 0; i < attr.nRow * attr.nCol; i++)
    freeReg[i] = {(int)attr.maxReg, !used[i]};

  // remove resources occupied by values in initGraph
  for (auto place : curGraph) {
    auto pe = place.pe;
    auto reg = place.regAttr;
    // if the place.val is not live, it does not occupy the resource
    bool deadVal =
        !isLive(place.val, scheduleOp->getBlock(), liveout, scheduledOps);
    if (deadVal)
      continue;

    SetVector<Operation *> totalScheduledOp = scheduledOps;
    totalScheduledOp.insert(layerScheduledOps.begin(), layerScheduledOps.end());
    bool avail = isLiveExcept(place.val, scheduleOp->getBlock(), scheduleOp,
                              liveout, totalScheduledOp);
    // if regAttr is IN, remove the resource from the freeReg
    if (reg == RegAttr::IN || reg == RegAttr::IE)
      if (!avail)
        freeReg[pe].inNum--;
    // if regAttr is EX, remove the resource from the freeReg
    if (reg == RegAttr::EX) {
      // schedule op does not affect current Rout
      if (scheduleOp->getNumResults() == 0)
        continue;
      freeReg[pe].exAvail &= avail;
    }
  }
  return freeReg;
}

static void printResourceGraph(std::map<int, PERegUse> freeReg,
                               GridAttribute attr) {
  // print nRow and nCol
  std::string message;
  for (int i = 0; i < attr.nCol; i++) {
    for (int j = 0; j < attr.nRow; j++) {
      auto pe = i * attr.nRow + j;
      auto availPE = freeReg[pe].exAvail;
      auto availRegs = freeReg[pe].inNum;
      if (availPE)
        message += "[? | " + std::to_string(availRegs) + "] ";
      else
        message += "[X | " + std::to_string(availRegs) + "] ";
    }
    message += "\n";
  }
  logMessage(message);
}

static ValuePlacement
getInitialLiveInPlacement(Value in, Block *block,
                          std::vector<ValuePlacement> &initGraph,
                          const std::map<Block *, SetVector<Value>> liveIns,
                          const std::map<Block *, SetVector<Value>> liveOuts,
                          GridAttribute &attr, bool debug = false) {
  // if the value has more than 3 users, it should be external register
  int userNum = 0;
  for (auto user : in.getUsers())
    // if (user->getBlock() == block)
    userNum++;

  // stored in external if the value has more than 3 users or is live in a
  // limited number of blocks
  bool isExternal =
      userNum > 3 || getValueLiveLength(in, liveIns, liveOuts) < 3;
  // if the value exists in a long path, it should be internal register
  bool isInternal =
      !isExternal || getValueLiveLength(in, liveIns, liveOuts) >= 3;

  RegAttr regAttr;
  if (isExternal && isInternal)
    regAttr = RegAttr::IE;
  else if (isExternal)
    regAttr = RegAttr::EX;
  else
    regAttr = RegAttr::IN;
  // get available slots
  std::map<int, PERegUse> freeReg = getResourceGraph(initGraph, attr);
  auto peList = getAvailablePEs(freeReg, regAttr);

  // pe is randomly assigned from the available PE list
  unsigned pe = peList[rand() % peList.size()];
  // initGraph.push_back({in, pe, regAttr});
  // update the freeReg
  if (regAttr == RegAttr::IN || regAttr == RegAttr::IE)
    freeReg[pe].inNum--;
  if (regAttr == RegAttr::EX || regAttr == RegAttr::IE)
    freeReg[pe].exAvail = false;

  if (debug) {
    std::string inStr;
    llvm::raw_string_ostream rso(inStr);
    rso << in;

    logMessage("INIT GRAPH: " + rso.str() + " " + std::to_string(pe) + " " +
               std::to_string(static_cast<int>(regAttr)));
  }
  return ValuePlacement{in, pe, regAttr};
}

void BasicBlockOpAssignment::initEmbeddingGraphWithLiveIn(
    std::map<Block *, SetVector<Value>> liveIns,
    std::map<Block *, SetVector<Value>> liveOuts,
    std::vector<ValuePlacement> &initGraph, OpBuilder &builder,
    GridAttribute attr) {

  auto liveIn = liveIns[curBlock];
  for (auto val : liveIn) {
    ValuePlacement place;
    auto it = std::find_if(initGraph.begin(), initGraph.end(),
                           [&](ValuePlacement p) { return p.val == val; });
    // randomly assign the PE to the liveIn value
    if (it == initGraph.end()) {
      place = getInitialLiveInPlacement(val, curBlock, initGraph, liveIns,
                                        liveOuts, attr, DebugMode);
      initGraph.push_back(place);
    } else {
      place = *it;
    }

    // if find in the internal register in the liveIn graph, pop it out
    int userCount = 0;
    for (auto user : val.getUsers())
      if (user->getBlock() == curBlock)
        userCount++;
    if (userCount >= 3 && place.regAttr == RegAttr::IN) {
      // create pop out operation
      builder.setInsertionPoint(&curBlock->getOperations().front());
      Operation *movOp = createAtomicMovOp(val, true, true);
    }
  }
  // if val not in initGraph, assign the liveIn value with the lowest cost
}

LogicalResult BasicBlockOpAssignment::adaptWithFinalPlacement(
    OpBuilder &builder, ValuePlacement targetPlace, ValuePlacement curPlace,
    SmallVector<ValuePlacement> existVals, SetVector<Value> liveout,
    unsigned maxRegNum, std::map<Operation *, ScheduleUnit> &scheduleResult) {
  if (targetPlace.val != curPlace.val || targetPlace.pe != curPlace.pe)
    return failure();
  if (targetPlace.regAttr == curPlace.regAttr ||
      curPlace.regAttr == RegAttr::IE)
    return success();

  auto curPE = curPlace.pe;
  if (curPlace.regAttr == RegAttr::EX) {
    // check whether enough registers to store the existVals
    // count how many existVals are in liveout
    int liveoutCount = 0;
    for (auto val : existVals) {
      if (liveout.count(val.val) > 0)
        liveoutCount++;
    }
    if (liveoutCount >= maxRegNum)
      return failure();

    return success();
  }

  if (curPlace.regAttr == RegAttr::IN) {
    // find the last scheduled operation
    Operation *lastProducer;
    int lastOpTime = 0;
    int bbEndTime = scheduleResult[curBlock->getTerminator()].time;
    DenseSet<int> occupiedTime;
    for (auto [op, sol] : scheduleResult) {
      if (sol.pe != curPE)
        continue;
      occupiedTime.insert(sol.time);
      if (op->getNumResults() > 0 && sol.time > lastOpTime) {
        lastOpTime = sol.time;
        lastProducer = op;
      }
    }

    int popTime = 1;
    if (lastProducer) {
      if (lastProducer && scheduleResult.count(lastProducer)) {
        // insert when the consumer in this block consumes this op
        for (auto &use : lastProducer->getResult(0).getUses()) {
          auto user = use.getOwner();
          llvm::errs() << "CHECK POP Use: " << *user << "\n";
          // if not current block user, skip
          if (user->getBlock() != curBlock || usedByBranch(use))
            continue;

          // if used inside current pe, skip as internal register can be used
          if (scheduleResult.count(user) &&
              scheduleResult.at(user).pe == curPE) {
            continue;
          }

          popTime = std::max(popTime, scheduleResult.at(user).time);
          logMessage("outside PE user : " + std::to_string(popTime), false,
                     DebugMode);
        }
      }
    }

    int findSlot = -1;
    popTime = std::max(popTime, lastOpTime + 1);
    for (auto i = popTime; i <= bbEndTime; i++) {
      if (occupiedTime.count(i) == 0) {
        findSlot = i;
        popTime = i;
        break;
      }
    }
    if (findSlot == -1) {
      // make the original bbEndTime as the popTime
      popTime = bbEndTime;
      Operation *delayOp = nullptr;
      for (auto [op, sol] : scheduleResult) {
        if (sol.pe == curPE && sol.time == bbEndTime)
          delayOp = op;
      }
      // the original bbEndTime operation execution time+1;
      scheduleResult[delayOp].time = bbEndTime + 1;
      scheduleResult[curBlock->getTerminator()].time = bbEndTime + 1;
    }
    logMessage("Pop time: " + std::to_string(popTime) + " " +
                   std::to_string(bbEndTime),
               false, DebugMode);
    if (lastProducer) {
      builder.setInsertionPoint(lastProducer);
    } else {
      builder.setInsertionPoint(
          &curPlace.val.getParentBlock()->getOperations().front());
    }
    // pop the curPlace.val at popTime
    Operation *popOp = createAtomicMovOp(curPlace.val, false, false);
    int reg = targetPlace.regAttr == RegAttr::EX ? maxRegNum : -1;
    scheduleResult[popOp] = {popTime, (int)curPlace.pe, reg};
    // curPlace.val.replaceUsesWithIf(popOp->getResult(0), [&](OpOperand &use) {
    //   auto owner = use.getOwner();
    //   return owner->getBlock() != curBlock ||
    //          (owner->getBlock() == curBlock &&
    //           (isa<cf::BranchOp>(owner) ||
    //            (isa<cgra::ConditionalBranchOp>(owner) &&
    //             use.getOperandNumber() > 1)));
    // });
    if (DebugMode) {
      std::string message;
      llvm::raw_string_ostream rso(message);
      rso << "Pop out " << curPlace.val << " at time " << popTime << " in PE "
          << curPlace.pe << " with " << *popOp << "\n";
      logMessage(rso.str());
    }
  }

  return success();
}

LogicalResult BasicBlockOpAssignment::finalizeEmbeddingGraphWithLiveOut(
    std::vector<ValuePlacement> &finiGraph,
    std::vector<ValuePlacement> &endScheduleGraph) {
  for (auto place : endScheduleGraph) {
    // if the value is not in the finiGraph, add it to the finiGraph
    auto pe = place.pe;
    auto it =
        std::find_if(finiGraph.begin(), finiGraph.end(),
                     [&](ValuePlacement p) { return p.val == place.val; });
    if (it == finiGraph.end()) {
      // change the regAttr to IN
      auto newPlace = place;
      // if in finiGraph, there is already EX or IE placement, change the
      // regAttr to IN
      for (auto p : finiGraph) {
        if (p.pe == pe &&
            (p.regAttr == RegAttr::EX || p.regAttr == RegAttr::IE)) {
          newPlace.regAttr = RegAttr::IN;
          break;
        }
      }
      // newPlace.regAttr = RegAttr::IN;
      finiGraph.push_back(newPlace);
    } else {
      // endScheduleGraph must match with finiGraph
      SmallVector<ValuePlacement> existVals;
      for (auto p : endScheduleGraph) {
        if (p.pe == pe && p.val != place.val)
          existVals.push_back(p);
      }
      // adapt the targetPlace with the existVals
      if (failed(adaptWithFinalPlacement(builder, *it, place, existVals,
                                         liveout, attr.maxReg, solution)))
        return failure();
    }
  }
  return success();
}

static void removeElement(SetVector<unsigned> &vec, unsigned pe) {
  auto it = std::find(vec.begin(), vec.end(), pe);
  if (it != vec.end())
    vec.erase(it);
}

ValuePlacement getSrcValuePlacement(Value src,
                                    std::vector<ValuePlacement> curGraph) {
  auto it = std::find_if(curGraph.begin(), curGraph.end(),
                         [&](ValuePlacement p) { return p.val == src; });

  if (it != curGraph.end())
    return *it;
  return {nullptr, UINT_MAX, RegAttr::NK};
}

void getOperandPlacement(Value opr, Operation *scheduleOp,
                         std::vector<compigra::ValuePlacement> &curGraph,
                         std::vector<Value> &routeVec,
                         std::vector<SetVector<unsigned>> &placementVec,
                         GridAttribute attr, SetVector<Value> spilledVals,
                         std::map<int, SmallVector<Operation *>> spillOps) {
  // SetVector<unsigned> fullPESet;
  // for (auto i = 0; i < attr.nRow * attr.nCol; i++)
  //   fullPESet.insert(i);

  // first get original value and its placement
  auto oprPlace = getSrcValuePlacement(opr, curGraph);
  if (oprPlace.val == nullptr) {
    // If the oprPlace is a constant, there is no placement limitation
    // routeVec.push_back(oprPlace.val);
    // placementVec.push_back(fullPESet);
    std::string valueStr;
    llvm::raw_string_ostream rso(valueStr);
    rso << "ERROR: " << opr << " not found in scheduled result\n";
    if (!opr.getDefiningOp() || !isa<arith::ConstantOp>(opr.getDefiningOp()))
      logMessage(rso.str());
  } else {
    routeVec.push_back(oprPlace.val);
    SetVector<unsigned> routingPEs;
    auto pe = oprPlace.pe;
    auto regAttr = oprPlace.regAttr;
    if (regAttr == RegAttr::IN)
      routingPEs.insert(pe);
    else if (regAttr == RegAttr::EX || regAttr == RegAttr::IE) {
      bool selfRoute =
          isRouteOp(scheduleOp) && isRouteOp(oprPlace.val.getDefiningOp());
      for (auto routingPE : getTorusRoutingPEs(pe, attr, !selfRoute))
        routingPEs.insert(routingPE);
    }
    placementVec.push_back(routingPEs);
  }

  auto rootVal = getRootValue(opr);
  if (spilledVals.count(rootVal) > 0) {
    // find corresponding route op in spillOps
    // auto index = std::distance(
    //     spilledVals.begin(),
    //     std::find_if(spilledVals.begin(), spilledVals.end(),
    //                  [&](const Value &v) { return v == rootVal; }));
    auto spillOpSet = getSpilledOps(opr, spilledVals, spillOps);
    for (auto op : spillOpSet) {
      // seek op->getResult(0) in the current graph
      auto spillOpResult = op->getResult(0);
      auto spillOpPlace = getSrcValuePlacement(spillOpResult, curGraph);
      if (spillOpPlace.val == nullptr)
        continue;

      // check whether the routing PE provide accessibility
      SetVector<unsigned> routingPEs;
      auto pe = spillOpPlace.pe;
      auto regAttr = spillOpPlace.regAttr;
      if (regAttr == RegAttr::IN)
        routingPEs.insert(pe);
      else if (regAttr == RegAttr::EX || regAttr == RegAttr::IE) {
        bool selfRoute = isRouteOp(scheduleOp) &&
                         isRouteOp(spillOpPlace.val.getDefiningOp());
        for (auto routingPE : getTorusRoutingPEs(pe, attr, !selfRoute))
          routingPEs.insert(routingPE);
      }
      routeVec.push_back(spillOpPlace.val);
      placementVec.push_back(routingPEs);
    }
  }
};

/// Get the valid placement space of the operation.
std::vector<placeunit> BasicBlockOpAssignment::searchOpPlacementSpace(
    Operation *scheduleOp, std::vector<ValuePlacement> &curGraph,
    const std::vector<ValuePlacement> &finiGraph,
    std::map<Operation *, std::pair<unsigned, RegAttr>> tmpResult) {
  int nRow = attr.nRow;
  int nCol = attr.nCol;
  std::vector<std::pair<int, RegAttr>> placementSpace;

  // get the available hardware resources
  auto regUse = getAvailableResourceGraph(curGraph, attr, scheduleOp, liveout,
                                          finiGraph, tmpResult, scheduledOps);
  // printResourceGraph(regUse, attr);

  SetVector<unsigned> availablePEs;
  for (int i = 0; i < nRow * nCol; i++) {
    if (!regUse[i].exAvail)
      continue;

    availablePEs.insert(i);
    if (regUse[i].inNum > 0)
      placementSpace.push_back({i, RegAttr::IE});
    if (regUse[i].inNum == 0)
      placementSpace.push_back({i, RegAttr::EX});
  }

  // limit the placement space according to its consumer and producer
  std::vector<Value> expandOp0;
  std::vector<SetVector<unsigned>> spillOp0Ranges;

  std::vector<Value> expandOp1;
  std::vector<SetVector<unsigned>> spillOp1Ranges;

  std::vector<Value> optOp;
  std::vector<SetVector<unsigned>> spillOptRanges;

  for (auto &opVal : scheduleOp->getOpOperands()) {
    if (usedByBranch(opVal))
      break;

    auto opr = opVal.get();
    auto oprInd = opVal.getOperandNumber();
    switch (oprInd) {
    case 0:
      getOperandPlacement(opr, scheduleOp, curGraph, expandOp0, spillOp0Ranges,
                          attr, spilledVals, spillOps);
      break;
    case 1:
      getOperandPlacement(opr, scheduleOp, curGraph, expandOp1, spillOp1Ranges,
                          attr, spilledVals, spillOps);
      break;
    case 2:
      getOperandPlacement(opr, scheduleOp, curGraph, optOp, spillOptRanges,
                          attr, spilledVals, spillOps);
      break;
    default:
      break;
    }
  }

  bool existOpr0 = !expandOp0.empty();
  bool existOpr1 = !expandOp1.empty();

  // if the operands are constant, no need to limit the placement space
  bool findInterSect = !existOpr0 && !existOpr1;
  // if expandOp0 is empty, but expandOp1 is not empty, swap them
  if (!existOpr0 && existOpr1) {
    std::swap(expandOp0, expandOp1);
    std::swap(spillOp0Ranges, spillOp1Ranges);
    // swap the existOpr0 and existOpr1
    existOpr0 = true;
    existOpr1 = false;
  }

  // get the spillOp0Ranges and spillOp1Ranges intersection
  for (auto i = 0; i < spillOp0Ranges.size(); i++) {
    auto availPEOpr0 = spillOp0Ranges[i];
    auto schedulePEs = getSubSet<unsigned>(availablePEs, availPEOpr0);
    findInterSect = !existOpr1;
    for (auto j = 0; j < spillOp1Ranges.size(); j++) {
      auto availPEOpr1 = spillOp1Ranges[j];
      auto schedulePEs1 = getInterSection(schedulePEs, availPEOpr1);
      if (!schedulePEs1.empty()) {
        findInterSect = true;
        schedulePEs = schedulePEs1;
        if (j != 0)
          replaceValMap[scheduleOp][1] = {expandOp1[0], expandOp1[j]};
        // scheduleOp->replaceUsesOfWith(expandOp1[0], expandOp1[j]);
        break;
      }
    }
    if (findInterSect) {
      availablePEs = schedulePEs;
      if (i != 0) {
        replaceValMap[scheduleOp][0] = {expandOp0[0], expandOp0[i]};
        // scheduleOp->replaceUsesOfWith(expandOp0[0], expandOp0[i]);
      }
      break;
    }
  }

  if (!findInterSect) {
    return {};
  }

  // check the optOp
  if (!optOp.empty()) {
    findInterSect = false;
    for (auto i = 0; i < spillOptRanges.size(); i++) {
      auto availPEOpt = spillOptRanges[i];
      auto schedulePEs = getSubSet<unsigned>(availablePEs, availPEOpt);
      if (!schedulePEs.empty()) {
        if (i != 0)
          replaceValMap[scheduleOp][2] = {optOp[0], optOp[i]};
        // scheduleOp->replaceUsesOfWith(optOp[0], optOp[i]);
        availablePEs = schedulePEs;
        findInterSect = true;
        break;
      }
    }
    if (!findInterSect)
      return {};
  }

  // Filter placementSpace to keep only PEs that are in routing scope
  placementSpace.erase(
      std::remove_if(placementSpace.begin(), placementSpace.end(),
                     [&](const std::pair<unsigned, RegAttr> &p) {
                       return availablePEs.count(p.first) == 0;
                     }),
      placementSpace.end());
  if (placementSpace.empty())
    return placementSpace;

  // if the operation result is restricted by the finiGraph, it is restricted
  // to the PE that is used by the finiGraph
  for (auto res : scheduleOp->getResults()) {
    auto it = std::find_if(finiGraph.begin(), finiGraph.end(),
                           [&](ValuePlacement p) { return p.val == res; });
    if (it != finiGraph.end()) {
      auto pe = it->pe;
      SetVector<unsigned> tempSet;
      tempSet.insert(it->pe);
      availablePEs = getSubSet<unsigned>(availablePEs, tempSet);

      if (availablePEs.empty())
        return {};
    }
  }
  placementSpace.erase(
      std::remove_if(placementSpace.begin(), placementSpace.end(),
                     [&](const std::pair<unsigned, RegAttr> &p) {
                       return availablePEs.count(p.first) == 0;
                     }),
      placementSpace.end());

  return placementSpace;
}

static std::pair<int, compigra::RegAttr>
getSample(std::vector<std::pair<int, compigra::RegAttr>> opSpace,
          bool allowNull = true) {
  std::pair<int, compigra::RegAttr> assignPE;
  double randomValue = static_cast<double>(rand()) / RAND_MAX;
  if (randomValue < 0.2 && allowNull) {
    assignPE = {-1, RegAttr::NK};
  } else {
    assignPE = *(opSpace.begin() + rand() % opSpace.size());
  }
  return assignPE;
}

int BasicBlockOpAssignment::placeOperations(
    int timeSlot, SmallVector<Operation *, 4> &schedulingOps,
    std::map<Operation *, ScheduleUnit> &scheduleResult,
    std::vector<ValuePlacement> &curGraph,
    std::map<Operation *, std::vector<placeunit>> &space,
    std::vector<ValuePlacement> &finiGraph, int shuffleOpIdx) {
  unsigned suc = 0;
  std::map<Operation *, std::pair<unsigned, RegAttr>> tmpResult;
  SetVector<Operation *> tmpScheduledOps;

  for (auto [idx, op] : llvm::enumerate(schedulingOps)) {
    std::pair<int, compigra::RegAttr> assignPE;
    // simulated annealing to get a random placement
    if ((int)idx <= shuffleOpIdx) {
      if (space.count(op) == 0)
        continue;

      auto opSpace = space.at(op);
      if (idx == shuffleOpIdx) {
        assignPE = getSample(opSpace);
        if (assignPE.first == -1) {
          // remove result from the scheduleResult
          scheduleResult.erase(op);
          continue;
        }
      } else {
        if (!scheduleResult.count(op))
          continue;

        assignPE = (scheduleResult.at(op).reg == attr.maxReg)
                       ? std::pair{scheduleResult.at(op).pe, RegAttr::EX}
                       : std::pair{scheduleResult.at(op).pe, RegAttr::IE};
      }
    } else {
      // search new placement space
      auto placementSpace =
          searchOpPlacementSpace(op, curGraph, finiGraph, tmpResult);
      // randomly choose a placement space
      if (placementSpace.empty())
        continue;

      // insert a non-scheduled decision
      space[op] = placementSpace;
      assignPE = getSample(placementSpace);
      if (assignPE.first == -1)
        continue;
    }

    suc++;
    tmpScheduledOps.insert(op);
    tmpResult[op] = assignPE;

    if (DebugMode) {
      std::string message;
      llvm::raw_string_ostream rso(message);
      rso << *op << "-> PE: " << assignPE.first
          << " RegAttr: " << static_cast<int>(assignPE.second) << "\n";
      logMessage(rso.str());
    }
  }

  for (auto op : tmpScheduledOps) {
    auto pe = tmpResult[op].first;
    RegAttr regAttr = tmpResult[op].second;
    SetVector<Operation *> totalScheduledOp = scheduledOps;
    totalScheduledOp.insert(tmpScheduledOps.begin(), tmpScheduledOps.end());
  }
  updateEmbeddingGraph(curGraph, tmpScheduledOps, tmpResult, liveout);

  // write tmpScheduleResult to scheduleResult
  for (auto [op, unit] : tmpResult) {
    if (unit.second == RegAttr::EX)
      scheduleResult[op] = {timeSlot, (int)unit.first, (int)attr.maxReg};
    else
      scheduleResult[op] = {timeSlot, (int)unit.first, -1};
  }
  return suc;
}

int shuffleSearchSpace(
    std::map<Operation *, std::vector<placeunit>> &searchSpace,
    std::map<Operation *, ScheduleUnit> &scheduleResult,
    SmallVector<mlir::Operation *, 4> schedulingOps) {
  // randomly choose an operation where its searchSpace has multiple elements
  std::vector<int> shuffleIds;
  // for (int i = 0; i < schedulingOps.size(); i++) {
  //   auto it = searchSpace.find(schedulingOps[i]);
  //   if (it != searchSpace.end() && it->second.size() > 1)
  //     shuffleIds.push_back(i);
  // }
  // // if no operation can be shuffled, return nullptr
  // if (shuffleIds.empty())
  //   return -1;
  // unsigned shuffleOpIdx = shuffleIds[rand() % shuffleIds.size()];

  unsigned shuffleOpIdx = rand() % schedulingOps.size();

  // keep all elements before the shuffleOpIdx in the searchSpace, and remove
  // others
  for (auto ind = shuffleOpIdx + 1; ind < schedulingOps.size(); ind++) {
    auto it = searchSpace.find(schedulingOps[ind]);
    if (it != searchSpace.end())
      searchSpace.erase(it);

    auto it2 = scheduleResult.find(schedulingOps[ind]);
    if (it2 != scheduleResult.end())
      scheduleResult.erase(it2);
  }
  // logMessage("shuffleOpIdx:" + std::to_string(shuffleOpIdx));
  return shuffleOpIdx;
}

static void sortProducersByWeight(std::vector<ValuePlacement> &producers,
                                  const std::vector<unsigned> &prodTime,
                                  const std::vector<unsigned> &weight) {
  std::vector<size_t> indices(producers.size());
  for (size_t i = 0; i < indices.size(); ++i)
    indices[i] = i;

  // Sort indices: first by prodTime (ascending), then by weight (descending)
  std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
    if (prodTime[a] != prodTime[b])
      return prodTime[a] < prodTime[b];
    return weight[a] > weight[b];
  });

  std::vector<ValuePlacement> sortedProducers;
  sortedProducers.reserve(producers.size());
  for (size_t i : indices)
    sortedProducers.push_back(producers[i]);

  producers = std::move(sortedProducers);
}

/// Returns:
/// - -2 if a producer is missing in the current graph.
/// - -1 if there are blocked PEs.
/// - 0 route producers to create a routing path.
/// - 1 The operation can access all producers without routing, but it needs to
/// be routed to match the final placement.
int BasicBlockOpAssignment::createRoutePath(
    Operation *failOp, std::vector<ValuePlacement> &producers,
    std::vector<unsigned> &movs, SetVector<unsigned int> &intersection,
    const std::map<mlir::Operation *, compigra::ScheduleUnit> &solution,
    std::vector<ValuePlacement> curGraph, std::vector<ValuePlacement> finiGraph,
    SmallVector<mlir::Operation *, 4> otherFailureOps, unsigned threshold,
    bool sortProducer) {
  for (auto &opr : failOp->getOpOperands()) {
    if (usedByBranchForNextCC(failOp, opr.getOperandNumber()))
      continue;
    auto opValue = opr.get();
    auto prodPtr =
        std::find_if(curGraph.begin(), curGraph.end(),
                     [&](ValuePlacement p) { return p.val == opValue; });
    //  TODO, if not produced, only increase the mov step
    if (prodPtr == curGraph.end() &&
        !isa<arith::ConstantOp>(opValue.getDefiningOp()))
      return -2;
    bool notInclude =
        std::find_if(producers.begin(), producers.end(), [&](ValuePlacement p) {
          return p.val == prodPtr->val;
        }) == producers.end();
    if (prodPtr != curGraph.end() && notInclude) {
      producers.push_back(*prodPtr);
      movs.push_back(0);
    }
  }

  if (sortProducer && producers.size() > 1) {
    // sort the producer with their usage by other failure operations
    std::vector<unsigned> weight(producers.size(), 0);
    std::vector<unsigned> prodTime(producers.size(), 0);
    std::set<unsigned> prodPEs;
    for (auto [ind, prod] : llvm::enumerate(producers)) {
      auto val = prod.val;
      prodPEs.insert(prod.pe);
      // get the produce time
      if (solution.count(val.getDefiningOp()))
        prodTime[ind] = solution.at(val.getDefiningOp()).time;
      for (auto user : val.getUsers())
        if (std::find(otherFailureOps.begin(), otherFailureOps.end(), user) !=
            otherFailureOps.end())
          weight[ind]++;
    }
    sortProducersByWeight(producers, prodTime, weight);
  }

  // get the available PEs
  auto avaiPEs = getAvailableResourceGraph(curGraph, attr, failOp, liveout,
                                           finiGraph, {}, scheduledOps);

  if (producers.size() == 0) {
    for (auto pe : avaiPEs)
      if (pe.second.exAvail) {
        intersection.insert(pe.first);
      }
    return 1;
  }

  blockedProdPEs.clear();
  for (auto prod : producers) {
    // if the producer PE is not available, mark it as blocked
    if (!avaiPEs.at(prod.pe).exAvail) {
      auto blockVal = getOccupiedValue(curGraph, finiGraph, scheduledOps,
                                       curBlock, prod.pe);
      if (blockVal.val && blockVal.val != prod.val) {
        blockedProdPEs.insert(prod.pe);
      }
    }
  }

  // SetVector<unsigned> avaiPEs;
  // for (auto i = 0; i < attr.nRow * attr.nCol; i++) {
  //   auto occupied =
  //       getOccupiedValue(curGraph, finiGraph, scheduledOps, curBlock, i);
  //   // if the pe is occupied but not by the producer, return false
  //   if (occupied.val && prodPEs.count(i) > 0) {
  //     if (DebugMode) {
  //       std::string message;
  //       llvm::raw_string_ostream rso(message);
  //       rso << *failOp << "\n";
  //       rso << i << " Occupied PE: " << occupied.val << " at " << occupied.pe
  //           << " by producer\n";
  //       logMessage(rso.str());
  //     }
  //     //  if the producer PE is occupied by other value, we need to route the
  //     //  other value
  //     bool blockPop = false;
  //     for (auto prod : producers) {
  //       if (prod.val == occupied.val) {
  //         blockPop = true;
  //         break;
  //       }
  //     }
  //     if (!blockPop) {
  //       blockedProdPEs.insert(i);
  //     }
  //   }
  //   if (occupied.val == nullptr)
  //     avaiPEs.insert(i);
  // }
  if (!blockedProdPEs.empty() && DebugMode) {
    logMessage("Blocked PE :" + std::to_string(blockedProdPEs[0]));
    return -1;
  }

  // define map population function of step 1
  auto populateRoutingPEs = [&](unsigned pe, SetVector<unsigned> &map) {
    for (auto routingPE : getTorusRoutingPEs(pe, attr))
      if (avaiPEs.at(routingPE).exAvail)
        map.insert(routingPE);
  };

  // initialize pop map for each producer
  std::map<unsigned, SetVector<unsigned>> popMap;
  // init popMap
  for (auto [ind, prod] : llvm::enumerate(producers)) {
    popMap[ind].insert(prod.pe);
    auto regAttr = prod.regAttr;
    if (regAttr == RegAttr::EX || regAttr == RegAttr::IE) {
      populateRoutingPEs(prod.pe, popMap[ind]);
    }
  }

  //  check whether the intersection exists
  intersection = popMap[0];
  for (size_t i = 1; i < producers.size(); ++i)
    intersection = getInterSection<unsigned>(intersection, popMap[i]);
  if (!intersection.empty()) {
    // for (size_t i = 1; i < producers.size(); ++i) {
    //   if (movs[i] > 0)
    //     return 0;
    // }
    return 1;
  }

  // replace values with its spilled versions if possible
  for (auto i = 0; i < producers.size(); i++) {
    auto prodPtr = &producers[i];
    auto opValue = prodPtr->val;

    auto spilOps = getSpilledOps(opValue, spilledVals, spillOps);
    auto coProducerInd = i == 0 ? 1 : 0;
    // if (i >= producers.size())
    //   break;
    auto coProducerPE = producers[coProducerInd].pe;

    auto optimalPlacement = *prodPtr;
    int optimalDist =
        getDistance(prodPtr->pe, coProducerPE, attr.nRow, attr.nCol);
    for (auto spilOp : spilOps) {
      auto val = spilOp->getResult(0);
      auto spilPlace = getSrcValuePlacement(val, curGraph);
      if (spilPlace.val == nullptr)
        continue;

      auto d1 = getDistance(spilPlace.pe, coProducerPE, attr.nRow, attr.nCol);
      if (d1 < optimalDist ||
          (d1 == optimalDist && spilPlace.regAttr > prodPtr->regAttr)) {
        optimalPlacement = spilPlace;
        optimalDist = d1;
      }
    }
    if (optimalPlacement.val != prodPtr->val) {
      llvm::errs() << "Replace " << *failOp << " operand " << prodPtr->val
                   << " with spilled version " << optimalPlacement.val << "...";
      failOp->replaceUsesOfWith(prodPtr->val, optimalPlacement.val);
      producers[i] = optimalPlacement;
      llvm::errs() << "Done\n";
    }
  }

  int population = 0;
  while (population < threshold) {
    population++;
    // check the intersection of the popMap
    // SetVector<unsigned> intersection;
    for (auto [ind, prod] : llvm::enumerate(producers)) {
      // populate the popMap
      auto pe = prod.pe;
      auto regAttr = prod.regAttr;
      auto prevPop = popMap[ind];
      for (auto routPE : prevPop)
        populateRoutingPEs(routPE, popMap[ind]);
      movs[ind] += 1;

      //  check whether the intersection exists
      intersection = popMap[0];
      for (size_t i = 1; i < producers.size(); ++i)
        intersection = getInterSection<unsigned>(intersection, popMap[i]);
      if (!intersection.empty())
        return 0;
    }
  }
  logMessage("failed to find path");
  return -1;
}

Operation *BasicBlockOpAssignment::createAtomicMovOp(Value val,
                                                     bool replaceCurBlkUse,
                                                     bool customLoc) {
  if (!customLoc) {
    if (isa<BlockArgument>(val) || val.getParentBlock() != curBlock)
      builder.setInsertionPoint(&curBlock->getOperations().front());
    else
      builder.setInsertionPointAfter(val.getDefiningOp());
  }
  Operation *movOp;
  if (isa<IntegerType>(val.getType())) {
    movOp =
        builder.create<arith::AddIOp>(val.getLoc(), val, zeroIntOp.getResult());
  } else if (isa<FloatType>(val.getType())) {
    movOp = builder.create<arith::AddFOp>(val.getLoc(), val,
                                          zeroFloatOp.getResult());
  }

  if (replaceCurBlkUse) {
    // replace the use of val to movOp
    val.replaceUsesWithIf(movOp->getResult(0), [&](OpOperand &use) {
      auto owner = use.getOwner();
      return owner->getBlock() == curBlock && owner != movOp &&
             !(isa<cf::BranchOp>(owner) ||
               (isa<cgra::ConditionalBranchOp>(owner) &&
                use.getOperandNumber() > 1));
    });
  }
  return movOp;
}

void BasicBlockOpAssignment::pushSpillOp(Value srcVal, Operation *spillOp) {
  // first get the root spill operation
  auto rootVal = getRootValue(srcVal);
  spilledVals.insert(rootVal);
  auto exist = std::find(spilledVals.begin(), spilledVals.end(), rootVal);
  // directly push back to the index of exist
  if (exist != spilledVals.end()) {
    unsigned idx = std::distance(spilledVals.begin(), exist);
    spillOps[idx].push_back(spillOp);
    return;
  }
  spillOps[spillOps.size()].push_back(spillOp);
}

SmallVector<Operation *, 4>
BasicBlockOpAssignment::routeOperation(std::vector<ValuePlacement> producers,
                                       std::vector<unsigned> movs,
                                       Operation *failedOp) {
  SmallVector<Operation *, 4> routeOps;
  for (size_t i = 0; i < producers.size(); ++i) {
    auto producer = producers[i];
    auto movNum = movs[i];
    if (movNum == 0)
      continue;
    // check whether the mov operation exists
    // unsigned movStep = 0;
    auto origVal = producer.val;

    // Route producer movNum times
    auto routeVal = origVal;
    Operation *finalRouteOp = nullptr;
    for (unsigned j = 0; j < movNum; ++j) {
      Operation *movOp = createAtomicMovOp(routeVal, false, false);
      finalRouteOp = movOp;
      // spilledVals.push_back(routeVal);
      routeOps.push_back(movOp);
      pushSpillOp(routeVal, movOp);
      routeVal = movOp->getResult(0);
    }
    // replace the use of origVal with the finalRouteOp if the use does not
    // belongs to scheduledOps
    // failedOp->replaceUsesOfWith(origVal, finalRouteOp->getResult(0));
    origVal.replaceUsesWithIf(finalRouteOp->getResult(0), [&](OpOperand &opr) {
      auto owner = opr.getOwner();
      if ((isa<cgra::ConditionalBranchOp>(owner) && opr.getOperandNumber() > 1))
        return false;
      return opr.getOwner() == failedOp;
    });
  }
  return routeOps;
}

// void BasicBlockOpAssignment::updateSchedulePriority(
//     int timeSlot, std::map<Block *, SetVector<Value>> liveIns,
//     std::map<Block *, SetVector<Value>> liveOuts) {
//   auto newSchedulePriority =
//       getSchedulePriority(curBlock, liveIns[curBlock], liveOuts[curBlock],
//                           timeSlot, schedulePriority);

//   // keep the delayed schedule priority
//   for (auto [op, priority] : newSchedulePriority) {
//     if (schedulePriority.count(op) == 0)
//       schedulePriority[op] = priority;
//     else {
//       auto oldPriority = schedulePriority[op];
//       schedulePriority[op] = {std::max(oldPriority.first, priority.first),
//                               std::max(oldPriority.second, priority.second)};
//     }
//   }
//   if (DebugMode) {
//     std::string message;
//     llvm::raw_string_ostream rso(message);
//     rso << "Schedule Priority:\n";
//     for (auto [op, priority] : schedulePriority) {
//       rso << *op << " [" << priority.first << " " << priority.second << "]";
//       rso << "\n";
//     }
//     logMessage(rso.str(), false);
//   }
// }

void BasicBlockOpAssignment::updateCDFG(Block *scheduleBB,
                                        std::vector<ValuePlacement> initGraph,
                                        std::vector<ValuePlacement> finiGraph) {
  // update the initGraph and finiGraph
  startEmbeddingGraph = initGraph;
  finiEmbeddingGraph = finiGraph;
}

double getLocalPopCost(SmallVector<Operation *, 4> &schedulingOps,
                       std::map<Operation *, ScheduleUnit> &tmpScheduleResult,
                       std::vector<ValuePlacement> lastGraph) {
  double localPopCost = 0.0;
  for (auto op : schedulingOps) {
    if (!isRouteOp(op) || tmpScheduleResult.count(op) == 0)
      continue;

    auto pe = tmpScheduleResult[op].pe;
    auto routeOpr = op->getOperand(0);
    // if in lastGraph routeOpr is in pe and regAttr is EX or IE, then it is a
    // local pop
    auto it =
        std::find_if(lastGraph.begin(), lastGraph.end(), [&](ValuePlacement p) {
          return p.val == routeOpr && p.pe == pe &&
                 (p.regAttr == RegAttr::EX || p.regAttr == RegAttr::IE);
        });
    if (it != lastGraph.end())
      localPopCost += 1.0; // increase the local pop costI
  }
  return localPopCost;
}

double getProducerAccessDistanceCost(
    Operation *scheduleOp,
    std::map<Operation *, ScheduleUnit> &tmpScheduleResult,
    std::vector<ValuePlacement> &tmpGraph,
    std::vector<ValuePlacement> &finiGraph, GridAttribute attr,
    std::map<Operation *, std::pair<unsigned, RegAttr>> &schedulePriority,
    int height) {
  double accessCost = 0.0;
  for (auto &opr : scheduleOp->getOpOperands()) {
    auto prodOp = opr.get().getDefiningOp();
    if (prodOp == nullptr || tmpScheduleResult.count(prodOp) == 0)
      continue;
  }
  return accessCost;
}

double BasicBlockOpAssignment::stepSA(
    int height, SmallVector<Operation *, 4> &schedulingOps,
    std::map<Operation *, ScheduleUnit> &tmpScheduleResult,
    std::vector<ValuePlacement> &tmpGraph,
    std::map<Operation *, std::vector<placeunit>> &existSpace,
    SetVector<Value> liveOut, std::vector<ValuePlacement> &finiGraph,
    GridAttribute attr, int shuffleOpIdx) {
  // Initial placement
  replaceValMap.clear();

  int suc = placeOperations(height, schedulingOps, tmpScheduleResult, tmpGraph,
                            existSpace, finiGraph, shuffleOpIdx);
  unsigned maxD = (attr.nRow + attr.nCol) / 2;
  double cost = 0;
  std::vector<int> peUsage(attr.nRow * attr.nCol, 0);
  // compute the placement cost
  for (auto &op : curBlock->getOperations()) {
    if (scheduledOps.count(&op) > 0 || tmpScheduleResult.count(&op) > 0 ||
        isa<arith::ConstantOp>(op))
      continue;

    // evaluate how schedulable the operation is
    auto priority = 0.5 * (std::max(height, schedulePriority[&op].first) +
                           schedulePriority[&op].second);
    int distance = 0;

    std::vector<ValuePlacement> producers;
    std::vector<unsigned> movs;
    SetVector<unsigned int> intersection;
    auto access = createRoutePath(&op, producers, movs, intersection, solution,
                                  tmpGraph, finiGraph, {}, maxD, false);
    // the producer is not produced yet, regarded as maxD cost
    if (access == -2)
      distance += maxD;
    // the producer is blocked, regarded as maxD cost
    else if (access == -1)
      distance += 2 * maxD;
    else if (access == 0) {
      distance += std::accumulate(movs.begin(), movs.end(), 0u);
    } else if (access == 1) {
      if (op.getNumResults() == 0)
        distance += 0;
      else {
        // the intersection exists, but if the value needs to be match to
        // finiGraph, evaluate the distance cost
        auto notMatchVal = std::find_if(
            finiGraph.begin(), finiGraph.end(), [&](ValuePlacement p) {
              return p.val == op.getResult(0) && intersection.count(p.pe) == 0;
            });
        if (notMatchVal != finiGraph.end()) {
          distance += getDistance(intersection[0], notMatchVal->pe, attr.nRow,
                                  attr.nCol);
        } else if (intersection.size() == 1) {
          // if the intersection exists, and is resctricted by certain PEs;
          distance += peUsage[intersection[0]];
          peUsage[intersection[0]]++;
        } else {
          // the operation can be scheduled, but not scheduled
          distance = priority <= height ? 1 : 0;
        }
      }
    }
    cost += std::pow(2.0, -priority) * log2(1 + distance) + 0.01;
    // if (DebugMode) {
    //   std::string message;
    //   llvm::raw_string_ostream rso(message);
    //   rso << op << " priority: " << priority << " distance cost: " <<
    //   distance
    //       << " -> " << cost << "\n";
    //   logMessage(rso.str());
    // }
  }

  // double sucCost = getSuccessCost(height, tmpScheduleResult,
  // schedulePriority,
  //                                 schedulingOps);
  // double affinityCost = getSpatialAffinityTotalCost(
  //     curBlock, tmpScheduleResult, tmpGraph, finiGraph, schedulePriority,
  //     attr, liveOut, scheduledOps, height);
  // double accessCost = getAccessCost(
  //     curBlock, tmpGraph, tmpScheduleResult, scheduledOps, liveOut,
  //     finiGraph, attr, schedulingOps.size(), schedulePriority, height);

  // auto prevGraph =
  //     height == 1 ? startEmbeddingGraph : transformGraphs.at(height - 1);
  // double localPopCost =
  //     getLocalPopCost(schedulingOps, tmpScheduleResult, prevGraph);

  // get the total cost
  // double currentCost = sucCost + affinityCost + accessCost + localPopCost;

  // if (DebugMode)
  //   logMessage("cost: " + std::to_string(sucCost) + " + " +
  //              std::to_string(affinityCost) + " + " +
  //              std::to_string(accessCost) + " + " +
  //              std::to_string(localPopCost) + " = " +
  //              std::to_string(currentCost) + "\n");
  cost = tmpScheduleResult.empty() ? cost + 1e3 : cost;
  logMessage("Placement cost: " + std::to_string(cost) + "\n", false,
             DebugMode);
  return cost;
}

LogicalResult BasicBlockOpAssignment::postSchedulingGraphTransformation(
    int &height, int &totalOpNum, std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts,
    SmallVector<Operation *, 4> graphTransformedOps,
    const std::map<mlir::Operation *, compigra::ScheduleUnit> &solution,
    std::vector<compigra::ValuePlacement> &curGraph,
    std::vector<compigra::ValuePlacement> &finiGraph) {
  bool transformed = false;
  int rollBackHeight = height;
  int longestRoutePath = 0;
  for (auto op : graphTransformedOps) {
    logMessage("Try to solve the graph transformation", false, DebugMode);
    unsigned producerNum = op->getNumOperands();
    std::vector<ValuePlacement> producers;
    std::vector<unsigned> movs;
    SetVector<unsigned int> intersection;
    // detect whether the operation is routable
    int routable = createRoutePath(op, producers, movs, intersection, solution,
                                   curGraph, finiGraph, graphTransformedOps);
    logMessage("routable: " + std::to_string(routable) + "\n", false,
               DebugMode);
    llvm::errs() << "Routable for " << *op << ": " << routable << "\n";
    if (routable >= 0) {
      std::string message;
      llvm::raw_string_ostream rso(message);
      if (routable == 0) {
        longestRoutePath = std::max(
            longestRoutePath, (int)*std::max_element(movs.begin(), movs.end()));
        if (isRouteOp(op))
          continue;
        int earliestTime = height;
        for (auto [idx, prodOp] : llvm::enumerate(producers)) {
          if (prodOp.val.getDefiningOp() &&
              isa<arith::ConstantOp>(prodOp.val.getDefiningOp())) {
            // if the producer is a constant, it can be scheduled at time 0
            continue;
          }

          bool initArg = isa<BlockArgument>(prodOp.val) ||
                         (prodOp.val.getParentBlock() != curBlock);
          auto prodTime =
              initArg ? 0 : solution.at(prodOp.val.getDefiningOp()).time;
          earliestTime = std::min(earliestTime, prodTime + 1);
        }
        rollBackHeight = std::min(rollBackHeight, earliestTime);

        logMessage("Route producers\n", false, DebugMode);
        unsigned newRouteOpsNum = 0;
        auto newRouteOps = routeOperation(producers, movs, op);
        newRouteOpsNum = newRouteOps.size();
        transformed = true;
        if (DebugMode)
          rso << "Warning: Route for: " << *op << "\n";
        for (size_t i = 0; i < producers.size(); ++i) {
          auto producer = producers[i];
          auto movNum = movs[i];
          if (DebugMode)
            rso << "    route: " << producer.val << " :" << movNum << "\n";
        }
        totalOpNum += newRouteOpsNum;
        logMessage(rso.str(), false, DebugMode);
      }
      // otherwise, route the operation itself
      if (routable == 1) {
        if (op->getNumResults() > 0 && liveout.count(op->getResult(0)) > 0) {
          if (DebugMode)
            rso << "Warning: Route liveout: " << *op;
          logMessage(rso.str(), false, DebugMode);
          auto newLiveOut = createAtomicMovOp(op->getResult(0), false, false);
          // replace the use if it is used outside the block
          op->getResult(0).replaceUsesWithIf(
              newLiveOut->getResult(0), [&](OpOperand &use) {
                auto owner = use.getOwner();
                return owner->getBlock() != curBlock ||
                       (owner->getBlock() == curBlock &&
                        (isa<cf::BranchOp>(owner) ||
                         (isa<cgra::ConditionalBranchOp>(owner) &&
                          use.getOperandNumber() > 1)));
              });
          // replace the value in finiGraph and liveout
          for (auto &place : finiGraph) {
            if (op->getResult(0) == place.val) {
              place.val = newLiveOut->getResult(0);
              // place.regAttr = RegAttr::EX;
              // place.pe = attr.maxReg; // use the maxReg to indicate EX
            }
          }
          liveout.remove(op->getResult(0));
          liveout.insert(newLiveOut->getResult(0));
          auto &parentRegion = *(curBlock->getParent());
          computeLiveValue(parentRegion, liveIns, liveOuts);
          totalOpNum++;
          return success();
        }
      }
    } else {
      if (DebugMode) {
        std::string message;
        llvm::raw_string_ostream rso(message);
        // check whether pop the blocked PE can solve the problem
        rso << "Warning: Cannot route for: " << *op << "due to block.\n";
        logMessage(rso.str());
      }

      SetVector<unsigned> avaiPEs;
      for (auto i = 0; i < attr.nRow * attr.nCol; i++) {
        auto occupied =
            getOccupiedValue(curGraph, finiGraph, scheduledOps, curBlock, i);
        if (occupied.val == nullptr)
          avaiPEs.insert(i);
      }
      bool routeBlockSuccess = true;
      unsigned newRouteOpsNum = 0;
      for (auto blockProd : blockedProdPEs) {
        // try to route the occupied value to other PEs;
        // if it is not routable, return false to indicate it is
        // impossible to route for the failOp
        auto cntPEs = getTorusRoutingPEs(blockProd, attr, false);
        // if none of the cntPEs in the avaiPEs return false
        SetVector<unsigned> cntPESet(cntPEs.begin(), cntPEs.end());
        auto intersection = getInterSection<unsigned>(cntPESet, avaiPEs);
        if (intersection.empty())
          routeBlockSuccess = false;

        // otherwise, route the blocked PE
        auto blockPlace = getOccupiedValue(curGraph, finiGraph, scheduledOps,
                                           curBlock, blockProd);

        // not create double route
        if (blockPlace.val.getDefiningOp() &&
            isRouteOp(blockPlace.val.getDefiningOp())) {
          continue;
        }

        transformed = true;
        if (DebugMode) {
          // log the block Place
          std::string message;
          llvm::raw_string_ostream rso1(message);
          rso1 << "Blocked PE: " << blockPlace.val << " ^^^ " << blockPlace.pe
               << "\n";
          logMessage(rso1.str());
        }
        // builder.setInsertionPoint
        auto freePEOp = createAtomicMovOp(blockPlace.val, false, false);
        newRouteOpsNum++;
        // replace the use of blockPlace->val with freePEOp
        blockPlace.val.replaceUsesWithIf(
            freePEOp->getResult(0), [&](OpOperand &use) {
              auto owner = use.getOwner();
              return owner->getBlock() == curBlock && owner != freePEOp &&
                     !scheduledOps.count(owner) &&
                     !(isa<cf::BranchOp>(owner) ||
                       (isa<cgra::ConditionalBranchOp>(owner) &&
                        use.getOperandNumber() > 1));
            });
      }
      if (!routeBlockSuccess) {
        logMessage("Error: Failed to route the blocked PEs, graph "
                   "transformation is needed.\n",
                   false, DebugMode);
        return failure();
      }
      totalOpNum += newRouteOpsNum;
    }
  }

  // re-schedule
  if (transformed) {
    height = std::max(rollBackHeight, height - longestRoutePath);
  } else {
    height = height + 1;
  }
  return success();
}

static bool acceptStep(double bestCost, double currentCost) {
  // Generate a random number between 0 and 1
  if (bestCost == 0.0)
    return true;
  double accept = static_cast<double>(rand()) / RAND_MAX;
  return currentCost < bestCost && accept <= 0.9;
}

static std::map<int, std::vector<Operation *>>
makeMemoryAccessMap(Block *curBlock) {
  // for the operations in curBlock, if they are lwi/swi and group them by their
  // memory address

  auto getSwiAddress = [](cgra::SwiOp swiOp) -> int {
    // get the address value
    auto addrValue = swiOp.getOperand(1);
    if (auto constOp = addrValue.getDefiningOp<arith::ConstantOp>()) {
      return constOp.getValue().cast<IntegerAttr>().getInt();
    }
    return -1;
  };

  std::map<int, std::vector<Operation *>> memAccessMap;
  for (auto &op : curBlock->getOperations()) {
    if (auto lwiOp = dyn_cast<cgra::LwiOp>(op)) {
      auto addr = getLwiAddress(lwiOp);
      if (addr >= 0) {
        if (memAccessMap.count(addr) == 0)
          memAccessMap[addr] = std::vector<Operation *>{&op};
        else
          memAccessMap[addr].push_back(&op);
      }
    } else if (auto swiOp = dyn_cast<cgra::SwiOp>(op)) {
      auto addr = getSwiAddress(swiOp);
      if (addr >= 0) {
        if (memAccessMap.count(addr) == 0)
          memAccessMap[addr] = std::vector<Operation *>{&op};
        else
          memAccessMap[addr].push_back(&op);
      }
    }
  }
  return memAccessMap;
}

LogicalResult BasicBlockOpAssignment::mappingBBdataflowToCGRA(
    std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts, ScheduleStrategy strategy) {
  // auto blockIn = liveIns[curBlock];
  // auto blockOut = liveOuts[curBlock];
  setUpLiveness(liveIns, liveOuts);

  auto initGraph = startEmbeddingGraph;
  auto finiGraph = finiEmbeddingGraph;
  // init all livein in initGraph
  initEmbeddingGraphWithLiveIn(liveIns, liveOuts, initGraph, builder, attr);

  int totalOpNum = getNonCstOpSize(curBlock);
  auto memoryAccessMap = makeMemoryAccessMap(curBlock);

  // get the schedule priority of the operations in the block
  schedulePriority = getSchedulePriority(totalOpNum, curBlock, livein, liveout,
                                         memoryAccessMap, DebugMode);

  int height = 1;
  int maxTry = 0;
  auto graphScheduleBefore = initGraph;
  transformGraphs[0] = initGraph;

  while (scheduledOps.size() < totalOpNum && maxTry < 30) {
    maxTry++;
    logMessage("----height: " + std::to_string(height) + "----\n", false,
               DebugMode);

    auto schedulingOps = getScheduleOps(totalOpNum, curBlock, scheduledOps,
                                        livein, memoryAccessMap, strategy);
    // log schedulingOps
    if (DebugMode) {
      std::string message;
      llvm::raw_string_ostream totalRso(message);
      totalRso << "Scheduling Operations at height " << height << ": ";
      for (auto op : schedulingOps) {
        totalRso << *op << "\n";
      }
      logMessage(totalRso.str());
    }

    std::map<Operation *, ScheduleUnit> tmpScheduleResult;
    std::map<Operation *, std::vector<placeunit>> searchSpace;
    // log graphScheduleBefore
    if (DebugMode) {
      std::string curGraph;
      llvm::raw_string_ostream rso(curGraph);
      rso << "Graph Schedule Before:\n";
      for (auto place : graphScheduleBefore) {
        rso << "  " << place.val << " at PE: " << place.pe
            << " RegAttr: " << static_cast<int>(place.regAttr) << "\n";
      }
      logMessage(rso.str());
    }
    auto tmpGraph = graphScheduleBefore;

    // Initial placement
    double currentCost =
        stepSA(height, schedulingOps, tmpScheduleResult, tmpGraph, searchSpace,
               liveout, finiGraph, attr);
    auto layerScheduleResult = tmpScheduleResult;
    auto graphScheduleAfter = tmpGraph;
    double bestCost = currentCost;

    for (auto [op, replaceUse] : replaceValMap)
      for (auto [_, pair] : replaceUse)
        // replace the use of op with the pair.first
        op->replaceUsesOfWith(pair.first, pair.second);

    // simulated annealing to create a loop that get random
    // placement, record status, cost and determine the final placement
    int iterSA = 100;
    std::vector<double> lastThreeCosts;
    double previousCost = bestCost;

    for (int iter = 0; iter < iterSA; iter++) {
      // get a random placement
      tmpGraph = graphScheduleBefore;
      int shuffleOpIdx =
          shuffleSearchSpace(searchSpace, tmpScheduleResult, schedulingOps);
      if (shuffleOpIdx < 0) {
        break;
      }

      double currentCost =
          stepSA(height, schedulingOps, tmpScheduleResult, tmpGraph,
                 searchSpace, liveout, finiGraph, attr, shuffleOpIdx);

      if (acceptStep(bestCost, currentCost)) {
        bestCost = currentCost;
        graphScheduleAfter = tmpGraph;
        layerScheduleResult = tmpScheduleResult;
        for (auto [op, replaceUse] : replaceValMap)
          for (auto [_, pair] : replaceUse)
            // replace the use of op with the pair.first
            op->replaceUsesOfWith(pair.first, pair.second);
      }

      // Track the last three costs
      lastThreeCosts.push_back(currentCost);
      if (lastThreeCosts.size() > 3)
        lastThreeCosts.erase(lastThreeCosts.begin());

      // Check if the last three states are stable
      auto costAvg =
          std::accumulate(lastThreeCosts.begin(), lastThreeCosts.end(), 0.0) /
          lastThreeCosts.size();
      // double maxDiff = 0;
      // for (auto cost : lastThreeCosts) {
      //   maxDiff = std::max(maxDiff, std::abs(cost - costAvg));
      // }
      if (currentCost == 0 ||
          (iter > 10 && std::abs(bestCost - costAvg) < 1e-3)) {
        break;
      }

      previousCost = currentCost;
    }
    logMessage("Best cost: " + std::to_string(bestCost) + "\n", false,
               DebugMode);

    // post simulated annealing, check whether graph transformation is needed
    SmallVector<Operation *, 4> graphTransformedOps;
    for (auto op : schedulingOps) {
      if (layerScheduleResult.count(op) == 0 &&
          schedulePriority.at(op).second <= height)
        graphTransformedOps.push_back(op);
    }

    int rollbackHeight = height;
    if (!graphTransformedOps.empty()) {
      if (failed(postSchedulingGraphTransformation(
              rollbackHeight, totalOpNum, liveIns, liveOuts,
              graphTransformedOps, solution, graphScheduleBefore, finiGraph)))
        return failure();
      logMessage("After graph transformation, totalOpNum: " +
                     std::to_string(totalOpNum) + "\n",
                 false, DebugMode);

      schedulePriority = getSchedulePriority(
          totalOpNum, curBlock, livein, liveout, memoryAccessMap, DebugMode);
      if (rollbackHeight <= height) {
        // rollback solution and scheduledOps
        for (auto sol : llvm::make_early_inc_range(solution)) {
          if (sol.second.time >= rollbackHeight) {
            scheduledOps.remove(sol.first);
            solution.erase(sol.first);
          }
        }
        graphScheduleBefore = transformGraphs[rollbackHeight - 1];
        height = rollbackHeight;
        logMessage("Rollback to height: " + std::to_string(height) + "\n",
                   false, DebugMode);
        continue;
      }
    }

    // prepare scheduling for the next layer
    logMessage("Time = " + std::to_string(height), false, DebugMode);
    // bool scheduleDone =
    //     scheduledOps.size() + layerScheduleResult.size() == totalOpNum;
    for (auto [op, res] : layerScheduleResult) {
      // remove it from the schedulingOps
      // auto it = std::find(schedulingOps.begin(), schedulingOps.end(), op);
      // if (!scheduleDone && (isa<func::ReturnOp>(op) || isa<cf::BranchOp>(op)
      // ||
      //                       isa<cgra::ConditionalBranchOp>(op))) {
      //   continue;
      // }
      // schedulingOps.erase(it);

      scheduledOps.insert(op);
      solution[op] = res;

      // log the final placement info
      if (DebugMode) {
        std::string message;
        llvm::raw_string_ostream rso(message);
        rso << "SCHEDULE RESULT: " << *op << " ---> " << std::to_string(res.pe)
            << "\n";
        logMessage(rso.str());
      }
    }
    logMessage("--Scheduled Ops: " + std::to_string(scheduledOps.size()) + "/" +
                   std::to_string(totalOpNum),
               false, DebugMode);
    graphScheduleBefore = graphScheduleAfter;
    transformGraphs[height] = graphScheduleBefore;
    height++;
  }

  if (failed(finalizeEmbeddingGraphWithLiveOut(finiGraph, graphScheduleBefore)))
    return failure();

  if (scheduledOps.size() < totalOpNum)
    return failure();

  updateCDFG(curBlock, initGraph, finiGraph);
  return success();
}

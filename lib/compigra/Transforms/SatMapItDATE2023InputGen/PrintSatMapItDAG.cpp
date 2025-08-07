//===- PrintSatMapItDAG.cpp - print text file for SatMapIt ------*- C++ -*-===//
//
// Copmigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the text printout functions for Sat-MapIt code base.
//
//===----------------------------------------------------------------------===//

#include "compigra/Transforms/SatMapItDATE2023InputGen/PrintSatMapItDAG.h"
#include "compigra/CgraDialect.h"
#include "compigra/CgraInterfaces.h"
#include "compigra/CgraOps.h"
#include "compigra/Support/Utils.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>

#define DEBUG_TYPE "PRINT_SATMAPIT_DAG"

using namespace mlir;
using namespace compigra;
using namespace compigra::satmapit;

static unsigned getPredecessorCount(Block *blk) {
  return std::distance(blk->getPredecessors().begin(),
                       blk->getPredecessors().end());
}

template <typename T>
static void getFalseDestOperands(T branchOp,
                                 SmallVector<Value> &falseDestOperands) {
  // The true branch arguments are the operands of the true branch
  falseDestOperands.clear();
  falseDestOperands.append(branchOp.getFalseDestOperands().begin(),
                           branchOp.getFalseDestOperands().end());
}

template <typename T>
static void getTrueDestOperands(T branchOp,
                                SmallVector<Value> &trueDestOperands) {
  // The true branch arguments are the operands of the true branch
  trueDestOperands.clear();
  trueDestOperands.append(branchOp.getTrueDestOperands().begin(),
                          branchOp.getTrueDestOperands().end());
}

/// Get the specific operand of the conditional branch operation. `block` is the
/// one of the successor blocks to get connected operand of the conditional
/// branch operation. If block is the true branch, then the operand is the
/// index-th of trueDestOperands. Otherwise, the operand is the index-th of the
/// falseDestOperands.
static Value getCondBranchOperand(unsigned ind,
                                  cgra::ConditionalBranchOp *termOp,
                                  Block *block) {
  bool targetBlk = termOp->getTrueDest() == block;

  if (targetBlk)
    return termOp->getTrueDestOperands()[ind];
  else
    return termOp->getFalseDestOperands()[ind];
}

static Value getCntBlockArgInPredcessor(unsigned ind, Block *pred,
                                        Block *block) {
  auto termOp = pred->getTerminator();
  if (auto brOp = dyn_cast<cf::BranchOp>(termOp))
    return brOp->getOperand(ind);
  if (auto condBrOp = dyn_cast<cgra::ConditionalBranchOp>(termOp)) {
    return getCondBranchOperand(ind, &condBrOp, block);
  }
  return NULL;
}

LogicalResult PrintSatMapItDAG::init() {
  // init the loop block and the predecessor block
  initLoopBlock();
  initPredBlock();
  //  Get the LiveIn and LiveOut arguments

  blockArgs.append(loopBlock->getArguments().begin(),
                   loopBlock->getArguments().end());
  blockArgNum = loopBlock->getNumArguments();

  // init constant, liveIn, and liveOut operations
  for (auto [ind, arg] : llvm::enumerate(blockArgs)) {
    // Loop block should have two predecessors
    if (getPredecessorCount(loopBlock) != 2)
      return failure();
    SmallVector<Value, 2> parameters;

    for (auto pred : loopBlock->getPredecessors()) {
      // Get the value that is passed to the loop block
      Value corrArg = pred->getTerminator()->getOperand(ind);
      if (isa<cgra::ConditionalBranchOp>(pred->getTerminator()))
        corrArg = getCntBlockArgInPredcessor(ind, pred, loopBlock);
      parameters.push_back(corrArg);
    }
    nodes[ind] = arg;
    argMaps[ind] = parameters;
  }

  for (auto [ind, op] : llvm::enumerate(loopBlock->getOperations())) {
    if (op.getNumResults() == 0) {
      freeResNodes[blockArgNum + ind] = &op;
      nodes[blockArgNum + ind] = nullptr;
      llvm::errs() << blockArgNum + ind << " " << op << "\n";
      continue;
    }
    llvm::errs() << blockArgNum + ind << " " << op << "\n";
    nodes[blockArgNum + ind] = op.getResult(0);
  }

  llvm::errs() << "Init success\n";
  return success();
}

static std::string getOperantionName(Operation *node) {
  size_t namePos = node->getName().getStringRef().str().find(".");
  std::string nodeName =
      node->getName().getStringRef().str().substr(namePos + 1);

  // remove data type
  if (nodeName == "addi" || nodeName == "addf" || nodeName == "subi" ||
      nodeName == "subf" || nodeName == "muli" || nodeName == "mulf") {
    nodeName = nodeName.substr(0, 3);
  }
  if (auto condBr = dyn_cast<cgra::ConditionalBranchOp>(node)) {
    switch (condBr.getPredicate()) {
    case cgra::CondBrPredicate::eq:
      nodeName = "beq";
      break;
    case cgra::CondBrPredicate::ne:
      nodeName = "bne";
      break;
    case cgra::CondBrPredicate::ge:
      nodeName = "bge";
      break;
    case cgra::CondBrPredicate::lt:
      nodeName = "blt";
      break;
    }
  }
  return nodeName;
}

LogicalResult PrintSatMapItDAG::printDAG(std::string fileName) {
  std::string nodeFile = fileName + "_nodes";
  std::string edgeFile = fileName + "_edges";

  std::ofstream nodeFStream;
  nodeFStream.open(nodeFile.c_str());
  if (!nodeFStream.is_open()) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to open the " << nodeFile << "\n");
    return failure();
  }

  std::ofstream edgeFStream;
  edgeFStream.open(edgeFile.c_str());
  if (!edgeFStream.is_open()) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to open the " << edgeFile << "\n");
    return failure();
  }

  auto opIndex = nodes.size();

  auto getNodeIndex = [&](Value val, bool append = true) -> int {
    for (size_t ind = 0; ind < nodes.size(); ++ind) {
      if (nodes[ind] == val) {
        return ind;
      }
    }
    if (!append)
      return -1;

    auto nodeIndex = nodes.size();
    nodes[nodeIndex] = val;
    return nodeIndex++;
  };

  // print the block arguments to be merge node
  for (auto ind = 0; ind < opIndex; ind++) {
    auto val = nodes[ind];
    std::string nodeName;
    SmallVector<Value, 3> operands;
    llvm::errs() << "Processing node: " << ind << "\n";

    Operation *defOp =
        (val == nullptr) ? freeResNodes[ind] : val.getDefiningOp();

    if (argMaps.count(ind)) {
      auto srcOps = argMaps[ind];
      operands.append(srcOps.begin(), srcOps.end());
      nodeName = "phi";
    } else {
      nodeName = getOperantionName(defOp);
      auto oprBase = 0;
      // skip the predicate operand
      if (isa<cgra::BzfaOp, cgra::BsfaOp>(defOp))
        oprBase = 1;

      auto opIdUpper =
          std::min(defOp->getNumOperands(), static_cast<unsigned>(oprBase + 2));
      for (auto opId = oprBase; opId < opIdUpper; opId++)
        operands.push_back(defOp->getOperand(opId));
      if (oprBase == 1)
        operands.push_back(defOp->getOperand(0));
    }

    if (val != nullptr)
      llvm::errs() << "Node value: " << val << "\n";

    auto leftOpInd = operands.size() > 0 ? getNodeIndex(operands[0]) : -1;
    auto rightOpInd = operands.size() > 1 ? getNodeIndex(operands[1]) : -1;
    auto predicateSel = operands.size() > 2 ? getNodeIndex(operands[2]) : -1;

    nodeFStream << std::to_string(ind) << " instruction " << nodeName << " "
                << std::to_string(CgraInsts[nodeName]) << " " << leftOpInd
                << " " << rightOpInd << " " << std::to_string(predicateSel)
                << " 0 0\n";

    auto outToEdgeFile = [&](int srcInd, int destInd) {
      if (srcInd < 0 || destInd < 0)
        return;
      bool backEdgeAttr =
          srcInd < opIndex && destInd < opIndex && srcInd > destInd;
      edgeFStream << std::to_string(srcInd) << " " << std::to_string(destInd)
                  << " " << std::to_string(backEdgeAttr) << " 1\n";
    };

    outToEdgeFile(leftOpInd, ind);
    outToEdgeFile(rightOpInd, ind);
    outToEdgeFile(predicateSel, ind);
  }

  // print operations without result operands

  for (auto opId = opIndex; opId < nodes.size(); opId++) {
    auto val = nodes[opId];
    auto defOp = val.getDefiningOp();

    if (!defOp || !isa<arith::ConstantOp>(defOp)) {
      nodeFStream << std::to_string(opId)
                  << " live_in LiveInFromArg  28 -1 -1 -1 0 0\n";
      continue;
    }

    for (auto user : defOp->getUsers()) {
      if (user->getBlock() != loopBlock)
        continue;
      unsigned posLR = user->getOperand(0).getDefiningOp() == defOp ? 0 : 1;

      // get the integer or floating point constant value of constOp
      int constVal;
      if (auto intAttr = defOp->getAttr("value").dyn_cast<IntegerAttr>()) {
        constVal = intAttr.getInt();
      } else if (auto floatAttr =
                     defOp->getAttr("value").dyn_cast<FloatAttr>()) {
        constVal = static_cast<int>(floatAttr.getValue().convertToFloat());
      } else {
        LLVM_DEBUG(llvm::dbgs() << "Unsupported constant type\n");
        return failure();
      }
      nodeFStream << std::to_string(opId) << " constant nil -1 -1 -1 -1 "
                  << constVal << " " << posLR << "\n";
    }
  }

  nodeFStream.close();
  edgeFStream.close();
  return success();
}

void satmapit::parsePKE(const std::string &line, unsigned termId,
                        std::vector<std::set<int>> &timeSlotsOfBBs,
                        std::map<int, std::set<int>> &opTimeMap) {
  std::istringstream lineStream(line);
  // first parse t: time
  std::string token, tStr;
  // Read the first part (t: t)
  std::getline(lineStream, token, ' ');
  std::getline(lineStream, token, ' ');
  int tVal = std::stoi(token.substr(token.find(":") + 1));
  timeSlotsOfBBs.back().insert(tVal);

  // Initialize the set for the values
  std::set<int> values;

  // Read the remaining parts (values)
  while (std::getline(lineStream, token, ' ')) {
    if (!token.empty()) {
      values.insert(std::stoi(token));
      if (std::stoi(token) == termId)
        // push back a new set for the new basic block
        timeSlotsOfBBs.push_back({});
    }
  }
  opTimeMap[tVal] = values;
}

void satmapit::parseLine(const std::string &line,
                         std::map<int, Instruction> &instMap,
                         const unsigned maxReg) {

  std::istringstream lineStream(line);
  std::string idStr, nameStr, timeStr, peStr, RoutStr, opAStr, opBStr, immStr;
  std::string idVal, nameVal, timeVal, peVal, RoutVal, opAVal, opBVal, immVal;
  // Read Id token
  std::getline(lineStream, idStr, ' ');
  std::getline(lineStream, idVal, ' ');
  int id = std::stoi(idVal.substr(idVal.find(":") + 1));

  // Read name
  std::getline(lineStream, nameStr, ' ');
  std::getline(lineStream, nameVal, ' ');
  nameVal = nameVal.substr(nameVal.find(":") + 1);

  // Read time
  std::getline(lineStream, timeStr, ' ');
  std::getline(lineStream, timeVal, ' ');
  timeVal = timeVal.substr(timeVal.find(":") + 1);

  // Read pe
  std::getline(lineStream, peStr, ' ');
  std::getline(lineStream, peVal, ' ');
  peVal = peVal.substr(peVal.find(":") + 1);

  // Read Rout
  std::getline(lineStream, RoutStr, ' ');
  std::getline(lineStream, RoutVal, ' ');
  RoutVal = RoutVal.substr(RoutVal.find(":") + 1);
  // if RoutVal = Ri:i, else RoutVal = Rout(mexReg)
  // Find substr after R, if it is out, then it is Rout, else it is Ri
  auto reg = RoutVal.substr(RoutVal.find("R") + 1);
  RoutVal = reg == "OUT" ? std::to_string(maxReg) : reg;

  // Read opA
  std::getline(lineStream, opAStr, ' ');
  std::getline(lineStream, opAVal, ' ');
  opAVal = opAVal.substr(opAVal.find(":") + 1);

  // Read opB
  std::getline(lineStream, opBStr, ' ');
  std::getline(lineStream, opBVal, ' ');
  opBVal = opBVal.substr(opBVal.find(":") + 1);

  // Read immediate
  std::getline(lineStream, immStr, ' ');
  std::getline(lineStream, immVal, ' ');
  immVal = immVal.substr(immVal.find(":") + 1);

  // Create and insert the Inst object
  Instruction inst;
  inst.name = nameVal;
  inst.time = std::stoi(timeVal);
  inst.pe = std::stoi(peVal);
  inst.Rout = std::stoi(RoutVal);
  // inst.opA = opAVal;
  // inst.opB = opBVal;

  instMap[id] = inst;
}
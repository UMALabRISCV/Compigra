//===- FastASMGenOpenEdge.cpp - Implements the functions for temporal CGRA ASM
// fast generation *- C++ -*-----------------------------------------------===//
//
// Compigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements assembly generation functions for OpenEdge.
//
//===----------------------------------------------------------------------===//

#include "compigra/ASMGen/FastASMGenTempCGRA.h"
#include "compigra/CgraDialect.h"
#include "compigra/CgraOps.h"
#include "compigra/Scheduler/BasicBlockILPModel.h"
#include "compigra/Scheduler/BasicBlockOpAssignment.h"
#include "compigra/Scheduler/ModuloScheduleAdapter.h"
#include "compigra/Support/OpenEdgeASM.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include <fstream>
#include <set>

using namespace mlir;
using namespace compigra;

void printBlockLiveValue(Region &region,
                         std::map<Block *, SetVector<Value>> &liveIns,
                         std::map<Block *, SetVector<Value>> &liveOuts) {

  unsigned blockNum = 0;
  // print liveIn and liveOut
  for (auto &block : region) {
    llvm::errs() << "Block: " << blockNum << "\n";
    llvm::errs() << "LiveIn: ";
    for (auto val : liveIns[&block]) {
      if (val.isa<BlockArgument>()) {
        for (auto [ind, bb] : llvm::enumerate(region))
          if (&bb == val.getParentBlock()) {
            llvm::errs() << ind << " ";
            break;
          }
      }
      llvm::errs() << val << "\n";
    }
    llvm::errs() << "LiveOut: ";
    for (auto val : liveOuts[&block]) {
      if (val.isa<BlockArgument>()) {
        for (auto [ind, bb] : llvm::enumerate(region))
          if (&bb == val.getParentBlock()) {
            llvm::errs() << ind << " ";
            break;
          }
      }

      llvm::errs() << val << "\n";
    }
    llvm::errs() << "\n";
    blockNum++;
  }
}

void printLiveGraph(std::map<Block *, std::vector<ValuePlacement>> graph) {
  std::string message;
  llvm::raw_string_ostream rso(message);
  for (auto [blk, graph] : graph) {
    rso << "------------------\n";
    rso << *blk->getTerminator() << "\n";
    for (auto val : graph) {
      rso << val.val << " [" << val.pe << " " << static_cast<int>(val.regAttr)
          << "]\n";
    }
    rso << "------------------\n";
  }
  logMessage(rso.str());
}

void maxIndependentSubGraphs(Block *block, SetVector<Value> liveIn) {}

arith::ConstantOp getZeroConstant(Region &region, OpBuilder &builder,
                                  bool isFloat = false) {
  arith::ConstantOp zeroOp;
  for (auto &op : region.getOps()) {
    auto zeroCst = dyn_cast_or_null<arith::ConstantOp>(op);
    if (!zeroCst)
      continue;

    if (auto intAttr = zeroCst.getValue().dyn_cast<IntegerAttr>()) {
      if (!isFloat && intAttr.getValue().isZero()) {
        zeroOp = zeroCst;
        break;
      }
    } else if (auto floatAttr = zeroCst.getValue().dyn_cast<FloatAttr>()) {
      if (isFloat && floatAttr.getValue().isZero()) {
        zeroOp = zeroCst;
        break;
      }
    }
  }

  // if zeroOp is not found, create a new one
  builder.setInsertionPointAfter(&region.front().front());
  if (!zeroOp && !isFloat) {
    zeroOp = builder.create<arith::ConstantOp>(
        region.getLoc(), builder.getI32Type(), builder.getI32IntegerAttr(0));
  } else if (!zeroOp && isFloat) {
    zeroOp = builder.create<arith::ConstantOp>(
        region.getLoc(), builder.getF32Type(), builder.getF32FloatAttr(0.0));
  }
  return zeroOp;
}

static Value getArgumentOperand(Operation *termOp, Block *sucBlk,
                                unsigned argInd) {
  if (auto branchOp = dyn_cast_or_null<cf::BranchOp>(termOp)) {
    return branchOp.getOperand(argInd);
  } else if (auto branchOp =
                 dyn_cast_or_null<cgra::ConditionalBranchOp>(termOp)) {
    if (sucBlk == branchOp.getTrueDest()) {
      return branchOp.getOperand(argInd + 2);
    } else if (sucBlk == branchOp.getFalseDest()) {
      return branchOp.getOperand(argInd + 2 +
                                 branchOp.getNumTrueDestOperands());
    }
  }
  return nullptr;
}

Value resolveInjectedValue(
    Value val, Block *curBlk, Block *prevBlk,
    const std::map<Block *, SetVector<Value>> &liveOuts) {
  if (liveOuts.at(prevBlk).count(val))
    return val;
  for (auto arg : curBlk->getArguments()) {
    if (arg == val)
      return getArgumentOperand(prevBlk->getTerminator(), curBlk,
                                arg.getArgNumber());
  }
  llvm::errs() << "Error: cannot resolve injected value " << val << " in "
               << *prevBlk->getTerminator() << "\n";
};

Value resolvePropagatedValue(
    Value val, Block *curBlk, Block *sucBlk,
    const std::map<Block *, SetVector<Value>> &liveIns) {
  if (liveIns.at(sucBlk).count(val))
    return val;

  BlockArgument arg;
  auto termOp = curBlk->getTerminator();
  for (auto &use : val.getUses()) {
    auto user = use.getOwner();
    if (user != termOp)
      continue;

    auto argInd = use.getOperandNumber();
    if (auto br = dyn_cast<cf::BranchOp>(use.getOwner())) {
      arg = sucBlk->getArgument(argInd);
      break;
    }

    if (auto cbr = dyn_cast<cgra::ConditionalBranchOp>(use.getOwner())) {
      if (sucBlk == cbr.getTrueDest()) {
        if (argInd >= 2 && argInd < 2 + cbr.getNumTrueDestOperands()) {
          arg = sucBlk->getArgument(argInd - 2);
          break;
        }
      } else if (sucBlk == cbr.getFalseDest()) {
        if (argInd >= 2 + cbr.getNumTrueDestOperands()) {
          arg = sucBlk->getArgument(argInd - cbr.getNumTrueDestOperands() - 2);
          break;
        }
      }
    }
  }

  if (liveIns.at(sucBlk).count(arg))
    return arg;
  return NULL;
};

void updateLiveGraph(std::vector<ValuePlacement> &graph,
                     ValuePlacement prequisite) {
  auto it = std::find_if(graph.begin(), graph.end(), [&](ValuePlacement p) {
    return p.val == prequisite.val;
  });
  if (it != graph.end()) {
    it->pe = prequisite.pe;
    it->regAttr = prequisite.regAttr;
  } else {
    graph.push_back(prequisite);
  }
};

void updatePredecessorPlacement(
    ValuePlacement valPlace, Block *curBlk,
    std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts,
    std::map<Block *, std::vector<ValuePlacement>> &bbInitGraphs,
    std::map<Block *, std::vector<ValuePlacement>> &bbFiniGraphs) {

  auto val = valPlace.val;
  DenseSet<Block *> visited;
  visited.insert(curBlk);
  // recursively propagate the value to the successors
  for (auto *pred : curBlk->getPredecessors()) {
    // if (pred == curBlk)
    //   continue;

    visited.insert(pred);
    Value propVal = resolveInjectedValue(val, curBlk, pred, liveOuts);
    updateLiveGraph(bbFiniGraphs[pred],
                    {propVal, valPlace.pe, valPlace.regAttr});

    std::vector<std::pair<Block *, Value>> stack;
    if (liveIns[pred].count(propVal)) {
      // update the initGraph
      updateLiveGraph(bbInitGraphs[pred],
                      {propVal, valPlace.pe, valPlace.regAttr});
      stack.emplace_back(pred, propVal);
    }

    // init an visited set to avoid infinite loop
    while (!stack.empty()) {
      auto [cur, curVal] = stack.back();
      stack.pop_back();
      visited.insert(cur);

      for (auto *prevPred : cur->getPredecessors()) {
        // if (prevPred == cur)
        //   continue;

        Value nextPropVal =
            resolveInjectedValue(curVal, cur, prevPred, liveOuts);
        updateLiveGraph(bbFiniGraphs[prevPred],
                        {nextPropVal, valPlace.pe, valPlace.regAttr});

        if (liveIns[prevPred].count(nextPropVal)) {
          // update the initGraph
          updateLiveGraph(bbInitGraphs[prevPred],
                          {nextPropVal, valPlace.pe, valPlace.regAttr});
          if (!visited.count(prevPred))
            stack.emplace_back(prevPred, nextPropVal);
        }
      }
    }
  }
}

void updateSuccessorPlacement(
    ValuePlacement valPlace, Block *curBlk,
    std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts,
    std::map<Block *, std::vector<ValuePlacement>> &bbInitGraphs,
    std::map<Block *, std::vector<ValuePlacement>> &bbFiniGraphs) {
  auto val = valPlace.val;
  DenseSet<Block *> visited;
  visited.insert(curBlk);

  for (auto *suc : curBlk->getSuccessors()) {
    if (suc == curBlk)
      continue;
    visited.insert(suc);
    Value propVal = resolvePropagatedValue(val, curBlk, suc, liveIns);
    if (propVal == NULL)
      continue;
    if (propVal != val)
      updatePredecessorPlacement({propVal, valPlace.pe, valPlace.regAttr}, suc,
                                 liveIns, liveOuts, bbInitGraphs, bbFiniGraphs);

    updateLiveGraph(bbInitGraphs[suc],
                    {propVal, valPlace.pe, valPlace.regAttr});

    std::vector<std::pair<Block *, Value>> stack;
    if (liveOuts[suc].count(propVal)) {
      // update the finiGraph
      updateLiveGraph(bbFiniGraphs[suc],
                      {propVal, valPlace.pe, valPlace.regAttr});
      stack.emplace_back(suc, propVal);
    }

    while (!stack.empty()) {
      auto [cur, curVal] = stack.back();
      stack.pop_back();
      visited.insert(cur);

      for (auto *nextSuc : cur->getSuccessors()) {
        if (nextSuc == cur)
          continue;

        Value nextPropVal =
            resolvePropagatedValue(curVal, curBlk, nextSuc, liveIns);
        if (nextPropVal == NULL)
          continue;

        updateLiveGraph(bbInitGraphs[nextSuc],
                        {nextPropVal, valPlace.pe, valPlace.regAttr});

        if (liveOuts[nextSuc].count(nextPropVal)) {
          updateLiveGraph(bbFiniGraphs[nextSuc],
                          {nextPropVal, valPlace.pe, valPlace.regAttr});
          if (!visited.count(nextSuc))
            stack.emplace_back(nextSuc, nextPropVal);
        }
      }
    }
  }
}

// Update the global value placement if the initGraph and finiGraph of the
// updateBlk changed. All the other placement of other blocks are changed to
// maintain consistency of the liveness graph.
void updateGlobalValPlacement(
    Block *updateBlk, Region &region,
    std::map<Block *, SetVector<Value>> &liveIns,
    std::map<Block *, SetVector<Value>> &liveOuts,
    std::map<Block *, std::vector<ValuePlacement>> &bbInitGraphs,
    std::map<Block *, std::vector<ValuePlacement>> &bbFiniGraphs,
    bool forceInteral = false) {
  // update liveness graph if graph transformation is performed
  computeLiveValue(region, liveIns, liveOuts);

  auto &initGraph = bbInitGraphs[updateBlk];
  auto &finiGraph = bbFiniGraphs[updateBlk];

  // only value that are live in the graph
  auto removeDeadValue = [](std::vector<ValuePlacement> &graph,
                            SetVector<Value> &liveSet) {
    graph.erase(std::remove_if(graph.begin(), graph.end(),
                               [&liveSet](const ValuePlacement &val) {
                                 return liveSet.count(val.val) == 0;
                               }),
                graph.end());
  };

  removeDeadValue(initGraph, liveIns[updateBlk]);
  removeDeadValue(finiGraph, liveOuts[updateBlk]);

  llvm::errs() << "InitGraph: \n";
  for (auto val : initGraph) {
    llvm::errs() << val.val << " " << val.pe << " "
                 << static_cast<int>(val.regAttr) << "\n";
  }
  llvm::errs() << "FiniGraph: \n";
  for (auto val : finiGraph) {
    llvm::errs() << val.val << " " << val.pe << " "
                 << static_cast<int>(val.regAttr) << "\n";
  }

  // update the global value placement with the updated initGraph
  for (auto &valPlace : initGraph) {
    auto val = valPlace.val;
    auto pe = valPlace.pe;
    auto &regAttr = valPlace.regAttr;
    if (forceInteral)
      regAttr = RegAttr::IN;
    // find the corresponding value if it is live in other bb's initGraph or
    // finiGraph
    auto curBlk = updateBlk;
    updatePredecessorPlacement(valPlace, updateBlk, liveIns, liveOuts,
                               bbInitGraphs, bbFiniGraphs);
  }

  // update the global value placement with the updated finiGraph
  for (auto &valPlace : finiGraph) {
    auto val = valPlace.val;
    auto pe = valPlace.pe;
    auto &regAttr = valPlace.regAttr;
    if (forceInteral)
      regAttr = RegAttr::IN;
    // find the corresponding value if it is live in other bb's initGraph or
    // finiGraph
    auto curBlk = updateBlk;
    updateSuccessorPlacement(valPlace, updateBlk, liveIns, liveOuts,
                             bbInitGraphs, bbFiniGraphs);
  }

  // remove dead values from the global value placement
  for (auto &[blk, graph] : bbInitGraphs) {
    removeDeadValue(graph, liveIns[blk]);
  }
  for (auto &[blk, graph] : bbFiniGraphs) {
    removeDeadValue(graph, liveOuts[blk]);
  }
}

void calculateTemporalSpatialSchedule(
    Region &region,
    std::map<mlir::Operation *, compigra::ScheduleUnit> &solution,
    const std::string fileName, DenseMap<Operation *, int> &blasLatency) {
  unsigned kernelTime = 0;
  for (auto &block : region.getBlocks()) {
    int alignStartTime = kernelTime;
    int endTime = kernelTime;
    int bbStart = INT32_MAX;

    Operation *blasKernel = &block.getOperations().front();
    if (isa<cgra::BlasGemmAsmOp>(blasKernel)) {
      solution[blasKernel] = {(int)kernelTime, 0};
      kernelTime += blasLatency[blasKernel];
      continue;
    }

    for (auto &op : block.getOperations()) {
      if (solution.find(&op) == solution.end())
        continue;
      if (solution[&op].time < bbStart) {
        bbStart = solution[&op].time;
      }
    }

    auto gap = kernelTime - bbStart;
    // blockStartT[&block] = alignStartTime;
    for (auto &op : block.getOperations()) {
      if (solution.find(&op) == solution.end())
        continue;

      auto &su = solution[&op];
      su.time += gap;
      endTime = std::max(endTime, su.time);
    }
    // blockEndT[&block] = endTime + 1;
    kernelTime = endTime + 1;
  }

  std::ofstream csvFile(fileName);
  for (auto [bbInd, bb] : llvm::enumerate(region.getBlocks())) {
    for (auto &op : bb.getOperations()) {
      if (solution.find(&op) == solution.end())
        continue;
      std::string str;
      llvm::raw_string_ostream rso(str);
      rso << op;
      auto su = solution[&op];
      csvFile << rso.str() << "&" << su.time << "&" << su.pe << "&" << bbInd
              << "\r\n";
    }
  }
  csvFile.close();
  llvm::errs() << "Temporal spatial schedule is saved to " << fileName << "\n";
}

static std::map<int, placeunit> getRandomInitialPlacement(
    std::map<int, SetVector<Value>> nodes, GridAttribute attr,
    const std::vector<ValuePlacement> liveValPlacement = {}) {
  std::map<int, placeunit> initialPlacement;

  for (auto &[index, vals] : nodes) {
    if (initialPlacement.count(index) > 0)
      continue;
    // get the placement for vals

    // if find any vals in liveValPlacement, use the placement
    bool useExisting = false;
    for (auto val : vals) {
      auto it =
          std::find_if(liveValPlacement.begin(), liveValPlacement.end(),
                       [&](const ValuePlacement &vp) { return vp.val == val; });
      if (it != liveValPlacement.end()) {
        initialPlacement[index] = {it->pe, it->regAttr};
        useExisting = true;
        break;
      }
    }
    if (useExisting)
      continue;
    // auto pe = std::rand() % (attr.nRow * attr.nCol);
    // initialPlacement[index] = {pe, RegAttr::NK};
  }
  return initialPlacement;
}

double computeInitialPlacementCost(std::map<int, compigra::placeunit> result,
                                   std::map<int, SetVector<Value>> nodes,
                                   std::map<Block *, SetVector<Value>> liveIns,
                                   GridAttribute grid) {
  double cost = 0.0;
  // count the register use
  std::vector<int> regUseCount = std::vector<int>(16, 0);
  int maxReg = 0;
  for (auto &[index, place] : result) {
    if (place.second == RegAttr::EX)
      continue;
    auto pe = place.first;
    regUseCount[pe]++;
    maxReg = std::max(maxReg, regUseCount[pe]);
  }
  // limit the maximum register use to 4
  if (maxReg > 4)
    return 1e2;

  // first compute the standard deviation of the register use
  double mean = 0.0;
  for (auto useCount : regUseCount) {
    mean += useCount;
  }
  mean /= regUseCount.size();
  double variance = 0.0;
  for (auto useCount : regUseCount) {
    variance += (useCount - mean) * (useCount - mean);
  }
  variance /= regUseCount.size();
  double stdDev = std::sqrt(variance);

  // compute the affinity of the values
  double affinityCost = 0.0;
  int costsCount = 0;
  // get the key of liveVals[i] in nodes
  auto getRes = [&](Value val) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [&](const std::pair<int, SetVector<Value>> &pair) {
                             return pair.second.count(val) > 0;
                           });
    return result[it->first];
  };

  for (auto [bb, liveVals] : liveIns) {
    // check whether liveVals have the same consumers
    for (auto i = 0; i < liveVals.size(); i++) {
      for (auto j = i + 1; j < liveVals.size(); j++) {
        auto val1 = liveVals[i];
        auto val2 = liveVals[j];

        auto pe1 = getRes(val1).first;
        auto pe2 = getRes(val2).first;
        auto distance = getDistance(pe1, pe2, grid.nRow, grid.nCol);
        double maxRouting = grid.nRow / 2 + grid.nCol / 2;

        SetVector<Operation *> val1Users;
        SetVector<Operation *> val2Users;

        buildChildTree(val1, val1Users, bb);
        buildChildTree(val2, val2Users, bb);

        bool hasCommonUser = false;
        int lowerDepth = INT32_MAX;

        for (auto user : val1Users) {
          if (val2Users.contains(user)) {
            // get the index of val1 and val2 in val1Users
            auto depth1 = std::distance(
                val1Users.begin(),
                std::find(val1Users.begin(), val1Users.end(), user));
            auto depth2 = std::distance(
                val2Users.begin(),
                std::find(val2Users.begin(), val2Users.end(), user));
            // estimation of the depth
            auto depth = (depth1 + depth2) / 3 / 2;
            affinityCost += std::pow(2, -depth) * distance / maxRouting;
            hasCommonUser = true;
            lowerDepth = std::min(lowerDepth, static_cast<int>(depth));
            costsCount++;
          }
        }
        if (!hasCommonUser || lowerDepth >= 3) {
          // val1 and val2 are not strong correlated, add penalty for their
          // affinity
          affinityCost += 0.5 * (maxRouting - distance) / maxRouting;
        }
      }
    }
  }
  affinityCost = affinityCost == 0.0 ? 0.0 : affinityCost / costsCount;

  // std::string message;
  // llvm::raw_string_ostream rso(message);
  // rso << "Nodes:\n";
  // for (auto &[index, vals] : nodes) {
  //   rso << "index: " << index << "\n";
  //   for (auto val : vals) {
  //     rso << "  " << val << "\n";
  //   }
  //   rso << "Placement: " << result[index].first
  //       << " reg attr: " << result[index].second << "\n";
  //   rso << "\n";
  // }
  // rso << "Cost: " << stdDev << " " << affinityCost << " = "
  //     << (stdDev + affinityCost) << "\n";
  // logMessage(rso.str());

  cost = stdDev + affinityCost;
  return cost;
}

void optimizeAcrossBBValuePlacement(
    int nRow, int nCol, std::map<Block *, SetVector<Value>> liveIns,
    std::map<Block *, SetVector<Value>> liveOuts,
    std::map<Block *, std::vector<ValuePlacement>> &bbInitGraphs,
    std::map<Block *, std::vector<ValuePlacement>> &bbFiniGraphs,
    const std::vector<ValuePlacement> liveValPlacement = {}) {
  DenseSet<Value> allLiveValues;
  for (auto &[blk, liveIn] : liveIns) {
    for (auto val : liveIn)
      allLiveValues.insert(val);
  }

  for (auto &[blk, liveOut] : liveOuts) {
    for (auto val : liveOut)
      allLiveValues.insert(val);
  }

  std::map<int, SetVector<Value>> nodes;
  DenseSet<Value> visited;
  for (auto val : allLiveValues) {
    if (visited.count(val) > 0)
      continue;
    SetVector<Value> relatedVals;
    getAllPhiRelatedValues(val, relatedVals);

    for (auto v : relatedVals) {
      visited.insert(v);
    }
    nodes[nodes.size()] = relatedVals;
  }

  GridAttribute gridAttr = GridAttribute{nRow, nCol, 4};
  std::map<int, compigra::placeunit> optimal;
  double minCost = 1e3;
  for (auto iter = 0; iter < 10; iter++) {
    auto place = getRandomInitialPlacement(nodes, gridAttr, liveValPlacement);
    // evaluate the placement
    double cost = computeInitialPlacementCost(place, nodes, liveIns, gridAttr);
    if (cost < minCost) {
      minCost = cost;
      optimal = place;
    }
  }

  // determine the register attributes based on the placement
  // get all the PE and its corresponding located values
  std::map<int, std::vector<int>> peValues;
  for (auto &[index, place] : optimal) {
    auto pe = place.first;
    peValues[pe].push_back(index);
  }

  for (auto &[pe, valIds] : peValues) {
    int externId = -1;
    int maxUser = 0;
    for (auto index : valIds) {
      auto vals = nodes[index];
      // get the maxinum number of users
      for (auto val : vals) {
        int userCount =
            std::distance(val.getUsers().begin(), val.getUsers().end());
        if (userCount > maxUser) {
          maxUser = userCount;
          externId = index; // update the externId
        }
      }
    }

    if (maxUser >= 5 && optimal[externId].second == RegAttr::NK) {
      // assign externId to IE
      optimal[externId].second = RegAttr::IE;
    }

    for (auto index : valIds) {
      if (maxUser >= 5 && index == externId)
        continue;
      optimal[index].second = RegAttr::IN;
    }
  }

  // log the optimal placement
  std::string message;
  llvm::raw_string_ostream rso(message);
  rso << "Optimal placement:\n";
  for (auto &[index, place] : optimal) {
    rso << "Node: " << nodes[index][0] << " PE: " << place.first
        << " RegAttr: " << static_cast<int>(place.second) << "\n";
  }
  rso << "Min cost: " << minCost << "\n";
  logMessage(rso.str());

  // update the optimal placement to bbInitGraph as prerequisite
  for (auto [bb, valIns] : liveIns) {
    // get the placement of valIns
    std::vector<ValuePlacement> initGraph;
    for (auto val : valIns) {
      auto it = std::find_if(nodes.begin(), nodes.end(),
                             [&](const std::pair<int, SetVector<Value>> &pair) {
                               return pair.second.count(val) > 0;
                             });
      if (it != nodes.end() && optimal.count(it->first)) {
        auto pe = optimal.at(it->first).first;
        auto regAttr = optimal.at(it->first).second;
        initGraph.push_back(
            ValuePlacement{val, (unsigned)optimal.at(it->first).first,
                           optimal.at(it->first).second}); // pe, regAttr
      }
    }
    bbInitGraphs[bb] = initGraph;
  }

  // update the optimal placement to bbFiniGraphs as prerequisite
  for (auto [bb, valOuts] : liveOuts) {
    // get the placement of valOuts
    std::vector<ValuePlacement> finiGraph;
    for (auto val : valOuts) {
      auto it = std::find_if(nodes.begin(), nodes.end(),
                             [&](const std::pair<int, SetVector<Value>> &pair) {
                               return pair.second.count(val) > 0;
                             });
      if (it != nodes.end() && optimal.count(it->first)) {
        auto pe = optimal.at(it->first).first;
        auto regAttr = optimal.at(it->first).second;
        finiGraph.push_back(
            ValuePlacement{val, (unsigned)optimal.at(it->first).first,
                           optimal.at(it->first).second}); // pe, regAttr
      }
    }
    bbFiniGraphs[bb] = finiGraph;
  }
}

static LogicalResult preScheduleWithExternalSupport(
    func::FuncOp funcOp, std::string outputDAG, std::string pythonExectuable,
    Region &r, OpBuilder &builder,
    std::vector<ValuePlacement> &globalConstraint,
    SmallVector<Block *, 4> &preScheduledBlks,
    std::map<mlir::Operation *, compigra::ScheduleUnit> &globalSolution,
    unsigned peGridSize = 4, unsigned maxReg = 3) {
  // Find the loop block
  int bbInd = -1;
  liveVec schedulerRequirements;
  for (auto &blk : llvm::make_early_inc_range(funcOp.getBlocks())) {
    bbInd++;
    // check whether the block can be compiled using blas
    if (isa<cgra::BlasGemmAsmOp>(blk.getOperations().front())) {
      preScheduledBlks.push_back(&blk);
      continue;
    }

    bool isLoop =
        std::find(blk.getSuccessors().begin(), blk.getSuccessors().end(),
                  &blk) != blk.getSuccessors().end();
    if (!isLoop)
      continue;

    // initialize print function
    satmapit::PrintSatMapItDAG printer(blk.getTerminator());
    printer.init();
    if (failed(printer.printDAG(outputDAG + "/bb" + std::to_string(bbInd))))
      continue;

    // detect whether the python executable exist
    std::string command = pythonExectuable + " -path " + outputDAG +
                          "/ -bench bb" + std::to_string(bbInd) + " -x " +
                          std::to_string(peGridSize) + " -y " +
                          std::to_string(peGridSize) + " > " + outputDAG +
                          "/out_raw_bb" + std::to_string(bbInd) + ".sat\n";

    // call the python code script to solve the MS
    llvm::errs() << "---> Running the Modulo Scheduler: \n" << command;

    int result = system(command.c_str());
    if (result != 0)
      continue;
    llvm::errs() << "Modulo Scheduler done\n";
    int opSize = blk.getOperations().size();

    int II;
    std::map<int, Instruction> instructions;
    std::map<int, std::set<int>> opTimeMap;
    std::vector<std::set<int>> basicBlocksWithOpIds = {};
    if (failed(readMapFile(outputDAG, "bb" + std::to_string(bbInd), maxReg,
    if (failed(readMapFile(outputDAG, "bb" + std::to_string(bbInd), maxReg,
                           opSize + blk.getNumArguments() - 1, II, opTimeMap,
                           basicBlocksWithOpIds, instructions)))
      continue;

    // print instructions
    for (auto [id, inst] : instructions) {
      llvm::errs() << "Id: " << id << ", Name: " << inst.name
                   << ", Time: " << inst.time << ", PE: " << inst.pe
                   << ", Rout: " << inst.Rout << ", OpA: " << inst.opA
                   << ", OpB: " << inst.opB << "\n";
    }

    std::map<int, int> execTime = getLoopOpUnfoldExeTime(opTimeMap);
    if (!memoryConsistencySchedule(execTime, II, &blk) ||
        !kernelOverlap(basicBlocksWithOpIds))
      continue;

    llvm::errs() << "II: " << II << "\n\n";
    if (failed(initBlockArgs(&blk, instructions, builder)))
      return failure();

    ModuloScheduleAdapter adapter(r, &blk, builder, II, execTime, opTimeMap,
                                  basicBlocksWithOpIds);
    if (failed(adapter.init()))
      continue;

    if (failed(adapter.adaptCFGWithLoopMS()))
      return failure();

    // assign basic block with the schedule result
    if (failed(adapter.assignScheduleResult(instructions, schedulerRequirements,
                                            maxReg, peGridSize * peGridSize)))
      return failure();
    auto prereq = adapter.getPrerequisites();
    schedulerRequirements.insert(schedulerRequirements.end(), prereq.begin(),
                                 prereq.end());

    //  write the schedule result to global constraint with register attributes
    auto sol = adapter.getSolutions();
    for (auto [op, su] : sol)
      globalSolution[op] = su;

    for (auto &valPlace : prereq) {
      Value val = valPlace.first;
      auto pe = valPlace.second;
      RegAttr regAttr = RegAttr::IN;
      if (val.getDefiningOp() && sol.count(val.getDefiningOp())) {
        if (sol[val.getDefiningOp()].reg == maxReg)
          regAttr = RegAttr::EX;
      }

      globalConstraint.push_back(
          ValuePlacement{val, (unsigned)pe, regAttr}); // pe, regAttr
      for (auto blk : adapter.getNewBlocks()) {
        if (std::find(preScheduledBlks.begin(), preScheduledBlks.end(), blk) ==
            preScheduledBlks.end()) {
          preScheduledBlks.push_back(blk);
        }
      }
      // preScheduledBlks.insert(preScheduledBlks.end(),
      //                         adapter.getNewBlocks().begin(),
      //                         adapter.getNewBlocks().end());
    }
  }
  return success();
}

namespace {
struct FastASMGenTemporalCGRAPass
    : public compigra::impl::FastASMGenTemporalCGRABase<
          FastASMGenTemporalCGRAPass> {

  explicit FastASMGenTemporalCGRAPass(int nRow, int nCol, int mem,
                                      StringRef msOpt, StringRef asmOutDir,
                                      bool debug) {}

  void runOnOperation() override {
    ModuleOp modOp = dyn_cast<ModuleOp>(getOperation());
    auto funcOp = *modOp.getOps<func::FuncOp>().begin();
    OpBuilder builder(funcOp.getContext());
    if (asmOutDir.empty())
      asmOutDir = "out";
    std::string outDir = asmOutDir;

    Region &region = funcOp.getBody();

    std::map<Block *, SetVector<Value>> liveIns;
    std::map<Block *, SetVector<Value>> liveOuts;

    std::map<Block *, std::vector<ValuePlacement>> bbInitGraphs;
    std::map<Block *, std::vector<ValuePlacement>> bbFiniGraphs;

    unsigned maxReg = 4;
    std::vector<ValuePlacement> liveValPlacement;
    SmallVector<Block *, 4> preScheduledBlks;
    std::map<mlir::Operation *, compigra::ScheduleUnit> rawSolution;

    size_t lastSlashPos = outDir.find_last_of("/");
    bool msEnable = false;
    // if msOpt is empty, skip the pre-schedule
    if (!msOpt.empty()) {
      if (failed(preScheduleWithExternalSupport(
              funcOp, outDir.substr(0, lastSlashPos) + "/IR_opt/satmapit",
              msOpt.substr(1, msOpt.size() - 2), region, builder,
              liveValPlacement, preScheduledBlks, rawSolution, nRow, maxReg))) {
        llvm::errs() << funcOp << "\n";
        return signalPassFailure();
      } else {
        msEnable = true;
        llvm::errs() << "MS pre-schedule done\n";
      }
    }

    computeLiveValue(region, liveIns, liveOuts);
    printBlockLiveValue(region, liveIns, liveOuts);

    int bbId = 0;

    logMessage("BasicBlock op assignment\n", true);
    // initialize the initGraph and finiGraph for each block
    optimizeAcrossBBValuePlacement(nRow, nCol, liveIns, liveOuts, bbInitGraphs,
                                   bbFiniGraphs, liveValPlacement);

    for (auto &bb : region.getBlocks()) {
      llvm::errs() << "\n";
      logMessage("\nBBId: " + std::to_string(bbId) +
                     "==============================\n",
                 false, debug);

      llvm::errs() << "BBId: " + std::to_string(bbId) +
                          "==============================\n";
      bbId++;

      logMessage("InitGraph: ", false, debug);
      if (debug)
        printLiveGraph(bbInitGraphs);
      logMessage("FiniGraph:", false, debug);
      if (debug)
        printLiveGraph(bbFiniGraphs);

      // Init operation assginer
      BasicBlockOpAssignment bbOpAssignment(&bb, maxReg, nRow, nCol, builder);
      bbOpAssignment.DebugMode = debug;
      llvm::errs() << "DEBUG MODE: " << debug << "\n";
      auto zeroIntOp = getZeroConstant(region, builder);
      auto zeroFloatOp = getZeroConstant(region, builder, true);
      bbOpAssignment.setUpZeroOp(zeroIntOp, zeroFloatOp);

      // set up liveness prerequisite
      bbOpAssignment.setPrerequisiteToStartGraph(bbInitGraphs[&bb]);
      bbOpAssignment.setPrerequisiteToFinishGraph(bbFiniGraphs[&bb]);

      if (std::find(preScheduledBlks.begin(), preScheduledBlks.end(), &bb) !=
          preScheduledBlks.end())
        continue;
      if (succeeded(
              bbOpAssignment.mappingBBdataflowToCGRA(liveIns, liveOuts))) {
        auto soluBB = bbOpAssignment.getSolution();
        // write soluBB into rawSolution
        for (auto [op, unit] : soluBB) {
          rawSolution[op] = unit;
        }
      } else {
        // DEBUG, print the liveIn and liveOut and their placement
        llvm::errs() << "Failed to map BB dataflow to CGRA\n";
        llvm::errs() << "LiveIn: ";
        for (auto val : liveIns[&bb]) {
          if (val.isa<BlockArgument>()) {
            for (auto [ind, bb] : llvm::enumerate(region.getBlocks()))
              if (&bb == val.getParentBlock()) {
                llvm::errs() << ind << " ";
                break;
              }
          }
          llvm::errs() << val << ": ";
          if (bbInitGraphs[&bb].empty()) {
            llvm::errs() << "No placement\n";
          } else {
            for (auto valPlace : bbInitGraphs[&bb]) {
              if (valPlace.val == val) {
                llvm::errs() << "[" << valPlace.pe << " "
                             << static_cast<int>(valPlace.regAttr) << "]\n";
              }
            }
          }
        }
        llvm::errs() << "LiveOut: ";
        for (auto val : liveOuts[&bb]) {
          if (val.isa<BlockArgument>()) {
            for (auto [ind, bb] : llvm::enumerate(region.getBlocks()))
              if (&bb == val.getParentBlock()) {
                llvm::errs() << ind << " ";
                break;
              }
          }
          llvm::errs() << val << ": ";
          if (bbFiniGraphs[&bb].empty()) {
            llvm::errs() << "No placement\n";
          } else {
            for (auto valPlace : bbFiniGraphs[&bb]) {
              if (valPlace.val == val) {
                llvm::errs() << "[" << valPlace.pe << " "
                             << static_cast<int>(valPlace.regAttr) << "]\n";
              }
            }
          }
        }

        return;
        return signalPassFailure();
      }

      // print the initGraph and finiGraph of the block
      auto initGraph = bbOpAssignment.getStartEmbeddingGraph();
      auto finiGraph = bbOpAssignment.getFiniEmbeddingGraph();

      bbInitGraphs[&bb] = initGraph;
      bbFiniGraphs[&bb] = finiGraph;
      // update the liveIn and liveOut with the initGraph and finiGraph
      // computeLiveValue(region, liveIns, liveOuts);
      updateGlobalValPlacement(&bb, region, liveIns, liveOuts, bbInitGraphs,
                               bbFiniGraphs, msEnable);
      logMessage("InitGraph: ", false, debug);
      if (debug)
        printLiveGraph(bbInitGraphs);
      logMessage("FiniGraph:", false, debug);
      if (debug)
        printLiveGraph(bbFiniGraphs);

      // if (bbId == 4)
      //   break;
    }

    // perform register allocation
    OpenEdgeASMGen asmGen(region, maxReg, nRow);
    // organize the rawSolution to a final solution
    DenseMap<Operation *, int> blasLatency;
    for (auto blasOp : funcOp.getOps<cgra::BlasGemmAsmOp>()) {
      int latencyCC = -1;
      if (nRow == 4 && nCol == 4)
        latencyCC = 27;
      else if (nRow == 3 && nCol == 3)
        latencyCC = 45;
      else
        return signalPassFailure();
      if (blasOp->getAttr("mulASM"))
        latencyCC++;
      if (blasOp->getAttr("addASM"))
        latencyCC++;
      if (blasOp->getAttr("preStoreAsm")) {
        // get the array attr size
        auto arrayAttr = blasOp->getAttrOfType<ArrayAttr>("preStoreAsm");
        latencyCC += arrayAttr.size();
      }
      asmGen.setBlasKernelLatency(blasOp, latencyCC);
      blasLatency[blasOp] = latencyCC;
      llvm::errs() << blasOp << " latency: " << latencyCC << "\n";
    }
    calculateTemporalSpatialSchedule(
        region, rawSolution, "space_temporal_assignment.csv", blasLatency);

    asmGen.setSolution(rawSolution);
    // asmGen.setRFAccessModel(RFAccessModel::RF_READ);
    if (failed(asmGen.allocateRegisters())) {
      llvm::errs() << "Failed to allocate registers\n";
      return signalPassFailure();
    }
    asmGen.printKnownSchedule(true, 0, outDir);
  };
};
} // namespace

namespace compigra {
std::unique_ptr<mlir::Pass>
createFastASMGenTemporalCGRA(int nRow, int nCol, int mem, StringRef msOpt,
                             StringRef asmOutDir, bool debug) {
  return std::make_unique<FastASMGenTemporalCGRAPass>(nRow, nCol, mem, msOpt,
                                                      asmOutDir, debug);
}
} // namespace compigra
//===-  PolyhedraSchedule.h - Transform to catch mmul Pattern-*- C++ ---*-===//
//
// Compigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Declares functions to schedule statements in polyhedra model to catch mmul
// pattern.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#ifdef HAVE_Z3
#include "z3++.h"
#endif

using namespace mlir;

bool isSinglePathStore(Value srcVal, affine::AffineStoreOp storeOp);

void getAllStatements(affine::AffineForOp outerFor,
                      SetVector<Operation *> &statements);

std::vector<std::vector<std::vector<int>>>
generateScheduleFunction(const SetVector<Operation *> &statements,
                         const SmallVector<Operation *> &blasOps,
                         int scheduleDimensions = 7);

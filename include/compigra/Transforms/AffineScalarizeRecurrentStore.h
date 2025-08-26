//===-  AffineExploitGemm.h - Expand the scope of gemm kernels -*-- C++ -*-===//
//
// Compigra is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Declares the --affine-maximize-gemm pass, which detects and maximize the gemm
// kernels through loop transformations.
//
//===----------------------------------------------------------------------===//
#ifndef COMPIGRA_AFFINE_SCALARIZE_RECURRENTSTORE_H
#define COMPIGRA_AFFINE_SCALARIZE_RECURRENTSTORE_H

#include "compigra/CgraDialect.h"
#include "compigra/CgraOps.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;
namespace compigra {

#define GEN_PASS_DEF_AFFINESCALARIZESTORE
#define GEN_PASS_DECL_AFFINESCALARIZESTORE
#include "compigra/Transforms/Passes.h.inc"

std::unique_ptr<mlir::Pass> createAffineScalarizeStore();

} // namespace compigra

#endif // COMPIGRA_AFFINE_SCALARIZE_RECURRENTSTORE_H
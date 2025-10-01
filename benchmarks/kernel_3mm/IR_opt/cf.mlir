module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_3mm(%arg0: memref<4x4xi32>, %arg1: memref<4x4xi32>, %arg2: memref<4x4xi32>, %arg3: memref<4x4xi32>, %arg4: memref<4x4xi32>, %arg5: memref<4x4xi32>, %arg6: memref<4x4xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c4_i32 = arith.constant 4 : i32
    cgra.gemm_blas [%arg0 : memref<4x4xi32>, %arg1 : memref<4x4xi32>, %arg4 : memref<4x4xi32>, %c4_i32 : i32, %c4_i32 : i32, %c4_i32 : i32]
    cgra.gemm_blas [%arg2 : memref<4x4xi32>, %arg3 : memref<4x4xi32>, %arg5 : memref<4x4xi32>, %c4_i32 : i32, %c4_i32 : i32, %c4_i32 : i32]
    cgra.gemm_blas [%arg4 : memref<4x4xi32>, %arg5 : memref<4x4xi32>, %arg6 : memref<4x4xi32>, %c4_i32 : i32, %c4_i32 : i32, %c4_i32 : i32]
    return
  }
}


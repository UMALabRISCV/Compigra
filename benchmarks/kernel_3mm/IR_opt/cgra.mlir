module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_3mm(%arg0: memref<4x4xi32>, %arg1: memref<4x4xi32>, %arg2: memref<4x4xi32>, %arg3: memref<4x4xi32>, %arg4: memref<4x4xi32>, %arg5: memref<4x4xi32>, %arg6: memref<4x4xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c65044_i32 = arith.constant 65044 : i32
    %c65040_i32 = arith.constant 65040 : i32
    %c65036_i32 = arith.constant 65036 : i32
    %c65032_i32 = arith.constant 65032 : i32
    %c65028_i32 = arith.constant 65028 : i32
    %c65024_i32 = arith.constant 65024 : i32
    %c65044_i32_0 = arith.constant 65044 : i32
    %c65040_i32_1 = arith.constant 65040 : i32
    %c65036_i32_2 = arith.constant 65036 : i32
    %c65032_i32_3 = arith.constant 65032 : i32
    %c65028_i32_4 = arith.constant 65028 : i32
    %c65024_i32_5 = arith.constant 65024 : i32
    %c65044_i32_6 = arith.constant 65044 : i32
    %c65040_i32_7 = arith.constant 65040 : i32
    %c65036_i32_8 = arith.constant 65036 : i32
    %c65032_i32_9 = arith.constant 65032 : i32
    %c65028_i32_10 = arith.constant 65028 : i32
    %c65024_i32_11 = arith.constant 65024 : i32
    %c4_i32 = arith.constant {blas = "2", blas_arg = "5"} 4 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0", blas = "0", blas_arg = "0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1", blas = "0", blas_arg = "1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "0", blas_arg = "2"}
    cgra.swi %0, %c65024_i32_11 : i32, i32
    cgra.swi %1, %c65028_i32_10 : i32, i32
    cgra.swi %2, %c65032_i32_9 : i32, i32
    cgra.swi %c4_i32, %c65036_i32_8 : i32, i32
    cgra.swi %c4_i32, %c65040_i32_7 : i32, i32
    cgra.swi %c4_i32, %c65044_i32_6 : i32, i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    "cgra.gemm_blas_asm"()[^bb2] {blas = "0"} : () -> ()
  ^bb2:  // pred: ^bb1
    %3 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "1", blas_arg = "0"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg3", blas = "1", blas_arg = "1"}
    %5 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "1", blas_arg = "2"}
    cgra.swi %3, %c65024_i32_5 : i32, i32
    cgra.swi %4, %c65028_i32_4 : i32, i32
    cgra.swi %5, %c65032_i32_3 : i32, i32
    cgra.swi %c4_i32, %c65036_i32_2 : i32, i32
    cgra.swi %c4_i32, %c65040_i32_1 : i32, i32
    cgra.swi %c4_i32, %c65044_i32_0 : i32, i32
    cf.br ^bb3
  ^bb3:  // pred: ^bb2
    "cgra.gemm_blas_asm"()[^bb4] {blas = "1"} : () -> ()
  ^bb4:  // pred: ^bb3
    %6 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "2", blas_arg = "0"}
    %7 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "2", blas_arg = "1"}
    %8 = cgra.lwd -> i32, {BaseAddr = "arg6", blas = "2", blas_arg = "2"}
    cgra.swi %6, %c65024_i32 : i32, i32
    cgra.swi %7, %c65028_i32 : i32, i32
    cgra.swi %8, %c65032_i32 : i32, i32
    cgra.swi %c4_i32, %c65036_i32 : i32, i32
    cgra.swi %c4_i32, %c65040_i32 : i32, i32
    cgra.swi %c4_i32, %c65044_i32 : i32, i32
    cf.br ^bb5
  ^bb5:  // pred: ^bb4
    "cgra.gemm_blas_asm"()[^bb6] {blas = "2"} : () -> ()
  ^bb6:  // pred: ^bb5
    return
  }
}


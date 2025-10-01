module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_2mm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<8x8xi32>, %arg3: memref<8x8xi32>, %arg4: memref<8x8xi32>, %arg5: memref<8x8xi32>, %arg6: memref<8x8xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c8 = arith.constant 8 : index
    %c0 = arith.constant 0 : index
    %c1_0 = arith.constant 1 : index
    %c8_1 = arith.constant 8 : index
    %c0_2 = arith.constant 0 : index
    %c65044_i32 = arith.constant 65044 : i32
    %c65040_i32 = arith.constant 65040 : i32
    %c65036_i32 = arith.constant 65036 : i32
    %c65032_i32 = arith.constant 65032 : i32
    %c65028_i32 = arith.constant 65028 : i32
    %c65024_i32 = arith.constant 65024 : i32
    %c65044_i32_3 = arith.constant 65044 : i32
    %c65040_i32_4 = arith.constant 65040 : i32
    %c65036_i32_5 = arith.constant 65036 : i32
    %c65032_i32_6 = arith.constant 65032 : i32
    %c65028_i32_7 = arith.constant 65028 : i32
    %c65024_i32_8 = arith.constant 65024 : i32
    %c8_i32 = arith.constant {blas = "0", blas_arg = "3"} 8 : i32
    %c8_i32_9 = arith.constant {blas = "0", blas_arg = "5"} 8 : i32
    %c8_i32_10 = arith.constant {blas = "0", blas_arg = "4"} 8 : i32
    %c8_i32_11 = arith.constant {blas = "1", blas_arg = "3"} 8 : i32
    %c8_i32_12 = arith.constant {blas = "1", blas_arg = "5"} 8 : i32
    %c8_i32_13 = arith.constant {blas = "1", blas_arg = "4"} 8 : i32
    %c6_i32 = arith.constant 6 : i32
    %c8_i32_14 = arith.constant {DimProd = 0 : i32, alloc = 0 : i32} 8 : i32
    %c116736_i32 = arith.constant {BaseAddr = "alloc0", blas = "1", blas_arg = "2"} 116736 : i32
    %c4_i32 = arith.constant 4 : i32
    %c8_i32_15 = arith.constant {DimProd = 0 : i32, arg = 6 : i32} 8 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg6"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg3", blas = "0", blas_arg = "0"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "0", blas_arg = "1"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "0", blas_arg = "2"}
    cgra.swi %1, %c65024_i32_8 : i32, i32
    cgra.swi %2, %c65028_i32_7 : i32, i32
    cgra.swi %3, %c65032_i32_6 : i32, i32
    cgra.swi %c8_i32, %c65036_i32_5 : i32, i32
    cgra.swi %c8_i32_10, %c65040_i32_4 : i32, i32
    cgra.swi %c8_i32_9, %c65044_i32_3 : i32, i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    "cgra.gemm_blas_asm"()[^bb2] {blas = "0", mulASM = "SMUL R3, R3, 3"} : () -> ()
  ^bb2:  // pred: ^bb1
    %4 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "1", blas_arg = "0"}
    %5 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "1", blas_arg = "1"}
    cgra.swi %4, %c65024_i32 : i32, i32
    cgra.swi %5, %c65028_i32 : i32, i32
    cgra.swi %c116736_i32, %c65032_i32 : i32, i32
    cgra.swi %c8_i32_11, %c65036_i32 : i32, i32
    cgra.swi %c8_i32_13, %c65040_i32 : i32, i32
    cgra.swi %c8_i32_12, %c65044_i32 : i32, i32
    cf.br ^bb3
  ^bb3:  // pred: ^bb2
    "cgra.gemm_blas_asm"()[^bb4] {blas = "1"} : () -> ()
  ^bb4:  // pred: ^bb3
    cf.br ^bb5(%c0_2 : index)
  ^bb5(%6: index):  // 2 preds: ^bb4, ^bb9
    %7 = arith.index_cast %6 : index to i32
    %8 = arith.index_cast %c8_1 : index to i32
    cgra.cond_br<ge> [%7 : i32, %8 : i32], ^bb10, ^bb6
  ^bb6:  // pred: ^bb5
    cf.br ^bb7(%c0 : index)
  ^bb7(%9: index):  // 2 preds: ^bb6, ^bb8
    %10 = arith.index_cast %9 : index to i32
    %11 = arith.index_cast %c8 : index to i32
    cgra.cond_br<ge> [%10 : i32, %11 : i32], ^bb9, ^bb8
  ^bb8:  // pred: ^bb7
    %12 = arith.index_cast %6 : index to i32
    %13 = arith.muli %12, %c8_i32_15 : i32
    %14 = arith.index_cast %9 : index to i32
    %15 = arith.addi %13, %14 : i32
    %16 = arith.muli %15, %c4_i32 : i32
    %17 = arith.addi %0, %16 : i32
    %18 = cgra.lwi %17 : i32->i32
    %19 = arith.muli %18, %c6_i32 : i32
    %20 = arith.index_cast %6 : index to i32
    %21 = arith.muli %20, %c8_i32_14 : i32
    %22 = arith.index_cast %9 : index to i32
    %23 = arith.addi %21, %22 : i32
    %24 = arith.muli %23, %c4_i32 : i32
    %25 = arith.addi %c116736_i32, %24 : i32
    %26 = cgra.lwi %25 : i32->i32
    %27 = arith.addi %26, %19 : i32
    %28 = arith.index_cast %6 : index to i32
    %29 = arith.muli %28, %c8_i32_15 : i32
    %30 = arith.index_cast %9 : index to i32
    %31 = arith.addi %29, %30 : i32
    %32 = arith.muli %31, %c4_i32 : i32
    %33 = arith.addi %0, %32 : i32
    cgra.swi %27, %33 : i32, i32
    %34 = arith.addi %9, %c1 : index
    cf.br ^bb7(%34 : index)
  ^bb9:  // pred: ^bb7
    %35 = arith.addi %6, %c1_0 : index
    cf.br ^bb5(%35 : index)
  ^bb10:  // pred: ^bb5
    return
  }
}


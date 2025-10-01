module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_2mm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<8x8xi32>, %arg3: memref<8x8xi32>, %arg4: memref<8x8xi32>, %arg5: memref<8x8xi32>, %arg6: memref<8x8xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c6_i32 = arith.constant 6 : i32
    %c8_i32 = arith.constant 8 : i32
    %c8_i32_0 = arith.constant 8 : i32
    %c8_i32_1 = arith.constant 8 : i32
    %c8_i32_2 = arith.constant 8 : i32
    %c8_i32_3 = arith.constant 8 : i32
    %c8_i32_4 = arith.constant 8 : i32
    %c8_i32_5 = arith.constant 8 : i32
    %c8_i32_6 = arith.constant 8 : i32
    %c8_i32_7 = arith.constant 8 : i32
    %c8_i32_8 = arith.constant 8 : i32
    %c8_i32_9 = arith.constant 8 : i32
    %c8_i32_10 = arith.constant 8 : i32
    %c8_i32_11 = arith.constant 8 : i32
    %c3_i32 = arith.constant 3 : i32
    %c0_i32 = arith.constant 0 : i32
    cgra.gemm_blas [%arg3 : memref<8x8xi32>, %arg4 : memref<8x8xi32>, %arg2 : memref<8x8xi32>, %c8_i32_9 : i32, %c8_i32_7 : i32, %c8_i32_8 : i32] {mulASM = "SMUL R3, R3, 3"}
    %c8_i32_12 = arith.constant 8 : i32
    %alloc = memref.alloc() : memref<8x8xi32>
    cgra.gemm_blas [%arg2 : memref<8x8xi32>, %arg5 : memref<8x8xi32>, %alloc : memref<8x8xi32>, %c8_i32_4 : i32, %c8_i32_2 : i32, %c8_i32_3 : i32]
    %c0 = arith.constant 0 : index
    %c8 = arith.constant 8 : index
    %c1 = arith.constant 1 : index
    cf.br ^bb1(%c0 : index)
  ^bb1(%0: index):  // 2 preds: ^bb0, ^bb5
    %1 = arith.cmpi slt, %0, %c8 : index
    cf.cond_br %1, ^bb2, ^bb6
  ^bb2:  // pred: ^bb1
    %c0_13 = arith.constant 0 : index
    %c8_14 = arith.constant 8 : index
    %c1_15 = arith.constant 1 : index
    cf.br ^bb3(%c0_13 : index)
  ^bb3(%2: index):  // 2 preds: ^bb2, ^bb4
    %3 = arith.cmpi slt, %2, %c8_14 : index
    cf.cond_br %3, ^bb4, ^bb5
  ^bb4:  // pred: ^bb3
    %4 = memref.load %arg6[%0, %2] : memref<8x8xi32>
    %5 = arith.muli %4, %c6_i32 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32_16 = arith.constant 0 : i32
    %c0_i32_17 = arith.constant 0 : i32
    %6 = memref.load %alloc[%0, %2] : memref<8x8xi32>
    %7 = arith.addi %6, %5 : i32
    memref.store %7, %arg6[%0, %2] : memref<8x8xi32>
    %8 = arith.addi %2, %c1_15 : index
    cf.br ^bb3(%8 : index)
  ^bb5:  // pred: ^bb3
    %9 = arith.addi %0, %c1 : index
    cf.br ^bb1(%9 : index)
  ^bb6:  // pred: ^bb1
    %c8_i32_18 = arith.constant 8 : i32
    return
  }
}


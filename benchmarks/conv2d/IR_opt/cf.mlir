module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @conv2d(%arg0: memref<12x12xi32>, %arg1: memref<3x3xi32>, %arg2: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c10_i32 = arith.constant 10 : i32
    %c10_i32_0 = arith.constant 10 : i32
    %c10_i32_1 = arith.constant 10 : i32
    %c3_i32 = arith.constant 3 : i32
    %c3_i32_2 = arith.constant 3 : i32
    %c3_i32_3 = arith.constant 3 : i32
    %c0 = arith.constant 0 : index
    %c10 = arith.constant 10 : index
    %c1 = arith.constant 1 : index
    cf.br ^bb1(%c0 : index)
  ^bb1(%0: index):  // 2 preds: ^bb0, ^bb11
    %1 = arith.cmpi slt, %0, %c10 : index
    cf.cond_br %1, ^bb2, ^bb12
  ^bb2:  // pred: ^bb1
    %c0_4 = arith.constant 0 : index
    %c10_5 = arith.constant 10 : index
    %c1_6 = arith.constant 1 : index
    cf.br ^bb3(%c0_4 : index)
  ^bb3(%2: index):  // 2 preds: ^bb2, ^bb10
    %3 = arith.cmpi slt, %2, %c10_5 : index
    cf.cond_br %3, ^bb4, ^bb11
  ^bb4:  // pred: ^bb3
    memref.store %c0_i32, %arg2[%0, %2] : memref<10x10xi32>
    %c0_7 = arith.constant 0 : index
    %c3 = arith.constant 3 : index
    %c1_8 = arith.constant 1 : index
    cf.br ^bb5(%c0_7 : index)
  ^bb5(%4: index):  // 2 preds: ^bb4, ^bb9
    %5 = arith.cmpi slt, %4, %c3 : index
    cf.cond_br %5, ^bb6, ^bb10
  ^bb6:  // pred: ^bb5
    %c0_9 = arith.constant 0 : index
    %c3_10 = arith.constant 3 : index
    %c1_11 = arith.constant 1 : index
    cf.br ^bb7(%c0_9 : index)
  ^bb7(%6: index):  // 2 preds: ^bb6, ^bb8
    %7 = arith.cmpi slt, %6, %c3_10 : index
    cf.cond_br %7, ^bb8, ^bb9
  ^bb8:  // pred: ^bb7
    %8 = arith.addi %0, %4 : index
    %9 = arith.addi %2, %6 : index
    %10 = memref.load %arg0[%8, %9] : memref<12x12xi32>
    %11 = memref.load %arg1[%4, %6] : memref<3x3xi32>
    %12 = arith.muli %10, %11 : i32
    %13 = memref.load %arg2[%0, %2] : memref<10x10xi32>
    %14 = arith.addi %13, %12 : i32
    memref.store %14, %arg2[%0, %2] : memref<10x10xi32>
    %15 = arith.addi %6, %c1_11 : index
    cf.br ^bb7(%15 : index)
  ^bb9:  // pred: ^bb7
    %c3_i32_12 = arith.constant 3 : i32
    %c3_i32_13 = arith.constant 3 : i32
    %16 = arith.addi %4, %c1_8 : index
    cf.br ^bb5(%16 : index)
  ^bb10:  // pred: ^bb5
    %c3_i32_14 = arith.constant 3 : i32
    %17 = arith.addi %2, %c1_6 : index
    cf.br ^bb3(%17 : index)
  ^bb11:  // pred: ^bb3
    %c10_i32_15 = arith.constant 10 : i32
    %c10_i32_16 = arith.constant 10 : i32
    %18 = arith.addi %0, %c1 : index
    cf.br ^bb1(%18 : index)
  ^bb12:  // pred: ^bb1
    %c10_i32_17 = arith.constant 10 : i32
    return
  }
}


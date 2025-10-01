module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @gemm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<10x10xi32>, %arg3: memref<10x10xi32>, %arg4: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0 = arith.constant 0 : index
    %0 = memref.load %arg0[%c0] : memref<1xi32>
    %c10_i32 = arith.constant 10 : i32
    %c10_i32_0 = arith.constant 10 : i32
    %c10_i32_1 = arith.constant 10 : i32
    %c10_i32_2 = arith.constant 10 : i32
    %c10_i32_3 = arith.constant 10 : i32
    %c0_4 = arith.constant 0 : index
    %1 = memref.load %arg1[%c0_4] : memref<1xi32>
    %c0_5 = arith.constant 0 : index
    %c10 = arith.constant 10 : index
    %c1 = arith.constant 1 : index
    cf.br ^bb1(%c0_5 : index)
  ^bb1(%2: index):  // 2 preds: ^bb0, ^bb11
    %3 = arith.cmpi slt, %2, %c10 : index
    cf.cond_br %3, ^bb2, ^bb12
  ^bb2:  // pred: ^bb1
    %c0_6 = arith.constant 0 : index
    %c10_7 = arith.constant 10 : index
    %c1_8 = arith.constant 1 : index
    cf.br ^bb3(%c0_6 : index)
  ^bb3(%4: index):  // 2 preds: ^bb2, ^bb4
    %5 = arith.cmpi slt, %4, %c10_7 : index
    cf.cond_br %5, ^bb4, ^bb5
  ^bb4:  // pred: ^bb3
    %6 = memref.load %arg2[%2, %4] : memref<10x10xi32>
    %7 = arith.muli %6, %1 : i32
    memref.store %7, %arg2[%2, %4] : memref<10x10xi32>
    %8 = arith.addi %4, %c1_8 : index
    cf.br ^bb3(%8 : index)
  ^bb5:  // pred: ^bb3
    %c0_9 = arith.constant 0 : index
    %c10_10 = arith.constant 10 : index
    %c1_11 = arith.constant 1 : index
    cf.br ^bb6(%c0_9 : index)
  ^bb6(%9: index):  // 2 preds: ^bb5, ^bb10
    %10 = arith.cmpi slt, %9, %c10_10 : index
    cf.cond_br %10, ^bb7, ^bb11
  ^bb7:  // pred: ^bb6
    %c0_12 = arith.constant 0 : index
    %c10_13 = arith.constant 10 : index
    %c1_14 = arith.constant 1 : index
    cf.br ^bb8(%c0_12 : index)
  ^bb8(%11: index):  // 2 preds: ^bb7, ^bb9
    %12 = arith.cmpi slt, %11, %c10_13 : index
    cf.cond_br %12, ^bb9, ^bb10
  ^bb9:  // pred: ^bb8
    %13 = memref.load %arg3[%2, %9] : memref<10x10xi32>
    %14 = arith.muli %0, %13 : i32
    %15 = memref.load %arg4[%9, %11] : memref<10x10xi32>
    %16 = arith.muli %14, %15 : i32
    %17 = memref.load %arg2[%2, %11] : memref<10x10xi32>
    %18 = arith.addi %17, %16 : i32
    memref.store %18, %arg2[%2, %11] : memref<10x10xi32>
    %19 = arith.addi %11, %c1_14 : index
    cf.br ^bb8(%19 : index)
  ^bb10:  // pred: ^bb8
    %20 = arith.addi %9, %c1_11 : index
    cf.br ^bb6(%20 : index)
  ^bb11:  // pred: ^bb6
    %21 = arith.addi %2, %c1 : index
    cf.br ^bb1(%21 : index)
  ^bb12:  // pred: ^bb1
    return
  }
}


module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @conv2d(%arg0: memref<12x12xi32>, %arg1: memref<3x3xi32>, %arg2: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c3_i32 = arith.constant 3 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c0_i32_1 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c3_i32_3 = arith.constant 3 : i32
    %c0_i32_4 = arith.constant 0 : i32
    %c0_i32_5 = arith.constant 0 : i32
    %c1_i32_6 = arith.constant 1 : i32
    %c0_i32_7 = arith.constant 0 : i32
    %c10_i32 = arith.constant 10 : i32
    %c0_i32_8 = arith.constant 0 : i32
    %c0_i32_9 = arith.constant 0 : i32
    %c1_i32_10 = arith.constant 1 : i32
    %c0_i32_11 = arith.constant 0 : i32
    %c10_i32_12 = arith.constant 10 : i32
    %c0_i32_13 = arith.constant 0 : i32
    %c0_i32_14 = arith.constant 0 : i32
    %c0_i32_15 = arith.constant 0 : i32
    %c0_i32_16 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c10_i32_17 = arith.constant {DimProd = 1 : i32, arg = 2 : i32} 10 : i32
    %c3_i32_18 = arith.constant {DimProd = 1 : i32, arg = 1 : i32} 3 : i32
    %c12_i32 = arith.constant {DimProd = 1 : i32, arg = 0 : i32} 12 : i32
    %c1_i32_19 = arith.constant 1 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = arith.addi %c0_i32_13, %c0_i32_14 {value = 0 : i32} : i32
    cf.br ^bb1(%3 : i32)
  ^bb1(%4: i32):  // 2 preds: ^bb0, ^bb11
    %5 = arith.addi %c0_i32_11, %c10_i32_12 {value = 10 : i32} : i32
    cgra.cond_br<ge> [%4 : i32, %5 : i32], ^bb12, ^bb2
  ^bb2:  // pred: ^bb1
    %6 = arith.addi %c0_i32_8, %c0_i32_9 {value = 0 : i32} : i32
    cf.br ^bb3(%6 : i32)
  ^bb3(%7: i32):  // 2 preds: ^bb2, ^bb10
    %8 = arith.addi %c0_i32_7, %c10_i32 {value = 10 : i32} : i32
    cgra.cond_br<ge> [%7 : i32, %8 : i32], ^bb11, ^bb4
  ^bb4:  // pred: ^bb3
    %9 = arith.muli %4, %c10_i32_17 : i32
    %10 = arith.addi %9, %7 : i32
    %11 = arith.muli %10, %c4_i32 : i32
    %12 = arith.addi %2, %11 : i32
    %13 = arith.addi %c0_i32_15, %c0_i32_16 {value = 0 : i32} : i32
    cgra.swi %13, %12 : i32, i32
    %14 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    cf.br ^bb5(%14 : i32)
  ^bb5(%15: i32):  // 2 preds: ^bb4, ^bb9
    %16 = arith.addi %c0_i32_2, %c3_i32_3 {value = 3 : i32} : i32
    cgra.cond_br<ge> [%15 : i32, %16 : i32], ^bb10, ^bb6
  ^bb6:  // pred: ^bb5
    %17 = arith.addi %c0_i32_0, %c0_i32_1 {value = 0 : i32} : i32
    cf.br ^bb7(%17 : i32)
  ^bb7(%18: i32):  // 2 preds: ^bb6, ^bb8
    %19 = arith.addi %c0_i32, %c3_i32 {value = 3 : i32} : i32
    cgra.cond_br<ge> [%18 : i32, %19 : i32], ^bb9, ^bb8
  ^bb8:  // pred: ^bb7
    %20 = arith.addi %4, %15 : i32
    %21 = arith.addi %7, %18 : i32
    %22 = arith.muli %20, %c12_i32 : i32
    %23 = arith.addi %22, %21 : i32
    %24 = arith.muli %23, %c4_i32 : i32
    %25 = arith.addi %0, %24 : i32
    %26 = cgra.lwi %25 : i32->i32
    %27 = arith.muli %15, %c3_i32_18 : i32
    %28 = arith.addi %27, %18 : i32
    %29 = arith.muli %28, %c4_i32 : i32
    %30 = arith.addi %1, %29 : i32
    %31 = cgra.lwi %30 : i32->i32
    %32 = arith.muli %26, %31 : i32
    %33 = arith.muli %4, %c10_i32_17 : i32
    %34 = arith.addi %33, %7 : i32
    %35 = arith.muli %34, %c4_i32 : i32
    %36 = arith.addi %2, %35 : i32
    %37 = cgra.lwi %36 : i32->i32
    %38 = arith.addi %37, %32 : i32
    %39 = arith.muli %4, %c10_i32_17 : i32
    %40 = arith.addi %39, %7 : i32
    %41 = arith.muli %40, %c4_i32 : i32
    %42 = arith.addi %2, %41 : i32
    cgra.swi %38, %42 : i32, i32
    %43 = arith.addi %18, %c1_i32_19 : i32
    cf.br ^bb7(%43 : i32)
  ^bb9:  // pred: ^bb7
    %44 = arith.addi %15, %c1_i32 : i32
    cf.br ^bb5(%44 : i32)
  ^bb10:  // pred: ^bb5
    %45 = arith.addi %7, %c1_i32_6 : i32
    cf.br ^bb3(%45 : i32)
  ^bb11:  // pred: ^bb3
    %46 = arith.addi %4, %c1_i32_10 : i32
    cf.br ^bb1(%46 : i32)
  ^bb12:  // pred: ^bb1
    return
  }
}


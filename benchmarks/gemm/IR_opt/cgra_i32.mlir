module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @gemm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<10x10xi32>, %arg3: memref<10x10xi32>, %arg4: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c10_i32 = arith.constant 10 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c0_i32_1 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c10_i32_3 = arith.constant 10 : i32
    %c0_i32_4 = arith.constant 0 : i32
    %c0_i32_5 = arith.constant 0 : i32
    %c1_i32_6 = arith.constant 1 : i32
    %c0_i32_7 = arith.constant 0 : i32
    %c10_i32_8 = arith.constant 10 : i32
    %c0_i32_9 = arith.constant 0 : i32
    %c0_i32_10 = arith.constant 0 : i32
    %c1_i32_11 = arith.constant 1 : i32
    %c0_i32_12 = arith.constant 0 : i32
    %c10_i32_13 = arith.constant 10 : i32
    %c0_i32_14 = arith.constant 0 : i32
    %c0_i32_15 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c10_i32_16 = arith.constant {DimProd = 0 : i32, arg = 4 : i32} 10 : i32
    %c10_i32_17 = arith.constant {DimProd = 0 : i32, arg = 3 : i32} 10 : i32
    %c10_i32_18 = arith.constant {DimProd = 0 : i32, arg = 2 : i32} 10 : i32
    %c1_i32_19 = arith.constant 1 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %5 = cgra.lwi %0 : i32->i32
    %6 = cgra.lwi %1 : i32->i32
    %7 = arith.addi %c0_i32_14, %c0_i32_15 {constant = 0 : i32} : i32
    cf.br ^bb1(%7 : i32)
  ^bb1(%8: i32):  // 2 preds: ^bb0, ^bb11
    %9 = arith.addi %c0_i32_12, %c10_i32_13 {constant = 10 : i32} : i32
    cgra.cond_br<ge> [%8 : i32, %9 : i32], ^bb12, ^bb2
  ^bb2:  // pred: ^bb1
    %10 = arith.addi %c0_i32_9, %c0_i32_10 {constant = 0 : i32} : i32
    cf.br ^bb3(%10 : i32)
  ^bb3(%11: i32):  // 2 preds: ^bb2, ^bb4
    %12 = arith.addi %c0_i32_7, %c10_i32_8 {constant = 10 : i32} : i32
    cgra.cond_br<ge> [%11 : i32, %12 : i32], ^bb5, ^bb4
  ^bb4:  // pred: ^bb3
    %13 = arith.muli %8, %c10_i32_18 : i32
    %14 = arith.addi %13, %11 : i32
    %15 = arith.muli %14, %c4_i32 : i32
    %16 = arith.addi %2, %15 : i32
    %17 = cgra.lwi %16 : i32->i32
    %18 = arith.muli %17, %6 : i32
    %19 = arith.muli %8, %c10_i32_18 : i32
    %20 = arith.addi %19, %11 : i32
    %21 = arith.muli %20, %c4_i32 : i32
    %22 = arith.addi %2, %21 : i32
    cgra.swi %18, %22 : i32, i32
    %23 = arith.addi %11, %c1_i32_6 : i32
    cf.br ^bb3(%23 : i32)
  ^bb5:  // pred: ^bb3
    %24 = arith.addi %c0_i32_4, %c0_i32_5 {constant = 0 : i32} : i32
    cf.br ^bb6(%24 : i32)
  ^bb6(%25: i32):  // 2 preds: ^bb5, ^bb10
    %26 = arith.addi %c0_i32_2, %c10_i32_3 {constant = 10 : i32} : i32
    cgra.cond_br<ge> [%25 : i32, %26 : i32], ^bb11, ^bb7
  ^bb7:  // pred: ^bb6
    %27 = arith.addi %c0_i32_0, %c0_i32_1 {constant = 0 : i32} : i32
    cf.br ^bb8(%27 : i32)
  ^bb8(%28: i32):  // 2 preds: ^bb7, ^bb9
    %29 = arith.addi %c0_i32, %c10_i32 {constant = 10 : i32} : i32
    cgra.cond_br<ge> [%28 : i32, %29 : i32], ^bb10, ^bb9
  ^bb9:  // pred: ^bb8
    %30 = arith.muli %8, %c10_i32_17 : i32
    %31 = arith.addi %30, %25 : i32
    %32 = arith.muli %31, %c4_i32 : i32
    %33 = arith.addi %3, %32 : i32
    %34 = cgra.lwi %33 : i32->i32
    %35 = arith.muli %5, %34 : i32
    %36 = arith.muli %25, %c10_i32_16 : i32
    %37 = arith.addi %36, %28 : i32
    %38 = arith.muli %37, %c4_i32 : i32
    %39 = arith.addi %4, %38 : i32
    %40 = cgra.lwi %39 : i32->i32
    %41 = arith.muli %35, %40 : i32
    %42 = arith.muli %8, %c10_i32_18 : i32
    %43 = arith.addi %42, %28 : i32
    %44 = arith.muli %43, %c4_i32 : i32
    %45 = arith.addi %2, %44 : i32
    %46 = cgra.lwi %45 : i32->i32
    %47 = arith.addi %46, %41 : i32
    %48 = arith.muli %8, %c10_i32_18 : i32
    %49 = arith.addi %48, %28 : i32
    %50 = arith.muli %49, %c4_i32 : i32
    %51 = arith.addi %2, %50 : i32
    cgra.swi %47, %51 : i32, i32
    %52 = arith.addi %28, %c1_i32_19 : i32
    cf.br ^bb8(%52 : i32)
  ^bb10:  // pred: ^bb8
    %53 = arith.addi %25, %c1_i32 : i32
    cf.br ^bb6(%53 : i32)
  ^bb11:  // pred: ^bb6
    %54 = arith.addi %8, %c1_i32_11 : i32
    cf.br ^bb1(%54 : i32)
  ^bb12:  // pred: ^bb1
    return
  }
}


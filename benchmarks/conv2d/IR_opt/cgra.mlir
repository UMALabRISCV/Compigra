module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @conv2d(%arg0: memref<12x12xi32>, %arg1: memref<3x3xi32>, %arg2: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c3 = arith.constant 3 : index
    %c0 = arith.constant 0 : index
    %c1_0 = arith.constant 1 : index
    %c3_1 = arith.constant 3 : index
    %c0_2 = arith.constant 0 : index
    %c1_3 = arith.constant 1 : index
    %c10 = arith.constant 10 : index
    %c0_4 = arith.constant 0 : index
    %c1_5 = arith.constant 1 : index
    %c10_6 = arith.constant 10 : index
    %c0_7 = arith.constant 0 : index
    %c0_i32 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c10_i32 = arith.constant {DimProd = 1 : i32, arg = 2 : i32} 10 : i32
    %c3_i32 = arith.constant {DimProd = 1 : i32, arg = 1 : i32} 3 : i32
    %c12_i32 = arith.constant {DimProd = 1 : i32, arg = 0 : i32} 12 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    cf.br ^bb1(%c0_7 : index)
  ^bb1(%3: index):  // 2 preds: ^bb0, ^bb11
    %4 = arith.index_cast %3 : index to i32
    %5 = arith.index_cast %c10_6 : index to i32
    cgra.cond_br<ge> [%4 : i32, %5 : i32], ^bb12, ^bb2
  ^bb2:  // pred: ^bb1
    cf.br ^bb3(%c0_4 : index)
  ^bb3(%6: index):  // 2 preds: ^bb2, ^bb10
    %7 = arith.index_cast %6 : index to i32
    %8 = arith.index_cast %c10 : index to i32
    cgra.cond_br<ge> [%7 : i32, %8 : i32], ^bb11, ^bb4
  ^bb4:  // pred: ^bb3
    %9 = arith.index_cast %3 : index to i32
    %10 = arith.muli %9, %c10_i32 : i32
    %11 = arith.index_cast %6 : index to i32
    %12 = arith.addi %10, %11 : i32
    %13 = arith.muli %12, %c4_i32 : i32
    %14 = arith.addi %2, %13 : i32
    cgra.swi %c0_i32, %14 : i32, i32
    cf.br ^bb5(%c0_2 : index)
  ^bb5(%15: index):  // 2 preds: ^bb4, ^bb9
    %16 = arith.index_cast %15 : index to i32
    %17 = arith.index_cast %c3_1 : index to i32
    cgra.cond_br<ge> [%16 : i32, %17 : i32], ^bb10, ^bb6
  ^bb6:  // pred: ^bb5
    cf.br ^bb7(%c0 : index)
  ^bb7(%18: index):  // 2 preds: ^bb6, ^bb8
    %19 = arith.index_cast %18 : index to i32
    %20 = arith.index_cast %c3 : index to i32
    cgra.cond_br<ge> [%19 : i32, %20 : i32], ^bb9, ^bb8
  ^bb8:  // pred: ^bb7
    %21 = arith.addi %3, %15 : index
    %22 = arith.addi %6, %18 : index
    %23 = arith.index_cast %21 : index to i32
    %24 = arith.muli %23, %c12_i32 : i32
    %25 = arith.index_cast %22 : index to i32
    %26 = arith.addi %24, %25 : i32
    %27 = arith.muli %26, %c4_i32 : i32
    %28 = arith.addi %0, %27 : i32
    %29 = cgra.lwi %28 : i32->i32
    %30 = arith.index_cast %15 : index to i32
    %31 = arith.muli %30, %c3_i32 : i32
    %32 = arith.index_cast %18 : index to i32
    %33 = arith.addi %31, %32 : i32
    %34 = arith.muli %33, %c4_i32 : i32
    %35 = arith.addi %1, %34 : i32
    %36 = cgra.lwi %35 : i32->i32
    %37 = arith.muli %29, %36 : i32
    %38 = arith.index_cast %3 : index to i32
    %39 = arith.muli %38, %c10_i32 : i32
    %40 = arith.index_cast %6 : index to i32
    %41 = arith.addi %39, %40 : i32
    %42 = arith.muli %41, %c4_i32 : i32
    %43 = arith.addi %2, %42 : i32
    %44 = cgra.lwi %43 : i32->i32
    %45 = arith.addi %44, %37 : i32
    %46 = arith.index_cast %3 : index to i32
    %47 = arith.muli %46, %c10_i32 : i32
    %48 = arith.index_cast %6 : index to i32
    %49 = arith.addi %47, %48 : i32
    %50 = arith.muli %49, %c4_i32 : i32
    %51 = arith.addi %2, %50 : i32
    cgra.swi %45, %51 : i32, i32
    %52 = arith.addi %18, %c1 : index
    cf.br ^bb7(%52 : index)
  ^bb9:  // pred: ^bb7
    %53 = arith.addi %15, %c1_0 : index
    cf.br ^bb5(%53 : index)
  ^bb10:  // pred: ^bb5
    %54 = arith.addi %6, %c1_3 : index
    cf.br ^bb3(%54 : index)
  ^bb11:  // pred: ^bb3
    %55 = arith.addi %3, %c1_5 : index
    cf.br ^bb1(%55 : index)
  ^bb12:  // pred: ^bb1
    return
  }
}


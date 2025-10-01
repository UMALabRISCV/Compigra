module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @gemm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<10x10xi32>, %arg3: memref<10x10xi32>, %arg4: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c10 = arith.constant 10 : index
    %c0 = arith.constant 0 : index
    %c1_0 = arith.constant 1 : index
    %c10_1 = arith.constant 10 : index
    %c0_2 = arith.constant 0 : index
    %c1_3 = arith.constant 1 : index
    %c10_4 = arith.constant 10 : index
    %c0_5 = arith.constant 0 : index
    %c1_6 = arith.constant 1 : index
    %c10_7 = arith.constant 10 : index
    %c0_8 = arith.constant 0 : index
    %c4_i32 = arith.constant 4 : i32
    %c10_i32 = arith.constant {DimProd = 0 : i32, arg = 4 : i32} 10 : i32
    %c10_i32_9 = arith.constant {DimProd = 0 : i32, arg = 3 : i32} 10 : i32
    %c10_i32_10 = arith.constant {DimProd = 0 : i32, arg = 2 : i32} 10 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %5 = cgra.lwi %0 : i32->i32
    %6 = cgra.lwi %1 : i32->i32
    cf.br ^bb1(%c0_8 : index)
  ^bb1(%7: index):  // 2 preds: ^bb0, ^bb11
    %8 = arith.index_cast %7 : index to i32
    %9 = arith.index_cast %c10_7 : index to i32
    cgra.cond_br<ge> [%8 : i32, %9 : i32], ^bb12, ^bb2
  ^bb2:  // pred: ^bb1
    cf.br ^bb3(%c0_5 : index)
  ^bb3(%10: index):  // 2 preds: ^bb2, ^bb4
    %11 = arith.index_cast %10 : index to i32
    %12 = arith.index_cast %c10_4 : index to i32
    cgra.cond_br<ge> [%11 : i32, %12 : i32], ^bb5, ^bb4
  ^bb4:  // pred: ^bb3
    %13 = arith.index_cast %7 : index to i32
    %14 = arith.muli %13, %c10_i32_10 : i32
    %15 = arith.index_cast %10 : index to i32
    %16 = arith.addi %14, %15 : i32
    %17 = arith.muli %16, %c4_i32 : i32
    %18 = arith.addi %2, %17 : i32
    %19 = cgra.lwi %18 : i32->i32
    %20 = arith.muli %19, %6 : i32
    %21 = arith.index_cast %7 : index to i32
    %22 = arith.muli %21, %c10_i32_10 : i32
    %23 = arith.index_cast %10 : index to i32
    %24 = arith.addi %22, %23 : i32
    %25 = arith.muli %24, %c4_i32 : i32
    %26 = arith.addi %2, %25 : i32
    cgra.swi %20, %26 : i32, i32
    %27 = arith.addi %10, %c1_3 : index
    cf.br ^bb3(%27 : index)
  ^bb5:  // pred: ^bb3
    cf.br ^bb6(%c0_2 : index)
  ^bb6(%28: index):  // 2 preds: ^bb5, ^bb10
    %29 = arith.index_cast %28 : index to i32
    %30 = arith.index_cast %c10_1 : index to i32
    cgra.cond_br<ge> [%29 : i32, %30 : i32], ^bb11, ^bb7
  ^bb7:  // pred: ^bb6
    cf.br ^bb8(%c0 : index)
  ^bb8(%31: index):  // 2 preds: ^bb7, ^bb9
    %32 = arith.index_cast %31 : index to i32
    %33 = arith.index_cast %c10 : index to i32
    cgra.cond_br<ge> [%32 : i32, %33 : i32], ^bb10, ^bb9
  ^bb9:  // pred: ^bb8
    %34 = arith.index_cast %7 : index to i32
    %35 = arith.muli %34, %c10_i32_9 : i32
    %36 = arith.index_cast %28 : index to i32
    %37 = arith.addi %35, %36 : i32
    %38 = arith.muli %37, %c4_i32 : i32
    %39 = arith.addi %3, %38 : i32
    %40 = cgra.lwi %39 : i32->i32
    %41 = arith.muli %5, %40 : i32
    %42 = arith.index_cast %28 : index to i32
    %43 = arith.muli %42, %c10_i32 : i32
    %44 = arith.index_cast %31 : index to i32
    %45 = arith.addi %43, %44 : i32
    %46 = arith.muli %45, %c4_i32 : i32
    %47 = arith.addi %4, %46 : i32
    %48 = cgra.lwi %47 : i32->i32
    %49 = arith.muli %41, %48 : i32
    %50 = arith.index_cast %7 : index to i32
    %51 = arith.muli %50, %c10_i32_10 : i32
    %52 = arith.index_cast %31 : index to i32
    %53 = arith.addi %51, %52 : i32
    %54 = arith.muli %53, %c4_i32 : i32
    %55 = arith.addi %2, %54 : i32
    %56 = cgra.lwi %55 : i32->i32
    %57 = arith.addi %56, %49 : i32
    %58 = arith.index_cast %7 : index to i32
    %59 = arith.muli %58, %c10_i32_10 : i32
    %60 = arith.index_cast %31 : index to i32
    %61 = arith.addi %59, %60 : i32
    %62 = arith.muli %61, %c4_i32 : i32
    %63 = arith.addi %2, %62 : i32
    cgra.swi %57, %63 : i32, i32
    %64 = arith.addi %31, %c1 : index
    cf.br ^bb8(%64 : index)
  ^bb10:  // pred: ^bb8
    %65 = arith.addi %28, %c1_0 : index
    cf.br ^bb6(%65 : index)
  ^bb11:  // pred: ^bb6
    %66 = arith.addi %7, %c1_6 : index
    cf.br ^bb1(%66 : index)
  ^bb12:  // pred: ^bb1
    return
  }
}


module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @symm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<10x10xi32>, %arg3: memref<10x10xi32>, %arg4: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c0 = arith.constant 0 : index
    %c1_0 = arith.constant 1 : index
    %c10 = arith.constant 10 : index
    %c0_1 = arith.constant 0 : index
    %c1_2 = arith.constant 1 : index
    %c10_3 = arith.constant 10 : index
    %c0_4 = arith.constant 0 : index
    %c0_i32 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c10_i32 = arith.constant {DimProd = 1 : i32, arg = 4 : i32} 10 : i32
    %c10_i32_5 = arith.constant {DimProd = 1 : i32, arg = 3 : i32} 10 : i32
    %c10_i32_6 = arith.constant {DimProd = 1 : i32, arg = 2 : i32} 10 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %5 = cgra.lwi %1 : i32->i32
    %6 = cgra.lwi %0 : i32->i32
    cf.br ^bb1(%c0_4 : index)
  ^bb1(%7: index):  // 2 preds: ^bb0, ^bb8
    %8 = arith.index_cast %7 : index to i32
    %9 = arith.index_cast %c10_3 : index to i32
    cgra.cond_br<ge> [%8 : i32, %9 : i32], ^bb9, ^bb2
  ^bb2:  // pred: ^bb1
    cf.br ^bb3(%c0_1 : index)
  ^bb3(%10: index):  // 2 preds: ^bb2, ^bb7
    %11 = arith.index_cast %10 : index to i32
    %12 = arith.index_cast %c10 : index to i32
    cgra.cond_br<ge> [%11 : i32, %12 : i32], ^bb8, ^bb4
  ^bb4:  // pred: ^bb3
    cf.br ^bb5(%c0, %c0_i32 : index, i32)
  ^bb5(%13: index, %14: i32):  // 2 preds: ^bb4, ^bb6
    %15 = arith.index_cast %13 : index to i32
    %16 = arith.index_cast %7 : index to i32
    cgra.cond_br<ge> [%15 : i32, %16 : i32], ^bb7, ^bb6
  ^bb6:  // pred: ^bb5
    %17 = arith.index_cast %7 : index to i32
    %18 = arith.muli %17, %c10_i32 : i32
    %19 = arith.index_cast %10 : index to i32
    %20 = arith.addi %18, %19 : i32
    %21 = arith.muli %20, %c4_i32 : i32
    %22 = arith.addi %4, %21 : i32
    %23 = cgra.lwi %22 : i32->i32
    %24 = arith.muli %6, %23 : i32
    %25 = arith.index_cast %7 : index to i32
    %26 = arith.muli %25, %c10_i32_5 : i32
    %27 = arith.index_cast %13 : index to i32
    %28 = arith.addi %26, %27 : i32
    %29 = arith.muli %28, %c4_i32 : i32
    %30 = arith.addi %3, %29 : i32
    %31 = cgra.lwi %30 : i32->i32
    %32 = arith.muli %24, %31 : i32
    %33 = arith.index_cast %13 : index to i32
    %34 = arith.muli %33, %c10_i32_6 : i32
    %35 = arith.index_cast %10 : index to i32
    %36 = arith.addi %34, %35 : i32
    %37 = arith.muli %36, %c4_i32 : i32
    %38 = arith.addi %2, %37 : i32
    %39 = cgra.lwi %38 : i32->i32
    %40 = arith.addi %39, %32 : i32
    %41 = arith.index_cast %13 : index to i32
    %42 = arith.muli %41, %c10_i32_6 : i32
    %43 = arith.index_cast %10 : index to i32
    %44 = arith.addi %42, %43 : i32
    %45 = arith.muli %44, %c4_i32 : i32
    %46 = arith.addi %2, %45 : i32
    cgra.swi %40, %46 : i32, i32
    %47 = arith.index_cast %13 : index to i32
    %48 = arith.muli %47, %c10_i32 : i32
    %49 = arith.index_cast %10 : index to i32
    %50 = arith.addi %48, %49 : i32
    %51 = arith.muli %50, %c4_i32 : i32
    %52 = arith.addi %4, %51 : i32
    %53 = cgra.lwi %52 : i32->i32
    %54 = arith.index_cast %7 : index to i32
    %55 = arith.muli %54, %c10_i32_5 : i32
    %56 = arith.index_cast %13 : index to i32
    %57 = arith.addi %55, %56 : i32
    %58 = arith.muli %57, %c4_i32 : i32
    %59 = arith.addi %3, %58 : i32
    %60 = cgra.lwi %59 : i32->i32
    %61 = arith.muli %53, %60 : i32
    %62 = arith.addi %14, %61 : i32
    %63 = arith.addi %13, %c1 : index
    cf.br ^bb5(%63, %62 : index, i32)
  ^bb7:  // pred: ^bb5
    %64 = arith.index_cast %7 : index to i32
    %65 = arith.muli %64, %c10_i32_6 : i32
    %66 = arith.index_cast %10 : index to i32
    %67 = arith.addi %65, %66 : i32
    %68 = arith.muli %67, %c4_i32 : i32
    %69 = arith.addi %2, %68 : i32
    %70 = cgra.lwi %69 : i32->i32
    %71 = arith.muli %5, %70 : i32
    %72 = arith.index_cast %7 : index to i32
    %73 = arith.muli %72, %c10_i32 : i32
    %74 = arith.index_cast %10 : index to i32
    %75 = arith.addi %73, %74 : i32
    %76 = arith.muli %75, %c4_i32 : i32
    %77 = arith.addi %4, %76 : i32
    %78 = cgra.lwi %77 : i32->i32
    %79 = arith.muli %6, %78 : i32
    %80 = arith.index_cast %7 : index to i32
    %81 = arith.muli %80, %c10_i32_5 : i32
    %82 = arith.index_cast %7 : index to i32
    %83 = arith.addi %81, %82 : i32
    %84 = arith.muli %83, %c4_i32 : i32
    %85 = arith.addi %3, %84 : i32
    %86 = cgra.lwi %85 : i32->i32
    %87 = arith.muli %79, %86 : i32
    %88 = arith.addi %71, %87 : i32
    %89 = arith.muli %6, %14 : i32
    %90 = arith.addi %88, %89 : i32
    %91 = arith.index_cast %7 : index to i32
    %92 = arith.muli %91, %c10_i32_6 : i32
    %93 = arith.index_cast %10 : index to i32
    %94 = arith.addi %92, %93 : i32
    %95 = arith.muli %94, %c4_i32 : i32
    %96 = arith.addi %2, %95 : i32
    cgra.swi %90, %96 : i32, i32
    %97 = arith.addi %10, %c1_0 : index
    cf.br ^bb3(%97 : index)
  ^bb8:  // pred: ^bb3
    %98 = arith.addi %7, %c1_2 : index
    cf.br ^bb1(%98 : index)
  ^bb9:  // pred: ^bb1
    return
  }
}


module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @symm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<10x10xi32>, %arg3: memref<10x10xi32>, %arg4: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32_1 = arith.constant 0 : i32
    %c10_i32 = arith.constant 10 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c0_i32_3 = arith.constant 0 : i32
    %c1_i32_4 = arith.constant 1 : i32
    %c0_i32_5 = arith.constant 0 : i32
    %c10_i32_6 = arith.constant 10 : i32
    %c0_i32_7 = arith.constant 0 : i32
    %c0_i32_8 = arith.constant 0 : i32
    %c0_i32_9 = arith.constant 0 : i32
    %c0_i32_10 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c10_i32_11 = arith.constant {DimProd = 1 : i32, arg = 4 : i32} 10 : i32
    %c10_i32_12 = arith.constant {DimProd = 1 : i32, arg = 3 : i32} 10 : i32
    %c10_i32_13 = arith.constant {DimProd = 1 : i32, arg = 2 : i32} 10 : i32
    %c1_i32_14 = arith.constant 1 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %5 = cgra.lwi %1 : i32->i32
    %6 = cgra.lwi %0 : i32->i32
    %7 = arith.addi %c0_i32_7, %c0_i32_8 {value = 0 : i32} : i32
    cf.br ^bb1(%7 : i32)
  ^bb1(%8: i32):  // 2 preds: ^bb0, ^bb8
    %9 = arith.addi %c0_i32_5, %c10_i32_6 {value = 10 : i32} : i32
    cgra.cond_br<ge> [%8 : i32, %9 : i32], ^bb9, ^bb2
  ^bb2:  // pred: ^bb1
    %10 = arith.addi %c0_i32_2, %c0_i32_3 {value = 0 : i32} : i32
    cf.br ^bb3(%10 : i32)
  ^bb3(%11: i32):  // 2 preds: ^bb2, ^bb7
    %12 = arith.addi %c0_i32_1, %c10_i32 {value = 10 : i32} : i32
    cgra.cond_br<ge> [%11 : i32, %12 : i32], ^bb8, ^bb4
  ^bb4:  // pred: ^bb3
    %13 = arith.addi %c0_i32, %c0_i32_0 {value = 0 : i32} : i32
    %14 = arith.addi %c0_i32_9, %c0_i32_10 {value = 0 : i32} : i32
    cf.br ^bb5(%13, %14 : i32, i32)
  ^bb5(%15: i32, %16: i32):  // 2 preds: ^bb4, ^bb6
    cgra.cond_br<ge> [%15 : i32, %8 : i32], ^bb7, ^bb6
  ^bb6:  // pred: ^bb5
    %17 = arith.muli %8, %c10_i32_11 : i32
    %18 = arith.addi %17, %11 : i32
    %19 = arith.muli %18, %c4_i32 : i32
    %20 = arith.addi %4, %19 : i32
    %21 = cgra.lwi %20 : i32->i32
    %22 = arith.muli %6, %21 : i32
    %23 = arith.muli %8, %c10_i32_12 : i32
    %24 = arith.addi %23, %15 : i32
    %25 = arith.muli %24, %c4_i32 : i32
    %26 = arith.addi %3, %25 : i32
    %27 = cgra.lwi %26 : i32->i32
    %28 = arith.muli %22, %27 : i32
    %29 = arith.muli %15, %c10_i32_13 : i32
    %30 = arith.addi %29, %11 : i32
    %31 = arith.muli %30, %c4_i32 : i32
    %32 = arith.addi %2, %31 : i32
    %33 = cgra.lwi %32 : i32->i32
    %34 = arith.addi %33, %28 : i32
    %35 = arith.muli %15, %c10_i32_13 : i32
    %36 = arith.addi %35, %11 : i32
    %37 = arith.muli %36, %c4_i32 : i32
    %38 = arith.addi %2, %37 : i32
    cgra.swi %34, %38 : i32, i32
    %39 = arith.muli %15, %c10_i32_11 : i32
    %40 = arith.addi %39, %11 : i32
    %41 = arith.muli %40, %c4_i32 : i32
    %42 = arith.addi %4, %41 : i32
    %43 = cgra.lwi %42 : i32->i32
    %44 = arith.muli %8, %c10_i32_12 : i32
    %45 = arith.addi %44, %15 : i32
    %46 = arith.muli %45, %c4_i32 : i32
    %47 = arith.addi %3, %46 : i32
    %48 = cgra.lwi %47 : i32->i32
    %49 = arith.muli %43, %48 : i32
    %50 = arith.addi %16, %49 : i32
    %51 = arith.addi %15, %c1_i32_14 : i32
    cf.br ^bb5(%51, %50 : i32, i32)
  ^bb7:  // pred: ^bb5
    %52 = arith.muli %8, %c10_i32_13 : i32
    %53 = arith.addi %52, %11 : i32
    %54 = arith.muli %53, %c4_i32 : i32
    %55 = arith.addi %2, %54 : i32
    %56 = cgra.lwi %55 : i32->i32
    %57 = arith.muli %5, %56 : i32
    %58 = arith.muli %8, %c10_i32_11 : i32
    %59 = arith.addi %58, %11 : i32
    %60 = arith.muli %59, %c4_i32 : i32
    %61 = arith.addi %4, %60 : i32
    %62 = cgra.lwi %61 : i32->i32
    %63 = arith.muli %6, %62 : i32
    %64 = arith.muli %8, %c10_i32_12 : i32
    %65 = arith.addi %64, %8 : i32
    %66 = arith.muli %65, %c4_i32 : i32
    %67 = arith.addi %3, %66 : i32
    %68 = cgra.lwi %67 : i32->i32
    %69 = arith.muli %63, %68 : i32
    %70 = arith.addi %57, %69 : i32
    %71 = arith.muli %6, %16 : i32
    %72 = arith.addi %70, %71 : i32
    %73 = arith.muli %8, %c10_i32_13 : i32
    %74 = arith.addi %73, %11 : i32
    %75 = arith.muli %74, %c4_i32 : i32
    %76 = arith.addi %2, %75 : i32
    cgra.swi %72, %76 : i32, i32
    %77 = arith.addi %11, %c1_i32 : i32
    cf.br ^bb3(%77 : i32)
  ^bb8:  // pred: ^bb3
    %78 = arith.addi %8, %c1_i32_4 : i32
    cf.br ^bb1(%78 : i32)
  ^bb9:  // pred: ^bb1
    return
  }
}


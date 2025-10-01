module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @bicg(%arg0: memref<30x20xf32>, %arg1: memref<20xf32>, %arg2: memref<30xf32>, %arg3: memref<20xf32>, %arg4: memref<30xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c1 = arith.constant 1 : index
    %c20 = arith.constant 20 : index
    %c0 = arith.constant 0 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c30 = arith.constant 30 : index
    %c4_i32 = arith.constant 4 : i32
    %c20_i32 = arith.constant {DimProd = 1 : i32, arg = 0 : i32} 20 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    cf.br ^bb1(%c0 : index)
  ^bb1(%5: index):  // 2 preds: ^bb0, ^bb1
    %6 = arith.index_cast %5 : index to i32
    %7 = arith.muli %6, %c4_i32 : i32
    %8 = arith.addi %1, %7 : i32
    cgra.swi %cst, %8 : f32, i32
    %9 = arith.addi %5, %c1 : index
    %10 = arith.index_cast %9 : index to i32
    %11 = arith.index_cast %c20 : index to i32
    cgra.cond_br<lt> [%10 : i32, %11 : i32], ^bb1(%9 : index), ^bb2(%c0 : index)
  ^bb2(%12: index):  // 2 preds: ^bb1, ^bb5
    %13 = arith.index_cast %12 : index to i32
    %14 = arith.index_cast %c30 : index to i32
    cgra.cond_br<ge> [%13 : i32, %14 : i32], ^bb6, ^bb3
  ^bb3:  // pred: ^bb2
    %15 = arith.index_cast %12 : index to i32
    %16 = arith.muli %15, %c4_i32 : i32
    %17 = arith.addi %2, %16 : i32
    cgra.swi %cst, %17 : f32, i32
    cf.br ^bb4(%c0 : index)
  ^bb4(%18: index):  // 2 preds: ^bb3, ^bb4
    %19 = arith.index_cast %18 : index to i32
    %20 = arith.muli %19, %c4_i32 : i32
    %21 = arith.addi %1, %20 : i32
    %22 = cgra.lwi %21 : i32->f32
    %23 = arith.index_cast %12 : index to i32
    %24 = arith.muli %23, %c4_i32 : i32
    %25 = arith.addi %4, %24 : i32
    %26 = cgra.lwi %25 : i32->f32
    %27 = arith.index_cast %12 : index to i32
    %28 = arith.muli %27, %c20_i32 : i32
    %29 = arith.index_cast %18 : index to i32
    %30 = arith.addi %28, %29 : i32
    %31 = arith.muli %30, %c4_i32 : i32
    %32 = arith.addi %0, %31 : i32
    %33 = cgra.lwi %32 : i32->f32
    %34 = arith.mulf %26, %33 : f32
    %35 = arith.addf %22, %34 : f32
    %36 = arith.index_cast %18 : index to i32
    %37 = arith.muli %36, %c4_i32 : i32
    %38 = arith.addi %1, %37 : i32
    cgra.swi %35, %38 : f32, i32
    %39 = arith.index_cast %12 : index to i32
    %40 = arith.muli %39, %c4_i32 : i32
    %41 = arith.addi %2, %40 : i32
    %42 = cgra.lwi %41 : i32->f32
    %43 = arith.index_cast %12 : index to i32
    %44 = arith.muli %43, %c20_i32 : i32
    %45 = arith.index_cast %18 : index to i32
    %46 = arith.addi %44, %45 : i32
    %47 = arith.muli %46, %c4_i32 : i32
    %48 = arith.addi %0, %47 : i32
    %49 = cgra.lwi %48 : i32->f32
    %50 = arith.index_cast %18 : index to i32
    %51 = arith.muli %50, %c4_i32 : i32
    %52 = arith.addi %3, %51 : i32
    %53 = cgra.lwi %52 : i32->f32
    %54 = arith.mulf %49, %53 : f32
    %55 = arith.addf %42, %54 : f32
    %56 = arith.index_cast %12 : index to i32
    %57 = arith.muli %56, %c4_i32 : i32
    %58 = arith.addi %2, %57 : i32
    cgra.swi %55, %58 : f32, i32
    %59 = arith.addi %18, %c1 : index
    %60 = arith.index_cast %59 : index to i32
    %61 = arith.index_cast %c20 : index to i32
    cgra.cond_br<lt> [%60 : i32, %61 : i32], ^bb4(%59 : index), ^bb5
  ^bb5:  // pred: ^bb4
    %62 = arith.addi %12, %c1 : index
    cf.br ^bb2(%62 : index)
  ^bb6:  // pred: ^bb2
    return
  }
}


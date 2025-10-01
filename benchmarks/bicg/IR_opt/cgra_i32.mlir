module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @bicg(%arg0: memref<30x20xf32>, %arg1: memref<20xf32>, %arg2: memref<30xf32>, %arg3: memref<20xf32>, %arg4: memref<30xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c20_i32 = arith.constant 20 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c20_i32_1 = arith.constant 20 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c0_i32_3 = arith.constant 0 : i32
    %c0_i32_4 = arith.constant 0 : i32
    %c0_i32_5 = arith.constant 0 : i32
    %c0_i32_6 = arith.constant 0 : i32
    %c0_i32_7 = arith.constant 0 : i32
    %c0_i32_8 = arith.constant 0 : i32
    %c30_i32 = arith.constant 30 : i32
    %c4_i32 = arith.constant 4 : i32
    %c20_i32_9 = arith.constant {DimProd = 1 : i32, arg = 0 : i32} 20 : i32
    %c1_i32 = arith.constant 1 : i32
    %cst = arith.constant 0.000000e+00 : f32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %5 = arith.addi %c0_i32_6, %c0_i32_7 {value = 0 : i32} : i32
    %6 = arith.addi %c0_i32_0, %c20_i32_1 {value = 20 : i32} : i32
    %7 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    cf.br ^bb1(%5 : i32)
  ^bb1(%8: i32):  // 2 preds: ^bb0, ^bb1
    %9 = arith.muli %8, %c4_i32 : i32
    %10 = arith.addi %1, %9 : i32
    cgra.swi %cst, %10 : f32, i32
    %11 = arith.addi %8, %c1_i32 : i32
    cgra.cond_br<lt> [%11 : i32, %6 : i32], ^bb1(%11 : i32), ^bb2(%7 : i32)
  ^bb2(%12: i32):  // 2 preds: ^bb1, ^bb5
    %13 = arith.addi %c0_i32_8, %c30_i32 {value = 30 : i32} : i32
    cgra.cond_br<ge> [%12 : i32, %13 : i32], ^bb6, ^bb3
  ^bb3:  // pred: ^bb2
    %14 = arith.muli %12, %c4_i32 : i32
    %15 = arith.addi %2, %14 : i32
    cgra.swi %cst, %15 : f32, i32
    %16 = arith.addi %c0_i32_2, %c0_i32_3 {value = 0 : i32} : i32
    %17 = arith.addi %c0_i32, %c20_i32 {value = 20 : i32} : i32
    cf.br ^bb4(%16 : i32)
  ^bb4(%18: i32):  // 2 preds: ^bb3, ^bb4
    %19 = arith.muli %18, %c4_i32 : i32
    %20 = arith.addi %1, %19 : i32
    %21 = cgra.lwi %20 : i32->f32
    %22 = arith.muli %12, %c4_i32 : i32
    %23 = arith.addi %4, %22 : i32
    %24 = cgra.lwi %23 : i32->f32
    %25 = arith.muli %12, %c20_i32_9 : i32
    %26 = arith.addi %25, %18 : i32
    %27 = arith.muli %26, %c4_i32 : i32
    %28 = arith.addi %0, %27 : i32
    %29 = cgra.lwi %28 : i32->f32
    %30 = arith.mulf %24, %29 : f32
    %31 = arith.addf %21, %30 : f32
    %32 = arith.muli %18, %c4_i32 : i32
    %33 = arith.addi %1, %32 : i32
    cgra.swi %31, %33 : f32, i32
    %34 = arith.muli %12, %c4_i32 : i32
    %35 = arith.addi %2, %34 : i32
    %36 = cgra.lwi %35 : i32->f32
    %37 = arith.muli %12, %c20_i32_9 : i32
    %38 = arith.addi %37, %18 : i32
    %39 = arith.muli %38, %c4_i32 : i32
    %40 = arith.addi %0, %39 : i32
    %41 = cgra.lwi %40 : i32->f32
    %42 = arith.muli %18, %c4_i32 : i32
    %43 = arith.addi %3, %42 : i32
    %44 = cgra.lwi %43 : i32->f32
    %45 = arith.mulf %41, %44 : f32
    %46 = arith.addf %36, %45 : f32
    %47 = arith.muli %12, %c4_i32 : i32
    %48 = arith.addi %2, %47 : i32
    cgra.swi %46, %48 : f32, i32
    %49 = arith.addi %18, %c1_i32 : i32
    cgra.cond_br<lt> [%49 : i32, %17 : i32], ^bb4(%49 : i32), ^bb5
  ^bb5:  // pred: ^bb4
    %50 = arith.addi %12, %c1_i32 : i32
    cf.br ^bb2(%50 : i32)
  ^bb6:  // pred: ^bb2
    return
  }
}


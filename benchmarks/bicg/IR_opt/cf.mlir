module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @bicg(%arg0: memref<30x20xf32>, %arg1: memref<20xf32>, %arg2: memref<30xf32>, %arg3: memref<20xf32>, %arg4: memref<30xf32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c30 = arith.constant 30 : index
    %cst = arith.constant 0.000000e+00 : f32
    %c0 = arith.constant 0 : index
    %c20 = arith.constant 20 : index
    %c1 = arith.constant 1 : index
    cf.br ^bb1(%c0 : index)
  ^bb1(%0: index):  // 2 preds: ^bb0, ^bb1
    memref.store %cst, %arg1[%0] : memref<20xf32>
    %1 = arith.addi %0, %c1 : index
    %2 = arith.cmpi slt, %1, %c20 : index
    cf.cond_br %2, ^bb1(%1 : index), ^bb2(%c0 : index)
  ^bb2(%3: index):  // 2 preds: ^bb1, ^bb5
    %4 = arith.cmpi slt, %3, %c30 : index
    cf.cond_br %4, ^bb3, ^bb6
  ^bb3:  // pred: ^bb2
    memref.store %cst, %arg2[%3] : memref<30xf32>
    cf.br ^bb4(%c0 : index)
  ^bb4(%5: index):  // 2 preds: ^bb3, ^bb4
    %6 = memref.load %arg1[%5] : memref<20xf32>
    %7 = memref.load %arg4[%3] : memref<30xf32>
    %8 = memref.load %arg0[%3, %5] : memref<30x20xf32>
    %9 = arith.mulf %7, %8 : f32
    %10 = arith.addf %6, %9 : f32
    memref.store %10, %arg1[%5] : memref<20xf32>
    %11 = memref.load %arg2[%3] : memref<30xf32>
    %12 = memref.load %arg0[%3, %5] : memref<30x20xf32>
    %13 = memref.load %arg3[%5] : memref<20xf32>
    %14 = arith.mulf %12, %13 : f32
    %15 = arith.addf %11, %14 : f32
    memref.store %15, %arg2[%3] : memref<30xf32>
    %16 = arith.addi %5, %c1 : index
    %17 = arith.cmpi slt, %16, %c20 : index
    cf.cond_br %17, ^bb4(%16 : index), ^bb5
  ^bb5:  // pred: ^bb4
    %18 = arith.addi %3, %c1 : index
    cf.br ^bb2(%18 : index)
  ^bb6:  // pred: ^bb2
    return
  }
}


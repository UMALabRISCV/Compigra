Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
Set parameter Username
Academic license - for non-commercial use only - expires 2026-07-25
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
    %1 = cgra.lwd -> i32, {BaseAddr = "arg0"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %3 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg1"}
    %5 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %6 = cgra.lwd -> i32, {BaseAddr = "arg2"}
    %7 = cgra.lwd -> i32, {BaseAddr = "arg3"}
    %8 = cgra.lwd -> i32, {BaseAddr = "arg4"}
    %9 = arith.addi %c0_i32_6, %c0_i32_7 {value = 0 : i32} : i32
    %10 = arith.addi %c0_i32_0, %c20_i32_1 {value = 20 : i32} : i32
    %11 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %12 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %13 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %14 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %c0_i32_10 = arith.constant 0 : i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    %15 = arith.addi %9, %c0_i32_10 : i32
    %16 = arith.muli %15, %c4_i32 : i32
    %17 = arith.addi %15, %c1_i32 : i32
    cf.br ^bb2(%16, %17, %17 : i32, i32, i32)
  ^bb2(%18: i32, %19: i32, %20: i32):  // 2 preds: ^bb1, ^bb2
    %21 = arith.addi %2, %18 : i32
    %22 = arith.addi %19, %c0_i32_10 : i32
    cgra.swi %cst, %21 : f32, i32
    %23 = arith.muli %22, %c4_i32 : i32
    %24 = arith.addi %22, %c1_i32 : i32
    cgra.cond_br<lt> [%20 : i32, %10 : i32], ^bb2(%23, %24, %24 : i32, i32, i32), ^bb3
  ^bb3:  // pred: ^bb2
    %25 = arith.addi %2, %23 : i32
    cgra.swi %cst, %25 : f32, i32
    %26 = arith.addi %11, %c0_i32 : i32
    %27 = arith.addi %14, %c0_i32 : i32
    %28 = arith.addi %13, %c0_i32 : i32
    %29 = arith.addi %12, %c0_i32 : i32
    cf.br ^bb4(%26, %27, %28, %29 : i32, i32, i32, i32)
  ^bb4(%30: i32, %31: i32, %32: i32, %33: i32):  // 2 preds: ^bb3, ^bb11
    %34 = arith.addi %c0_i32_8, %c30_i32 {value = 30 : i32} : i32
    cgra.cond_br<ge> [%30 : i32, %34 : i32], ^bb12, ^bb5
  ^bb5:  // pred: ^bb4
    %35 = arith.muli %30, %c4_i32 : i32
    %36 = arith.addi %35, %c0_i32 : i32
    %37 = arith.addi %36, %c0_i32 : i32
    %38 = arith.addi %37, %c0_i32 : i32
    %39 = arith.addi %5, %38 : i32
    cgra.swi %cst, %39 : f32, i32
    %40 = arith.addi %c0_i32_2, %c0_i32_3 {value = 0 : i32} : i32
    %41 = arith.addi %c0_i32, %c20_i32 {value = 20 : i32} : i32
    %c0_i32_11 = arith.constant 0 : i32
    cf.br ^bb6
  ^bb6:  // pred: ^bb5
    %42 = arith.addi %40, %c0_i32_11 : i32
    %43 = arith.muli %31, %c20_i32_9 : i32
    %44 = arith.muli %30, %c20_i32_9 : i32
    %45 = arith.muli %42, %c4_i32 : i32
    %46 = arith.addi %43, %42 : i32
    %47 = arith.muli %42, %c4_i32 : i32
    %48 = arith.addi %44, %42 : i32
    %49 = arith.addi %42, %c1_i32 : i32
    %50 = arith.addi %4, %45 : i32
    %51 = arith.muli %30, %c4_i32 : i32
    %52 = arith.muli %46, %c4_i32 : i32
    %53 = arith.muli %48, %c4_i32 : i32
    %54 = arith.muli %42, %c4_i32 : i32
    cgra.cond_br<ge> [%49 : i32, %41 : i32], ^bb10, ^bb7
  ^bb7:  // pred: ^bb6
    %55 = arith.addi %8, %51 : i32
    %56 = arith.addi %0, %52 : i32
    %57 = arith.muli %32, %c4_i32 : i32
    %58 = arith.addi %1, %53 : i32
    %59 = arith.addi %7, %54 : i32
    %60 = arith.addi %49, %c0_i32_11 : i32
    %61 = arith.muli %31, %c20_i32_9 : i32
    %62 = arith.muli %30, %c20_i32_9 : i32
    %63 = cgra.lwi %50 : i32->f32
    %64 = cgra.lwi %55 : i32->f32
    %65 = cgra.lwi %56 : i32->f32
    %66 = arith.addi %3, %47 : i32
    %67 = arith.addi %5, %57 : i32
    %68 = cgra.lwi %58 : i32->f32
    %69 = cgra.lwi %59 : i32->f32
    %70 = arith.muli %33, %c4_i32 : i32
    %71 = arith.muli %60, %c4_i32 : i32
    %72 = arith.addi %61, %60 : i32
    %73 = arith.muli %60, %c4_i32 : i32
    %74 = arith.addi %62, %60 : i32
    %75 = arith.addi %60, %c1_i32 : i32
    %76 = arith.mulf %64, %65 : f32
    %77 = cgra.lwi %67 : i32->f32
    %78 = arith.mulf %68, %69 : f32
    %79 = arith.addi %6, %70 : i32
    %80 = arith.addi %4, %71 : i32
    %81 = arith.muli %30, %c4_i32 : i32
    %82 = arith.muli %72, %c4_i32 : i32
    %83 = arith.muli %74, %c4_i32 : i32
    %84 = arith.muli %60, %c4_i32 : i32
    cgra.cond_br<ge> [%75 : i32, %41 : i32], ^bb9(%63, %76, %66, %77, %78, %79, %80, %81, %82, %73, %83, %84 : f32, f32, i32, f32, f32, i32, i32, i32, i32, i32, i32, i32), ^bb8(%63, %76, %77, %78, %81, %82, %83, %84, %75, %66, %79, %80, %73 : f32, f32, f32, f32, i32, i32, i32, i32, i32, i32, i32, i32, i32)
  ^bb8(%85: f32, %86: f32, %87: f32, %88: f32, %89: i32, %90: i32, %91: i32, %92: i32, %93: i32, %94: i32, %95: i32, %96: i32, %97: i32):  // 2 preds: ^bb7, ^bb8
    %98 = arith.addf %85, %86 : f32
    %99 = arith.addf %87, %88 : f32
    %100 = arith.addi %8, %89 : i32
    %101 = arith.addi %0, %90 : i32
    %102 = arith.muli %32, %c4_i32 : i32
    %103 = arith.addi %1, %91 : i32
    %104 = arith.addi %7, %92 : i32
    %105 = arith.addi %93, %c0_i32_11 : i32
    %106 = arith.muli %31, %c20_i32_9 : i32
    %107 = arith.muli %30, %c20_i32_9 : i32
    cgra.swi %98, %94 : f32, i32
    cgra.swi %99, %95 : f32, i32
    %108 = cgra.lwi %96 : i32->f32
    %109 = cgra.lwi %100 : i32->f32
    %110 = cgra.lwi %101 : i32->f32
    %111 = arith.addi %3, %97 : i32
    %112 = arith.addi %5, %102 : i32
    %113 = cgra.lwi %103 : i32->f32
    %114 = cgra.lwi %104 : i32->f32
    %115 = arith.muli %33, %c4_i32 : i32
    %116 = arith.muli %105, %c4_i32 : i32
    %117 = arith.addi %106, %105 : i32
    %118 = arith.muli %105, %c4_i32 : i32
    %119 = arith.addi %107, %105 : i32
    %120 = arith.addi %105, %c1_i32 : i32
    %121 = arith.mulf %109, %110 : f32
    %122 = cgra.lwi %112 : i32->f32
    %123 = arith.mulf %113, %114 : f32
    %124 = arith.addi %6, %115 : i32
    %125 = arith.addi %4, %116 : i32
    %126 = arith.muli %30, %c4_i32 : i32
    %127 = arith.muli %117, %c4_i32 : i32
    %128 = arith.muli %119, %c4_i32 : i32
    %129 = arith.muli %105, %c4_i32 : i32
    cgra.cond_br<lt> [%120 : i32, %41 : i32], ^bb8(%108, %121, %122, %123, %126, %127, %128, %129, %120, %111, %124, %125, %118 : f32, f32, f32, f32, i32, i32, i32, i32, i32, i32, i32, i32, i32), ^bb9(%108, %121, %111, %122, %123, %124, %125, %126, %127, %118, %128, %129 : f32, f32, i32, f32, f32, i32, i32, i32, i32, i32, i32, i32)
  ^bb9(%130: f32, %131: f32, %132: i32, %133: f32, %134: f32, %135: i32, %136: i32, %137: i32, %138: i32, %139: i32, %140: i32, %141: i32):  // 2 preds: ^bb7, ^bb8
    %142 = arith.addf %130, %131 : f32
    cgra.swi %142, %132 : f32, i32
    %143 = arith.addf %133, %134 : f32
    cgra.swi %143, %135 : f32, i32
    %144 = cgra.lwi %136 : i32->f32
    %145 = arith.addi %8, %137 : i32
    %146 = cgra.lwi %145 : i32->f32
    %147 = arith.addi %0, %138 : i32
    %148 = cgra.lwi %147 : i32->f32
    %149 = arith.mulf %146, %148 : f32
    %150 = arith.addf %144, %149 : f32
    %151 = arith.addi %3, %139 : i32
    cgra.swi %150, %151 : f32, i32
    %152 = arith.muli %32, %c4_i32 : i32
    %153 = arith.addi %5, %152 : i32
    %154 = cgra.lwi %153 : i32->f32
    %155 = arith.addi %1, %140 : i32
    %156 = cgra.lwi %155 : i32->f32
    %157 = arith.addi %7, %141 : i32
    %158 = cgra.lwi %157 : i32->f32
    %159 = arith.mulf %156, %158 : f32
    %160 = arith.addf %154, %159 : f32
    %161 = arith.muli %33, %c4_i32 : i32
    %162 = arith.addi %6, %161 : i32
    cgra.swi %160, %162 : f32, i32
    cf.br ^bb11
  ^bb10:  // pred: ^bb6
    %163 = cgra.lwi %50 : i32->f32
    %164 = arith.addi %8, %51 : i32
    %165 = cgra.lwi %164 : i32->f32
    %166 = arith.addi %0, %52 : i32
    %167 = cgra.lwi %166 : i32->f32
    %168 = arith.mulf %165, %167 : f32
    %169 = arith.addf %163, %168 : f32
    %170 = arith.addi %3, %47 : i32
    cgra.swi %169, %170 : f32, i32
    %171 = arith.muli %32, %c4_i32 : i32
    %172 = arith.addi %5, %171 : i32
    %173 = cgra.lwi %172 : i32->f32
    %174 = arith.addi %1, %53 : i32
    %175 = cgra.lwi %174 : i32->f32
    %176 = arith.addi %7, %54 : i32
    %177 = cgra.lwi %176 : i32->f32
    %178 = arith.mulf %175, %177 : f32
    %179 = arith.addf %173, %178 : f32
    %180 = arith.muli %33, %c4_i32 : i32
    %181 = arith.addi %6, %180 : i32
    cgra.swi %179, %181 : f32, i32
    cf.br ^bb11
  ^bb11:  // 2 preds: ^bb9, ^bb10
    %182 = arith.addi %30, %c1_i32 : i32
    %183 = arith.addi %33, %c1_i32 : i32
    %184 = arith.addi %32, %c1_i32 : i32
    %185 = arith.addi %31, %c1_i32 : i32
    cf.br ^bb4(%182, %185, %184, %183 : i32, i32, i32, i32)
  ^bb12:  // pred: ^bb4
    return
  }
}


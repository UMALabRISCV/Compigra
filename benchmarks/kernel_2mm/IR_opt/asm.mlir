module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_2mm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<8x8xi32>, %arg3: memref<8x8xi32>, %arg4: memref<8x8xi32>, %arg5: memref<8x8xi32>, %arg6: memref<8x8xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %cst = arith.constant 0.000000e+00 : f32
    %c8_i32 = arith.constant 8 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c0_i32_1 = arith.constant 0 : i32
    %c1_i32 = arith.constant 1 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c8_i32_3 = arith.constant 8 : i32
    %c0_i32_4 = arith.constant 0 : i32
    %c0_i32_5 = arith.constant 0 : i32
    %c0_i32_6 = arith.constant 0 : i32
    %c8_i32_7 = arith.constant 8 : i32
    %c0_i32_8 = arith.constant 0 : i32
    %c8_i32_9 = arith.constant 8 : i32
    %c0_i32_10 = arith.constant 0 : i32
    %c8_i32_11 = arith.constant 8 : i32
    %c0_i32_12 = arith.constant 0 : i32
    %c8_i32_13 = arith.constant 8 : i32
    %c0_i32_14 = arith.constant 0 : i32
    %c8_i32_15 = arith.constant 8 : i32
    %c0_i32_16 = arith.constant 0 : i32
    %c8_i32_17 = arith.constant 8 : i32
    %c6_i32 = arith.constant 6 : i32
    %c8_i32_18 = arith.constant {DimProd = 0 : i32, alloc = 0 : i32} 8 : i32
    %c2048_i32 = arith.constant 2048 : i32
    %c28_i32 = arith.constant 28 : i32
    %c0_i32_19 = arith.constant 0 : i32
    %c12_i32 = arith.constant 12 : i32
    %c4_i32 = arith.constant 4 : i32
    %c8_i32_20 = arith.constant {DimProd = 0 : i32, arg = 6 : i32} 8 : i32
    %c3584_i32 = arith.constant 3584 : i32
    %c15_i32 = arith.constant 15 : i32
    %c0_i32_21 = arith.constant 0 : i32
    %c12_i32_22 = arith.constant 12 : i32
    %c3588_i32 = arith.constant 3588 : i32
    %c15_i32_23 = arith.constant 15 : i32
    %c0_i32_24 = arith.constant 0 : i32
    %c12_i32_25 = arith.constant 12 : i32
    %c3592_i32 = arith.constant 3592 : i32
    %c15_i32_26 = arith.constant 15 : i32
    %c0_i32_27 = arith.constant 0 : i32
    %c12_i32_28 = arith.constant 12 : i32
    %c3596_i32 = arith.constant 3596 : i32
    %c15_i32_29 = arith.constant 15 : i32
    %c0_i32_30 = arith.constant 0 : i32
    %c12_i32_31 = arith.constant 12 : i32
    %c3600_i32 = arith.constant 3600 : i32
    %c15_i32_32 = arith.constant 15 : i32
    %c0_i32_33 = arith.constant 0 : i32
    %c12_i32_34 = arith.constant 12 : i32
    %c3604_i32 = arith.constant 3604 : i32
    %c15_i32_35 = arith.constant 15 : i32
    %c0_i32_36 = arith.constant 0 : i32
    %c12_i32_37 = arith.constant 12 : i32
    %c3608_i32 = arith.constant 3608 : i32
    %c15_i32_38 = arith.constant 15 : i32
    %c0_i32_39 = arith.constant 0 : i32
    %c12_i32_40 = arith.constant 12 : i32
    %c3612_i32 = arith.constant 3612 : i32
    %c15_i32_41 = arith.constant 15 : i32
    %c0_i32_42 = arith.constant 0 : i32
    %c12_i32_43 = arith.constant 12 : i32
    %c3612_i32_44 = arith.constant 3612 : i32
    %c15_i32_45 = arith.constant 15 : i32
    %c0_i32_46 = arith.constant 0 : i32
    %c12_i32_47 = arith.constant 12 : i32
    %c3608_i32_48 = arith.constant 3608 : i32
    %c15_i32_49 = arith.constant 15 : i32
    %c0_i32_50 = arith.constant 0 : i32
    %c12_i32_51 = arith.constant 12 : i32
    %c3584_i32_52 = arith.constant 3584 : i32
    %c15_i32_53 = arith.constant 15 : i32
    %c0_i32_54 = arith.constant 0 : i32
    %c12_i32_55 = arith.constant 12 : i32
    %c3588_i32_56 = arith.constant 3588 : i32
    %c15_i32_57 = arith.constant 15 : i32
    %c0_i32_58 = arith.constant 0 : i32
    %c12_i32_59 = arith.constant 12 : i32
    %c3592_i32_60 = arith.constant 3592 : i32
    %c15_i32_61 = arith.constant 15 : i32
    %c0_i32_62 = arith.constant 0 : i32
    %c12_i32_63 = arith.constant 12 : i32
    %c3596_i32_64 = arith.constant 3596 : i32
    %c15_i32_65 = arith.constant 15 : i32
    %c0_i32_66 = arith.constant 0 : i32
    %c12_i32_67 = arith.constant 12 : i32
    %c3600_i32_68 = arith.constant 3600 : i32
    %c15_i32_69 = arith.constant 15 : i32
    %c0_i32_70 = arith.constant 0 : i32
    %c12_i32_71 = arith.constant 12 : i32
    %c3604_i32_72 = arith.constant 3604 : i32
    %c15_i32_73 = arith.constant 15 : i32
    %c0_i32_74 = arith.constant 0 : i32
    %c12_i32_75 = arith.constant 12 : i32
    %c3608_i32_76 = arith.constant 3608 : i32
    %c15_i32_77 = arith.constant 15 : i32
    %c0_i32_78 = arith.constant 0 : i32
    %c12_i32_79 = arith.constant 12 : i32
    %c3612_i32_80 = arith.constant 3612 : i32
    %c15_i32_81 = arith.constant 15 : i32
    %c0_i32_82 = arith.constant 0 : i32
    %c12_i32_83 = arith.constant 12 : i32
    %c1_i32_84 = arith.constant 1 : i32
    %0 = arith.addi %c28_i32, %c0_i32_19 {value = 28 : i32} : i32
    %1 = arith.shli %0, %c12_i32 {value = 114688 : i32} : i32
    %2 = arith.addi %c2048_i32, %1 {value = 116736 : i32} : i32
    %3 = cgra.lwd -> i32, {BaseAddr = "arg6"}
    %4 = cgra.lwd -> i32, {BaseAddr = "arg3", blas = "0", blas_arg = "0"}
    %5 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "0", blas_arg = "1"}
    %6 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "0", blas_arg = "2"}
    %7 = arith.addi %6, %c0_i32 : i32
    %8 = arith.addi %c15_i32, %c0_i32_21 {value = 15 : i32} : i32
    %9 = arith.shli %8, %c12_i32_22 {value = 61440 : i32} : i32
    %10 = arith.addi %c3584_i32, %9 {value = 65024 : i32} : i32
    cgra.swi %4, %10 : i32, i32
    %11 = arith.addi %c15_i32_23, %c0_i32_24 {value = 15 : i32} : i32
    %12 = arith.shli %11, %c12_i32_25 {value = 61440 : i32} : i32
    %13 = arith.addi %c3588_i32, %12 {value = 65028 : i32} : i32
    cgra.swi %5, %13 : i32, i32
    %14 = arith.addi %c15_i32_26, %c0_i32_27 {value = 15 : i32} : i32
    %15 = arith.shli %14, %c12_i32_28 {value = 61440 : i32} : i32
    %16 = arith.addi %c3592_i32, %15 {value = 65032 : i32} : i32
    cgra.swi %7, %16 : i32, i32
    %17 = arith.addi %c15_i32_29, %c0_i32_30 {value = 15 : i32} : i32
    %18 = arith.shli %17, %c12_i32_31 {value = 61440 : i32} : i32
    %19 = arith.addi %c3596_i32, %18 {value = 65036 : i32} : i32
    %20 = arith.addi %19, %c0_i32 : i32
    %21 = arith.addi %c0_i32_16, %c8_i32_17 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    %22 = arith.addi %21, %c0_i32 : i32
    cgra.swi %22, %20 : i32, i32
    %23 = arith.addi %c15_i32_32, %c0_i32_33 {value = 15 : i32} : i32
    %24 = arith.shli %23, %c12_i32_34 {value = 61440 : i32} : i32
    %25 = arith.addi %c3600_i32, %24 {value = 65040 : i32} : i32
    %26 = arith.addi %25, %c0_i32 : i32
    %27 = arith.addi %c0_i32_14, %c8_i32_15 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    %28 = arith.addi %27, %c0_i32 : i32
    %29 = arith.addi %28, %c0_i32 : i32
    cgra.swi %29, %26 : i32, i32
    %30 = arith.addi %c15_i32_35, %c0_i32_36 {value = 15 : i32} : i32
    %31 = arith.shli %30, %c12_i32_37 {value = 61440 : i32} : i32
    %32 = arith.addi %c3604_i32, %31 {value = 65044 : i32} : i32
    %33 = arith.addi %c0_i32_12, %c8_i32_13 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    cgra.swi %33, %32 : i32, i32
    %34 = arith.addi %c15_i32_38, %c0_i32_39 {value = 15 : i32} : i32
    %35 = arith.shli %34, %c12_i32_40 {value = 61440 : i32} : i32
    %36 = arith.addi %c3608_i32, %35 {value = 65048 : i32} : i32
    cgra.swi %2, %36 : i32, i32
    %37 = arith.addi %c15_i32_41, %c0_i32_42 {value = 15 : i32} : i32
    %38 = arith.shli %37, %c12_i32_43 {value = 61440 : i32} : i32
    %39 = arith.addi %c3612_i32, %38 {value = 65052 : i32} : i32
    cgra.swi %3, %39 : i32, i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    "cgra.gemm_blas_asm"()[^bb2] {blas = "0", mulASM = "SMUL R3, R3, 3"} : () -> ()
  ^bb2:  // pred: ^bb1
    %40 = arith.addi %c15_i32_45, %c0_i32_46 {value = 15 : i32} : i32
    %41 = arith.shli %40, %c12_i32_47 {value = 61440 : i32} : i32
    %42 = arith.addi %c3612_i32_44, %41 {value = 65052 : i32} : i32
    %43 = cgra.lwi %42 : i32->i32
    %44 = arith.addi %c15_i32_49, %c0_i32_50 {value = 15 : i32} : i32
    %45 = arith.shli %44, %c12_i32_51 {value = 61440 : i32} : i32
    %46 = arith.addi %c3608_i32_48, %45 {value = 65048 : i32} : i32
    %47 = cgra.lwi %46 : i32->i32
    %48 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "1", blas_arg = "0"}
    %49 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "1", blas_arg = "1"}
    %50 = arith.addi %c15_i32_53, %c0_i32_54 {value = 15 : i32} : i32
    %51 = arith.shli %50, %c12_i32_55 {value = 61440 : i32} : i32
    %52 = arith.addi %c3584_i32_52, %51 {value = 65024 : i32} : i32
    cgra.swi %48, %52 : i32, i32
    %53 = arith.addi %c15_i32_57, %c0_i32_58 {value = 15 : i32} : i32
    %54 = arith.shli %53, %c12_i32_59 {value = 61440 : i32} : i32
    %55 = arith.addi %c3588_i32_56, %54 {value = 65028 : i32} : i32
    cgra.swi %49, %55 : i32, i32
    %56 = arith.addi %c15_i32_61, %c0_i32_62 {value = 15 : i32} : i32
    %57 = arith.shli %56, %c12_i32_63 {value = 61440 : i32} : i32
    %58 = arith.addi %c3592_i32_60, %57 {value = 65032 : i32} : i32
    cgra.swi %47, %58 : i32, i32
    %59 = arith.addi %c15_i32_65, %c0_i32_66 {value = 15 : i32} : i32
    %60 = arith.shli %59, %c12_i32_67 {value = 61440 : i32} : i32
    %61 = arith.addi %c3596_i32_64, %60 {value = 65036 : i32} : i32
    %62 = arith.addi %c0_i32_10, %c8_i32_11 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    %63 = arith.addi %62, %c0_i32 : i32
    cgra.swi %63, %61 : i32, i32
    %64 = arith.addi %c15_i32_69, %c0_i32_70 {value = 15 : i32} : i32
    %65 = arith.shli %64, %c12_i32_71 {value = 61440 : i32} : i32
    %66 = arith.addi %c3600_i32_68, %65 {value = 65040 : i32} : i32
    %67 = arith.addi %66, %c0_i32 : i32
    %68 = arith.addi %c0_i32_8, %c8_i32_9 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    %69 = arith.addi %68, %c0_i32 : i32
    cgra.swi %69, %67 : i32, i32
    %70 = arith.addi %c15_i32_73, %c0_i32_74 {value = 15 : i32} : i32
    %71 = arith.shli %70, %c12_i32_75 {value = 61440 : i32} : i32
    %72 = arith.addi %c3604_i32_72, %71 {value = 65044 : i32} : i32
    %73 = arith.addi %c0_i32_6, %c8_i32_7 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    cgra.swi %73, %72 : i32, i32
    cf.br ^bb3
  ^bb3:  // pred: ^bb2
    "cgra.gemm_blas_asm"()[^bb4] {blas = "1"} : () -> ()
  ^bb4:  // pred: ^bb3
    %74 = arith.addi %c15_i32_77, %c0_i32_78 {value = 15 : i32} : i32
    %75 = arith.shli %74, %c12_i32_79 {value = 61440 : i32} : i32
    %76 = arith.addi %c3608_i32_76, %75 {value = 65048 : i32} : i32
    %77 = cgra.lwi %76 : i32->i32
    %78 = arith.addi %c15_i32_81, %c0_i32_82 {value = 15 : i32} : i32
    %79 = arith.shli %78, %c12_i32_83 {value = 61440 : i32} : i32
    %80 = arith.addi %c3612_i32_80, %79 {value = 65052 : i32} : i32
    %81 = cgra.lwi %80 : i32->i32
    %82 = arith.addi %81, %c0_i32 : i32
    %83 = arith.addi %82, %c0_i32 : i32
    %84 = cgra.lwi %80 : i32->i32
    %85 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %86 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    %87 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    cf.br ^bb5(%85, %87, %86 : i32, i32, i32)
  ^bb5(%88: i32, %89: i32, %90: i32):  // 2 preds: ^bb4, ^bb13
    %91 = arith.addi %c0_i32_2, %c8_i32_3 {value = 8 : i32} : i32
    cgra.cond_br<ge> [%88 : i32, %91 : i32], ^bb14, ^bb6
  ^bb6:  // pred: ^bb5
    %92 = arith.addi %c0_i32_0, %c0_i32_1 {value = 0 : i32} : i32
    %93 = arith.addi %c0_i32, %c8_i32 {value = 8 : i32} : i32
    %c0_i32_85 = arith.constant 0 : i32
    cf.br ^bb7
  ^bb7:  // pred: ^bb6
    %94 = arith.addi %92, %c0_i32_85 : i32
    %95 = arith.muli %88, %c8_i32_20 : i32
    %96 = arith.muli %89, %c8_i32_18 : i32
    %97 = arith.addi %95, %94 : i32
    %98 = arith.addi %96, %94 : i32
    %99 = arith.muli %90, %c8_i32_20 : i32
    %100 = arith.addi %94, %c1_i32_84 : i32
    cf.br ^bb8
  ^bb8:  // pred: ^bb7
    %101 = arith.muli %97, %c4_i32 : i32
    %102 = arith.muli %98, %c4_i32 : i32
    %103 = arith.addi %99, %94 : i32
    %104 = arith.addi %100, %c0_i32_85 : i32
    %105 = arith.muli %88, %c8_i32_20 : i32
    %106 = arith.muli %89, %c8_i32_18 : i32
    %107 = arith.addi %83, %101 : i32
    %108 = arith.addi %102, %77 : i32
    %109 = arith.addi %105, %104 : i32
    %110 = arith.addi %106, %104 : i32
    %111 = arith.muli %90, %c8_i32_20 : i32
    %112 = arith.addi %104, %c1_i32_84 : i32
    cgra.cond_br<ge> [%100 : i32, %93 : i32], ^bb12, ^bb9
  ^bb9:  // pred: ^bb8
    %113 = cgra.lwi %107 : i32->i32
    %114 = arith.muli %103, %c4_i32 : i32
    %115 = arith.muli %109, %c4_i32 : i32
    %116 = arith.muli %110, %c4_i32 : i32
    %117 = arith.addi %111, %104 : i32
    %118 = arith.addi %112, %c0_i32_85 : i32
    %119 = arith.muli %88, %c8_i32_20 : i32
    %120 = arith.muli %89, %c8_i32_18 : i32
    %121 = arith.muli %113, %c6_i32 : i32
    %122 = cgra.lwi %108 : i32->i32
    %123 = arith.addi %83, %115 : i32
    %124 = arith.addi %116, %77 : i32
    %125 = arith.addi %119, %118 : i32
    %126 = arith.addi %120, %118 : i32
    %127 = arith.muli %90, %c8_i32_20 : i32
    %128 = arith.addi %118, %c1_i32_84 : i32
    cgra.cond_br<ge> [%112 : i32, %93 : i32], ^bb11(%122, %121, %114, %123, %124, %117 : i32, i32, i32, i32, i32, i32), ^bb10(%122, %121, %114, %123, %117, %125, %126, %127, %118, %128, %124, %128 : i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32)
  ^bb10(%129: i32, %130: i32, %131: i32, %132: i32, %133: i32, %134: i32, %135: i32, %136: i32, %137: i32, %138: i32, %139: i32, %140: i32):  // 2 preds: ^bb9, ^bb10
    %141 = arith.addi %129, %130 : i32
    %142 = arith.addi %84, %131 : i32
    %143 = cgra.lwi %132 : i32->i32
    %144 = arith.muli %133, %c4_i32 : i32
    %145 = arith.muli %134, %c4_i32 : i32
    %146 = arith.muli %135, %c4_i32 : i32
    %147 = arith.addi %136, %137 : i32
    %148 = arith.addi %138, %c0_i32_85 : i32
    %149 = arith.muli %88, %c8_i32_20 : i32
    %150 = arith.muli %89, %c8_i32_18 : i32
    cgra.swi %141, %142 : i32, i32
    %151 = arith.muli %143, %c6_i32 : i32
    %152 = cgra.lwi %139 : i32->i32
    %153 = arith.addi %83, %145 : i32
    %154 = arith.addi %146, %77 : i32
    %155 = arith.addi %149, %148 : i32
    %156 = arith.addi %150, %148 : i32
    %157 = arith.muli %90, %c8_i32_20 : i32
    %158 = arith.addi %148, %c1_i32_84 : i32
    cgra.cond_br<lt> [%140 : i32, %93 : i32], ^bb10(%152, %151, %144, %153, %147, %155, %156, %157, %148, %158, %154, %158 : i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32), ^bb11(%152, %151, %144, %153, %154, %147 : i32, i32, i32, i32, i32, i32)
  ^bb11(%159: i32, %160: i32, %161: i32, %162: i32, %163: i32, %164: i32):  // 2 preds: ^bb9, ^bb10
    %165 = arith.addi %159, %160 : i32
    %166 = arith.addi %84, %161 : i32
    cgra.swi %165, %166 : i32, i32
    %167 = cgra.lwi %162 : i32->i32
    %168 = arith.muli %167, %c6_i32 : i32
    %169 = cgra.lwi %163 : i32->i32
    %170 = arith.addi %169, %168 : i32
    %171 = arith.muli %164, %c4_i32 : i32
    %172 = arith.addi %84, %171 : i32
    cgra.swi %170, %172 : i32, i32
    cf.br ^bb13
  ^bb12:  // pred: ^bb8
    %173 = cgra.lwi %107 : i32->i32
    %174 = arith.muli %173, %c6_i32 : i32
    %175 = cgra.lwi %108 : i32->i32
    %176 = arith.addi %175, %174 : i32
    %177 = arith.muli %103, %c4_i32 : i32
    %178 = arith.addi %84, %177 : i32
    cgra.swi %176, %178 : i32, i32
    cf.br ^bb13
  ^bb13:  // 2 preds: ^bb11, ^bb12
    %179 = arith.addi %88, %c1_i32 : i32
    %180 = arith.addi %90, %c1_i32 : i32
    %181 = arith.addi %89, %c1_i32 : i32
    cf.br ^bb5(%179, %181, %180 : i32, i32, i32)
  ^bb14:  // pred: ^bb5
    return
  }
}


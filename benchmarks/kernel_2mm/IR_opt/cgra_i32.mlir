module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_2mm(%arg0: memref<1xi32>, %arg1: memref<1xi32>, %arg2: memref<8x8xi32>, %arg3: memref<8x8xi32>, %arg4: memref<8x8xi32>, %arg5: memref<8x8xi32>, %arg6: memref<8x8xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
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
    %7 = arith.addi %c15_i32, %c0_i32_21 {value = 15 : i32} : i32
    %8 = arith.shli %7, %c12_i32_22 {value = 61440 : i32} : i32
    %9 = arith.addi %c3584_i32, %8 {value = 65024 : i32} : i32
    cgra.swi %4, %9 : i32, i32
    %10 = arith.addi %c15_i32_23, %c0_i32_24 {value = 15 : i32} : i32
    %11 = arith.shli %10, %c12_i32_25 {value = 61440 : i32} : i32
    %12 = arith.addi %c3588_i32, %11 {value = 65028 : i32} : i32
    cgra.swi %5, %12 : i32, i32
    %13 = arith.addi %c15_i32_26, %c0_i32_27 {value = 15 : i32} : i32
    %14 = arith.shli %13, %c12_i32_28 {value = 61440 : i32} : i32
    %15 = arith.addi %c3592_i32, %14 {value = 65032 : i32} : i32
    cgra.swi %6, %15 : i32, i32
    %16 = arith.addi %c15_i32_29, %c0_i32_30 {value = 15 : i32} : i32
    %17 = arith.shli %16, %c12_i32_31 {value = 61440 : i32} : i32
    %18 = arith.addi %c3596_i32, %17 {value = 65036 : i32} : i32
    %19 = arith.addi %c0_i32_6, %c8_i32_7 {blas = "0", blas_arg = "3", value = 8 : i32} : i32
    cgra.swi %19, %18 : i32, i32
    %20 = arith.addi %c15_i32_32, %c0_i32_33 {value = 15 : i32} : i32
    %21 = arith.shli %20, %c12_i32_34 {value = 61440 : i32} : i32
    %22 = arith.addi %c3600_i32, %21 {value = 65040 : i32} : i32
    %23 = arith.addi %c0_i32_10, %c8_i32_11 {blas = "0", blas_arg = "4", value = 8 : i32} : i32
    cgra.swi %23, %22 : i32, i32
    %24 = arith.addi %c15_i32_35, %c0_i32_36 {value = 15 : i32} : i32
    %25 = arith.shli %24, %c12_i32_37 {value = 61440 : i32} : i32
    %26 = arith.addi %c3604_i32, %25 {value = 65044 : i32} : i32
    %27 = arith.addi %c0_i32_8, %c8_i32_9 {blas = "0", blas_arg = "5", value = 8 : i32} : i32
    cgra.swi %27, %26 : i32, i32
    %28 = arith.addi %c15_i32_38, %c0_i32_39 {value = 15 : i32} : i32
    %29 = arith.shli %28, %c12_i32_40 {value = 61440 : i32} : i32
    %30 = arith.addi %c3608_i32, %29 {value = 65048 : i32} : i32
    cgra.swi %2, %30 : i32, i32
    %31 = arith.addi %c15_i32_41, %c0_i32_42 {value = 15 : i32} : i32
    %32 = arith.shli %31, %c12_i32_43 {value = 61440 : i32} : i32
    %33 = arith.addi %c3612_i32, %32 {value = 65052 : i32} : i32
    cgra.swi %3, %33 : i32, i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    "cgra.gemm_blas_asm"()[^bb2] {blas = "0", mulASM = "SMUL R3, R3, 3"} : () -> ()
  ^bb2:  // pred: ^bb1
    %34 = arith.addi %c15_i32_45, %c0_i32_46 {value = 15 : i32} : i32
    %35 = arith.shli %34, %c12_i32_47 {value = 61440 : i32} : i32
    %36 = arith.addi %c3612_i32_44, %35 {value = 65052 : i32} : i32
    %37 = cgra.lwi %36 : i32->i32
    %38 = arith.addi %c15_i32_49, %c0_i32_50 {value = 15 : i32} : i32
    %39 = arith.shli %38, %c12_i32_51 {value = 61440 : i32} : i32
    %40 = arith.addi %c3608_i32_48, %39 {value = 65048 : i32} : i32
    %41 = cgra.lwi %40 : i32->i32
    %42 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "1", blas_arg = "0"}
    %43 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "1", blas_arg = "1"}
    %44 = arith.addi %c15_i32_53, %c0_i32_54 {value = 15 : i32} : i32
    %45 = arith.shli %44, %c12_i32_55 {value = 61440 : i32} : i32
    %46 = arith.addi %c3584_i32_52, %45 {value = 65024 : i32} : i32
    cgra.swi %42, %46 : i32, i32
    %47 = arith.addi %c15_i32_57, %c0_i32_58 {value = 15 : i32} : i32
    %48 = arith.shli %47, %c12_i32_59 {value = 61440 : i32} : i32
    %49 = arith.addi %c3588_i32_56, %48 {value = 65028 : i32} : i32
    cgra.swi %43, %49 : i32, i32
    %50 = arith.addi %c15_i32_61, %c0_i32_62 {value = 15 : i32} : i32
    %51 = arith.shli %50, %c12_i32_63 {value = 61440 : i32} : i32
    %52 = arith.addi %c3592_i32_60, %51 {value = 65032 : i32} : i32
    cgra.swi %41, %52 : i32, i32
    %53 = arith.addi %c15_i32_65, %c0_i32_66 {value = 15 : i32} : i32
    %54 = arith.shli %53, %c12_i32_67 {value = 61440 : i32} : i32
    %55 = arith.addi %c3596_i32_64, %54 {value = 65036 : i32} : i32
    %56 = arith.addi %c0_i32_12, %c8_i32_13 {blas = "1", blas_arg = "3", value = 8 : i32} : i32
    cgra.swi %56, %55 : i32, i32
    %57 = arith.addi %c15_i32_69, %c0_i32_70 {value = 15 : i32} : i32
    %58 = arith.shli %57, %c12_i32_71 {value = 61440 : i32} : i32
    %59 = arith.addi %c3600_i32_68, %58 {value = 65040 : i32} : i32
    %60 = arith.addi %c0_i32_16, %c8_i32_17 {blas = "1", blas_arg = "4", value = 8 : i32} : i32
    cgra.swi %60, %59 : i32, i32
    %61 = arith.addi %c15_i32_73, %c0_i32_74 {value = 15 : i32} : i32
    %62 = arith.shli %61, %c12_i32_75 {value = 61440 : i32} : i32
    %63 = arith.addi %c3604_i32_72, %62 {value = 65044 : i32} : i32
    %64 = arith.addi %c0_i32_14, %c8_i32_15 {blas = "1", blas_arg = "5", value = 8 : i32} : i32
    cgra.swi %64, %63 : i32, i32
    cf.br ^bb3
  ^bb3:  // pred: ^bb2
    "cgra.gemm_blas_asm"()[^bb4] {blas = "1"} : () -> ()
  ^bb4:  // pred: ^bb3
    %65 = arith.addi %c15_i32_77, %c0_i32_78 {value = 15 : i32} : i32
    %66 = arith.shli %65, %c12_i32_79 {value = 61440 : i32} : i32
    %67 = arith.addi %c3608_i32_76, %66 {value = 65048 : i32} : i32
    %68 = cgra.lwi %67 : i32->i32
    %69 = arith.addi %c15_i32_81, %c0_i32_82 {value = 15 : i32} : i32
    %70 = arith.shli %69, %c12_i32_83 {value = 61440 : i32} : i32
    %71 = arith.addi %c3612_i32_80, %70 {value = 65052 : i32} : i32
    %72 = cgra.lwi %71 : i32->i32
    %73 = arith.addi %c0_i32_4, %c0_i32_5 {value = 0 : i32} : i32
    cf.br ^bb5(%73 : i32)
  ^bb5(%74: i32):  // 2 preds: ^bb4, ^bb9
    %75 = arith.addi %c0_i32_2, %c8_i32_3 {value = 8 : i32} : i32
    cgra.cond_br<ge> [%74 : i32, %75 : i32], ^bb10, ^bb6
  ^bb6:  // pred: ^bb5
    %76 = arith.addi %c0_i32_0, %c0_i32_1 {value = 0 : i32} : i32
    cf.br ^bb7(%76 : i32)
  ^bb7(%77: i32):  // 2 preds: ^bb6, ^bb8
    %78 = arith.addi %c0_i32, %c8_i32 {value = 8 : i32} : i32
    cgra.cond_br<ge> [%77 : i32, %78 : i32], ^bb9, ^bb8
  ^bb8:  // pred: ^bb7
    %79 = arith.muli %74, %c8_i32_20 : i32
    %80 = arith.addi %79, %77 : i32
    %81 = arith.muli %80, %c4_i32 : i32
    %82 = arith.addi %72, %81 : i32
    %83 = cgra.lwi %82 : i32->i32
    %84 = arith.muli %83, %c6_i32 : i32
    %85 = arith.muli %74, %c8_i32_18 : i32
    %86 = arith.addi %85, %77 : i32
    %87 = arith.muli %86, %c4_i32 : i32
    %88 = arith.addi %87, %68 : i32
    %89 = cgra.lwi %88 : i32->i32
    %90 = arith.addi %89, %84 : i32
    %91 = arith.muli %74, %c8_i32_20 : i32
    %92 = arith.addi %91, %77 : i32
    %93 = arith.muli %92, %c4_i32 : i32
    %94 = arith.addi %72, %93 : i32
    cgra.swi %90, %94 : i32, i32
    %95 = arith.addi %77, %c1_i32_84 : i32
    cf.br ^bb7(%95 : i32)
  ^bb9:  // pred: ^bb7
    %96 = arith.addi %74, %c1_i32 : i32
    cf.br ^bb5(%96 : i32)
  ^bb10:  // pred: ^bb5
    return
  }
}


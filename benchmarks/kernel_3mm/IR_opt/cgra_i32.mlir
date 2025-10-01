module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>, #dlti.dl_entry<"dlti.endianness", "little">>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @kernel_3mm(%arg0: memref<4x4xi32>, %arg1: memref<4x4xi32>, %arg2: memref<4x4xi32>, %arg3: memref<4x4xi32>, %arg4: memref<4x4xi32>, %arg5: memref<4x4xi32>, %arg6: memref<4x4xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c4_i32 = arith.constant 4 : i32
    %c0_i32_0 = arith.constant 0 : i32
    %c4_i32_1 = arith.constant 4 : i32
    %c0_i32_2 = arith.constant 0 : i32
    %c4_i32_3 = arith.constant 4 : i32
    %c0_i32_4 = arith.constant 0 : i32
    %c4_i32_5 = arith.constant 4 : i32
    %c0_i32_6 = arith.constant 0 : i32
    %c4_i32_7 = arith.constant 4 : i32
    %c0_i32_8 = arith.constant 0 : i32
    %c4_i32_9 = arith.constant 4 : i32
    %c0_i32_10 = arith.constant 0 : i32
    %c4_i32_11 = arith.constant 4 : i32
    %c0_i32_12 = arith.constant 0 : i32
    %c4_i32_13 = arith.constant 4 : i32
    %c0_i32_14 = arith.constant 0 : i32
    %c4_i32_15 = arith.constant 4 : i32
    %c3584_i32 = arith.constant 3584 : i32
    %c15_i32 = arith.constant 15 : i32
    %c0_i32_16 = arith.constant 0 : i32
    %c12_i32 = arith.constant 12 : i32
    %c3588_i32 = arith.constant 3588 : i32
    %c15_i32_17 = arith.constant 15 : i32
    %c0_i32_18 = arith.constant 0 : i32
    %c12_i32_19 = arith.constant 12 : i32
    %c3592_i32 = arith.constant 3592 : i32
    %c15_i32_20 = arith.constant 15 : i32
    %c0_i32_21 = arith.constant 0 : i32
    %c12_i32_22 = arith.constant 12 : i32
    %c3596_i32 = arith.constant 3596 : i32
    %c15_i32_23 = arith.constant 15 : i32
    %c0_i32_24 = arith.constant 0 : i32
    %c12_i32_25 = arith.constant 12 : i32
    %c3600_i32 = arith.constant 3600 : i32
    %c15_i32_26 = arith.constant 15 : i32
    %c0_i32_27 = arith.constant 0 : i32
    %c12_i32_28 = arith.constant 12 : i32
    %c3604_i32 = arith.constant 3604 : i32
    %c15_i32_29 = arith.constant 15 : i32
    %c0_i32_30 = arith.constant 0 : i32
    %c12_i32_31 = arith.constant 12 : i32
    %c3584_i32_32 = arith.constant 3584 : i32
    %c15_i32_33 = arith.constant 15 : i32
    %c0_i32_34 = arith.constant 0 : i32
    %c12_i32_35 = arith.constant 12 : i32
    %c3588_i32_36 = arith.constant 3588 : i32
    %c15_i32_37 = arith.constant 15 : i32
    %c0_i32_38 = arith.constant 0 : i32
    %c12_i32_39 = arith.constant 12 : i32
    %c3592_i32_40 = arith.constant 3592 : i32
    %c15_i32_41 = arith.constant 15 : i32
    %c0_i32_42 = arith.constant 0 : i32
    %c12_i32_43 = arith.constant 12 : i32
    %c3596_i32_44 = arith.constant 3596 : i32
    %c15_i32_45 = arith.constant 15 : i32
    %c0_i32_46 = arith.constant 0 : i32
    %c12_i32_47 = arith.constant 12 : i32
    %c3600_i32_48 = arith.constant 3600 : i32
    %c15_i32_49 = arith.constant 15 : i32
    %c0_i32_50 = arith.constant 0 : i32
    %c12_i32_51 = arith.constant 12 : i32
    %c3604_i32_52 = arith.constant 3604 : i32
    %c15_i32_53 = arith.constant 15 : i32
    %c0_i32_54 = arith.constant 0 : i32
    %c12_i32_55 = arith.constant 12 : i32
    %c3584_i32_56 = arith.constant 3584 : i32
    %c15_i32_57 = arith.constant 15 : i32
    %c0_i32_58 = arith.constant 0 : i32
    %c12_i32_59 = arith.constant 12 : i32
    %c3588_i32_60 = arith.constant 3588 : i32
    %c15_i32_61 = arith.constant 15 : i32
    %c0_i32_62 = arith.constant 0 : i32
    %c12_i32_63 = arith.constant 12 : i32
    %c3592_i32_64 = arith.constant 3592 : i32
    %c15_i32_65 = arith.constant 15 : i32
    %c0_i32_66 = arith.constant 0 : i32
    %c12_i32_67 = arith.constant 12 : i32
    %c3596_i32_68 = arith.constant 3596 : i32
    %c15_i32_69 = arith.constant 15 : i32
    %c0_i32_70 = arith.constant 0 : i32
    %c12_i32_71 = arith.constant 12 : i32
    %c3600_i32_72 = arith.constant 3600 : i32
    %c15_i32_73 = arith.constant 15 : i32
    %c0_i32_74 = arith.constant 0 : i32
    %c12_i32_75 = arith.constant 12 : i32
    %c3604_i32_76 = arith.constant 3604 : i32
    %c15_i32_77 = arith.constant 15 : i32
    %c0_i32_78 = arith.constant 0 : i32
    %c12_i32_79 = arith.constant 12 : i32
    %0 = cgra.lwd -> i32, {BaseAddr = "arg0", blas = "0", blas_arg = "0"}
    %1 = cgra.lwd -> i32, {BaseAddr = "arg1", blas = "0", blas_arg = "1"}
    %2 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "0", blas_arg = "2"}
    %3 = arith.addi %c15_i32, %c0_i32_16 {value = 15 : i32} : i32
    %4 = arith.shli %3, %c12_i32 {value = 61440 : i32} : i32
    %5 = arith.addi %c3584_i32, %4 {value = 65024 : i32} : i32
    cgra.swi %0, %5 : i32, i32
    %6 = arith.addi %c15_i32_17, %c0_i32_18 {value = 15 : i32} : i32
    %7 = arith.shli %6, %c12_i32_19 {value = 61440 : i32} : i32
    %8 = arith.addi %c3588_i32, %7 {value = 65028 : i32} : i32
    cgra.swi %1, %8 : i32, i32
    %9 = arith.addi %c15_i32_20, %c0_i32_21 {value = 15 : i32} : i32
    %10 = arith.shli %9, %c12_i32_22 {value = 61440 : i32} : i32
    %11 = arith.addi %c3592_i32, %10 {value = 65032 : i32} : i32
    cgra.swi %2, %11 : i32, i32
    %12 = arith.addi %c15_i32_23, %c0_i32_24 {value = 15 : i32} : i32
    %13 = arith.shli %12, %c12_i32_25 {value = 61440 : i32} : i32
    %14 = arith.addi %c3596_i32, %13 {value = 65036 : i32} : i32
    %15 = arith.addi %c0_i32_14, %c4_i32_15 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %15, %14 : i32, i32
    %16 = arith.addi %c15_i32_26, %c0_i32_27 {value = 15 : i32} : i32
    %17 = arith.shli %16, %c12_i32_28 {value = 61440 : i32} : i32
    %18 = arith.addi %c3600_i32, %17 {value = 65040 : i32} : i32
    %19 = arith.addi %c0_i32_12, %c4_i32_13 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %19, %18 : i32, i32
    %20 = arith.addi %c15_i32_29, %c0_i32_30 {value = 15 : i32} : i32
    %21 = arith.shli %20, %c12_i32_31 {value = 61440 : i32} : i32
    %22 = arith.addi %c3604_i32, %21 {value = 65044 : i32} : i32
    %23 = arith.addi %c0_i32_10, %c4_i32_11 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %23, %22 : i32, i32
    cf.br ^bb1
  ^bb1:  // pred: ^bb0
    "cgra.gemm_blas_asm"()[^bb2] {blas = "0"} : () -> ()
  ^bb2:  // pred: ^bb1
    %24 = cgra.lwd -> i32, {BaseAddr = "arg2", blas = "1", blas_arg = "0"}
    %25 = cgra.lwd -> i32, {BaseAddr = "arg3", blas = "1", blas_arg = "1"}
    %26 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "1", blas_arg = "2"}
    %27 = arith.addi %c15_i32_33, %c0_i32_34 {value = 15 : i32} : i32
    %28 = arith.shli %27, %c12_i32_35 {value = 61440 : i32} : i32
    %29 = arith.addi %c3584_i32_32, %28 {value = 65024 : i32} : i32
    cgra.swi %24, %29 : i32, i32
    %30 = arith.addi %c15_i32_37, %c0_i32_38 {value = 15 : i32} : i32
    %31 = arith.shli %30, %c12_i32_39 {value = 61440 : i32} : i32
    %32 = arith.addi %c3588_i32_36, %31 {value = 65028 : i32} : i32
    cgra.swi %25, %32 : i32, i32
    %33 = arith.addi %c15_i32_41, %c0_i32_42 {value = 15 : i32} : i32
    %34 = arith.shli %33, %c12_i32_43 {value = 61440 : i32} : i32
    %35 = arith.addi %c3592_i32_40, %34 {value = 65032 : i32} : i32
    cgra.swi %26, %35 : i32, i32
    %36 = arith.addi %c15_i32_45, %c0_i32_46 {value = 15 : i32} : i32
    %37 = arith.shli %36, %c12_i32_47 {value = 61440 : i32} : i32
    %38 = arith.addi %c3596_i32_44, %37 {value = 65036 : i32} : i32
    %39 = arith.addi %c0_i32_8, %c4_i32_9 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %39, %38 : i32, i32
    %40 = arith.addi %c15_i32_49, %c0_i32_50 {value = 15 : i32} : i32
    %41 = arith.shli %40, %c12_i32_51 {value = 61440 : i32} : i32
    %42 = arith.addi %c3600_i32_48, %41 {value = 65040 : i32} : i32
    %43 = arith.addi %c0_i32_6, %c4_i32_7 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %43, %42 : i32, i32
    %44 = arith.addi %c15_i32_53, %c0_i32_54 {value = 15 : i32} : i32
    %45 = arith.shli %44, %c12_i32_55 {value = 61440 : i32} : i32
    %46 = arith.addi %c3604_i32_52, %45 {value = 65044 : i32} : i32
    %47 = arith.addi %c0_i32_4, %c4_i32_5 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %47, %46 : i32, i32
    cf.br ^bb3
  ^bb3:  // pred: ^bb2
    "cgra.gemm_blas_asm"()[^bb4] {blas = "1"} : () -> ()
  ^bb4:  // pred: ^bb3
    %48 = cgra.lwd -> i32, {BaseAddr = "arg4", blas = "2", blas_arg = "0"}
    %49 = cgra.lwd -> i32, {BaseAddr = "arg5", blas = "2", blas_arg = "1"}
    %50 = cgra.lwd -> i32, {BaseAddr = "arg6", blas = "2", blas_arg = "2"}
    %51 = arith.addi %c15_i32_57, %c0_i32_58 {value = 15 : i32} : i32
    %52 = arith.shli %51, %c12_i32_59 {value = 61440 : i32} : i32
    %53 = arith.addi %c3584_i32_56, %52 {value = 65024 : i32} : i32
    cgra.swi %48, %53 : i32, i32
    %54 = arith.addi %c15_i32_61, %c0_i32_62 {value = 15 : i32} : i32
    %55 = arith.shli %54, %c12_i32_63 {value = 61440 : i32} : i32
    %56 = arith.addi %c3588_i32_60, %55 {value = 65028 : i32} : i32
    cgra.swi %49, %56 : i32, i32
    %57 = arith.addi %c15_i32_65, %c0_i32_66 {value = 15 : i32} : i32
    %58 = arith.shli %57, %c12_i32_67 {value = 61440 : i32} : i32
    %59 = arith.addi %c3592_i32_64, %58 {value = 65032 : i32} : i32
    cgra.swi %50, %59 : i32, i32
    %60 = arith.addi %c15_i32_69, %c0_i32_70 {value = 15 : i32} : i32
    %61 = arith.shli %60, %c12_i32_71 {value = 61440 : i32} : i32
    %62 = arith.addi %c3596_i32_68, %61 {value = 65036 : i32} : i32
    %63 = arith.addi %c0_i32_2, %c4_i32_3 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %63, %62 : i32, i32
    %64 = arith.addi %c15_i32_73, %c0_i32_74 {value = 15 : i32} : i32
    %65 = arith.shli %64, %c12_i32_75 {value = 61440 : i32} : i32
    %66 = arith.addi %c3600_i32_72, %65 {value = 65040 : i32} : i32
    %67 = arith.addi %c0_i32_0, %c4_i32_1 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %67, %66 : i32, i32
    %68 = arith.addi %c15_i32_77, %c0_i32_78 {value = 15 : i32} : i32
    %69 = arith.shli %68, %c12_i32_79 {value = 61440 : i32} : i32
    %70 = arith.addi %c3604_i32_76, %69 {value = 65044 : i32} : i32
    %71 = arith.addi %c0_i32, %c4_i32 {blas = "2", blas_arg = "5", value = 4 : i32} : i32
    cgra.swi %71, %70 : i32, i32
    cf.br ^bb5
  ^bb5:  // pred: ^bb4
    "cgra.gemm_blas_asm"()[^bb6] {blas = "2"} : () -> ()
  ^bb6:  // pred: ^bb5
    return
  }
}


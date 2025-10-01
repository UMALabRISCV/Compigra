module attributes {dlti.dl_spec = #dlti.dl_spec<#dlti.dl_entry<!llvm.ptr<271>, dense<32> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<272>, dense<64> : vector<4xi32>>, #dlti.dl_entry<!llvm.ptr<270>, dense<32> : vector<4xi32>>, #dlti.dl_entry<f128, dense<128> : vector<2xi32>>, #dlti.dl_entry<f64, dense<64> : vector<2xi32>>, #dlti.dl_entry<f16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i32, dense<32> : vector<2xi32>>, #dlti.dl_entry<i16, dense<16> : vector<2xi32>>, #dlti.dl_entry<i8, dense<8> : vector<2xi32>>, #dlti.dl_entry<i1, dense<8> : vector<2xi32>>, #dlti.dl_entry<!llvm.ptr, dense<64> : vector<4xi32>>, #dlti.dl_entry<f80, dense<128> : vector<2xi32>>, #dlti.dl_entry<i64, dense<64> : vector<2xi32>>, #dlti.dl_entry<"dlti.endianness", "little">, #dlti.dl_entry<"dlti.stack_alignment", 128 : i32>>, llvm.data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", llvm.target_triple = "x86_64-unknown-linux-gnu", "polygeist.target-cpu" = "x86-64", "polygeist.target-features" = "+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87", "polygeist.tune-cpu" = "generic"} {
  func.func @conv2d(%arg0: memref<12x12xi32>, %arg1: memref<3x3xi32>, %arg2: memref<10x10xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
    %c0_i32 = arith.constant 0 : i32
    %c10_i32 = arith.constant 10 : i32
    %c10_i32_0 = arith.constant 10 : i32
    %c10_i32_1 = arith.constant 10 : i32
    %c3_i32 = arith.constant 3 : i32
    %c3_i32_2 = arith.constant 3 : i32
    %c3_i32_3 = arith.constant 3 : i32
    %c0 = arith.constant 0 : index
    %c10 = arith.constant 10 : index
    %c1 = arith.constant 1 : index
    scf.for %arg3 = %c0 to %c10 step %c1 {
      %c0_5 = arith.constant 0 : index
      %c10_6 = arith.constant 10 : index
      %c1_7 = arith.constant 1 : index
      scf.for %arg4 = %c0_5 to %c10_6 step %c1_7 {
        memref.store %c0_i32, %arg2[%arg3, %arg4] : memref<10x10xi32>
        %c0_10 = arith.constant 0 : index
        %c3 = arith.constant 3 : index
        %c1_11 = arith.constant 1 : index
        scf.for %arg5 = %c0_10 to %c3 step %c1_11 {
          %c0_13 = arith.constant 0 : index
          %c3_14 = arith.constant 3 : index
          %c1_15 = arith.constant 1 : index
          scf.for %arg6 = %c0_13 to %c3_14 step %c1_15 {
            %0 = arith.addi %arg3, %arg5 : index
            %1 = arith.addi %arg4, %arg6 : index
            %2 = memref.load %arg0[%0, %1] : memref<12x12xi32>
            %3 = memref.load %arg1[%arg5, %arg6] : memref<3x3xi32>
            %4 = arith.muli %2, %3 : i32
            %5 = memref.load %arg2[%arg3, %arg4] : memref<10x10xi32>
            %6 = arith.addi %5, %4 : i32
            memref.store %6, %arg2[%arg3, %arg4] : memref<10x10xi32>
          }
          %c3_i32_16 = arith.constant 3 : i32
          %c3_i32_17 = arith.constant 3 : i32
        }
        %c3_i32_12 = arith.constant 3 : i32
      }
      %c10_i32_8 = arith.constant 10 : i32
      %c10_i32_9 = arith.constant 10 : i32
    }
    %c10_i32_4 = arith.constant 10 : i32
    return
  }
}


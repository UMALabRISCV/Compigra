#!/bin/bash
# MLIR frontend
POLYGEIST_PATH="YOUR POLYGEIST_PATH HERE" 
MLIR_OPT="$POLYGEIST_PATH/llvm-project/build/bin/mlir-opt"
COMPIGRA_OPT="$POLYGEIST_PATH/../build/bin/compigra-opt"
BENCH_BASE="$POLYGEIST_PATH/../benchmarks"
MS_PLUGIN="MODULO SCHEDULER PLUGIN PATH HERE"

compile() { 
    local start_time=$(date +%s%3N)  # Start time in milliseconds

    local bench_name="$1"
    local bench_path="$BENCH_BASE/$bench_name"
    local bench_c="$bench_path/$bench_name.c"


    local f_affine="$bench_path/IR_opt/affine.mlir"
    local f_scf="$bench_path/IR_opt/scf.mlir"
    local f_cf="$bench_path/IR_opt/cf.mlir"

    # Check if the benchmark file exists
    if [ ! -f "$bench_c" ]; then
        echo "Error: $bench_c not found."
        exit 1
    fi

    # C -> Affine
    local include="$POLYGEIST_PATH/llvm-project/clang/lib/Headers/"
    $POLYGEIST_PATH/build/bin/cgeist "$bench_c" -I "$include" \
        -function="$bench_name" -S --O3 --memref-fullrank --raise-scf-to-affine \
        > "$f_affine"

    # scalar optimizations and loop transformations
    local f_affine1="$bench_path/IR_opt/affine1.mlir"
    $COMPIGRA_OPT --allow-unregistered-dialect --scalarize-store \
        $f_affine > $f_affine1
    mv $f_affine1 $f_affine
    
    # exploit the gemm_blas kernel
    $COMPIGRA_OPT --allow-unregistered-dialect --cse --affine-maximize-gemm \
        $f_affine > $f_affine1
    if [ $? -eq 0 ]; then
        echo "Gemm kernel exploitation success"
    else
        echo "Failed to exploit gemm kernel"
        exit 1
    fi
    mv $f_affine1 $f_affine
    
    # Lower down to scf
    $COMPIGRA_OPT --allow-unregistered-dialect \
        --lower-affine $f_affine > $f_scf

    $COMPIGRA_OPT --allow-unregistered-dialect \
        --convert-scf-to-cf $f_scf > $f_cf

    optimizer "$@" 
    if [ $? -eq 0 ]; then
        echo "MIDDLE END: CFG SIMPLIFICATION SUCCESS."
    else
        echo "MIDDLE END: CFG SIMPLIFICATION FAILED."
        exit 1
    fi 
    

    # ================================== BACK END ==================================
    mapper "$@"


    local end_time=$(date +%s%3N)  # End time in milliseconds
    local elapsed_time=$((end_time - start_time))  # Convert to milliseconds
    echo "Execution time: $((elapsed_time / 1000)).$((elapsed_time % 1000)) seconds"
    
}

optimizer(){
    local bench_name="$1"
    echo $bench_name
    shift
    local bench_path="$BENCH_BASE/$bench_name"
    local f_cf="$bench_path/IR_opt/cf.mlir"

    # run optimization passes
    for i in {1..3}; do
        local f_cfopt="$bench_path/IR_opt/cf_opt.mlir"
        $COMPIGRA_OPT --allow-unregistered-dialect \
            --merge-if-to-select "$f_cf" > "$f_cfopt"
        mv "$f_cfopt" "$f_cf"

        $COMPIGRA_OPT --allow-unregistered-dialect \
            --fuse-loop "$f_cf" > "$f_cfopt"
        mv "$f_cfopt" "$f_cf"
    done

    echo "MIDDLE END: AFFINE -> CF CONVERSION SUCCESS."
        
    return 0
}

mapper() {
    local bench_name="$1"
    local row_size="$2" 
    local col_size="$2"

    local bench_path="$BENCH_BASE/$bench_name"

    local f_cf="$bench_path/IR_opt/cf.mlir"
    local f_cgra="$bench_path/IR_opt/cgra.mlir"
    local f_cgra_i32="$bench_path/IR_opt/cgra_i32.mlir"
    local f_asm="$bench_path/IR_opt/asm.mlir"
    
    # Create structured output directory for generated files
    local output_dir="$bench_path/output"
    mkdir -p "$output_dir/schedule"
    mkdir -p "$output_dir/logs"
    mkdir -p "$output_dir/asm"

    rm -f "$bench_path/IR_opt/"$f_cgra
    rm -f "$bench_path/IR_opt/"$f_cgra_i32
    rm -f "$bench_path/IR_opt/"$f_asm


    # Convert cf to cgra
    $COMPIGRA_OPT --allow-unregistered-dialect --convert-cf-to-cgra \
            "$f_cf" > "$f_cgra"
    if [ $? -eq 0 ]; then
        echo "Convert to CGRA success."
    else
        echo "Convert to CGRA failed."
        exit 1
    fi

    # Fix the index width
    $COMPIGRA_OPT --allow-unregistered-dialect --fix-index-width --fit-openedge  "$f_cgra" > "$f_cgra_i32"
        
    if [ $? -eq 0 ]; then
        echo "Bitwidth To I32 success."
    else
        echo "Fix bit width failed."
        exit 1
    fi


    # create modulo schedule folder
    local msOpt="python3 $MS_PLUGIN"
    $COMPIGRA_OPT --allow-unregistered-dialect --debug-only=REGISTER_ALLOCATION \
                --gen-openedge-asm-fast="row=$row_size col=$col_size ms-opt='$msOpt' debug=0
                asm-out=$output_dir/asm output-dir=$output_dir" \
                "$f_cgra_i32" > "$f_asm"

    # Check if the compilation was successful
    if [ $? -eq 0 ]; then
        echo "Compilation successful."
    else
        echo "Compilation failed."
    fi
    return 0
}

benchmark=$1
cgra_size=$2
compile "$benchmark" "$cgra_size"
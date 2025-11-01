# MMUL Kernel Code Generation and Simulation

This repository provides scripts and instructions for generating and simulating the **matrix multiplication (MMUL) kernel** using both the kernel-specific and plain CDFG compilation methods.

---

## 🔧 Code Generation

You can generate the assembly code for the MMUL kernel using one of the following approaches.

### 1. Generate Assembly for the MMUL Kernel

To generate the assembly code associated with the MMUL kernel, run:

```bash
./kernel_mmul_runner.sh <benchmark> <cgra_size>
```

To generate the code compiled in plain CDFG method, run
```bash
./cdfg_runner.sh <benchmark> yes <row_size> <col_size> 1
```
The parameters, 'yes' means running middle-end optimization passes to enable CFG simplification, and '1' represent call modulo scheduler to optimize the runtime efficiency.

# How to simulate the results

A preconfigured Docker container is provided with the compiled assembly and necessary environment for simulation.
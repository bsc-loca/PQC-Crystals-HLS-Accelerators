# PQC-Crystals-HLS-Accelerators
This technology implements a PQC accelerator for FPGA-based SoCs using HLS. It supports CRYSTALS-Kyber (ML-KEM) and CRYSTALS-Dilithium (ML-DSA). Both schemes are NIST standards for post-quantum cryptography.

## Repository Structure

```
LICENSE
README.md
setenv.sh                  # Environment setup script for Vitis-HLS 2021.2 
mldsa/
    hlsIntegration/
        k_dsa.cpp          # High-level synthesis (HLS) implementation for ML-DSA
        kernel.hpp         # Header file for ML-DSA kernel
        Makefile           # Build instructions for ML-DSA HLS, prepared for synthesis on the SELENE SoC
        README.md          # Documentation for ML-DSA module
        scripts/
            mldsa2.tcl     # TCL script for ML-DSA synthesis
    src/
        mldsa_kernel.vhd   # AXI Wrapper to VHDL implementation of the ML-DSA kernel
        mldsa_pkg.vhd      # AXI VHDL package for ML-DSA
        vhdlsyn.txt        # Synthesis configuration for ML-DSA
mlkem/
    hlsIntegration/
        k_kem.cpp          # High-level synthesis (HLS) implementation for ML-KEM
        kernel.hpp         # Header file for ML-KEM kernel
        Makefile           # Build instructions for ML-KEM HLS, prepared for synthesis on the SELENE SoC
        README.md          # Documentation for ML-KEM module
        scripts/
            mlkem512.tcl   # TCL script for ML-KEM synthesis
    src/
        mlkem_kernel.vhd   # AXI Wrapper to VHDL implementation of the ML-KEM kernel
        mlkem_pkg.vhd      # AXI VHDL package for ML-KEM
        vhdlsyn.txt        # Synthesis configuration for ML-KEM
```

## Modules

### `mldsa` - CRYSTALS-Dilithium (ML-DSA)
This module implements FPGA-based ML-DSA cryptographic operations. It includes:
- High-Level Synthesis (HLS) code in `hlsIntegration/`.
- AXI VHDL implementation in `src/`.

Refer to the [mldsa/hlsIntegration/README.md](mldsa/hlsIntegration/README.md) for detailed instructions on building and testing the ML-DSA module.

### `mlkem` - CRYSTALS-Kyber (ML-KEM)
This module implements FPGA-based ML-KEM cryptographic operations. It includes:
- High-Level Synthesis (HLS) code in `hlsIntegration/`.
- AXI VHDL implementation in `src/`.

Refer to the [mlkem/hlsIntegration/README.md](mlkem/hlsIntegration/README.md) for detailed instructions on building and testing the ML-KEM module.

## Prerequisites

- **Xilinx Vivado 2020.2**: Required for synthesis of VHDL code.
- **Xilinx Vitis HLS 2021.2**: For compiling HLS C++ code.

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/bsc-loca/PQC-Crystals-HLS-Accelerators.git 
   cd PQC-Crystals-HLS-Accelerators 
   ```

2. Set up the environment to your PATH:
   ```bash
   source setenv.sh 
   ```

3. Build the HLS code:
   ```bash
   cd mldsa/hlsIntegration
   make  # For ML-DSA

   cd mlkem/hlsIntegration
   make  # For ML-KEM
   ```

## Integrations

A public integration of both accelerators has been done into the SELENE SoC. You can find it and generate a bitstream for the AMD Virtex™ UltraScale+™ FPGA VCU118 here: [https://gitlab.com/selene-riscv-platform/selene-hardware/-/tree/ft/PQC](https://gitlab.com/selene-riscv-platform/selene-hardware/-/tree/ft/PQC).

An example software is also available here: [https://gitlab.com/selene-riscv-platform/selene-software](https://gitlab.com/selene-riscv-platform/selene-software)

## Contact

For questions or support, please contact the repository maintainers.

## License

This project is licensed under the [Solderpad Hardware License v0.51](http://solderpad.org/licenses/SHL-0.51/).
See the [LICENSE](./LICENSE) file for more details.

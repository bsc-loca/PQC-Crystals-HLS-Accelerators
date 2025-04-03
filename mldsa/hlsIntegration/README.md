# README
## How to Generate the MLDSA Accelerator

### Requirements:

- Vitis\_HLS 2021.2 version

To generate the accelerator, run:

```sh
make mldsa2
```

The Makefile will configure the **MLDSA in SELENE PLATFORM** to be synthesized with **Vivado 2020.2** by executing the appropriate `make` command in `selene-soc/selene-xilinx-vcu118`.
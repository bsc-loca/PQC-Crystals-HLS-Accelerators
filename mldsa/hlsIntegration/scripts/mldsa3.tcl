############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
############################################################
set parent_dir [pwd]
open_project mldsa
set_top mldsa_accelerator
#Check
add_files k_dsa.cpp -cflags "-DDILITHIUM_MODE=3"
add_files kernel.hpp
open_solution "mldsa3" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 300MHz -name default
config_dataflow -fifo_depth 100
config_export -deadlock_detection none -format ip_catalog -ipname mldsa_accelerator_3 -output $parent_dir/mldsa/mldsa3 -rtl verilog -version 0.0.1 -vivado_clock 300MHz
config_interface -m_axi_addr64=0 -m_axi_conservative_mode
set_directive_top -name mldsa_accelerator "mldsa_accelerator"
csynth_design
export_design -flow syn -rtl verilog -format ip_catalog -output $parent_dir/mldsa/mldsa3
exit

############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
############################################################
set parent_dir [pwd]
open_project mlkem
set_top mlkem_accelerator
add_files k_kem.cpp
add_files kernel.hpp
open_solution "mlkem512" -flow_target vivado
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 300MHz -name default
config_dataflow -fifo_depth 100
config_export -deadlock_detection none -format ip_catalog -ipname mlkem_accelerator_512 -output $parent_dir/mlkem/mlkem512 -rtl verilog -version 0.0.1 -vivado_clock 300MHz
config_interface -m_axi_addr64=0 -m_axi_conservative_mode
set_directive_top -name mlkem_accelerator "mlkem_accelerator"
#csim_design
csynth_design
#cosim_design
export_design -flow syn -rtl verilog -format ip_catalog -output $parent_dir/mlkem/mlkem512
exit

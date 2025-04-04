-- Copyright (c) 2025 Barcelona Supercomputing Center
-- Licensed under the Solderpad Hardware License, Version 0.51 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at:
--
--     http://solderpad.org/licenses/SHL-0.51/
--
-- Unless required by applicable law or agreed to in writing, software,
-- hardware and materials distributed under this License are distributed
-- on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
-- either express or implied. See the License for the specific language
-- governing permissions and limitations under the License.
--
-- SPDX-License-Identifier: SHL-0.51

library ieee;
use ieee.std_logic_1164.all;

library grlib;
use grlib.stdlib.all;
use grlib.amba.all;

library interconnect;
use interconnect.libnoc.all;
use interconnect.libnoc_pkg.all;

package mldsa_pkg is

  constant C_M_AXI_ACC_MST_ADDR_WIDTH  : integer := 32;
  constant C_M_AXI_ACC_MST_DATA_WIDTH  : integer := 512;
  
  constant C_S_AXI_CONTROL_ADDR_WIDTH  : integer := 7;
  constant C_S_AXI_CONTROL_DATA_WIDTH  : integer := 32;

  constant C_M_AXI_GMEMOUT_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMOUT_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMOUT_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMSIGN_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMSIGN_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMSIGN_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMM_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMM_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMM_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMPK_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMPK_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMPK_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMSK_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMSK_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMSK_DATA_WIDTH   : integer := 32;
  

  type axi_acc_mst_out is record
    m00_axi_awvalid:   std_logic;
    m00_axi_awaddr:    std_logic_vector(C_M_AXI_ACC_MST_ADDR_WIDTH-1 downto 0);       
    m00_axi_awlen:     std_logic_vector(7 downto 0);
    m00_axi_awsize:    std_logic_vector(2 downto 0);
    m00_axi_awid:      std_logic_vector(3 downto 0); 
    m00_axi_wvalid:    std_logic;
    m00_axi_awburst:   std_logic_vector(1 downto 0);
    m00_axi_awlock :   std_logic_vector(1 downto 0);
    m00_axi_awcache:   std_logic_Vector(3 downto 0);
    m00_axi_awprot:    std_logic_vector(2 downto 0);
    m00_axi_awqos:     std_logic_vector(3 downto 0); 
    m00_axi_awregion:  std_logic_vector(3 downto 0);
    m00_axi_wdata:    std_logic_vector(C_M_AXI_ACC_MST_DATA_WIDTH-1 downto 0);
    m00_axi_wstrb:    std_logic_vector(C_M_AXI_ACC_MST_DATA_WIDTH/8-1 downto 0);
    m00_axi_wlast:    std_logic;
    m00_axi_bready:   std_logic;
    m00_axi_arvalid:  std_logic;
    m00_axi_araddr:   std_logic_vector(C_M_AXI_ACC_MST_ADDR_WIDTH-1 downto 0);
    m00_axi_arlen:    std_logic_vector(7 downto 0);
    m00_axi_arid:     std_logic_vector(3 downto 0);
    m00_axi_arsize:   std_logic_vector(2 downto 0);
    m00_axi_arburst:  std_logic_vector(1 downto 0); 
    m00_axi_arlock:   std_logic_vector(1 downto 0);
    m00_axi_arcache:  std_logic_vector(3 downto 0);
    m00_axi_arprot:   std_logic_vector(2 downto 0);
    m00_axi_arqos:    std_logic_vector(3 downto 0);
    m00_axi_arregion: std_logic_vector(3 downto 0);
    m00_axi_rready:   std_logic;
  end record;
  
  type axi_acc_mst_in is record
    m00_axi_rid:     std_logic_vector(3 downto 0);
    m00_axi_rresp:   std_logic_vector(1 downto 0);
    m00_axi_awready:  std_logic;
    m00_axi_wready:   std_logic;
    m00_axi_bvalid:   std_logic;
    m00_axi_bresp:    std_logic_vector(1 downto 0);
    m00_axi_bid:      std_logic_vector(3 downto 0);
    m00_axi_arready:  std_logic;
    m00_axi_rvalid:   std_logic;
    m00_axi_rdata:    std_logic_vector(C_M_AXI_ACC_MST_DATA_WIDTH-1 downto 0);
    m00_axi_rlast:    std_logic; 
  end record;
  
  type axi_acc_slv_out is record --AXI-lite slave interface
    s_axi_control_awready:    std_logic;
    s_axi_control_wready:     std_logic;
    s_axi_control_arready:    std_logic;
    s_axi_control_rvalid:     std_logic;
    s_axi_control_rdata:     std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH-1 downto 0);
    s_axi_control_rresp:     std_logic_vector(1 downto 0);
    s_axi_control_bvalid:    std_logic;
    s_axi_control_bresp:     std_logic_vector(1 downto 0);
  end record;
  
  type axi_acc_slv_in is record
    s_axi_control_awvalid:   std_logic;
    s_axi_control_awaddr:    std_logic_vector(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0);
    s_axi_control_wvalid:    std_logic;
    s_axi_control_wdata:     std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH-1 downto 0);
    s_axi_control_wstrb:     std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH/8-1 downto 0);
    s_axi_control_arvalid:   std_logic;
    s_axi_control_araddr:    std_logic_vector(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0);
    s_axi_control_rready:    std_logic;
    s_axi_control_bready:    std_logic;
  end record; 

  component mldsa_kernel
    port (
      clk:              in  std_logic;
      rst_n:            in  std_logic;
      axi_control_in:   in  axi_mosi_type;
      axi_control_out:  out axi_somi_type; 
      axi_to_mem:     out axi4wide_mosi_type;
      axi_from_mem:   in  axiwide_somi_type;
      axi_to_mem_1:     out axi4wide_mosi_type;
      axi_from_mem_1:   in  axiwide_somi_type;
      axi_to_mem_2:     out axi4wide_mosi_type;
      axi_from_mem_2:   in  axiwide_somi_type;
      axi_to_mem_3:     out axi4wide_mosi_type;
      axi_from_mem_3:   in  axiwide_somi_type;
      axi_to_mem_4:     out axi4wide_mosi_type;
      axi_from_mem_4:   in  axiwide_somi_type;
      interrupt:        out std_logic
    );  
  end component;

  component mldsa_accelerator 
  is
    generic (
      C_S_AXI_CONTROL_DATA_WIDTH : integer := C_S_AXI_CONTROL_DATA_WIDTH;
      C_M_AXI_GMEMOUT_ID_WIDTH      : integer := C_M_AXI_GMEMOUT_ID_WIDTH;
      C_M_AXI_GMEMSIGN_ID_WIDTH     : integer := C_M_AXI_GMEMSIGN_ID_WIDTH;
      C_M_AXI_GMEMM_ID_WIDTH     : integer := C_M_AXI_GMEMM_ID_WIDTH;
      C_M_AXI_GMEMPK_ID_WIDTH     : integer := C_M_AXI_GMEMPK_ID_WIDTH;
      C_M_AXI_GMEMSK_ID_WIDTH     : integer := C_M_AXI_GMEMSK_ID_WIDTH;
      
      C_M_AXI_GMEMSIGN_DATA_WIDTH     : integer := C_M_AXI_GMEMSIGN_DATA_WIDTH;
      C_M_AXI_GMEMM_DATA_WIDTH     : integer := C_M_AXI_GMEMM_DATA_WIDTH;
      C_M_AXI_GMEMPK_DATA_WIDTH     : integer := C_M_AXI_GMEMPK_DATA_WIDTH;
      C_M_AXI_GMEMSK_DATA_WIDTH     : integer := C_M_AXI_GMEMSK_DATA_WIDTH
      

    );
    port (
      ap_clk                      : in std_logic;
      ap_rst_n                    : in std_logic;
      
      m_axi_gmemout_AWVALID          : out std_logic;
      m_axi_gmemout_AWREADY          : in std_logic;
      m_axi_gmemout_AWADDR           : out std_logic_vector(C_M_AXI_GMEMOUT_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemout_AWID             : out std_logic_vector(C_M_AXI_GMEMOUT_ID_WIDTH-1 downto 0);
      m_axi_gmemout_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemout_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemout_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemout_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemout_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemout_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemout_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemout_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemout_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemout_WVALID :         out std_logic;
      m_axi_gmemout_WREADY :         in  std_logic;
      m_axi_gmemout_WDATA :          out std_logic_vector (C_M_AXI_GMEMOUT_DATA_WIDTH-1 downto 0);
      m_axi_gmemout_WSTRB :          out std_logic_vector (C_M_AXI_GMEMOUT_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemout_WLAST :          out std_logic;
      m_axi_gmemout_WID :            out std_logic_vector (C_M_AXI_GMEMOUT_ID_WIDTH -1 downto 0);
      m_axi_gmemout_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemout_ARVALID :        out std_logic;
      m_axi_gmemout_ARREADY :        in  std_logic;
      m_axi_gmemout_ARADDR :         out std_logic_vector (C_M_AXI_GMEMOUT_ADDR_WIDTH-1 downto 0);
      m_axi_gmemout_ARID :           out std_logic_vector (C_M_AXI_GMEMOUT_ID_WIDTH-1 downto 0);
      m_axi_gmemout_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemout_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemout_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemout_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemout_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemout_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemout_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemout_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemout_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemout_RVALID :         in  std_logic;
      m_axi_gmemout_RREADY :         out std_logic;
      m_axi_gmemout_RDATA :          in  std_logic_vector (C_M_AXI_GMEMOUT_DATA_WIDTH - 1 downto 0);
      m_axi_gmemout_RLAST :          in  std_logic;
      m_axi_gmemout_RID :            in  std_logic_vector (C_M_AXI_GMEMOUT_ID_WIDTH - 1 downto 0);
      m_axi_gmemout_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemout_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemout_BVALID :         in  std_logic;
      m_axi_gmemout_BREADY :         out std_logic;
      m_axi_gmemout_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemout_BID :            in  std_logic_vector (C_M_AXI_GMEMOUT_ID_WIDTH - 1 downto 0);
      m_axi_gmemout_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmemsign_AWVALID          : out std_logic;
      m_axi_gmemsign_AWREADY          : in std_logic;
      m_axi_gmemsign_AWADDR           : out std_logic_vector(C_M_AXI_GMEMSIGN_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemsign_AWID             : out std_logic_vector(C_M_AXI_GMEMSIGN_ID_WIDTH-1 downto 0);
      m_axi_gmemsign_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemsign_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemsign_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemsign_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemsign_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemsign_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemsign_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemsign_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemsign_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemsign_WVALID :         out std_logic;
      m_axi_gmemsign_WREADY :         in  std_logic;
      m_axi_gmemsign_WDATA :          out std_logic_vector (C_M_AXI_GMEMSIGN_DATA_WIDTH-1 downto 0);
      m_axi_gmemsign_WSTRB :          out std_logic_vector (C_M_AXI_GMEMSIGN_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemsign_WLAST :          out std_logic;
      m_axi_gmemsign_WID :            out std_logic_vector (C_M_AXI_GMEMSIGN_ID_WIDTH -1 downto 0);
      m_axi_gmemsign_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemsign_ARVALID :        out std_logic;
      m_axi_gmemsign_ARREADY :        in  std_logic;
      m_axi_gmemsign_ARADDR :         out std_logic_vector (C_M_AXI_GMEMSIGN_ADDR_WIDTH-1 downto 0);
      m_axi_gmemsign_ARID :           out std_logic_vector (C_M_AXI_GMEMSIGN_ID_WIDTH-1 downto 0);
      m_axi_gmemsign_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemsign_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemsign_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemsign_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemsign_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemsign_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemsign_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemsign_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemsign_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemsign_RVALID :         in  std_logic;
      m_axi_gmemsign_RREADY :         out std_logic;
      m_axi_gmemsign_RDATA :          in  std_logic_vector (C_M_AXI_GMEMSIGN_DATA_WIDTH - 1 downto 0);
      m_axi_gmemsign_RLAST :          in  std_logic;
      m_axi_gmemsign_RID :            in  std_logic_vector (C_M_AXI_GMEMSIGN_ID_WIDTH - 1 downto 0);
      m_axi_gmemsign_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemsign_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemsign_BVALID :         in  std_logic;
      m_axi_gmemsign_BREADY :         out std_logic;
      m_axi_gmemsign_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemsign_BID :            in  std_logic_vector (C_M_AXI_GMEMSIGN_ID_WIDTH - 1 downto 0);
      m_axi_gmemsign_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmemm_AWVALID          : out std_logic;
      m_axi_gmemm_AWREADY          : in std_logic;
      m_axi_gmemm_AWADDR           : out std_logic_vector(C_M_AXI_GMEMM_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemm_AWID             : out std_logic_vector(C_M_AXI_GMEMM_ID_WIDTH-1 downto 0);
      m_axi_gmemm_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemm_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemm_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemm_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemm_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemm_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemm_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemm_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemm_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemm_WVALID :         out std_logic;
      m_axi_gmemm_WREADY :         in  std_logic;
      m_axi_gmemm_WDATA :          out std_logic_vector (C_M_AXI_GMEMM_DATA_WIDTH-1 downto 0);
      m_axi_gmemm_WSTRB :          out std_logic_vector (C_M_AXI_GMEMM_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemm_WLAST :          out std_logic;
      m_axi_gmemm_WID :            out std_logic_vector (C_M_AXI_GMEMM_ID_WIDTH -1 downto 0);
      m_axi_gmemm_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemm_ARVALID :        out std_logic;
      m_axi_gmemm_ARREADY :        in  std_logic;
      m_axi_gmemm_ARADDR :         out std_logic_vector (C_M_AXI_GMEMM_ADDR_WIDTH-1 downto 0);
      m_axi_gmemm_ARID :           out std_logic_vector (C_M_AXI_GMEMM_ID_WIDTH-1 downto 0);
      m_axi_gmemm_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemm_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemm_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemm_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemm_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemm_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemm_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemm_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemm_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemm_RVALID :         in  std_logic;
      m_axi_gmemm_RREADY :         out std_logic;
      m_axi_gmemm_RDATA :          in  std_logic_vector (C_M_AXI_GMEMM_DATA_WIDTH - 1 downto 0);
      m_axi_gmemm_RLAST :          in  std_logic;
      m_axi_gmemm_RID :            in  std_logic_vector (C_M_AXI_GMEMM_ID_WIDTH - 1 downto 0);
      m_axi_gmemm_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemm_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemm_BVALID :         in  std_logic;
      m_axi_gmemm_BREADY :         out std_logic;
      m_axi_gmemm_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemm_BID :            in  std_logic_vector (C_M_AXI_GMEMM_ID_WIDTH - 1 downto 0);
      m_axi_gmemm_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmempk_AWVALID          : out std_logic;
      m_axi_gmempk_AWREADY          : in std_logic;
      m_axi_gmempk_AWADDR           : out std_logic_vector(C_M_AXI_GMEMPK_ADDR_WIDTH-1 downto 0);       
      m_axi_gmempk_AWID             : out std_logic_vector(C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0);
      m_axi_gmempk_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmempk_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmempk_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmempk_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmempk_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmempk_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmempk_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmempk_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmempk_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmempk_WVALID :         out std_logic;
      m_axi_gmempk_WREADY :         in  std_logic;
      m_axi_gmempk_WDATA :          out std_logic_vector (C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0);
      m_axi_gmempk_WSTRB :          out std_logic_vector (C_M_AXI_GMEMPK_DATA_WIDTH/8-1 downto 0);
      m_axi_gmempk_WLAST :          out std_logic;
      m_axi_gmempk_WID :            out std_logic_vector (C_M_AXI_GMEMPK_ID_WIDTH -1 downto 0);
      m_axi_gmempk_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmempk_ARVALID :        out std_logic;
      m_axi_gmempk_ARREADY :        in  std_logic;
      m_axi_gmempk_ARADDR :         out std_logic_vector (C_M_AXI_GMEMPK_ADDR_WIDTH-1 downto 0);
      m_axi_gmempk_ARID :           out std_logic_vector (C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0);
      m_axi_gmempk_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmempk_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmempk_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmempk_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmempk_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmempk_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmempk_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmempk_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmempk_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmempk_RVALID :         in  std_logic;
      m_axi_gmempk_RREADY :         out std_logic;
      m_axi_gmempk_RDATA :          in  std_logic_vector (C_M_AXI_GMEMPK_DATA_WIDTH - 1 downto 0);
      m_axi_gmempk_RLAST :          in  std_logic;
      m_axi_gmempk_RID :            in  std_logic_vector (C_M_AXI_GMEMPK_ID_WIDTH - 1 downto 0);
      m_axi_gmempk_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmempk_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmempk_BVALID :         in  std_logic;
      m_axi_gmempk_BREADY :         out std_logic;
      m_axi_gmempk_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmempk_BID :            in  std_logic_vector (C_M_AXI_GMEMPK_ID_WIDTH - 1 downto 0);
      m_axi_gmempk_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmemsk_AWVALID          : out std_logic;
      m_axi_gmemsk_AWREADY          : in std_logic;
      m_axi_gmemsk_AWADDR           : out std_logic_vector(C_M_AXI_GMEMSK_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemsk_AWID             : out std_logic_vector(C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0);
      m_axi_gmemsk_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemsk_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemsk_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemsk_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemsk_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemsk_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemsk_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemsk_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemsk_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemsk_WVALID :         out std_logic;
      m_axi_gmemsk_WREADY :         in  std_logic;
      m_axi_gmemsk_WDATA :          out std_logic_vector (C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0);
      m_axi_gmemsk_WSTRB :          out std_logic_vector (C_M_AXI_GMEMSK_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemsk_WLAST :          out std_logic;
      m_axi_gmemsk_WID :            out std_logic_vector (C_M_AXI_GMEMSK_ID_WIDTH -1 downto 0);
      m_axi_gmemsk_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemsk_ARVALID :        out std_logic;
      m_axi_gmemsk_ARREADY :        in  std_logic;
      m_axi_gmemsk_ARADDR :         out std_logic_vector (C_M_AXI_GMEMSK_ADDR_WIDTH-1 downto 0);
      m_axi_gmemsk_ARID :           out std_logic_vector (C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0);
      m_axi_gmemsk_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemsk_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemsk_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemsk_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemsk_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemsk_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemsk_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemsk_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemsk_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemsk_RVALID :         in  std_logic;
      m_axi_gmemsk_RREADY :         out std_logic;
      m_axi_gmemsk_RDATA :          in  std_logic_vector (C_M_AXI_GMEMSK_DATA_WIDTH - 1 downto 0);
      m_axi_gmemsk_RLAST :          in  std_logic;
      m_axi_gmemsk_RID :            in  std_logic_vector (C_M_AXI_GMEMSK_ID_WIDTH - 1 downto 0);
      m_axi_gmemsk_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemsk_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemsk_BVALID :         in  std_logic;
      m_axi_gmemsk_BREADY :         out std_logic;
      m_axi_gmemsk_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemsk_BID :            in  std_logic_vector (C_M_AXI_GMEMSK_ID_WIDTH - 1 downto 0);
      m_axi_gmemsk_BUSER :          in  std_logic_vector (0 downto 0);
  
      -- AXI4-Lite slave interface
      s_axi_control_AWVALID: in std_logic;
      s_axi_control_AWREADY: out std_logic;
      s_axi_control_AWADDR: in std_logic_vector(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0);
      s_axi_control_WVALID: in std_logic;
      s_axi_control_WREADY: out std_logic;
      s_axi_control_WDATA: in std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH-1 downto 0);
      s_axi_control_WSTRB: in std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH/8-1 downto 0);
      s_axi_control_ARVALID: in std_logic;
      s_axi_control_ARREADY: out  std_logic;
      s_axi_control_ARADDR: in std_logic_vector(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0);
      s_axi_control_RVALID: out std_logic;
      s_axi_control_RREADY: in std_logic;
      s_axi_control_RDATA: out std_logic_vector(C_S_AXI_CONTROL_DATA_WIDTH-1 downto 0);
      s_axi_control_RRESP: out std_logic_vector(1 downto 0);
      s_axi_control_BVALID: out std_logic;
      s_axi_control_BREADY: in std_logic;
      s_axi_control_BRESP: out std_logic_vector(1 downto 0);
      interrupt: out std_logic
    );
    end component;
    
end package;



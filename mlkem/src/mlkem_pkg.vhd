--MIT License
--
--Copyright (c) 2023 BSC 
--
--Permission is hereby granted, free of charge, to any person obtaining a copy
--of this software and associated documentation files (the "Software"), to deal
--in the Software without restriction, including without limitation the rights
--to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
--copies of the Software, and to permit persons to whom the Software is
--furnished to do so, subject to the following conditions:
--
--The above copyright notice and this permission notice shall be included in all
--copies or substantial portions of the Software.
--
--THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
--IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
--FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
--AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
--LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
--OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
--SOFTWARE.

library ieee;
use ieee.std_logic_1164.all;

library grlib;
use grlib.stdlib.all;
use grlib.amba.all;

library interconnect;
use interconnect.libnoc.all;
use interconnect.libnoc_pkg.all;

package mlkem_pkg is

  constant C_M_AXI_ACC_MST_ADDR_WIDTH  : integer := 32;
  constant C_M_AXI_ACC_MST_DATA_WIDTH  : integer := 512;
  
  constant C_S_AXI_CONTROL_ADDR_WIDTH  : integer := 7;
  constant C_S_AXI_CONTROL_DATA_WIDTH  : integer := 32;

  constant C_M_AXI_GMEMCT_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMCT_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMCT_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMSS_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMSS_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMSS_DATA_WIDTH   : integer := 32;
  
  constant C_M_AXI_GMEMBUF_ID_WIDTH     : integer := 1;
  constant C_M_AXI_GMEMBUF_ADDR_WIDTH   : integer := 32;
  constant C_M_AXI_GMEMBUF_DATA_WIDTH   : integer := 32;
  
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

  component mlkem_kernel
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

  component mlkem_accelerator 
  is
    generic (
      C_S_AXI_CONTROL_DATA_WIDTH : integer := C_S_AXI_CONTROL_DATA_WIDTH;
      --C_M_AXI_ACC_MST_ADDR_WIDTH  : integer := C_M_AXI_ACC_MST_ADDR_WIDTH;
      --C_M_AXI_ACC_MST_DATA_WIDTH  : integer := C_M_AXI_ACC_MST_DATA_WIDTH;
      C_M_AXI_GMEMCT_ID_WIDTH      : integer := C_M_AXI_GMEMCT_ID_WIDTH;
      C_M_AXI_GMEMSS_ID_WIDTH     : integer := C_M_AXI_GMEMSS_ID_WIDTH;
      C_M_AXI_GMEMBUF_ID_WIDTH     : integer := C_M_AXI_GMEMBUF_ID_WIDTH;
      C_M_AXI_GMEMPK_ID_WIDTH     : integer := C_M_AXI_GMEMPK_ID_WIDTH;
      C_M_AXI_GMEMSK_ID_WIDTH     : integer := C_M_AXI_GMEMSK_ID_WIDTH;
      
      C_M_AXI_GMEMSS_DATA_WIDTH     : integer := C_M_AXI_GMEMSS_DATA_WIDTH;
      C_M_AXI_GMEMBUF_DATA_WIDTH     : integer := C_M_AXI_GMEMBUF_DATA_WIDTH;
      C_M_AXI_GMEMPK_DATA_WIDTH     : integer := C_M_AXI_GMEMPK_DATA_WIDTH;
      C_M_AXI_GMEMSK_DATA_WIDTH     : integer := C_M_AXI_GMEMSK_DATA_WIDTH
      
    );
    port (
      ap_clk                      : in std_logic;
      ap_rst_n                    : in std_logic;
      
      m_axi_gmemct_AWVALID          : out std_logic;
      m_axi_gmemct_AWREADY          : in std_logic;
      m_axi_gmemct_AWADDR           : out std_logic_vector(C_M_AXI_GMEMCT_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemct_AWID             : out std_logic_vector(C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0);
      m_axi_gmemct_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemct_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemct_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemct_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemct_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemct_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemct_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemct_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemct_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemct_WVALID :         out std_logic;
      m_axi_gmemct_WREADY :         in  std_logic;
      m_axi_gmemct_WDATA :          out std_logic_vector (C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0);
      m_axi_gmemct_WSTRB :          out std_logic_vector (C_M_AXI_GMEMCT_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemct_WLAST :          out std_logic;
      m_axi_gmemct_WID :            out std_logic_vector (C_M_AXI_GMEMCT_ID_WIDTH -1 downto 0);
      m_axi_gmemct_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemct_ARVALID :        out std_logic;
      m_axi_gmemct_ARREADY :        in  std_logic;
      m_axi_gmemct_ARADDR :         out std_logic_vector (C_M_AXI_GMEMCT_ADDR_WIDTH-1 downto 0);
      m_axi_gmemct_ARID :           out std_logic_vector (C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0);
      m_axi_gmemct_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemct_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemct_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemct_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemct_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemct_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemct_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemct_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemct_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemct_RVALID :         in  std_logic;
      m_axi_gmemct_RREADY :         out std_logic;
      m_axi_gmemct_RDATA :          in  std_logic_vector (C_M_AXI_GMEMCT_DATA_WIDTH - 1 downto 0);
      m_axi_gmemct_RLAST :          in  std_logic;
      m_axi_gmemct_RID :            in  std_logic_vector (C_M_AXI_GMEMCT_ID_WIDTH - 1 downto 0);
      m_axi_gmemct_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemct_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemct_BVALID :         in  std_logic;
      m_axi_gmemct_BREADY :         out std_logic;
      m_axi_gmemct_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemct_BID :            in  std_logic_vector (C_M_AXI_GMEMCT_ID_WIDTH - 1 downto 0);
      m_axi_gmemct_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmemss_AWVALID          : out std_logic;
      m_axi_gmemss_AWREADY          : in std_logic;
      m_axi_gmemss_AWADDR           : out std_logic_vector(C_M_AXI_GMEMSS_ADDR_WIDTH-1 downto 0);       
      m_axi_gmemss_AWID             : out std_logic_vector(C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0);
      m_axi_gmemss_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmemss_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmemss_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmemss_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemss_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemss_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemss_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemss_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemss_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemss_WVALID :         out std_logic;
      m_axi_gmemss_WREADY :         in  std_logic;
      m_axi_gmemss_WDATA :          out std_logic_vector (C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0);
      m_axi_gmemss_WSTRB :          out std_logic_vector (C_M_AXI_GMEMSS_DATA_WIDTH/8-1 downto 0);
      m_axi_gmemss_WLAST :          out std_logic;
      m_axi_gmemss_WID :            out std_logic_vector (C_M_AXI_GMEMSS_ID_WIDTH -1 downto 0);
      m_axi_gmemss_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmemss_ARVALID :        out std_logic;
      m_axi_gmemss_ARREADY :        in  std_logic;
      m_axi_gmemss_ARADDR :         out std_logic_vector (C_M_AXI_GMEMSS_ADDR_WIDTH-1 downto 0);
      m_axi_gmemss_ARID :           out std_logic_vector (C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0);
      m_axi_gmemss_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmemss_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmemss_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmemss_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmemss_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmemss_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmemss_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmemss_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmemss_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmemss_RVALID :         in  std_logic;
      m_axi_gmemss_RREADY :         out std_logic;
      m_axi_gmemss_RDATA :          in  std_logic_vector (C_M_AXI_GMEMSS_DATA_WIDTH - 1 downto 0);
      m_axi_gmemss_RLAST :          in  std_logic;
      m_axi_gmemss_RID :            in  std_logic_vector (C_M_AXI_GMEMSS_ID_WIDTH - 1 downto 0);
      m_axi_gmemss_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmemss_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemss_BVALID :         in  std_logic;
      m_axi_gmemss_BREADY :         out std_logic;
      m_axi_gmemss_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmemss_BID :            in  std_logic_vector (C_M_AXI_GMEMSS_ID_WIDTH - 1 downto 0);
      m_axi_gmemss_BUSER :          in  std_logic_vector (0 downto 0);

      m_axi_gmembuf_AWVALID          : out std_logic;
      m_axi_gmembuf_AWREADY          : in std_logic;
      m_axi_gmembuf_AWADDR           : out std_logic_vector(C_M_AXI_GMEMBUF_ADDR_WIDTH-1 downto 0);       
      m_axi_gmembuf_AWID             : out std_logic_vector(C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0);
      m_axi_gmembuf_AWLEN            : out std_logic_vector(7 downto 0);
      m_axi_gmembuf_AWSIZE           : out std_logic_vector(2 downto 0);
      m_axi_gmembuf_AWBURST         : out std_logic_vector (1 downto 0);
      m_axi_gmembuf_AWLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmembuf_AWCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmembuf_AWPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmembuf_AWQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmembuf_AWREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmembuf_AWUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmembuf_WVALID :         out std_logic;
      m_axi_gmembuf_WREADY :         in  std_logic;
      m_axi_gmembuf_WDATA :          out std_logic_vector (C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0);
      m_axi_gmembuf_WSTRB :          out std_logic_vector (C_M_AXI_GMEMBUF_DATA_WIDTH/8-1 downto 0);
      m_axi_gmembuf_WLAST :          out std_logic;
      m_axi_gmembuf_WID :            out std_logic_vector (C_M_AXI_GMEMBUF_ID_WIDTH -1 downto 0);
      m_axi_gmembuf_WUSER :          out std_logic_vector (0 downto 0);
      m_axi_gmembuf_ARVALID :        out std_logic;
      m_axi_gmembuf_ARREADY :        in  std_logic;
      m_axi_gmembuf_ARADDR :         out std_logic_vector (C_M_AXI_GMEMBUF_ADDR_WIDTH-1 downto 0);
      m_axi_gmembuf_ARID :           out std_logic_vector (C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0);
      m_axi_gmembuf_ARLEN :          out std_logic_vector (7 downto 0);
      m_axi_gmembuf_ARSIZE :         out std_logic_vector (2 downto 0);
      m_axi_gmembuf_ARBURST :        out std_logic_vector (1 downto 0);
      m_axi_gmembuf_ARLOCK :         out std_logic_vector (1 downto 0);
      m_axi_gmembuf_ARCACHE :        out std_logic_vector (3 downto 0);
      m_axi_gmembuf_ARPROT :         out std_logic_vector (2 downto 0);
      m_axi_gmembuf_ARQOS :          out std_logic_vector (3 downto 0);
      m_axi_gmembuf_ARREGION :       out std_logic_vector (3 downto 0);
      m_axi_gmembuf_ARUSER :         out std_logic_vector (0 downto 0);
      m_axi_gmembuf_RVALID :         in  std_logic;
      m_axi_gmembuf_RREADY :         out std_logic;
      m_axi_gmembuf_RDATA :          in  std_logic_vector (C_M_AXI_GMEMBUF_DATA_WIDTH - 1 downto 0);
      m_axi_gmembuf_RLAST :          in  std_logic;
      m_axi_gmembuf_RID :            in  std_logic_vector (C_M_AXI_GMEMBUF_ID_WIDTH - 1 downto 0);
      m_axi_gmembuf_RUSER :          in  std_logic_vector (0 downto 0); 
      m_axi_gmembuf_RRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmembuf_BVALID :         in  std_logic;
      m_axi_gmembuf_BREADY :         out std_logic;
      m_axi_gmembuf_BRESP :          in  std_logic_vector (1 downto 0);
      m_axi_gmembuf_BID :            in  std_logic_vector (C_M_AXI_GMEMBUF_ID_WIDTH - 1 downto 0);
      m_axi_gmembuf_BUSER :          in  std_logic_vector (0 downto 0);

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



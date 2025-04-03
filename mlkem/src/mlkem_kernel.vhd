--MIT License
--
--Copyright (c) 2021 UPV 
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

library accelerators;
use accelerators.mlkem_pkg.all;

entity mlkem_kernel is
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
end entity;

architecture RTL of mlkem_kernel is

signal acc_mst_in:  axi_acc_mst_in;
signal acc_mst_out: axi_acc_mst_out;

signal acc_mst_in_1:  axi_acc_mst_in;
signal acc_mst_out_1: axi_acc_mst_out;

signal acc_mst_in_2:  axi_acc_mst_in;
signal acc_mst_out_2: axi_acc_mst_out;

signal acc_mst_in_3:  axi_acc_mst_in;
signal acc_mst_out_3: axi_acc_mst_out;

signal acc_mst_in_4:  axi_acc_mst_in;
signal acc_mst_out_4: axi_acc_mst_out;

signal acc_slv_in:    axi_acc_slv_in;
signal acc_slv_out:   axi_acc_slv_out;

signal aux_axi_awaddr:    std_logic_vector(C_M_AXI_GMEMCT_ADDR_WIDTH-1 downto 0);
signal aux_axi_araddr:    std_logic_vector(C_M_AXI_GMEMCT_ADDR_WIDTH-1 downto 0);

signal aux_1_axi_awaddr:    std_logic_vector(C_M_AXI_GMEMSS_ADDR_WIDTH-1 downto 0);
signal aux_1_axi_araddr:    std_logic_vector(C_M_AXI_GMEMSS_ADDR_WIDTH-1 downto 0);

signal aux_2_axi_awaddr:    std_logic_vector(C_M_AXI_GMEMBUF_ADDR_WIDTH-1 downto 0);
signal aux_2_axi_araddr:    std_logic_vector(C_M_AXI_GMEMBUF_ADDR_WIDTH-1 downto 0);

signal aux_3_axi_awaddr:    std_logic_vector(C_M_AXI_GMEMPK_ADDR_WIDTH-1 downto 0);
signal aux_3_axi_araddr:    std_logic_vector(C_M_AXI_GMEMPK_ADDR_WIDTH-1 downto 0);

signal aux_4_axi_awaddr:    std_logic_vector(C_M_AXI_GMEMSK_ADDR_WIDTH-1 downto 0);
signal aux_4_axi_araddr:    std_logic_vector(C_M_AXI_GMEMSK_ADDR_WIDTH-1 downto 0);

constant C_M_AXI_CURRENT_GMEM_ADDR_WIDTH   : integer := 32;

begin
-- mst AXI4 interface OUT
  axi_to_mem.aw.valid  <= acc_mst_out.m00_axi_awvalid; 
  axi_to_mem.aw.addr   <= acc_mst_out.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem.aw.len    <= acc_mst_out.m00_axi_awlen;
  axi_to_mem.aw.id     <= acc_mst_out.m00_axi_awid;
  axi_to_mem.aw.size   <= acc_mst_out.m00_axi_awsize; --log2(data width in bytes)
  axi_to_mem.aw.burst  <= acc_mst_out.m00_axi_awburst; 
  axi_to_mem.aw.lock   <= acc_mst_out.m00_axi_awlock(0);
  axi_to_mem.aw.cache  <= acc_mst_out.m00_axi_awcache;
  axi_to_mem.aw.prot   <= acc_mst_out.m00_axi_awprot;
 
  axi_to_mem.w.valid   <= acc_mst_out.m00_axi_wvalid;
  axi_to_mem.w.data(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0)    <= acc_mst_out.m00_axi_wdata(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0); 
  axi_to_mem.w.strb(C_M_AXI_GMEMCT_DATA_WIDTH/8-1 downto 0)    <= acc_mst_out.m00_axi_wstrb(C_M_AXI_GMEMCT_DATA_WIDTH/8-1 downto 0);
  axi_to_mem.w.last    <= acc_mst_out.m00_axi_wlast; 
  axi_to_mem.b.ready   <= acc_mst_out.m00_axi_bready;
  axi_to_mem.ar.valid <= acc_mst_out.m00_axi_arvalid;
  axi_to_mem.ar.addr  <= acc_mst_out.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem.ar.len   <= acc_mst_out.m00_axi_arlen;
  axi_to_mem.ar.id    <= acc_mst_out.m00_axi_arid;
  axi_to_mem.ar.size  <= acc_mst_out.m00_axi_arsize;
  axi_to_mem.ar.burst <= acc_mst_out.m00_axi_arburst;
  axi_to_mem.ar.lock  <= acc_mst_out.m00_axi_arlock(0);
  axi_to_mem.ar.cache <= acc_mst_out.m00_axi_arcache;
  axi_to_mem.ar.prot  <= acc_mst_out.m00_axi_arprot;
  axi_to_mem.r.ready  <= acc_mst_out.m00_axi_rready;
 
  axi_to_mem.aw.qos   <= "0000";
  axi_to_mem.ar.qos   <= "0000";
 
 -- mst AXI4 interface IN
  acc_mst_in.m00_axi_awready <= axi_from_mem.aw.ready;
  acc_mst_in.m00_axi_wready  <= axi_from_mem.w.ready;
  acc_mst_in.m00_axi_bvalid  <= axi_from_mem.b.valid;
  acc_mst_in.m00_axi_bresp   <= axi_from_mem.b.resp;
  acc_mst_in.m00_axi_bid     <= axi_from_mem.b.id;
  acc_mst_in.m00_axi_arready <= axi_from_mem.ar.ready;
  acc_mst_in.m00_axi_rvalid  <= axi_from_mem.r.valid;
  acc_mst_in.m00_axi_rdata(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0)   <= axi_from_mem.r.data(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0);
  acc_mst_in.m00_axi_rlast   <= axi_from_mem.r.last;
  acc_mst_in.m00_axi_rid     <= axi_from_mem.r.id;
  acc_mst_in.m00_axi_rresp   <= axi_from_mem.r.resp;
 
 
-- mst1 AXI4 interface OUT
  axi_to_mem_1.aw.valid  <= acc_mst_out_1.m00_axi_awvalid; 
  axi_to_mem_1.aw.addr   <= acc_mst_out_1.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_1.aw.len    <= acc_mst_out_1.m00_axi_awlen;
  axi_to_mem_1.aw.id     <= acc_mst_out_1.m00_axi_awid;
  axi_to_mem_1.aw.size   <= acc_mst_out_1.m00_axi_awsize; --log2(data width in bytes)
  axi_to_mem_1.aw.burst  <= acc_mst_out_1.m00_axi_awburst; 
  axi_to_mem_1.aw.lock   <= acc_mst_out_1.m00_axi_awlock(0);
  axi_to_mem_1.aw.cache  <= acc_mst_out_1.m00_axi_awcache;
  axi_to_mem_1.aw.prot   <= acc_mst_out_1.m00_axi_awprot;
 
  axi_to_mem_1.w.valid   <= acc_mst_out_1.m00_axi_wvalid;
  axi_to_mem_1.w.data(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0)    <= acc_mst_out_1.m00_axi_wdata(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0); 
  axi_to_mem_1.w.strb(C_M_AXI_GMEMSS_DATA_WIDTH/8-1 downto 0)    <= acc_mst_out_1.m00_axi_wstrb(C_M_AXI_GMEMSS_DATA_WIDTH/8-1 downto 0);
  axi_to_mem_1.w.last    <= acc_mst_out_1.m00_axi_wlast; 
  axi_to_mem_1.b.ready   <= acc_mst_out_1.m00_axi_bready;
  axi_to_mem_1.ar.valid <= acc_mst_out_1.m00_axi_arvalid;
  axi_to_mem_1.ar.addr  <= acc_mst_out_1.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_1.ar.len   <= acc_mst_out_1.m00_axi_arlen;
  axi_to_mem_1.ar.id    <= acc_mst_out_1.m00_axi_arid;
  axi_to_mem_1.ar.size  <= acc_mst_out_1.m00_axi_arsize;
  axi_to_mem_1.ar.burst <= acc_mst_out_1.m00_axi_arburst;
  axi_to_mem_1.ar.lock  <= acc_mst_out_1.m00_axi_arlock(0);
  axi_to_mem_1.ar.cache <= acc_mst_out_1.m00_axi_arcache;
  axi_to_mem_1.ar.prot  <= acc_mst_out_1.m00_axi_arprot;
  axi_to_mem_1.r.ready  <= acc_mst_out_1.m00_axi_rready;
 
  axi_to_mem_1.aw.qos   <= "0000";
  axi_to_mem_1.ar.qos   <= "0000";
 
 -- mst1 AXI4 interface IN
  acc_mst_in_1.m00_axi_awready <= axi_from_mem_1.aw.ready;
  acc_mst_in_1.m00_axi_wready  <= axi_from_mem_1.w.ready;
  acc_mst_in_1.m00_axi_bvalid  <= axi_from_mem_1.b.valid;
  acc_mst_in_1.m00_axi_bresp   <= axi_from_mem_1.b.resp;
  acc_mst_in_1.m00_axi_bid     <= axi_from_mem_1.b.id;
  acc_mst_in_1.m00_axi_arready <= axi_from_mem_1.ar.ready;
  acc_mst_in_1.m00_axi_rvalid  <= axi_from_mem_1.r.valid;
  acc_mst_in_1.m00_axi_rdata(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0)   <= axi_from_mem_1.r.data(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0);
  acc_mst_in_1.m00_axi_rlast   <= axi_from_mem_1.r.last;
  acc_mst_in_1.m00_axi_rid     <= axi_from_mem_1.r.id;
  acc_mst_in_1.m00_axi_rresp   <= axi_from_mem_1.r.resp;
 
 
-- mst2 AXI4 interface OUT
  axi_to_mem_2.aw.valid  <= acc_mst_out_2.m00_axi_awvalid; 
  axi_to_mem_2.aw.addr   <= acc_mst_out_2.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_2.aw.len    <= acc_mst_out_2.m00_axi_awlen;
  axi_to_mem_2.aw.id     <= acc_mst_out_2.m00_axi_awid;
  axi_to_mem_2.aw.size   <= acc_mst_out_2.m00_axi_awsize; --log2(data width in bytes)
  axi_to_mem_2.aw.burst  <= acc_mst_out_2.m00_axi_awburst; 
  axi_to_mem_2.aw.lock   <= acc_mst_out_2.m00_axi_awlock(0);
  axi_to_mem_2.aw.cache  <= acc_mst_out_2.m00_axi_awcache;
  axi_to_mem_2.aw.prot   <= acc_mst_out_2.m00_axi_awprot;
 
  axi_to_mem_2.w.valid   <= acc_mst_out_2.m00_axi_wvalid;
  axi_to_mem_2.w.data(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0)    <= acc_mst_out_2.m00_axi_wdata(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0); 
  axi_to_mem_2.w.strb(C_M_AXI_GMEMBUF_DATA_WIDTH/8-1 downto 0)    <= acc_mst_out_2.m00_axi_wstrb(C_M_AXI_GMEMBUF_DATA_WIDTH/8-1 downto 0);
  axi_to_mem_2.w.last    <= acc_mst_out_2.m00_axi_wlast; 
  axi_to_mem_2.b.ready   <= acc_mst_out_2.m00_axi_bready;
  axi_to_mem_2.ar.valid <= acc_mst_out_2.m00_axi_arvalid;
  axi_to_mem_2.ar.addr  <= acc_mst_out_2.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_2.ar.len   <= acc_mst_out_2.m00_axi_arlen;
  axi_to_mem_2.ar.id    <= acc_mst_out_2.m00_axi_arid;
  axi_to_mem_2.ar.size  <= acc_mst_out_2.m00_axi_arsize;
  axi_to_mem_2.ar.burst <= acc_mst_out_2.m00_axi_arburst;
  axi_to_mem_2.ar.lock  <= acc_mst_out_2.m00_axi_arlock(0);
  axi_to_mem_2.ar.cache <= acc_mst_out_2.m00_axi_arcache;
  axi_to_mem_2.ar.prot  <= acc_mst_out_2.m00_axi_arprot;
  axi_to_mem_2.r.ready  <= acc_mst_out_2.m00_axi_rready;
 
  axi_to_mem_2.aw.qos   <= "0000";
  axi_to_mem_2.ar.qos   <= "0000";
 
 -- mst2 AXI4 interface IN
  acc_mst_in_2.m00_axi_awready <= axi_from_mem_2.aw.ready;
  acc_mst_in_2.m00_axi_wready  <= axi_from_mem_2.w.ready;
  acc_mst_in_2.m00_axi_bvalid  <= axi_from_mem_2.b.valid;
  acc_mst_in_2.m00_axi_bresp   <= axi_from_mem_2.b.resp;
  acc_mst_in_2.m00_axi_bid     <= axi_from_mem_2.b.id;
  acc_mst_in_2.m00_axi_arready <= axi_from_mem_2.ar.ready;
  acc_mst_in_2.m00_axi_rvalid  <= axi_from_mem_2.r.valid;
  acc_mst_in_2.m00_axi_rdata(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0)   <= axi_from_mem_2.r.data(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0);
  acc_mst_in_2.m00_axi_rlast   <= axi_from_mem_2.r.last;
  acc_mst_in_2.m00_axi_rid     <= axi_from_mem_2.r.id;
  acc_mst_in_2.m00_axi_rresp   <= axi_from_mem_2.r.resp;
 
 
-- mst3 AXI4 interface OUT
  axi_to_mem_3.aw.valid  <= acc_mst_out_3.m00_axi_awvalid; 
  axi_to_mem_3.aw.addr   <= acc_mst_out_3.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_3.aw.len    <= acc_mst_out_3.m00_axi_awlen;
  axi_to_mem_3.aw.id     <= acc_mst_out_3.m00_axi_awid;
  axi_to_mem_3.aw.size   <= acc_mst_out_3.m00_axi_awsize; --log2(data width in bytes)
  axi_to_mem_3.aw.burst  <= acc_mst_out_3.m00_axi_awburst; 
  axi_to_mem_3.aw.lock   <= acc_mst_out_3.m00_axi_awlock(0);
  axi_to_mem_3.aw.cache  <= acc_mst_out_3.m00_axi_awcache;
  axi_to_mem_3.aw.prot   <= acc_mst_out_3.m00_axi_awprot;
 
  axi_to_mem_3.w.valid   <= acc_mst_out_3.m00_axi_wvalid;
  axi_to_mem_3.w.data(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0)    <= acc_mst_out_3.m00_axi_wdata(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0); 
  axi_to_mem_3.w.strb(C_M_AXI_GMEMPK_DATA_WIDTH/8-1 downto 0)    <= acc_mst_out_3.m00_axi_wstrb(C_M_AXI_GMEMPK_DATA_WIDTH/8-1 downto 0);
  axi_to_mem_3.w.last    <= acc_mst_out_3.m00_axi_wlast; 
  axi_to_mem_3.b.ready   <= acc_mst_out_3.m00_axi_bready;
  axi_to_mem_3.ar.valid <= acc_mst_out_3.m00_axi_arvalid;
  axi_to_mem_3.ar.addr  <= acc_mst_out_3.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_3.ar.len   <= acc_mst_out_3.m00_axi_arlen;
  axi_to_mem_3.ar.id    <= acc_mst_out_3.m00_axi_arid;
  axi_to_mem_3.ar.size  <= acc_mst_out_3.m00_axi_arsize;
  axi_to_mem_3.ar.burst <= acc_mst_out_3.m00_axi_arburst;
  axi_to_mem_3.ar.lock  <= acc_mst_out_3.m00_axi_arlock(0);
  axi_to_mem_3.ar.cache <= acc_mst_out_3.m00_axi_arcache;
  axi_to_mem_3.ar.prot  <= acc_mst_out_3.m00_axi_arprot;
  axi_to_mem_3.r.ready  <= acc_mst_out_3.m00_axi_rready;
 
  axi_to_mem_3.aw.qos   <= "0000";
  axi_to_mem_3.ar.qos   <= "0000";
 
 -- mst3 AXI4 interface IN
  acc_mst_in_3.m00_axi_awready <= axi_from_mem_3.aw.ready;
  acc_mst_in_3.m00_axi_wready  <= axi_from_mem_3.w.ready;
  acc_mst_in_3.m00_axi_bvalid  <= axi_from_mem_3.b.valid;
  acc_mst_in_3.m00_axi_bresp   <= axi_from_mem_3.b.resp;
  acc_mst_in_3.m00_axi_bid     <= axi_from_mem_3.b.id;
  acc_mst_in_3.m00_axi_arready <= axi_from_mem_3.ar.ready;
  acc_mst_in_3.m00_axi_rvalid  <= axi_from_mem_3.r.valid;
  acc_mst_in_3.m00_axi_rdata(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0)   <= axi_from_mem_3.r.data(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0);
  acc_mst_in_3.m00_axi_rlast   <= axi_from_mem_3.r.last;
  acc_mst_in_3.m00_axi_rid     <= axi_from_mem_3.r.id;
  acc_mst_in_3.m00_axi_rresp   <= axi_from_mem_3.r.resp;
 
 
-- mst5 AXI4 interface OUT
  axi_to_mem_4.aw.valid  <= acc_mst_out_4.m00_axi_awvalid; 
  axi_to_mem_4.aw.addr   <= acc_mst_out_4.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_4.aw.len    <= acc_mst_out_4.m00_axi_awlen;
  axi_to_mem_4.aw.id     <= acc_mst_out_4.m00_axi_awid;
  axi_to_mem_4.aw.size   <= acc_mst_out_4.m00_axi_awsize; --log2(data width in bytes)
  axi_to_mem_4.aw.burst  <= acc_mst_out_4.m00_axi_awburst; 
  axi_to_mem_4.aw.lock   <= acc_mst_out_4.m00_axi_awlock(0);
  axi_to_mem_4.aw.cache  <= acc_mst_out_4.m00_axi_awcache;
  axi_to_mem_4.aw.prot   <= acc_mst_out_4.m00_axi_awprot;
 
  axi_to_mem_4.w.valid   <= acc_mst_out_4.m00_axi_wvalid;
  axi_to_mem_4.w.data(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0)    <= acc_mst_out_4.m00_axi_wdata(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0); 
  axi_to_mem_4.w.strb(C_M_AXI_GMEMSK_DATA_WIDTH/8-1 downto 0)    <= acc_mst_out_4.m00_axi_wstrb(C_M_AXI_GMEMSK_DATA_WIDTH/8-1 downto 0);
  axi_to_mem_4.w.last    <= acc_mst_out_4.m00_axi_wlast; 
  axi_to_mem_4.b.ready   <= acc_mst_out_4.m00_axi_bready;
  axi_to_mem_4.ar.valid <= acc_mst_out_4.m00_axi_arvalid;
  axi_to_mem_4.ar.addr  <= acc_mst_out_4.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
  axi_to_mem_4.ar.len   <= acc_mst_out_4.m00_axi_arlen;
  axi_to_mem_4.ar.id    <= acc_mst_out_4.m00_axi_arid;
  axi_to_mem_4.ar.size  <= acc_mst_out_4.m00_axi_arsize;
  axi_to_mem_4.ar.burst <= acc_mst_out_4.m00_axi_arburst;
  axi_to_mem_4.ar.lock  <= acc_mst_out_4.m00_axi_arlock(0);
  axi_to_mem_4.ar.cache <= acc_mst_out_4.m00_axi_arcache;
  axi_to_mem_4.ar.prot  <= acc_mst_out_4.m00_axi_arprot;
  axi_to_mem_4.r.ready  <= acc_mst_out_4.m00_axi_rready;
 
  axi_to_mem_4.aw.qos   <= "0000";
  axi_to_mem_4.ar.qos   <= "0000";
 
 -- mst5 AXI4 interface IN
  acc_mst_in_4.m00_axi_awready <= axi_from_mem_4.aw.ready;
  acc_mst_in_4.m00_axi_wready  <= axi_from_mem_4.w.ready;
  acc_mst_in_4.m00_axi_bvalid  <= axi_from_mem_4.b.valid;
  acc_mst_in_4.m00_axi_bresp   <= axi_from_mem_4.b.resp;
  acc_mst_in_4.m00_axi_bid     <= axi_from_mem_4.b.id;
  acc_mst_in_4.m00_axi_arready <= axi_from_mem_4.ar.ready;
  acc_mst_in_4.m00_axi_rvalid  <= axi_from_mem_4.r.valid;
  acc_mst_in_4.m00_axi_rdata(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0)   <= axi_from_mem_4.r.data(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0);
  acc_mst_in_4.m00_axi_rlast   <= axi_from_mem_4.r.last;
  acc_mst_in_4.m00_axi_rid     <= axi_from_mem_4.r.id;
  acc_mst_in_4.m00_axi_rresp   <= axi_from_mem_4.r.resp; 


-- slv AXI-lite out
  axi_control_out.aw.ready   <= acc_slv_out.s_axi_control_awready; 
  axi_control_out.w.ready    <= acc_slv_out.s_axi_control_wready; 
  axi_control_out.ar.ready   <= acc_slv_out.s_axi_control_arready;
  axi_control_out.r.valid    <= acc_slv_out.s_axi_control_rvalid;
  axi_control_out.r.data((C_S_AXI_CONTROL_DATA_WIDTH*1)-1 downto C_S_AXI_CONTROL_DATA_WIDTH*0) <= acc_slv_out.s_axi_control_rdata;
  axi_control_out.r.data((C_S_AXI_CONTROL_DATA_WIDTH*2)-1 downto C_S_AXI_CONTROL_DATA_WIDTH*1) <= acc_slv_out.s_axi_control_rdata;
  axi_control_out.r.data((C_S_AXI_CONTROL_DATA_WIDTH*3)-1 downto C_S_AXI_CONTROL_DATA_WIDTH*2) <= acc_slv_out.s_axi_control_rdata;
  axi_control_out.r.data((C_S_AXI_CONTROL_DATA_WIDTH*4)-1 downto C_S_AXI_CONTROL_DATA_WIDTH*3) <= acc_slv_out.s_axi_control_rdata; 
    
  axi_control_out.r.resp     <= acc_slv_out.s_axi_control_rresp;     
  axi_control_out.r.last     <= '0';
  axi_control_out.r.id       <= "0000";
  axi_control_out.b.valid    <= acc_slv_out.s_axi_control_bvalid;    
  axi_control_out.b.resp     <= acc_slv_out.s_axi_control_bresp;
  axi_control_out.b.id       <= "0000";

-- slv AXI-lite in
  acc_slv_in.s_axi_control_awvalid <= axi_control_in.aw.valid;
  acc_slv_in.s_axi_control_awaddr  <= axi_control_in.aw.addr(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0);
  acc_slv_in.s_axi_control_wvalid  <= axi_control_in.w.valid;
  acc_slv_in.s_axi_control_wdata   <= axi_control_in.w.data(C_S_AXI_CONTROL_DATA_WIDTH-1 downto 0);
  --This is a workaround to adapt the 32 bit wide control axi to the 128 bit wide input channel
  acc_slv_in.s_axi_control_wstrb(3 downto 0)   <= axi_control_in.w.strb(3 downto 0) or
                                      axi_control_in.w.strb(7 downto 4) or
                                      axi_control_in.w.strb(11 downto 8) or
                                      axi_control_in.w.strb(15 downto 12);  
  acc_slv_in.s_axi_control_arvalid <= axi_control_in.ar.valid;  
  acc_slv_in.s_axi_control_araddr  <= axi_control_in.ar.addr(C_S_AXI_CONTROL_ADDR_WIDTH-1 downto 0); 
  acc_slv_in.s_axi_control_rready  <= axi_control_in.r.ready;
  acc_slv_in.s_axi_control_bready  <= axi_control_in.b.ready;

acc_mst_out.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
acc_mst_out.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);

acc_mst_out_1.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_1_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
acc_mst_out_1.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_1_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);

acc_mst_out_2.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_2_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
acc_mst_out_2.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_2_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);

acc_mst_out_3.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_3_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
acc_mst_out_3.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_3_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);

acc_mst_out_4.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_4_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
acc_mst_out_4.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_4_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);

-- acc_mst_out_6.m00_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_6_axi_awaddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);
-- acc_mst_out_6.m00_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0) <= aux_6_axi_araddr(C_M_AXI_CURRENT_GMEM_ADDR_WIDTH-1 downto 0);



acc_inst : mlkem_accelerator
  generic map(
    C_S_AXI_CONTROL_DATA_WIDTH => C_S_AXI_CONTROL_DATA_WIDTH,
    --C_M_AXI_ACC_MST_ADDR_WIDTH => C_M_AXI_ACC_MST_ADDR_WIDTH,
    --C_M_AXI_ACC_MST_DATA_WIDTH => C_M_AXI_ACC_MST_DATA_WIDTH,
    C_M_AXI_GMEMCT_ID_WIDTH      => C_M_AXI_GMEMCT_ID_WIDTH,
    C_M_AXI_GMEMSS_ID_WIDTH     => C_M_AXI_GMEMSS_ID_WIDTH,
    C_M_AXI_GMEMBUF_ID_WIDTH     => C_M_AXI_GMEMBUF_ID_WIDTH,
    C_M_AXI_GMEMPK_ID_WIDTH     => C_M_AXI_GMEMPK_ID_WIDTH,
    C_M_AXI_GMEMSK_ID_WIDTH     => C_M_AXI_GMEMSK_ID_WIDTH,

    C_M_AXI_GMEMSS_DATA_WIDTH   => C_M_AXI_GMEMSS_DATA_WIDTH,
    C_M_AXI_GMEMBUF_DATA_WIDTH   => C_M_AXI_GMEMBUF_DATA_WIDTH,
    C_M_AXI_GMEMPK_DATA_WIDTH   => C_M_AXI_GMEMPK_DATA_WIDTH,
    C_M_AXI_GMEMSK_DATA_WIDTH   => C_M_AXI_GMEMSK_DATA_WIDTH
  )
  port map (
    ap_clk               => clk,
    ap_rst_n             => rst_n,
    --m_axi_gmemct
    m_axi_gmemct_AWVALID   => acc_mst_out.m00_axi_awvalid,
    m_axi_gmemct_AWREADY   => acc_mst_in.m00_axi_awready,
    m_axi_gmemct_AWADDR    => aux_axi_awaddr,
    m_axi_gmemct_AWID      => acc_mst_out.m00_axi_awid(C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0), 
    m_axi_gmemct_AWLEN     => acc_mst_out.m00_axi_awlen,
    m_axi_gmemct_AWSIZE    => acc_mst_out.m00_axi_awsize, 
    m_axi_gmemct_AWBURST   => acc_mst_out.m00_axi_awburst, 
    m_axi_gmemct_AWLOCK    => acc_mst_out.m00_axi_awlock, 
    m_axi_gmemct_AWCACHE   => acc_mst_out.m00_axi_awcache, 
    m_axi_gmemct_AWPROT    => acc_mst_out.m00_axi_awprot, 
    m_axi_gmemct_AWQOS     => acc_mst_out.m00_axi_awqos,      --not used
    m_axi_gmemct_AWREGION  => acc_mst_out.m00_axi_awregion, --not used
    m_axi_gmemct_AWUSER    => open,
    m_axi_gmemct_WVALID    => acc_mst_out.m00_axi_wvalid,
    m_axi_gmemct_WREADY    => acc_mst_in.m00_axi_wready,
    m_axi_gmemct_WDATA     => acc_mst_out.m00_axi_wdata(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0),
    m_axi_gmemct_WSTRB     => acc_mst_out.m00_axi_wstrb(C_M_AXI_GMEMCT_DATA_WIDTH/8-1 downto 0),
    m_axi_gmemct_WID       => open,
    m_axi_gmemct_WUSER     => open,
    m_axi_gmemct_WLAST     => acc_mst_out.m00_axi_wlast,
    m_axi_gmemct_ARVALID   => acc_mst_out.m00_axi_arvalid, 
    m_axi_gmemct_ARREADY   => acc_mst_in.m00_axi_arready,
    m_axi_gmemct_ARADDR    => aux_axi_araddr,
    m_axi_gmemct_ARID      => acc_mst_out.m00_axi_arid(C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0), 
    m_axi_gmemct_ARLEN     => acc_mst_out.m00_axi_arlen,
    m_axi_gmemct_ARSIZE    => acc_mst_out.m00_axi_arsize, 
    m_axi_gmemct_ARBURST   => acc_mst_out.m00_axi_arburst,  
    m_axi_gmemct_ARLOCK    => acc_mst_out.m00_axi_arlock, 
    m_axi_gmemct_ARCACHE   => acc_mst_out.m00_axi_arcache, 
    m_axi_gmemct_ARPROT    => acc_mst_out.m00_axi_arprot,  
    m_axi_gmemct_ARQOS     => acc_mst_out.m00_axi_arqos, 
    m_axi_gmemct_ARREGION  => acc_mst_out.m00_axi_arregion, 
    m_axi_gmemct_ARUSER    => open,
    m_axi_gmemct_RVALID    => acc_mst_in.m00_axi_rvalid,
    m_axi_gmemct_RREADY    => acc_mst_out.m00_axi_rready,
    m_axi_gmemct_RDATA     => acc_mst_in.m00_axi_rdata(C_M_AXI_GMEMCT_DATA_WIDTH-1 downto 0),
    m_axi_gmemct_RLAST     => acc_mst_in.m00_axi_rlast,
    m_axi_gmemct_RUSER     => "0",
    m_axi_gmemct_RID       => acc_mst_in.m00_axi_rid(C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0),  
    m_axi_gmemct_RRESP     => acc_mst_in.m00_axi_rresp, 
    m_axi_gmemct_BVALID    => acc_mst_in.m00_axi_bvalid,
    m_axi_gmemct_BREADY    => acc_mst_out.m00_axi_bready,
    m_axi_gmemct_BRESP     => acc_mst_in.m00_axi_bresp, 
    m_axi_gmemct_BID       => acc_mst_in.m00_axi_bid(C_M_AXI_GMEMCT_ID_WIDTH-1 downto 0), 
    m_axi_gmemct_BUSER     => "0",

    --m_axi_gmemss
    m_axi_gmemss_AWVALID   => acc_mst_out_1.m00_axi_awvalid,
    m_axi_gmemss_AWREADY   => acc_mst_in_1.m00_axi_awready,
    m_axi_gmemss_AWADDR    => aux_1_axi_awaddr,
    m_axi_gmemss_AWID      => acc_mst_out_1.m00_axi_awid(C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0), 
    m_axi_gmemss_AWLEN     => acc_mst_out_1.m00_axi_awlen,
    m_axi_gmemss_AWSIZE    => acc_mst_out_1.m00_axi_awsize, 
    m_axi_gmemss_AWBURST   => acc_mst_out_1.m00_axi_awburst, 
    m_axi_gmemss_AWLOCK    => acc_mst_out_1.m00_axi_awlock, 
    m_axi_gmemss_AWCACHE   => acc_mst_out_1.m00_axi_awcache, 
    m_axi_gmemss_AWPROT    => acc_mst_out_1.m00_axi_awprot, 
    m_axi_gmemss_AWQOS     => acc_mst_out_1.m00_axi_awqos,      --not used
    m_axi_gmemss_AWREGION  => acc_mst_out_1.m00_axi_awregion, --not used
    m_axi_gmemss_AWUSER    => open,
    m_axi_gmemss_WVALID    => acc_mst_out_1.m00_axi_wvalid,
    m_axi_gmemss_WREADY    => acc_mst_in_1.m00_axi_wready,
    m_axi_gmemss_WDATA     => acc_mst_out_1.m00_axi_wdata(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0),
    m_axi_gmemss_WSTRB     => acc_mst_out_1.m00_axi_wstrb(C_M_AXI_GMEMSS_DATA_WIDTH/8-1 downto 0),
    m_axi_gmemss_WID       => open,
    m_axi_gmemss_WUSER     => open,
    m_axi_gmemss_WLAST     => acc_mst_out_1.m00_axi_wlast,
    m_axi_gmemss_ARVALID   => acc_mst_out_1.m00_axi_arvalid, 
    m_axi_gmemss_ARREADY   => acc_mst_in_1.m00_axi_arready,
    m_axi_gmemss_ARADDR    => aux_1_axi_araddr,
    m_axi_gmemss_ARID      => acc_mst_out_1.m00_axi_arid(C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0), 
    m_axi_gmemss_ARLEN     => acc_mst_out_1.m00_axi_arlen,
    m_axi_gmemss_ARSIZE    => acc_mst_out_1.m00_axi_arsize, 
    m_axi_gmemss_ARBURST   => acc_mst_out_1.m00_axi_arburst,  
    m_axi_gmemss_ARLOCK    => acc_mst_out_1.m00_axi_arlock, 
    m_axi_gmemss_ARCACHE   => acc_mst_out_1.m00_axi_arcache, 
    m_axi_gmemss_ARPROT    => acc_mst_out_1.m00_axi_arprot,  
    m_axi_gmemss_ARQOS     => acc_mst_out_1.m00_axi_arqos, 
    m_axi_gmemss_ARREGION  => acc_mst_out_1.m00_axi_arregion, 
    m_axi_gmemss_ARUSER    => open,
    m_axi_gmemss_RVALID    => acc_mst_in_1.m00_axi_rvalid,
    m_axi_gmemss_RREADY    => acc_mst_out_1.m00_axi_rready,
    m_axi_gmemss_RDATA     => acc_mst_in_1.m00_axi_rdata(C_M_AXI_GMEMSS_DATA_WIDTH-1 downto 0),
    m_axi_gmemss_RLAST     => acc_mst_in_1.m00_axi_rlast,
    m_axi_gmemss_RUSER     => "0",
    m_axi_gmemss_RID       => acc_mst_in_1.m00_axi_rid(C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0),  
    m_axi_gmemss_RRESP     => acc_mst_in_1.m00_axi_rresp, 
    m_axi_gmemss_BVALID    => acc_mst_in_1.m00_axi_bvalid,
    m_axi_gmemss_BREADY    => acc_mst_out_1.m00_axi_bready,
    m_axi_gmemss_BRESP     => acc_mst_in_1.m00_axi_bresp, 
    m_axi_gmemss_BID       => acc_mst_in_1.m00_axi_bid(C_M_AXI_GMEMSS_ID_WIDTH-1 downto 0), 
    m_axi_gmemss_BUSER     => "0",

    --m_axi_gmembuf
    m_axi_gmembuf_AWVALID   => acc_mst_out_2.m00_axi_awvalid,
    m_axi_gmembuf_AWREADY   => acc_mst_in_2.m00_axi_awready,
    m_axi_gmembuf_AWADDR    => aux_2_axi_awaddr,
    m_axi_gmembuf_AWID      => acc_mst_out_2.m00_axi_awid(C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0), 
    m_axi_gmembuf_AWLEN     => acc_mst_out_2.m00_axi_awlen,
    m_axi_gmembuf_AWSIZE    => acc_mst_out_2.m00_axi_awsize, 
    m_axi_gmembuf_AWBURST   => acc_mst_out_2.m00_axi_awburst, 
    m_axi_gmembuf_AWLOCK    => acc_mst_out_2.m00_axi_awlock, 
    m_axi_gmembuf_AWCACHE   => acc_mst_out_2.m00_axi_awcache, 
    m_axi_gmembuf_AWPROT    => acc_mst_out_2.m00_axi_awprot, 
    m_axi_gmembuf_AWQOS     => acc_mst_out_2.m00_axi_awqos,      --not used
    m_axi_gmembuf_AWREGION  => acc_mst_out_2.m00_axi_awregion, --not used
    m_axi_gmembuf_AWUSER    => open,
    m_axi_gmembuf_WVALID    => acc_mst_out_2.m00_axi_wvalid,
    m_axi_gmembuf_WREADY    => acc_mst_in_2.m00_axi_wready,
    m_axi_gmembuf_WDATA     => acc_mst_out_2.m00_axi_wdata(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0),
    m_axi_gmembuf_WSTRB     => acc_mst_out_2.m00_axi_wstrb(C_M_AXI_GMEMBUF_DATA_WIDTH/8-1 downto 0),
    m_axi_gmembuf_WID       => open,
    m_axi_gmembuf_WUSER     => open,
    m_axi_gmembuf_WLAST     => acc_mst_out_2.m00_axi_wlast,
    m_axi_gmembuf_ARVALID   => acc_mst_out_2.m00_axi_arvalid, 
    m_axi_gmembuf_ARREADY   => acc_mst_in_2.m00_axi_arready,
    m_axi_gmembuf_ARADDR    => aux_2_axi_araddr,
    m_axi_gmembuf_ARID      => acc_mst_out_2.m00_axi_arid(C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0), 
    m_axi_gmembuf_ARLEN     => acc_mst_out_2.m00_axi_arlen,
    m_axi_gmembuf_ARSIZE    => acc_mst_out_2.m00_axi_arsize, 
    m_axi_gmembuf_ARBURST   => acc_mst_out_2.m00_axi_arburst,  
    m_axi_gmembuf_ARLOCK    => acc_mst_out_2.m00_axi_arlock, 
    m_axi_gmembuf_ARCACHE   => acc_mst_out_2.m00_axi_arcache, 
    m_axi_gmembuf_ARPROT    => acc_mst_out_2.m00_axi_arprot,  
    m_axi_gmembuf_ARQOS     => acc_mst_out_2.m00_axi_arqos, 
    m_axi_gmembuf_ARREGION  => acc_mst_out_2.m00_axi_arregion, 
    m_axi_gmembuf_ARUSER    => open,
    m_axi_gmembuf_RVALID    => acc_mst_in_2.m00_axi_rvalid,
    m_axi_gmembuf_RREADY    => acc_mst_out_2.m00_axi_rready,
    m_axi_gmembuf_RDATA     => acc_mst_in_2.m00_axi_rdata(C_M_AXI_GMEMBUF_DATA_WIDTH-1 downto 0),
    m_axi_gmembuf_RLAST     => acc_mst_in_2.m00_axi_rlast,
    m_axi_gmembuf_RUSER     => "0",
    m_axi_gmembuf_RID       => acc_mst_in_2.m00_axi_rid(C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0),  
    m_axi_gmembuf_RRESP     => acc_mst_in_2.m00_axi_rresp, 
    m_axi_gmembuf_BVALID    => acc_mst_in_2.m00_axi_bvalid,
    m_axi_gmembuf_BREADY    => acc_mst_out_2.m00_axi_bready,
    m_axi_gmembuf_BRESP     => acc_mst_in_2.m00_axi_bresp, 
    m_axi_gmembuf_BID       => acc_mst_in_2.m00_axi_bid(C_M_AXI_GMEMBUF_ID_WIDTH-1 downto 0), 
    m_axi_gmembuf_BUSER     => "0",

    --m_axi_gmempk
    m_axi_gmempk_AWVALID   => acc_mst_out_3.m00_axi_awvalid,
    m_axi_gmempk_AWREADY   => acc_mst_in_3.m00_axi_awready,
    m_axi_gmempk_AWADDR    => aux_3_axi_awaddr,
    m_axi_gmempk_AWID      => acc_mst_out_3.m00_axi_awid(C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0), 
    m_axi_gmempk_AWLEN     => acc_mst_out_3.m00_axi_awlen,
    m_axi_gmempk_AWSIZE    => acc_mst_out_3.m00_axi_awsize, 
    m_axi_gmempk_AWBURST   => acc_mst_out_3.m00_axi_awburst, 
    m_axi_gmempk_AWLOCK    => acc_mst_out_3.m00_axi_awlock, 
    m_axi_gmempk_AWCACHE   => acc_mst_out_3.m00_axi_awcache, 
    m_axi_gmempk_AWPROT    => acc_mst_out_3.m00_axi_awprot, 
    m_axi_gmempk_AWQOS     => acc_mst_out_3.m00_axi_awqos,      --not used
    m_axi_gmempk_AWREGION  => acc_mst_out_3.m00_axi_awregion, --not used
    m_axi_gmempk_AWUSER    => open,
    m_axi_gmempk_WVALID    => acc_mst_out_3.m00_axi_wvalid,
    m_axi_gmempk_WREADY    => acc_mst_in_3.m00_axi_wready,
    m_axi_gmempk_WDATA     => acc_mst_out_3.m00_axi_wdata(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0),
    m_axi_gmempk_WSTRB     => acc_mst_out_3.m00_axi_wstrb(C_M_AXI_GMEMPK_DATA_WIDTH/8-1 downto 0),
    m_axi_gmempk_WID       => open,
    m_axi_gmempk_WUSER     => open,
    m_axi_gmempk_WLAST     => acc_mst_out_3.m00_axi_wlast,
    m_axi_gmempk_ARVALID   => acc_mst_out_3.m00_axi_arvalid, 
    m_axi_gmempk_ARREADY   => acc_mst_in_3.m00_axi_arready,
    m_axi_gmempk_ARADDR    => aux_3_axi_araddr,
    m_axi_gmempk_ARID      => acc_mst_out_3.m00_axi_arid(C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0), 
    m_axi_gmempk_ARLEN     => acc_mst_out_3.m00_axi_arlen,
    m_axi_gmempk_ARSIZE    => acc_mst_out_3.m00_axi_arsize, 
    m_axi_gmempk_ARBURST   => acc_mst_out_3.m00_axi_arburst,  
    m_axi_gmempk_ARLOCK    => acc_mst_out_3.m00_axi_arlock, 
    m_axi_gmempk_ARCACHE   => acc_mst_out_3.m00_axi_arcache, 
    m_axi_gmempk_ARPROT    => acc_mst_out_3.m00_axi_arprot,  
    m_axi_gmempk_ARQOS     => acc_mst_out_3.m00_axi_arqos, 
    m_axi_gmempk_ARREGION  => acc_mst_out_3.m00_axi_arregion, 
    m_axi_gmempk_ARUSER    => open,
    m_axi_gmempk_RVALID    => acc_mst_in_3.m00_axi_rvalid,
    m_axi_gmempk_RREADY    => acc_mst_out_3.m00_axi_rready,
    m_axi_gmempk_RDATA     => acc_mst_in_3.m00_axi_rdata(C_M_AXI_GMEMPK_DATA_WIDTH-1 downto 0),
    m_axi_gmempk_RLAST     => acc_mst_in_3.m00_axi_rlast,
    m_axi_gmempk_RUSER     => "0",
    m_axi_gmempk_RID       => acc_mst_in_3.m00_axi_rid(C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0),  
    m_axi_gmempk_RRESP     => acc_mst_in_3.m00_axi_rresp, 
    m_axi_gmempk_BVALID    => acc_mst_in_3.m00_axi_bvalid,
    m_axi_gmempk_BREADY    => acc_mst_out_3.m00_axi_bready,
    m_axi_gmempk_BRESP     => acc_mst_in_3.m00_axi_bresp, 
    m_axi_gmempk_BID       => acc_mst_in_3.m00_axi_bid(C_M_AXI_GMEMPK_ID_WIDTH-1 downto 0), 
    m_axi_gmempk_BUSER     => "0",

    --m_axi_gmemsk
    m_axi_gmemsk_AWVALID   => acc_mst_out_4.m00_axi_awvalid,
    m_axi_gmemsk_AWREADY   => acc_mst_in_4.m00_axi_awready,
    m_axi_gmemsk_AWADDR    => aux_4_axi_awaddr,
    m_axi_gmemsk_AWID      => acc_mst_out_4.m00_axi_awid(C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0), 
    m_axi_gmemsk_AWLEN     => acc_mst_out_4.m00_axi_awlen,
    m_axi_gmemsk_AWSIZE    => acc_mst_out_4.m00_axi_awsize, 
    m_axi_gmemsk_AWBURST   => acc_mst_out_4.m00_axi_awburst, 
    m_axi_gmemsk_AWLOCK    => acc_mst_out_4.m00_axi_awlock, 
    m_axi_gmemsk_AWCACHE   => acc_mst_out_4.m00_axi_awcache, 
    m_axi_gmemsk_AWPROT    => acc_mst_out_4.m00_axi_awprot, 
    m_axi_gmemsk_AWQOS     => acc_mst_out_4.m00_axi_awqos,      --not used
    m_axi_gmemsk_AWREGION  => acc_mst_out_4.m00_axi_awregion, --not used
    m_axi_gmemsk_AWUSER    => open,
    m_axi_gmemsk_WVALID    => acc_mst_out_4.m00_axi_wvalid,
    m_axi_gmemsk_WREADY    => acc_mst_in_4.m00_axi_wready,
    m_axi_gmemsk_WDATA     => acc_mst_out_4.m00_axi_wdata(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0),
    m_axi_gmemsk_WSTRB     => acc_mst_out_4.m00_axi_wstrb(C_M_AXI_GMEMSK_DATA_WIDTH/8-1 downto 0),
    m_axi_gmemsk_WID       => open,
    m_axi_gmemsk_WUSER     => open,
    m_axi_gmemsk_WLAST     => acc_mst_out_4.m00_axi_wlast,
    m_axi_gmemsk_ARVALID   => acc_mst_out_4.m00_axi_arvalid, 
    m_axi_gmemsk_ARREADY   => acc_mst_in_4.m00_axi_arready,
    m_axi_gmemsk_ARADDR    => aux_4_axi_araddr,
    m_axi_gmemsk_ARID      => acc_mst_out_4.m00_axi_arid(C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0), 
    m_axi_gmemsk_ARLEN     => acc_mst_out_4.m00_axi_arlen,
    m_axi_gmemsk_ARSIZE    => acc_mst_out_4.m00_axi_arsize, 
    m_axi_gmemsk_ARBURST   => acc_mst_out_4.m00_axi_arburst,  
    m_axi_gmemsk_ARLOCK    => acc_mst_out_4.m00_axi_arlock, 
    m_axi_gmemsk_ARCACHE   => acc_mst_out_4.m00_axi_arcache, 
    m_axi_gmemsk_ARPROT    => acc_mst_out_4.m00_axi_arprot,  
    m_axi_gmemsk_ARQOS     => acc_mst_out_4.m00_axi_arqos, 
    m_axi_gmemsk_ARREGION  => acc_mst_out_4.m00_axi_arregion, 
    m_axi_gmemsk_ARUSER    => open,
    m_axi_gmemsk_RVALID    => acc_mst_in_4.m00_axi_rvalid,
    m_axi_gmemsk_RREADY    => acc_mst_out_4.m00_axi_rready,
    m_axi_gmemsk_RDATA     => acc_mst_in_4.m00_axi_rdata(C_M_AXI_GMEMSK_DATA_WIDTH-1 downto 0),
    m_axi_gmemsk_RLAST     => acc_mst_in_4.m00_axi_rlast,
    m_axi_gmemsk_RUSER     => "0",
    m_axi_gmemsk_RID       => acc_mst_in_4.m00_axi_rid(C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0),  
    m_axi_gmemsk_RRESP     => acc_mst_in_4.m00_axi_rresp, 
    m_axi_gmemsk_BVALID    => acc_mst_in_4.m00_axi_bvalid,
    m_axi_gmemsk_BREADY    => acc_mst_out_4.m00_axi_bready,
    m_axi_gmemsk_BRESP     => acc_mst_in_4.m00_axi_bresp, 
    m_axi_gmemsk_BID       => acc_mst_in_4.m00_axi_bid(C_M_AXI_GMEMSK_ID_WIDTH-1 downto 0), 
    m_axi_gmemsk_BUSER     => "0",

    -- AXI4-Lite slave interface
    s_axi_control_AWVALID => acc_slv_in.s_axi_control_awvalid,
    s_axi_control_AWREADY => acc_slv_out.s_axi_control_awready,
    s_axi_control_AWADDR  => acc_slv_in.s_axi_control_awaddr, 
    s_axi_control_WVALID  => acc_slv_in.s_axi_control_wvalid,
    s_axi_control_WREADY  => acc_slv_out.s_axi_control_wready, 
    s_axi_control_WDATA   => acc_slv_in.s_axi_control_wdata, 
    s_axi_control_WSTRB   => acc_slv_in.s_axi_control_wstrb(3 downto 0), 
    s_axi_control_ARVALID => acc_slv_in.s_axi_control_arvalid, 
    s_axi_control_ARREADY => acc_slv_out.s_axi_control_arready,
    s_axi_control_ARADDR  => acc_slv_in.s_axi_control_araddr,
    s_axi_control_RVALID  => acc_slv_out.s_axi_control_rvalid,
    s_axi_control_RREADY  => acc_slv_in.s_axi_control_rready,
    s_axi_control_RDATA   => acc_slv_out.s_axi_control_rdata, 
    s_axi_control_RRESP   => acc_slv_out.s_axi_control_rresp,
    s_axi_control_BVALID  => acc_slv_out.s_axi_control_bvalid,
    s_axi_control_BREADY  => acc_slv_in.s_axi_control_bready,
    s_axi_control_BRESP   => acc_slv_out.s_axi_control_bresp,
    interrupt             => interrupt
  );


end;

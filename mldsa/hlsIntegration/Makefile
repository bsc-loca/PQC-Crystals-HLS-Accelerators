default : mldsa2

SRC_HLS = k_dsa.cpp

.PHONY: default all

mldsa2: $(SRC_HLS)
	rm -f ../src/*.v
	rm -f ../src/*.vh
	rm -f ../src/*.dat
	rm -f ../src/vlogsyn.txt
	vitis_hls -f scripts/mldsa2.tcl
	rm -rf rtl/$@
	mkdir -p rtl/$@
	unzip mldsa/$@/export.zip -d rtl/$@
	(echo "mldsa_accelerator.v"; ls rtl/$@/hdl/verilog/*.v 2>/dev/null | grep -v '\.vh$$' | sed 's|rtl/'"$@"'/hdl/verilog/||' | grep -v '^mldsa_accelerator.v$$') > vlogsyn.txt
	mv rtl/$@/hdl/verilog/* ../src
	mv vlogsyn.txt ../src
	rm -rf vitis_hls.log rtl mldsa
	
clean: 
	rm -rf mldsa

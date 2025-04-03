default : mlkem512

SRC_HLS = k_kem.cpp

.PHONY: default all

mlkem512: $(SRC_HLS)
	rm -f ../src/*.v
	rm -f ../src/*.vh
	rm -f ../src/*.dat
	rm -f ../src/vlogsyn.txt
	vitis_hls -f scripts/mlkem512.tcl
	rm -rf rtl/$@
	mkdir -p rtl/$@
	unzip mlkem/$@/export.zip -d rtl/$@
	(echo "mlkem_accelerator.v"; ls rtl/$@/hdl/verilog/*.v 2>/dev/null | grep -v '\.vh$$' | sed 's|rtl/'"$@"'/hdl/verilog/||' | grep -v '^mlkem_accelerator.v$$') > vlogsyn.txt
	mv rtl/$@/hdl/verilog/* ../src
	mv vlogsyn.txt ../src
	rm -rf vitis_hls.log rtl mlkem

clean: 
	rm -rf mlkem

#VITIS 2021.2 ENV
#export PATH=$PATH:/path/to/Vivado_2021_2/Vitis_HLS/2021.2/bin

if ! command -v vitis_hls &> /dev/null; then
    echo "vitis_hls is not in your PATH."
    echo "Please add it to your PATH. Example:"
    echo 'export PATH=$PATH:/path/to/Vivado_2021_2/Vitis_HLS/2021.2/bin'
else
    echo "vitis_hls is already in your PATH."
fi




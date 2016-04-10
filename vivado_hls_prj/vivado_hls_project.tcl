open_project vhls_prj
set_top sdx_cppKernel_top

add_files -tb ../src/getCPUTime.cpp
add_files -tb ../src/tb_sdx_cppKernel_top.cpp -cflags "-DVHLS_FLOW -DGPP_ONLY_FLOW -I/home/applications/Xilinx/SDAccel/2015.4/runtime/include/1_2 -I/home/applications/Xilinx/SDAccel/2015.4/Vivado_HLS/include -L/home/applications/Xilinx/SDAccel/2015.4/runtime/lib/x86_64 -lrt"
add_files ../src/matrix_utility_class.h
add_files ../src/matrix_determinant_class.h
add_files ../src/matrix_operation_wrapper.h
add_files ../src/matrix_operation_wrapper.cpp
add_files ../src/sdx_cppKernel_top.h
add_files ../src/sdx_cppKernel_top.cpp

open_solution "solution1"
set_part {xcku060-ffva1156-2-e}
create_clock -period 10 -name default
csim_design
csynth_design
#cosim_design -ldflags {-lm -lrt}
#export_design -format ip_catalog
exit


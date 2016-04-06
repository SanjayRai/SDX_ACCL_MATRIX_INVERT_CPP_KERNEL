#ifndef SDX_CPPKERNEL_TOP_H_
#define SDX_CPPKERNEL_TOP_H_ 

#include "matrix_operation_wrapper.h"

struct input_data_type {
    matrix_data_t data_in[DIM][DIM];
};

struct output_data_type {
    matrix_data_t data_out[DIM][DIM];
};

#ifdef XOCC_CPP_KERNEL 
extern "C" {
#endif
void sdx_cppKernel_top(input_data_type *a, output_data_type *a_inv);
#ifdef XOCC_CPP_KERNEL 
}
#endif

#endif

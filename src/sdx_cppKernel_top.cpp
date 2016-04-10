#include <string.h>

#include "sdx_cppKernel_top.h"

void sdx_cppKernel_top(input_data_type *a, output_data_type *a_inv)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem depth=8192
#pragma HLS INTERFACE m_axi port=a_inv offset=slave bundle=gmem depth=8192
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=a_inv bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

  matrix_data_t bufa[DIM][DIM];
//#pragma HLS ARRAY_RESHAPE
  matrix_data_t bufb[DIM][DIM];
//#pragma HLS ARRAY_RESHAPE

  memcpy(bufa, a, sizeof(input_data_type));

  matrix_operation_wrapper(bufa, bufb);

  memcpy(a_inv, bufb, sizeof(input_data_type));
  return;
}


#include "matrix_determinant_class.h"
#include "matrix_utility_class.h"
#include "matrix_operation_wrapper.h"


void matrix_operation_wrapper(matrix_data_t A[DIM][DIM], matrix_data_t Y[DIM][DIM]){
#pragma HLS PIPELINE

   matrix_data_t co_fac_A[DIM][DIM];
   matrix_data_t adjoint_A[DIM][DIM];
   matrix_utility_class<matrix_data_t, DIM> my_matrix; 
   matrix_determinant_class<matrix_data_t, DIM> my_determinant;
   matrix_data_t determinant_A;

    my_matrix.cofactor(A,co_fac_A);
    my_matrix.transpose(co_fac_A,adjoint_A);
    determinant_A = my_determinant.crout_decomposition(A);

    for (int j = 0; j < DIM; j++){
        for (int i = 0; i < DIM; i++) {
            Y[i][j] = ((adjoint_A[i][j])/determinant_A);
            
        }
    }
        
}

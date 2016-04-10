#include<stdio.h>
#include<math.h>
#include"matrix_operation_wrapper.h"

#include "sdaccel_utils.h" 
#include "sdx_cppKernel_top.h" 



#ifndef NUMBER_OF_COMPUTE_UNITS
#define NUMBER_OF_COMPUTE_UNITS 1 
#endif

#define MAX_ITERATION 20000 
#define GLOBAL_SIZE MAX_ITERATION*NUMBER_OF_COMPUTE_UNITS

double getCPUTime();

void gen_test_matrix(input_data_type *a) {

  static input_data_type temp = {{{1.0f, 2.0f, 3.0f, 4.0f, 1.0f},
                                  {5.0f, 6.0f, 7.0f, 8.0f, 2.0f},
                                  {2.0f, 6.0f, 4.0f, 8.0f, 3.0f},
                                  {3.0f, 1.0f, 1.0f, 2.0f, 4.0f},
                                  {1.0f, 4.0f, 5.0f, 6.0f, 5.0f}}};
  for (int i = 0; i <  DIM;i++) { 
      for (int j = 0; j <  DIM;j++) { 
          a->data_in[i][j] = temp.data_in[i][j];
          temp.data_in[i][j] = (temp.data_in[i][j])*1.002;
      }
  }
}


int main(int argc, char** argv)
{

  double startTime;
  double endTime;
  double ElapsedTime;

  input_data_type *a_ptr;
  input_data_type *a_ptr_head;
  output_data_type *a_inv_ptr;
  output_data_type *a_inv_ptr_head;
  int i,j;

  int in_args_size[10];               // Array of input Argument sizes
  int out_args_size[10];              // Array of output Argument sizes
  int num_input_args;
  int num_output_args;
  int init_success;
  int run_success;
  int sdaccel_clean_success;
  int cu = 0;
  int in_index;
  int out_index;

  num_input_args = 1;
  num_output_args = 1;
  
  a_ptr = new input_data_type[GLOBAL_SIZE];
  a_ptr_head = a_ptr;
  a_inv_ptr = new output_data_type[GLOBAL_SIZE];
  a_inv_ptr_head = a_inv_ptr;


  printf("-------------------------------------------------------------\n");
  printf("--------Initialize the Test vector space---------------------\n");
  printf("-------------------------------------------------------------\n");
    for (int num_itn = 0 ; num_itn < MAX_ITERATION; num_itn++) {
        for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
              for (int i = 0; i <  DIM;i++) { 
                  for (int j = 0; j <  DIM;j++) { 
                      a_ptr->data_in[i][j] = 0.0f; 
                      a_inv_ptr->data_out[i][j] = 1.111111f; 
                  }
              }
        a_ptr++;
        a_inv_ptr++;
        }
    } // MAX_ITERATION

     a_ptr = a_ptr_head;
     a_inv_ptr = a_inv_ptr_head;

  printf("-------------------------------------------------------------\n");
  printf("Create Test Matrix\n");
  printf("Note GLOBAL_SIZE (Number of test loops run) = %d\n", GLOBAL_SIZE);
  printf("-------------------------------------------------------------\n\n\n");

    for (int num_itn = 0 ; num_itn < GLOBAL_SIZE; num_itn++) {
        gen_test_matrix(a_ptr);
        a_ptr++;
    }
  
 a_ptr = a_ptr_head;

    //-----------------------------------------------
#ifdef GPP_ONLY_FLOW  
    startTime = getCPUTime();
    for (int num_itn = 0 ; num_itn < MAX_ITERATION; num_itn++) {
        for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
        sdx_cppKernel_top(a_ptr, a_inv_ptr);
        a_ptr++;
        a_inv_ptr++;
        }
    } // MAX_ITERATION
    endTime = getCPUTime();

     a_ptr = a_ptr_head;
     a_inv_ptr = a_inv_ptr_head;
#else 
    fpga_hw_accel<input_data_type,output_data_type, NUMBER_OF_COMPUTE_UNITS, MAX_ITERATION> Alpha_Data_ku060_card;

    if (argc != 2){
    printf("%s <inputfile>\n", argv[0]);
    return EXIT_FAILURE;
    }
    in_index = 0;
    for (i = 0; i <  num_input_args;i++) { 
        in_args_size[in_index] = sizeof(input_data_type)*(MAX_ITERATION);
        in_index++;
    }
    out_index = 0;
    for (i = 0; i <  num_output_args;i++) { 
        out_args_size[out_index] = sizeof(output_data_type)*(MAX_ITERATION);
        out_index++;
    }
    init_success = Alpha_Data_ku060_card.initalize_fpga_hw_accel(argv[1], "sdx_cppKernel_top", in_args_size, out_args_size, num_input_args, num_output_args); 
    if (init_success ) {
        startTime = getCPUTime();


        a_ptr = a_ptr_head;
        a_inv_ptr = a_inv_ptr_head;

        run_success =  Alpha_Data_ku060_card.run_fpga_accel (a_ptr, a_inv_ptr);

        a_ptr = a_ptr_head;
        a_inv_ptr = a_inv_ptr_head;


        endTime = getCPUTime();
        if (!run_success ) {
            printf("Error: SdAccel CPP Kernel execution Failed !!\n");
        }
    } else {
        printf("Error: SdAccel provisioning Failed !!\n");
    }
    sdaccel_clean_success = Alpha_Data_ku060_card.clean_fpga_hw_accel();
    if (!sdaccel_clean_success) {
        printf("Error: SdAccel resource cleanip Failed !!\n");
    }

#endif
      printf (" ------------   Results  ------------------------------\n");
      printf (" ------------------------------------------------------\n");
    ElapsedTime = (endTime - startTime);
    for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
      printf("\n Input Test Matrix : %d:\n\n", cu);
      for (i=0;i<DIM;i++) {
         for (j=0;j<DIM;j++) {
              printf ("\t%f ",a_ptr->data_in[i][j]);
         }
              printf ("\n");
       }
       a_ptr++;
      printf (" ------------------------------------------------------\n");
    printf("\n**** Inverse Matrix : %d ****\n\n", cu);
        for (i=0;i<DIM;i++) {
           for (j=0;j<DIM;j++) {
                printf ("\t%f ",a_inv_ptr->data_out[i][j]);
           }
           printf ("\n");
        }
        a_inv_ptr++;
        printf ("-----------------------------------------------------\n");
    }
    free(a_ptr_head);
    free(a_inv_ptr_head);
    printf (" Elapsed Time for algorithm = %1f sec\n", ElapsedTime);
}

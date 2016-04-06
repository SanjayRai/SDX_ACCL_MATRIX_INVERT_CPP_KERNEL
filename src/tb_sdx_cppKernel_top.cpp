#include<stdio.h>
#include<math.h>
#include"matrix_operation_wrapper.h"

#include "sdaccel_utils.h" 
#include "sdx_cppKernel_top.h" 

#ifndef NUMBER_OF_COMPUTE_UNITS
#define NUMBER_OF_COMPUTE_UNITS 1 
#endif

#define MAX_ITERATION 1000000 

double getCPUTime();

void gen_test_matrix(input_data_type a[NUMBER_OF_COMPUTE_UNITS], float twiddle) {
  a[0].data_in[0][0] = 1.0f * twiddle; 
  a[0].data_in[0][1] = 2.0f * twiddle;
  a[0].data_in[0][2] = 3.0f * twiddle;
  a[0].data_in[0][3] = 4.0f * twiddle;
  a[0].data_in[0][4] = 1.0f * twiddle;
  a[0].data_in[1][0] = 5.0f * twiddle;
  a[0].data_in[1][1] = 6.0f * twiddle;
  a[0].data_in[1][2] = 7.0f * twiddle;
  a[0].data_in[1][3] = 8.0f * twiddle;
  a[0].data_in[1][4] = 2.0f * twiddle;
  a[0].data_in[2][0] = 2.0f * twiddle;
  a[0].data_in[2][1] = 6.0f * twiddle;
  a[0].data_in[2][2] = 4.0f * twiddle;
  a[0].data_in[2][3] = 8.0f * twiddle;
  a[0].data_in[2][4] = 3.0f * twiddle;
  a[0].data_in[3][0] = 3.0f * twiddle;
  a[0].data_in[3][1] = 1.0f * twiddle;
  a[0].data_in[3][2] = 1.0f * twiddle;
  a[0].data_in[3][3] = 2.0f * twiddle;
  a[0].data_in[3][4] = 4.0f * twiddle;
  a[0].data_in[4][0] = 1.0f * twiddle;
  a[0].data_in[4][1] = 4.0f * twiddle;
  a[0].data_in[4][2] = 5.0f * twiddle;
  a[0].data_in[4][3] = 6.0f * twiddle;
  a[0].data_in[4][4] = 5.0f * twiddle;

  for (int cu = 1; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
      for (int i = 0; i <  DIM;i++) { 
          for (int j = 0; j <  DIM;j++) { 
              a[cu].data_in[i][j] = ((a[cu-1].data_in[i][j])/(cu*twiddle+1.0f));
          }
      }
  }
}

int main(int argc, char** argv)
{

  double startTime;
  double endTime;
  double ElapsedTime;

  input_data_type a[NUMBER_OF_COMPUTE_UNITS];
  output_data_type a_inv[NUMBER_OF_COMPUTE_UNITS];
  int i,j;

  input_data_type *in_args[10];       // Array of pointers to Input Arguments (Max number of input argument arbitrarily set to 10)
  int in_args_size[10];               // Array of input Argument sizes
  output_data_type *out_args[10];     // Array of pointers to Output Arguments  (Max number of output argument arbitrarily set to 10)
  int out_args_size[10];              // Array of output Argument sizes
  int num_input_args;
  int num_output_args;
  int in_index;
  int out_index;
  int init_success;
  int run_success;
  int sdaccel_clean_success;
  int cu = 0;
  float twiddle;


  num_input_args = 1;
  num_output_args = 1;



  printf("-------------------------------------------------------------\n");
  printf("Create Test Matrix\n");
  printf("-------------------------------------------------------------\n\n\n");
  


    //-----------------------------------------------
#ifdef GPP_ONLY_FLOW  
    startTime = getCPUTime();
    for (int num_itn = 0 ; num_itn < MAX_ITERATION; num_itn++) {
        twiddle = (float)(12.34 + (float)((float)num_itn/(float)MAX_ITERATION)*10.0f);
        gen_test_matrix(a, twiddle);

        for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
            sdx_cppKernel_top(&a[cu], &a_inv[cu]);
        }
    } // MAX_ITERATION
    endTime = getCPUTime();

#else 
    fpga_hw_accel<input_data_type,output_data_type, NUMBER_OF_COMPUTE_UNITS> Alpha_Data_ku060_card;

    if (argc != 2){
    printf("%s <inputfile>\n", argv[0]);
    return EXIT_FAILURE;
    }
    in_index = 0;
    out_index = 0;
    for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
        in_args_size[in_index] = sizeof(input_data_type);
        in_index++;
        out_args_size[out_index] = sizeof(output_data_type);
        out_index++;
    }
    init_success = Alpha_Data_ku060_card.initalize_fpga_hw_accel(argv[1], "sdx_cppKernel_top", in_args_size, out_args_size, num_input_args, num_output_args); 
    if (init_success ) {
        startTime = getCPUTime();

        gen_test_matrix(a, 23.23);

        in_index = 0;
        out_index = 0;
        for (cu = 0; cu <  NUMBER_OF_COMPUTE_UNITS;cu++) { 
            in_args[in_index] = (input_data_type *)&a[cu].data_in;
            in_index++;

            out_args[out_index] = (output_data_type *)&a_inv[cu].data_out;
            out_index++;
        }
        run_success =  Alpha_Data_ku060_card.run_fpga_accel (in_args, out_args);
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
              printf ("\t%f ",(double)a[cu].data_in[i][j]);
         }
              printf ("\n");
       }
      printf (" ------------------------------------------------------\n");
    printf("\n**** Inverse Matrix : %d ****\n\n", cu);
        for (i=0;i<DIM;i++) {
           for (j=0;j<DIM;j++) {
                printf ("\t%f ",(double)a_inv[cu].data_out[i][j]);
           }
           printf ("\n");
        }
           printf ("-----------------------------------------------------\n");
    }
    printf (" Elapsed Time for algorithm = %1f sec\n", ElapsedTime);
}

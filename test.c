// Name: test.c
// Description: Test file to exercise the MLS polynomial fitting function.
// Author: Henry Forson, Melbourne, Florida USA

//------------------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2020 Henry M. Forson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------------

#include  <stdio.h>
#include  <string.h>
#include  "polyfit.h"
#include  "openMP_polyfit.h"

//for timing
#include <time.h>
//for csv
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>

int readCSV(const char* csvFileName, double** x, double** y, size_t* size) {
    FILE* file = fopen(csvFileName, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the CSV file.\n");
        return 1; // Return an error code
    }

    *size = 0;
    size_t capacity = 1;  // Initial capacity, can be adjusted based on your needs
    char line[100];

    // Skip the first line
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: Unable to read the first line.\n");
        fclose(file);
        return 1;
    }

    // Allocate memory for *x and *y
    *x = (double*)malloc(capacity * sizeof(double));
    *y = (double*)malloc(capacity * sizeof(double));

    if (*x == NULL || *y == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        double xValue, yValue;

        if (sscanf(line, "%lf,%lf", &xValue, &yValue) == 2) {
            if (*size == capacity) {
                // Double the capacity
                capacity *= 2;
                double* tempX = realloc(*x, capacity * sizeof(double));
                double* tempY = realloc(*y, capacity * sizeof(double));

                if (tempX == NULL || tempY == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed.\n");
                    fclose(file);

                    // Clean up the existing memory
                    free(*x);
                    free(*y);

                    return 1;
                }

                *x = tempX;
                *y = tempY;
            }

            (*x)[*size] = xValue;
            (*y)[*size] = yValue;
            (*size)++;
        } else {
            fprintf(stderr, "Error: Invalid line format in the CSV file.\n");
        }
    }

    fclose(file);
    return 0; // Return 0 on success
}



// Buffer to hold a string representation of a polynomial:
#define POLY_STRING_BF_SZ   (256)
char polyStringBf[POLY_STRING_BF_SZ];

//--------------------------------------------------------
// main()
// Unit tests the poly() function.
//--------------------------------------------------------
int main()
{

  // ---------------- TEST 3 DATA ------------------------
  // Create a failure test case with impossible to solve data.
  const char* csvFileNamePlane = "FlightDurationPrice.csv";
  double* x3 = NULL;
  double* y3 = NULL;
  size_t size = 0;

  int result = readCSV(csvFileNamePlane, &x3, &y3, &size);

  if(x3 == NULL || y3 == NULL){
    printf("reading failed");
  }
  printf("Size3: %zu\n", size);

  int pc3       = (int)size;           // pointCount  
  double cr3[]  = {0, 0, 0, 0, 0};                                       // coefficientResults
  int cc3       =  (int) (sizeof(cr3) / sizeof(cr3[0]));        // coefficientCount
  char *er3     = "Will fail";                  // expected result
    
  // ---------------- TEST 4 DATA ------------------------
  //double x4[]   = { 1.2, 13.69, 0.95, 1.24, 1.1, 1.9, 0.0, 0.66 };
  //double y4[]   = { 7.3, 43.3, 10.14, 7.8, 8.3, 15.05, 18.8, 6.3 };
  const char* csvFileName = "filteredDistTotal.csv";
  double* x4 = NULL;
  double* y4 = NULL;
  size = 0;

  result = readCSV(csvFileName, &x4, &y4, &size);

  if(x4 == NULL || y4 == NULL){
    printf("reading failed");
  }
  printf("Size4: %zu\n", size);

  int pc4       = (int)size;           // pointCount  
  double cr4[]  = {0, 0, 0, 0, 0};                                       // coefficientResults
  int cc4       =  (int) (sizeof(cr4) / sizeof(cr4[0]));        // coefficientCount
  char *er4     = "Will fail";                  // expected result
  
  int rVal;
  int passedCount = 0;
  int failedCount = 0;

 //---------------------TEST 3---------------------------

  struct timespec start_time_p, end_time_p, start_time_pop, end_time_pop;

  clock_gettime(CLOCK_MONOTONIC, &start_time_p);
  rVal = polyfit( pc3, x3, y3, cc3, cr3);
  clock_gettime(CLOCK_MONOTONIC, &end_time_p);
  double elapsed_time = (end_time_p.tv_sec - start_time_p.tv_sec) +
                       (end_time_p.tv_nsec - start_time_p.tv_nsec) / 1e9;
  printf("Execution time of plane: %f seconds\n", elapsed_time);
    
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc3, cr3 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test plane produced %s\n", polyStringBf);
    
  clock_gettime(CLOCK_MONOTONIC, &start_time_pop);
  rVal = openmp_polyfit( pc3, x3, y3, cc3, cr3);
  clock_gettime(CLOCK_MONOTONIC, &end_time_pop);
  elapsed_time = (end_time_pop.tv_sec - start_time_pop.tv_sec) +
                       (end_time_pop.tv_nsec - start_time_pop.tv_nsec) / 1e9;
  printf("Execution time of openmp plane: %f seconds\n", elapsed_time_pop);
    
  if( 0 == rVal)
  { 
    openmp_polyToString( polyStringBf, POLY_STRING_BF_SZ, cc3, cr3 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test plane openmp produced %s\n", polyStringBf);
 
 
//---------------------TEST 4---------------------------

  struct timespec start_time_t, end_time_t, start_time_top, end_time_top;

  clock_gettime(CLOCK_MONOTONIC, &start_time_t);
  rVal = polyfit( pc4, x4, y4, cc4, cr4);
  clock_gettime(CLOCK_MONOTONIC, &end_time_t);
  elapsed_time = (end_time_t.tv_sec - start_time_t.tv_sec) +
                       (end_time_t.tv_nsec - start_time_t.tv_nsec) / 1e9;
  printf("Execution time of taxi: %f seconds\n", elapsed_time);
    
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc4, cr4 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test taxi produced %s\n", polyStringBf);
    
  clock_gettime(CLOCK_MONOTONIC, &start_time_top);
  rVal = openmp_polyfit( pc4, x4, y4, cc4, cr4);
  clock_gettime(CLOCK_MONOTONIC, &end_time_top);
  elapsed_time = (end_time_top.tv_sec - start_time_top.tv_sec) +
                       (end_time_top.tv_nsec - start_time_top.tv_nsec) / 1e9;
  printf("Execution time of openmp taxi: %f seconds\n", elapsed_time_top);
    
  if( 0 == rVal)
  { 
    openmp_polyToString( polyStringBf, POLY_STRING_BF_SZ, cc4, cr4 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test taxi openmp produced %s\n", polyStringBf);
 
  
//---------------------SUMMARY--------------------------- 
  return( -failedCount );
}

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
//#include  "pthreads_polyfit.h"

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
  const char* csvFileNamePlane = "medCost10k.csv";
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
  const char* csvFileName = "medCost100k.csv";
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

  const char* csvFileNameTest5 = "medCost1M.csv";  // Replace with your actual CSV file name
double* x5 = NULL;
double* y5 = NULL;
size_t size5 = 0;

int result5 = readCSV(csvFileNameTest5, &x5, &y5, &size5);

if (x5 == NULL || y5 == NULL) {
    printf("Reading failed for test 5\n");
}
printf("Size5: %zu\n", size5);

int pc5 = static_cast<int>(size5);  // pointCount
double cr5[] = {0, 0, 0, 0, 0};  // coefficientResults
int cc5 = static_cast<int>(sizeof(cr5) / sizeof(cr5[0]));  // coefficientCount
char* er5 = "Expected result for test 5";  // expected result

// ---------------- TEST 6 DATA ------------------------
const char* csvFileNameTest6 = "medCost10M.csv";  // Replace with your actual CSV file name
double* x6 = NULL;
double* y6 = NULL;
size_t size6 = 0;

int result6 = readCSV(csvFileNameTest6, &x6, &y6, &size6);

if (x6 == NULL || y6 == NULL) {
    printf("Reading failed for test 6\n");
}
printf("Size6: %zu\n", size6);

int pc6 = static_cast<int>(size6);  // pointCount
double cr6[] = {0, 0, 0, 0, 0};  // coefficientResults
int cc6 = static_cast<int>(sizeof(cr6) / sizeof(cr6[0]));  // coefficientCount
char* er6 = "Expected result for test 6";  // expected result

int rVal;
int passedCount = 0;
int failedCount = 0;

  struct timespec start_time_p, end_time_p, start_time_pop, end_time_pop, start_time_ppt, end_time_ppt;
    //NAIVE
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
  printf( "10K plane produced %s\n", polyStringBf);

    //OPENMP
  clock_gettime(CLOCK_MONOTONIC, &start_time_pop);
  rVal = openmp_polyfit( pc3, x3, y3, cc3, cr3);
  clock_gettime(CLOCK_MONOTONIC, &end_time_pop);
  elapsed_time = (end_time_pop.tv_sec - start_time_pop.tv_sec) +
                       (end_time_pop.tv_nsec - start_time_pop.tv_nsec) / 1e9;
  printf("Execution time of openmp 10K: %f seconds\n", elapsed_time);
    
  if( 0 == rVal)
  { 
    openmp_polyToString( polyStringBf, POLY_STRING_BF_SZ, cc3, cr3 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "10K openmp produced %s\n", polyStringBf);

 
//---------------------TEST 4---------------------------

  struct timespec start_time_t, end_time_t, start_time_top, end_time_top, start_time_tpt, end_time_tpt;
    //NAIVE
  clock_gettime(CLOCK_MONOTONIC, &start_time_t);
  rVal = polyfit( pc4, x4, y4, cc4, cr4);
  clock_gettime(CLOCK_MONOTONIC, &end_time_t);
  elapsed_time = (end_time_t.tv_sec - start_time_t.tv_sec) +
                       (end_time_t.tv_nsec - start_time_t.tv_nsec) / 1e9;
  printf("Execution time of 100K: %f seconds\n", elapsed_time);
    
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc4, cr4 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "100K produced %s\n", polyStringBf);
    //OPENMP
  clock_gettime(CLOCK_MONOTONIC, &start_time_top);
  rVal = openmp_polyfit( pc4, x4, y4, cc4, cr4);
  clock_gettime(CLOCK_MONOTONIC, &end_time_top);
  elapsed_time = (end_time_top.tv_sec - start_time_top.tv_sec) +
                       (end_time_top.tv_nsec - start_time_top.tv_nsec) / 1e9;
  printf("Execution time of openmp 100K: %f seconds\n", elapsed_time);
    
  if( 0 == rVal)
  { 
    openmp_polyToString( polyStringBf, POLY_STRING_BF_SZ, cc4, cr4 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "100K openmp produced %s\n", polyStringBf);

    //TEST 5
  struct timespec start_time_m, end_time_m, start_time_mp, end_time_mp, start_time_mpt, end_time_mpt;
// NAIVE
clock_gettime(CLOCK_MONOTONIC, &start_time_m);
rVal = polyfit(pc5, x5, y5, cc5, cr5);
clock_gettime(CLOCK_MONOTONIC, &end_time_m);
elapsed_time = (end_time_m.tv_sec - start_time_m.tv_sec) +
               (end_time_m.tv_nsec - start_time_m.tv_nsec) / 1e9;
printf("Execution time of 1M points: %f seconds\n", elapsed_time);

if (0 == rVal)
{
    polyToString(polyStringBf, POLY_STRING_BF_SZ, cc5, cr5);
}
else
{
    snprintf(polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal);
}
printf("1M points produced %s\n", polyStringBf);

// OPENMP
clock_gettime(CLOCK_MONOTONIC, &start_time_mp);
rVal = openmp_polyfit(pc5, x5, y5, cc5, cr5);
clock_gettime(CLOCK_MONOTONIC, &end_time_mp);
elapsed_time = (end_time_mp.tv_sec - start_time_mp.tv_sec) +
               (end_time_mp.tv_nsec - start_time_mp.tv_nsec) / 1e9;
printf("Execution time of openmp 1M points: %f seconds\n", elapsed_time);

if (0 == rVal)
{
    openmp_polyToString(polyStringBf, POLY_STRING_BF_SZ, cc5, cr5);
}
else
{
    snprintf(polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal);
}
printf("1M points openmp produced %s\n", polyStringBf);

//Test6
struct timespec start_time_l, end_time_l, start_time_lp, end_time_lp, start_time_lpt, end_time_lpt;
// NAIVE
clock_gettime(CLOCK_MONOTONIC, &start_time_l);
rVal = polyfit(pc6, x6, y6, cc6, cr6);
clock_gettime(CLOCK_MONOTONIC, &end_time_l);
elapsed_time = (end_time_l.tv_sec - start_time_l.tv_sec) +
               (end_time_l.tv_nsec - start_time_l.tv_nsec) / 1e9;
printf("Execution time of 10M points: %f seconds\n", elapsed_time);

if (0 == rVal)
{
    polyToString(polyStringBf, POLY_STRING_BF_SZ, cc6, cr6);
}
else
{
    snprintf(polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal);
}
printf("10M points produced %s\n", polyStringBf);

// OPENMP
clock_gettime(CLOCK_MONOTONIC, &start_time_lp);
rVal = openmp_polyfit(pc6, x6, y6, cc6, cr6);
clock_gettime(CLOCK_MONOTONIC, &end_time_lp);
elapsed_time = (end_time_lp.tv_sec - start_time_lp.tv_sec) +
               (end_time_lp.tv_nsec - start_time_lp.tv_nsec) / 1e9;
printf("Execution time of openmp 10M points: %f seconds\n", elapsed_time);

if (0 == rVal)
{
    openmp_polyToString(polyStringBf, POLY_STRING_BF_SZ, cc6, cr6);
}
else
{
    snprintf(polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal);
}
printf("10M points openmp produced %s\n", polyStringBf);

//---------------------SUMMARY--------------------------- 
  return( -failedCount );
}

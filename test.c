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

//for timing
#include <time.h>
//for csv
#include <stdlib.h>

// Buffer to hold a string representation of a polynomial:
#define POLY_STRING_BF_SZ   (256)
char polyStringBf[POLY_STRING_BF_SZ];

// ---------------- TEST 1 DATA ------------------------
// Create test data to find the closest line to (0,6), (1,0), and (2,0).
// See also page 218 of: https://math.mit.edu/~gs/linearalgebra/ila0403.pdf
double x1[]   = { 0, 1, 2};
double y1[]   = { 6, 0, 0};
int pc1       = (int) (sizeof(x1) / sizeof(x1[0]));           // pointCount
double cr1[]  = {0, 0};                                       // coefficientResults
int cc1       =  (int) (sizeof(cr1) / sizeof(cr1[0]));        // coefficientCount
char *er1     = "(-3.000000 * x) + 5.000000";                 // expected result

// ---------------- TEST 2 DATA ------------------------
// Create test data to find the closest parabola to the points shown.
// as example "4.3 B" on page 226 of: https://math.mit.edu/~gs/linearalgebra/ila0403.pdf
double x2[]   = { -2, -1, 00, 1, 2};
double y2[]   = { 0, 0, 1, 0, 0};
int pc2       = (int) (sizeof(x2) / sizeof(x2[0]));           // pointCount
double cr2[]  = {0, 0, 0};                                    // coefficientResults
int cc2       =  (int) (sizeof(cr2) / sizeof(cr2[0]));        // coefficientCount
char *er2     = "(-0.142857 * x^2) + 0.485714";               // expected result

// ---------------- TEST 3 DATA ------------------------
// Create a failure test case with impossible to solve data.
double x3[]   = { 0, 1, 1, 0};
double y3[]   = { 0, 1, 1, 0};
int pc3       = (int) (sizeof(x3) / sizeof(x3[0]));           // pointCount
double cr3[]  = {0, 0, 0};                                    // coefficientResults
int cc3       =  (int) (sizeof(cr3) / sizeof(cr3[0]));        // coefficientCount
char *er3     = "error = -4";                                 // expected result

// ---------------- TEST 4 DATA ------------------------
// test MLS regeression example from https://www.mathsisfun.com/data/least-squares-regression.html
const char* csvFileName = "filteredDistTotal.csv"; // Replace with your CSV file name
    FILE* file = fopen(csvFileName, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open the CSV file.\n");
        return 1; // Return an error code
    }

    double* x4 = NULL;
    double* y4 = NULL;
    size_t size = 0;
    char line[100];  // Directly using a constant value for simplicity

    while (fgets(line, sizeof(line), file) != NULL) {
        double x, y;

        if (sscanf(line, "%lf,%lf", &x, &y) == 2) {
            x4 = realloc(x4, (size + 1) * sizeof(double));
            y4 = realloc(y4, (size + 1) * sizeof(double));

            if (x4 == NULL || y4 == NULL) {
                fprintf(stderr, "Error: Memory allocation failed.\n");
                fclose(file);
                free(x4);
                free(y4);
                return 1;
            }

            x4[size] = x;
            x4[size] = y;
            size++;
        } else {
            fprintf(stderr, "Error: Invalid line format in the CSV file.\n");
        }
    }

    fclose(file);
//double x4[]   = { 1.2, 13.69, 0.95, 1.24, 1.1, 1.9, 0.0, 0.66 };
//double y4[]   = { 7.3, 43.3, 10.14, 7.8, 8.3, 15.05, 18.8, 6.3 };
int pc4       = (int) (sizeof(x4) / sizeof(x4[0]));           // pointCount
double cr4[]  = {0, 0, 0, 0, 0, 0};                                       // coefficientResults
int cc4       =  (int) (sizeof(cr4) / sizeof(cr4[0]));        // coefficientCount
char *er4     = "Will fail";                  // expected result


//--------------------------------------------------------
// main()
// Unit tests the poly() function.
//--------------------------------------------------------
int main()
{
  //for time measurements
  clock_t start, end;
  double cpu_time_used;
  
  int rVal;
  int passedCount = 0;
  int failedCount = 0;

  //---------------------TEST 1---------------------------
  printf( "Test 1 expected %s\n", er1);
  rVal = polyfit( pc1, x1, y1, cc1, cr1);
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc1, cr1 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test 1 produced %s\n", polyStringBf);
  if( 0 == strcmp( polyStringBf, er1) )
  {
    printf( "Test 1 passed OK.\n\n");
    passedCount += 1;
  }
  else
  {
    printf( "Test failed.\n\n");
    failedCount += 1;
  }
 
  //---------------------TEST 2---------------------------
  printf( "Test 2 expected %s\n", er2);
  rVal = polyfit( pc2, x2, y2, cc2, cr2);
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc2, cr2 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test 2 produced %s\n", polyStringBf);
  if( 0 == strcmp( polyStringBf, er2) )
  {
    printf( "Test 2 passed OK.\n\n");
    passedCount += 1;
  }
  else
  {
    printf( "Test failed.\n\n");
    failedCount += 1;
  }
  
 //---------------------TEST 3---------------------------
  printf( "Test 3 expected %s\n", er3);
  rVal = polyfit( pc3, x3, y3, cc3, cr3);
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc3, cr3 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }
  printf( "Test 3 produced %s\n", polyStringBf);
  if( 0 == strcmp( polyStringBf, er3) )
  {
    printf( "Test 3 passed OK.\n\n");
    passedCount += 1;
  }
  else
  {
    printf( "Test failed.\n\n");
    failedCount += 1;
  }
 
//---------------------TEST 4---------------------------
  printf( "Test 4 expected %s\n", er4);

  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  
  rVal = polyfit( pc4, x4, y4, cc4, cr4);
  
  if( 0 == rVal)
  { 
    polyToString( polyStringBf, POLY_STRING_BF_SZ, cc4, cr4 );
  }
  else
  {
    snprintf( polyStringBf, POLY_STRING_BF_SZ, "error = %d", rVal );
  }

  clock_gettime(CLOCK_MONOTONIC, &end_time);
  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                       (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
  printf("Execution time of custom test (4): %f seconds\n", elapsed_time);
  
  printf( "Test 4 produced %s\n", polyStringBf);
  if( 0 == strcmp( polyStringBf, er4) )
  {
    printf( "Test 4 passed OK.\n\n");
    passedCount += 1;
  }
  else
  {
    printf( "Test failed.\n\n");
    failedCount += 1;
  }
  
//---------------------SUMMARY--------------------------- 
  printf( "Tests complete: %d passed, %d failed.\n", passedCount, failedCount); 
  return( -failedCount );
}

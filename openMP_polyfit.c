// Name: polyfit.c
// Description: Simple polynomial fitting functions.
// Author: Henry M. Forson, Melbourne, Florida USA

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

#include <math.h>       // pow()
#include <stdbool.h>    // bool
#include <stdio.h>      // printf()
#include <stdlib.h>     // calloc()
#include <string.h>     // strlen()

#include "openMP_polyfit.h"
#include <omp.h>

//timing
#include <time.h>

//block initialiation
int blockSize = 32;
// Define SHOW_MATRIX to display intermediate matrix values:
// #define SHOW_MATRIX 1

// Structure of a matrix.
typedef struct matrix_s
{
	int	rows;
	int	cols;
	double *pContents;
} matrix_t;

// MACRO to access a value with a matrix.
#define MATRIX_VALUE_PTR( pA, row, col )  (&(((pA)->pContents)[ (row * (pA)->cols) + col]))

#ifdef SHOW_MATRIX
#define showMatrix( x ) do {\
    printf( "   @%d: " #x " =\n", __LINE__ ); \
    reallyShowMatrix( x ); \
    printf( "\n" ); \
} while( 0 )
#else   // SHOW_MATRIX
#define showMatrix( x )
#endif   // SHOW_MATRIX


//------------------------------------------------
// Private Function Prototypes
//------------------------------------------------

static matrix_t *   createMatrix( int rows, int cols );
static void         destroyMatrix( matrix_t *pMat );
#ifdef SHOW_MATRIX
static void         reallyShowMatrix( matrix_t *pMat );
#endif  // SHOW_MATRIX
static matrix_t *   createTranspose( matrix_t *pMat );
static matrix_t *   createProduct( matrix_t *pLeft, matrix_t *pRight );


//=========================================================
//      Global function definitions
//=========================================================


//--------------------------------------------------------
// polyfit()
// Computes polynomial coefficients that best fit a set
// of input points.
//
// The degree of the fitted polynomial is one less than
// the count of elements in the polynomial vector.
//
// Uses matrix algebra of the form:
//              A * x = b
// To solve the MLS equation:
//              (AT) * A * x = (AT) * b
// where (AT) is the transpose of A.
//
// If the n+1 input points are {(x0, y0), (x1, y1), ... (xn, yn)},
// then the i'th row of A is: {(xi)^0, (xi)^1, ... (xn)^n},
// and the i'th row of b is: {yi}.
//
// Returns   0 if success, 
//          -1 if passed a NULL pointer,
//          -2 if (pointCount < coefficientCount),
//          -3 if unable to allocate memory,
//          -4 if unable to solve equations.
//--------------------------------------------------------
//int polyfit( int pointCount, point_t pointArray[],  int coeffCount, double coeffArray[] )
int openmp_polyfit( int pointCount, double *xValues, double *yValues, int coefficientCount, double *coefficientResults )
{
    struct timespec s_create, e_create, s_fill, e_fill, s_mult, e_mult, s_trans, e_trans;
    double elapsed_time;
    int rVal = 0;
    int degree = coefficientCount - 1;

    // Check that the input pointers aren't null.
    if( (NULL == xValues) || (NULL == yValues) || (NULL == coefficientResults) )
    {
        return -1;
    }
    // Check that pointCount >= coefficientCount.
    if(pointCount < coefficientCount)
    {
        return -2;
    }

    // printf( "pointCount = %d:", pointCount );

    // for( i = 0; i < pointCount; i++ )
    // {
    //     printf( " ( %f, %f )", xValues[i], yValues[i] );
    // }
    // printf( "\n");

    // printf( "coefficientCount = %d\n", coefficientCount );

    // Make the A matrix:
    clock_gettime(CLOCK_MONOTONIC, &s_create);
    matrix_t *pMatA = createMatrix( pointCount, coefficientCount );
    if( NULL == pMatA)
    {
        return -3;
    }
    clock_gettime(CLOCK_MONOTONIC, &e_create);
    elapsed_time = (e_create.tv_sec - s_create.tv_sec) +
                       (e_create.tv_nsec - s_create.tv_nsec) / 1e9;
    printf("Execution time of create (A): %f seconds\n", elapsed_time);

    clock_gettime(CLOCK_MONOTONIC, &s_fill);
    #pragma omp parallel for 
    for( int r = 0; r < pointCount; r++)
    {
        for( int c = 0; c < coefficientCount; c++)
        {
            *(MATRIX_VALUE_PTR(pMatA, r, c)) = pow((xValues[r]), (double) (degree -c));
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &e_fill);
    elapsed_time = (e_fill.tv_sec - s_fill.tv_sec) +
                       (e_fill.tv_nsec - s_fill.tv_nsec) / 1e9;
    printf("Execution time of fill (A): %f seconds\n", elapsed_time);
	
    showMatrix( pMatA );

    // Make the b matrix
    matrix_t *pMatB = createMatrix( pointCount, 1);
    if( NULL == pMatB )
    {
        return -3;
    }

    #pragma omp parallel for 
    for( int r = 0; r < pointCount; r++)
    {
        *(MATRIX_VALUE_PTR(pMatB, r, 0)) = yValues[r];
    }

    clock_gettime(CLOCK_MONOTONIC, &s_trans);
    // Make the transpose of matrix A
    matrix_t * pMatAT = createTranspose( pMatA );
    if( NULL == pMatAT )
    {
        return -3;
    }
    clock_gettime(CLOCK_MONOTONIC, &e_trans);
    elapsed_time = (e_trans.tv_sec - s_trans.tv_sec) +
                       (e_trans.tv_nsec - s_trans.tv_nsec) / 1e9;
    printf("Execution time of trans (A): %f seconds\n", elapsed_time);
	
    showMatrix( pMatAT );

    clock_gettime(CLOCK_MONOTONIC, &s_mult);
    // Make the product of matrices AT and A:
    matrix_t *pMatATA = createProduct( pMatAT, pMatA );
    if( NULL == pMatATA )
    {
        return -3;
    }
    clock_gettime(CLOCK_MONOTONIC, &e_mult);
    elapsed_time = (e_mult.tv_sec - s_mult.tv_sec) +
                       (e_mult.tv_nsec - s_mult.tv_nsec) / 1e9;
    printf("Execution time of mult (A): %f seconds\n", elapsed_time);
    showMatrix( pMatATA );

    // Make the product of matrices AT and b:
    matrix_t *pMatATB = createProduct( pMatAT, pMatB );
    if( NULL == pMatATB )
    {
        return -3;
    }

    showMatrix( pMatATB );

    // Now we need to solve the system of linear equations,
    // (AT)Ax = (AT)b for "x", the coefficients of the polynomial.

    for( int c = 0; c < pMatATA->cols; c++ )
    {
        int pr = c;     // pr is the pivot row.
        double prVal = *MATRIX_VALUE_PTR(pMatATA, pr, c);
        // If it's zero, we can't solve the equations.
        if( 0.0 == prVal )
        {
            // printf( "Unable to solve equations, pr = %d, c = %d.\n", pr, c );
            showMatrix( pMatATA );
            rVal = -4;
            break;
        }
        for( int r = 0; r < pMatATA->rows; r++)
        {
            if( r != pr )
            {
                double targetRowVal = *MATRIX_VALUE_PTR(pMatATA, r, c);
                double factor = targetRowVal / prVal;
                for( int c2 = 0; c2 < pMatATA->cols; c2++ )
                {
                    *MATRIX_VALUE_PTR(pMatATA, r, c2) -=  *MATRIX_VALUE_PTR(pMatATA, pr, c2) * factor; 
                    // printf( "c = %d, pr = %d, r = %d, c2=%d, targetRowVal = %f, prVal = %f, factor = %f.\n",
                    //         c, pr, r, c2, targetRowVal, prVal, factor );

                    // showMatrix( pMatATA );
                   
                }
                *MATRIX_VALUE_PTR(pMatATB, r, 0) -=  *MATRIX_VALUE_PTR(pMatATB, pr, 0) * factor;

                showMatrix( pMatATB );
            }
        }
    }
    for( int c = 0; c < pMatATA->cols; c++ )
    {
        int pr = c;
        // now, pr is the pivot row.
        double prVal = *MATRIX_VALUE_PTR(pMatATA, pr, c);
        *MATRIX_VALUE_PTR(pMatATA, pr, c) /= prVal;
        *MATRIX_VALUE_PTR(pMatATB, pr, 0) /= prVal;
    }

    showMatrix( pMatATA );

    showMatrix( pMatATB );

    for( int i = 0; i < coefficientCount; i++)
    {
        coefficientResults[i] = *MATRIX_VALUE_PTR(pMatATB, i, 0);
    }

    
    destroyMatrix( pMatATB );
    destroyMatrix( pMatATA );
    destroyMatrix( pMatAT );

    destroyMatrix( pMatA );
    destroyMatrix( pMatB );
    return rVal;
}

//--------------------------------------------------------
// polyToString()
// Produces a string representation of a polynomial from
// its coefficients.
// Returns 0 on success.
//--------------------------------------------------------
int openmp_polyToString( char *stringBuffer, size_t stringBufferSz, int coeffCount, double *coefficients )
{
    bool isThisTheFirstTermShown = true;
    if( (NULL == stringBuffer) || (NULL == coefficients) )
    {
        return -1;  // NULL pointer passed as a parameter
    }
        if( (0 == stringBufferSz) || (coeffCount <= 0) )
    {
        return -2;  // parameter out of range.
    }

    stringBuffer[0] = 0;

    for( int i = 0; i < coeffCount; i++)
    {
        int exponent = (coeffCount - 1) - i;
        bool isTermPrintable = (coefficients[i] != 0.0);
        if( isTermPrintable )
        {
            int stringIndex = strlen( stringBuffer );           // Index of where to write the next term.
            char *pNext = &(stringBuffer[ stringIndex ]);       // Pointer to where to write the next term.
            int remainingSize = stringBufferSz - stringIndex;   // Space left in buffer.
 
            if( 0 == exponent )
            {
                snprintf( pNext, remainingSize, "%s%f", isThisTheFirstTermShown ? "" : " + ", coefficients[ i ] );
            }
            else if( 1 == exponent)
            {
                snprintf( pNext, remainingSize, "%s(%f * x)", isThisTheFirstTermShown ? "" : " + ", coefficients[ i ] );
            }
            else
            {
                snprintf( pNext, remainingSize, "%s(%f * x^%d)", isThisTheFirstTermShown ? "" : " + ", coefficients[i], exponent );
            }
            isThisTheFirstTermShown = false;
        }
    }
    return 0;
}

//=========================================================
//      Private function definitions
//=========================================================

#ifdef SHOW_MATRIX
//--------------------------------------------------------
// reallyShowMatrix()
// Printf the contents of a matrix
//--------------------------------------------------------
static void reallyShowMatrix( matrix_t *pMat )
{
    for( int r = 0; r < pMat->rows; r++ )
    {
        for( int c = 0; c < pMat->cols; c++)
        {
            printf( "   %f", *MATRIX_VALUE_PTR(pMat, r, c));
        }
        printf( "\n" );
    }
}
#endif  // SHOW_MATRIX

//--------------------------------------------------------
// createTranspose()
// Returns the transpose of a matrix, or NULL.
//
// The caller must free both the allocated matrix
// and its contents array.
//--------------------------------------------------------
static matrix_t * createTranspose( matrix_t *pMat )
{
    matrix_t *rVal = (matrix_t *) calloc(1, sizeof(matrix_t));
    rVal->pContents = (double *) calloc( pMat->rows * pMat->cols, sizeof( double ));
    rVal->cols = pMat->rows;
    rVal->rows = pMat->cols;
    int r, c;
    int num_threads;

	
    int numBlocksRow = (pMat->rows + blockSize - 1) / blockSize;
    int numBlocksCol = (pMat->cols + blockSize - 1) / blockSize;

    #pragma omp parallel for collapse(2)
    for (int blockRow = 0; blockRow < numBlocksRow; blockRow++) {
        for (int blockCol = 0; blockCol < numBlocksCol; blockCol++) {
            int startRow = blockRow * blockSize;
            int startCol = blockCol * blockSize;

            for (int r = startRow; r < MIN(startRow + blockSize, pMat->rows); r++) {
                for (int c = startCol; c < MIN(startCol + blockSize, pMat->cols); c++) {
                    *MATRIX_VALUE_PTR(rVal, c, r) = *MATRIX_VALUE_PTR(pMat, r, c);
                }
            }
        }
    }

    return rVal;
}

//--------------------------------------------------------
// createProduct()
// Returns the product of two matrices, or NULL.
//
// The caller must free both the allocated product matrix
// and its contents array.
//--------------------------------------------------------
static matrix_t * createProduct( matrix_t *pLeft, matrix_t *pRight )
{
    matrix_t *rVal = NULL;
    if( (NULL == pLeft) || (NULL == pRight) || (pLeft->cols != pRight->rows) )
    {
        printf( "Illegal parameter passed to createProduct().\n");
    }
    else
    {
        // Allocate the product matrix.
        rVal = (matrix_t *) calloc(1, sizeof(matrix_t));
        rVal->rows = pLeft->rows;
        rVal->cols = pRight->cols;
        rVal->pContents = (double *) calloc( rVal->rows * rVal->cols, sizeof( double ));


	int i, j, k;
        double sum;
	    
        // Initialize the product matrix contents:
        // product[i,j] = sum{k = 0 .. (pLeft->cols - 1)} (pLeft[i,k] * pRight[ k, j])
        #pragma omp parallel for collapse(2) private(i, j, k) reduction(+:sum) 

        for( i = 0; i < rVal->rows; i++)
        {
            for( k = 0; k < pLeft->cols; k++)
            {
		sum = 0.0;
                for( j = 0; j < rVal->cols; j++ 
                {
                    sum += (*MATRIX_VALUE_PTR(pLeft, i, k)) * (*MATRIX_VALUE_PTR(pRight, k, j));
                }
		*MATRIX_VALUE_PTR(rVal, i, j) = sum;
            }
        }
    }    
       
    return rVal;
}

//--------------------------------------------------------
// destroyMatrix()
// Frees both the allocated matrix and its contents array.
//--------------------------------------------------------
static void destroyMatrix( matrix_t *pMat )
{
    if(NULL != pMat)
    {
        if(NULL != pMat->pContents)
        {
            free(pMat->pContents);
        }
        free( pMat );
    }
}

//--------------------------------------------------------
// createMatrix()
// Allocates the matrix and clears its contents array.
//--------------------------------------------------------
static matrix_t *createMatrix( int rows, int cols )
{
    matrix_t *rVal = (matrix_t *) calloc(1, sizeof(matrix_t));
    if(NULL != rVal)
    {
        rVal->rows = rows;
        rVal->cols = cols;
        rVal->pContents = (double *) calloc( rows * cols, sizeof( double ));
        if(NULL == rVal->pContents)
        {
            free( rVal );
            rVal = NULL;
        }
    }

    return rVal;
}

 

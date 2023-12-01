#ifndef OPENMP_POLYFIT_H
#define OPENMP_POLYFIT_H


//------------------------------------------------
// Function Prototypes
//------------------------------------------------

//--------------------------------------------------------
// polyfit()
// Computes polynomial coefficients that best fit a set
// of input points.
//
// Returns 0 if success.
//--------------------------------------------------------
int openmp_polyfit( int pointCount, double *xValues, double *yValues, int coefficientCount, double *coefficientResults );

//--------------------------------------------------------
// polyToString()
// Produces a string representation of a polynomial from
// its coefficients.
// Returns 0 on success.
//--------------------------------------------------------
int openmp_polyToString( char *stringBuffer, size_t stringBufferSz, int coeffCount, double *coefficients );



#endif	// POLYFIT_H

# 451-Project
Parallel Linear Regression


# Error Log
pthreads_polyfit.c: In function ‘pthreads_polyfit’:
pthreads_polyfit.c:179:25: error: too many arguments to function ‘createTranspose’
  179 |     matrix_t * pMatAT = createTranspose( pMatA, 8 );
      |                         ^~~~~~~~~~~~~~~
pthreads_polyfit.c:89:21: note: declared here
   89 | static matrix_t *   createTranspose( matrix_t *pMat );
      |                     ^~~~~~~~~~~~~~~
pthreads_polyfit.c:188:25: error: too many arguments to function ‘createProduct’
  188 |     matrix_t *pMatATA = createProduct( pMatAT, pMatA, 8 );
      |                         ^~~~~~~~~~~~~
pthreads_polyfit.c:90:21: note: declared here
   90 | static matrix_t *   createProduct( matrix_t *pLeft, matrix_t *pRight );
      |                     ^~~~~~~~~~~~~
pthreads_polyfit.c:197:25: error: too many arguments to function ‘createProduct’
  197 |     matrix_t *pMatATB = createProduct( pMatAT, pMatB, 8 );
      |                         ^~~~~~~~~~~~~
pthreads_polyfit.c:90:21: note: declared here
   90 | static matrix_t *   createProduct( matrix_t *pLeft, matrix_t *pRight );
      |                     ^~~~~~~~~~~~~
pthreads_polyfit.c: At top level:
pthreads_polyfit.c:361:18: error: conflicting types for ‘createTranspose’; have ‘matrix_t *(matrix_t *, int)’ {aka ‘struct matrix_s *(struct matrix_s *, int)’}
  361 | static matrix_t *createTranspose(matrix_t *pInput, int numThreads)
      |                  ^~~~~~~~~~~~~~~
pthreads_polyfit.c:89:21: note: previous declaration of ‘createTranspose’ with type ‘matrix_t *(matrix_t *)’ {aka ‘struct matrix_s *(struct matrix_s *)’}
   89 | static matrix_t *   createTranspose( matrix_t *pMat );
      |                     ^~~~~~~~~~~~~~~
pthreads_polyfit.c:435:18: error: conflicting types for ‘createProduct’; have ‘matrix_t *(matrix_t *, matrix_t *, int)’ {aka ‘struct matrix_s *(struct matrix_s *, struct matrix_s *, int)’}
  435 | static matrix_t *createProduct(matrix_t *pLeft, matrix_t *pRight, int numThreads)
      |                  ^~~~~~~~~~~~~
pthreads_polyfit.c:90:21: note: previous declaration of ‘createProduct’ with type ‘matrix_t *(matrix_t *, matrix_t *)’ {aka ‘struct matrix_s *(struct matrix_s *, struct matrix_s *)’}
   90 | static matrix_t *   createProduct( matrix_t *pLeft, matrix_t *pRight );
      |                     ^~~~~~~~~~~~~
pthreads_polyfit.c:89:21: warning: ‘createTranspose’ used but never defined
   89 | static matrix_t *   createTranspose( matrix_t *pMat );
      |                     ^~~~~~~~~~~~~~~
pthreads_polyfit.c:90:21: warning: ‘createProduct’ used but never defined
   90 | static matrix_t *   createProduct( matrix_t *pLeft, matrix_t *pRight );

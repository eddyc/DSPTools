//
//  Matrix.h
//  Matrix
//
//  Created by Edward Costello on 02/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>

#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct Matrix
    {
        size_t rowCount;
        size_t columnCount;
        size_t elementCount;
        Float64 *data;
        Float64 *temp;
        

        
        struct Matrix *submatrixView;
        
    } Matrix;
    
    OVERLOADED Matrix *const Matrix_new(size_t rowCount, size_t columnCount);
    OVERLOADED Matrix *const Matrix_new(Matrix *original);
    
    Matrix *Matrix_newFromHDF(const char *path, const char *dataSet);
    
    void Matrix_scopedDelete(Matrix **self);
    void Matrix_delete(Matrix *self);
    
    Matrix *Matrix_newWithArray(size_t rowCount, size_t columnCount, Float64 *array);
    Matrix *Matrix_newWithValues(size_t rowCount, size_t columnCount, size_t valuesCount,...);
    Matrix *Matrix_newIdentity(size_t dimensionSize);
    
#define _Matrix __attribute__((unused)) __attribute__((cleanup(Matrix_scopedDelete))) Matrix
    
#define Matrix_getRow(self,row) (&(self)->data[(row) * (self)->columnCount])
    
#pragma mark Print
    
    void Matrix_print(const Matrix *const self);
    void Matrix_printRow(const Matrix *const self, size_t rowNumber);
    
    void Matrix_printRange(const Matrix *const self,
                           size_t startRow,
                           size_t rowCount,
                           size_t startColumn,
                           size_t columnCount);
    
#pragma mark Submatrix
    
    Matrix *Matrix_submatrixView(Matrix *self,
                                 size_t startRow,
                                 size_t rowCount,
                                 size_t startColumn,
                                 size_t columnCount);
    
    void Matrix_subMatrixCopy(Matrix *source,
                              size_t sourceRowOffset,
                              size_t sourceColumnOffset,
                              size_t rowCount,
                              size_t columnCount,
                              Matrix *destination,
                              size_t destinationRowOffset,
                              size_t destinationColumnOffset);
    
#pragma mark Serialise
    
    void Matrix_saveAsHDF(Matrix *self, const char *path, const char *dataSet);
    void Matrix_addDataSetToHDF(Matrix *self, const char *path, const char *dataSet);
    
#pragma mark - Arithmetic -
    
#pragma mark Addition
    
    void Matrix_add(Matrix *inputA,
                    Matrix *inputB,
                    Matrix *result);
    
#pragma mark Subtraction
    
    void Matrix_subtract(Matrix *inputA,
                         Matrix *inputB,
                         Matrix *result);
    
    void Matrix_negate(Matrix *input, Matrix *output);
    
#pragma mark Multiplication
    
    void Matrix_multiply(Matrix *inputA,
                         Boolean transposeA,
                         Matrix *inputB,
                         Boolean transposeB,
                         Matrix *result);
    
    void Matrix_elementWiseMultiply(Matrix *inputA,
                                    Matrix *inputB,
                                    Matrix *result);
    
    void Matrix_scalarMultiply(Matrix *input,
                               Float64 scalar,
                               Matrix *result);
    
#pragma mark Division
    
    void Matrix_scalarDivide(Matrix *input,
                             Float64 scalar,
                             Matrix *result);
    
    void Matrix_elementWiseDivide(Matrix *inputA,
                                  Matrix *inputB,
                                  Matrix *result);
    
#pragma mark Factorisation
    
    void Matrix_choleskyFactorisation(Matrix *input);
    
#pragma mark Trigonometric
    
    void Matrix_cosine(Matrix *input, Matrix *output);
    void Matrix_exponent(Matrix *input, Matrix *output);
    
#pragma mark Other
    
    void Matrix_setElementsToScalar(Matrix *self, Float64 scalar);
    void Matrix_normalise(Matrix *input, Matrix *output);
    void Matrix_reverse(Matrix *input);
    void Matrix_clear(const Matrix *const input);
    void Matrix_identity(Matrix *input);
    void Matrix_transpose(Matrix *input, Matrix *output);
    void Matrix_squareRoot(Matrix *input, Matrix *output);
    

#ifdef __cplusplus
}
#endif
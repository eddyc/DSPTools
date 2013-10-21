//
//  Matrix.h
//  Matrix
//
//  Created by Edward Costello on 02/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class Matrix
     *
     * @abstract Object for working with matrices and vectors
     *
     * @param rowCount    The number of rows in the matrix
     * @param columnCount The number of columns in the matrix
     * @param elementCount The number of elements in the matrix, rowCount * columnCount
     * @param data The data store buffer
     * @param temp Temporary storage for certain operations that require it
     * @param submatrixView A temporary matrix view used for nested functions
     */
    typedef struct _Matrix
    {
        size_t rowCount;
        size_t columnCount;
        size_t elementCount;
        Float64 *data;
        Float64 *temp;
        struct Matrix *submatrixView;
    } Matrix;
    
    /**
     *  Contruct a Matrix object
     *
     *  @param rowCount    The number of rows in the matrix
     *  @param columnCount The number of columns in the matrix
     *
     *  @return A pointer to a Matrix object
     */
    OVERLOADED Matrix *const Matrix_new(size_t rowCount, size_t columnCount);
    
    /**
     *  Contruct a Matrix object
     *
     *  @param original Original matrix instance to copy
     *
     *  @return A pointer to a Matrix object
     */
    OVERLOADED Matrix *const Matrix_new(Matrix *original);
    
    /**
     *  Construct a Matrix object from a HDF5 file
     *
     *  @param path    Path to HDF5 object
     *  @param dataSet The data set within the HDF5 object to use
     *
     *  @return A pointer to a Matrix object
     */
    Matrix *Matrix_newFromHDF(const char *path, const char *dataSet);
    
    /**
     *  Definition used to construct a scoped Matrix pseudoclass
     */
#define _Matrix __attribute__((unused)) __attribute__((cleanup(Matrix_scopedDelete))) Matrix
    
    /**
     *  Definition used to get an element from a Matrix pseudoclass
     *  @abstract Usage: Matrix_getRow(matrix,row)[column]
     *  @param self A pointer to a Matrix
     *  @param row  The specified row
     *
     *  @return The value at the specified row and column in the matrix
     */
#define Matrix_getRow(self,row) (&(self)->data[(row) * (self)->columnCount])
    /**
     *  Scoped deletion of a Matrix object
     *
     *  @param self Pointer to Matrix object
     */
    void Matrix_scopedDelete(Matrix **self);
    
    /**
     *  Delete a Matrix object
     *
     *  @param self Pointer to Matrix object
     */
    void Matrix_delete(Matrix *self);
    
    /**
     *  Construct a Matrix using a Float64 array
     *
     *  @param rowCount    The number of rows in the matrix
     *  @param columnCount The number of columns in the matrix
     *  @param array       The array of data to use
     *
     *  @return A pointer to a Matrix object
     */
    Matrix *Matrix_newWithArray(size_t rowCount,
                                size_t columnCount,
                                Float64 *array);
    
    /**
     *  Construct a Matrix using specified Float64 values
     *
     *  @param rowCount    The number of rows in the matrix
     *  @param columnCount The number of columns in the matrix
     *  @param valuesCount The amount of specified Float64 values
     *  @param ...         A variable parameter list of Float64 values
     *
     *  @return A pointer to a Matrix object
     */
    Matrix *Matrix_newWithValues(size_t rowCount,
                                 size_t columnCount,
                                 size_t valuesCount,
                                 ...);
    
    /**
     *  Construct an identity matrix
     *
     *  @param dimensionSize The size of the dimensions of the matrix
     *
     *  @return A pointer to a Matrix object
     */
    Matrix *Matrix_newIdentity(size_t dimensionSize);
    
#pragma mark Print
    
    /**
     *  Print a Matrix pseudoclass
     *
     *  @param self A pointer to a Matrix pseudoclass
     */
    void Matrix_print(const Matrix *const self);
    
    /**
     *  Print a specified row from a Matrix pseudoclass
     *
     *  @param self A pointer to a Matrix pseudoclass
     *  @param rowNumber The specified row number
     */
    void Matrix_printRow(const Matrix *const self, size_t rowNumber);
    
    /**
     *  Print a range of values from a Matrix pseudoclass
     *
     *  @param self A pointer to a Matrix pseudoclass
     *  @param startRow    The specified starting row
     *  @param rowCount    The specified amount of rows
     *  @param startColumn The specified starting column
     *  @param columnCount The specified amount of columns
     */
    void Matrix_printRange(const Matrix *const self,
                           size_t startRow,
                           size_t rowCount,
                           size_t startColumn,
                           size_t columnCount);
    
#pragma mark Submatrix
    
    /**
     *  Get a submatrixView of a Matrix pseudoclass
     *
     *  @param self A pointer to a Matrix pseudoclass
     *  @param startRow    The specified starting row
     *  @param rowCount    The specified amount of rows
     *  @param startColumn The specified starting column
     *  @param columnCount The specified amount of columns
     *
     *  @return A pointer to a submatrixView
     */
    Matrix *Matrix_submatrixView(Matrix *self,
                                 size_t startRow,
                                 size_t rowCount,
                                 size_t startColumn,
                                 size_t columnCount);
    
    
    /**
     *  Copy a submatrix from one Matrix pseudoclass to another
     *
     *  @param source                  The source matrix
     *  @param sourceRowOffset         The source row offset
     *  @param sourceColumnOffset      The source column offset
     *  @param rowCount                The number of rows to copy
     *  @param columnCount             The number of columns to copy
     *  @param destination             The destination matrix
     *  @param destinationRowOffset    The destination row offset
     *  @param destinationColumnOffset The destination column offset
     */
    void Matrix_subMatrixCopy(Matrix *source,
                              size_t sourceRowOffset,
                              size_t sourceColumnOffset,
                              size_t rowCount,
                              size_t columnCount,
                              Matrix *destination,
                              size_t destinationRowOffset,
                              size_t destinationColumnOffset);
    
#pragma mark Serialise
    
    /**
     *  Save a Matrix pseudoclass as a HDF5 file
     *
     *  @param self    A pointer to a Matrix pseudoclass
     *  @param path    The path to where to save the HDF5
     *  @param dataSet The name of the dataset to save
     */
    void Matrix_saveAsHDF(Matrix *self,
                          const char *path,
                          const char *dataSet);
    
    /**
     *  Add a Matrix pseudoclass as a dataset and save in an existing HDF5 file
     *
     *  @param self    A pointer to a Matrix pseudoclass
     *  @param path    The path to the existing HDF5 file
     *  @param dataSet The name of the dataset to save
     */
    void Matrix_addDataSetToHDF(Matrix *self,
                                const char *path,
                                const char *dataSet);
    
#pragma mark - Arithmetic -
    
#pragma mark Addition
    
    /**
     *  Add two matrices
     *
     *  @param inputA Input matrix
     *  @param inputB Input matrix
     *  @param result Result matrix
     */
    void Matrix_add(Matrix *inputA,
                    Matrix *inputB,
                    Matrix *result);
    
#pragma mark Subtraction
    
    /**
     *  Subtract two matrices
     *
     *  @param inputA Input matrix
     *  @param inputB Input matrix
     *  @param result Result matrix
     */
    void Matrix_subtract(Matrix *inputA,
                         Matrix *inputB,
                         Matrix *result);
    
    /**
     *  Negate a matrix
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_negate(Matrix *input, Matrix *output);
    
#pragma mark Multiplication
    
    /**
     *  Multiply two matrices
     *
     *  @param inputA     Input matrix
     *  @param transposeA Boolean for transposing the matrix
     *  @param inputB     Input matrix
     *  @param transposeB Boolean for transposing the matrix
     *  @param result     The result matrix
     */
    void Matrix_multiply(Matrix *inputA,
                         Boolean transposeA,
                         Matrix *inputB,
                         Boolean transposeB,
                         Matrix *result);
    
    /**
     *  Element wise multiplication of two matrices
     *
     *  @param inputA Input Matrix
     *  @param inputB Input Matrix
     *  @param result Result Matrix
     */
    void Matrix_elementWiseMultiply(Matrix *inputA,
                                    Matrix *inputB,
                                    Matrix *result);
    
    /**
     *  Scalar multiply of a matrix
     *
     *  @param input  Input matrix
     *  @param scalar Input scalar
     *  @param result Result matrix
     */
    void Matrix_scalarMultiply(Matrix *input,
                               Float64 scalar,
                               Matrix *result);
    
#pragma mark Division
    
    /**
     *  Element wise division of two matrices
     *
     *  @param inputA Input Matrix
     *  @param inputB Input Matrix
     *  @param result Result Matrix
     */
    void Matrix_elementWiseDivide(Matrix *inputA,
                                  Matrix *inputB,
                                  Matrix *result);
    
    /**
     *  Scalar division of a matrix
     *
     *  @param input  Input matrix
     *  @param scalar Input scalar
     *  @param result Result matrix
     */
    void Matrix_scalarDivide(Matrix *input,
                             Float64 scalar,
                             Matrix *result);
    
#pragma mark Factorisation
    
    /**
     *  Cholesky factorisation of a matrix
     *
     *  @param input Input matrix
     */
    void Matrix_choleskyFactorisation(Matrix *input);
    
#pragma mark Trigonometric
    
    /**
     *  Cosine of each element in a matrix
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_cosine(Matrix *input, Matrix *output);
    
    /**
     *  Exponent of each element in a matrix
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_exponent(Matrix *input, Matrix *output);
    
#pragma mark Other
    
    /**
     *  Set each matrix element to a specified scalar value
     *
     *  @param self   Input matrix
     *  @param scalar Specified scalar
     */
    void Matrix_setElementsToScalar(Matrix *self, Float64 scalar);
    
    /**
     *  Normalise the values of a matrix to between 1 and -1
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_normalise(Matrix *input, Matrix *output);
    
    /**
     *  Reverse the elements in a matrix
     *
     *  @param input Input matrix
     */
    void Matrix_reverse(Matrix *input);
    
    /**
     *  Set each element in a matrix to zero
     *
     *  @param input Input matrix
     */
    void Matrix_clear(const Matrix *const input);
    
    /**
     *  Create an identity matrix using existing matrix
     *
     *  @param input Input matrix
     */
    void Matrix_identity(Matrix *input);
    
    /**
     *  Transpose the elements in a matrix
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_transpose(Matrix *input, Matrix *output);
    
    /**
     *  Set each element in a matrix to its square root value
     *
     *  @param input  Input matrix
     *  @param output Output matrix
     */
    void Matrix_squareRoot(Matrix *input, Matrix *output);
    
    
#ifdef __cplusplus
}
#endif
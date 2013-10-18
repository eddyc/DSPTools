//
//  Matrix.c
//  Matrix
//
//  Created by Edward Costello on 02/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "Matrix.h"
#import <stdarg.h>
#import <string.h>
#import "hdf5.h"

static Matrix *Matrix_allocate(size_t rowCount, size_t columnCount)
{
    Matrix *self = calloc(1, sizeof(Matrix));
    self->rowCount = rowCount;
    self->columnCount = columnCount;
    self->elementCount = self->rowCount * self->columnCount;
    self->data = calloc(self->elementCount, sizeof(Float64));
    self->temp = calloc(self->elementCount, sizeof(Float64));
    
    return self;
}

OVERLOADED Matrix *const Matrix_new(size_t rowCount, size_t columnCount)
{
    Matrix *self = Matrix_allocate(rowCount, columnCount);
    self->submatrixView = Matrix_allocate(rowCount, columnCount);
    
    return self;
}

OVERLOADED Matrix *const Matrix_new(Matrix *original)
{
    Matrix *const copy = Matrix_new(original->rowCount,
                                    original->columnCount);
    cblas_dcopy((UInt32)original->elementCount,
                original->data, 1, copy->data, 1);
    return copy;
}



Matrix *Matrix_newFromHDF(const char *path, const char *dataSet)
{
    
    if(strncmp(path, "desktop", 7) == 0
       ||
       strncmp(path, "Desktop", 7) == 0) {
        
        char newPath[200];
        sprintf(newPath, "/Users/eddyc/Desktop%s.hdf", dataSet);
        path = newPath;
    }
    
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2];
    
    
    herr_t status;
    
    fileID = H5Fopen(path,
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    
    
    dataSetID = H5Dopen2(fileID, dataSet, H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    
    Matrix *self = Matrix_new((size_t)dimensions[0], (size_t)dimensions[1]);
    
    status = H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, self->data);
    
    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);
    
    return self;
}



void Matrix_deallocate(Matrix *self)
{
    free(self->data);
    free(self->temp);
    free(self);
    self = NULL;
}

void Matrix_delete(Matrix *self)
{
    Matrix_deallocate(self->submatrixView);
    Matrix_deallocate(self);
}

void Matrix_scopedDelete(Matrix **self)
{
    Matrix_delete(*self);
    printf("deleted\n");
}


Matrix *Matrix_newWithValues(size_t rowCount,
                             size_t columnCount,
                             size_t valuesCount,
                             ...)

{
    Matrix *self = Matrix_new(rowCount, columnCount);
    va_list argumentList;
    
    va_start (argumentList, valuesCount);
    
    for (size_t i = 0; i < valuesCount; ++i) {
        
        self->data[i] = va_arg(argumentList, double);
    }
    
    va_end (argumentList);
    
    return self;
}

Matrix *Matrix_newWithArray(size_t rowCount, size_t columnCount, Float64 *array)
{
    Matrix *self = Matrix_new(rowCount, columnCount);
    cblas_dcopy((UInt32)self->elementCount, array, 1, self->data, 1);
    return self;
}

Matrix *Matrix_newIdentity(size_t dimensionSize)
{
    Matrix *self = Matrix_new(dimensionSize, dimensionSize);
    
    for (size_t i = 0; i < dimensionSize; ++i) {
        
        Matrix_getRow(self, i)[i] = 1.;
    }
    
    return self;
}



void Matrix_print(const Matrix *const self)
{
    printf("Matrix_print:\n\nrows = %zd, columns = %zd\n\n", self->rowCount, self->columnCount);
    
    for (size_t i = 0; i < self->rowCount; ++i) {
        
        for (size_t j = 0; j < self->columnCount; ++j) {
            
            printf("[%g]\t", self->data[i * self->columnCount + j]);
        }
        printf("\n");
    }
    
    printf("\n\nEnd\n\n");
}

void Matrix_printRow(const Matrix *const self, size_t rowNumber)
{
    printf("Matrix_printRow:\n\nrows = %zd, columns = %zd\n\n", self->rowCount, self->columnCount);
    
    for (size_t i = 0; i < self->columnCount; ++i) {
        
        printf("[%g]\t", self->data[rowNumber * self->columnCount + i]);
    }
    printf("\n");
    
    printf("\n\nEnd\n\n");
}

void Matrix_printRange(const Matrix *const self,
                       size_t startRow,
                       size_t rowCount,
                       size_t startColumn,
                       size_t columnCount)
{
    if (startRow >= self->rowCount
        ||
        startColumn >= self->columnCount
        ||
        startRow + rowCount > self->rowCount
        ||
        startColumn + columnCount > self->columnCount) {
        
        printf("Matrix_printRange: Specified range illegal for matrix\nExiting\n");
        exit(-1);
    }
    
    printf("Matrix_printRange:\n\nrows = %zd, columns = %zd\n\nStart Row = %zd, Row Count = %zd, Start Column = %zd, Column Count = %zd\n", self->rowCount, self->columnCount, startRow, rowCount, startColumn, columnCount);
    
    for (size_t i = startRow; i < rowCount; ++i) {
        
        for (size_t j = startColumn; j < columnCount; ++j) {
            
            printf("[%g]\t", self->data[i * self->columnCount + j]);
        }
        printf("\n");
    }
    
    printf("\n\nEnd\n\n");
}

#pragma mark Serialise

void Matrix_saveAsHDF(Matrix *self, const char *path, const char *dataSet)
{
    
    if(strncmp(path, "desktop", 7) == 0
       ||
       strncmp(path, "Desktop", 7) == 0) {
        
        char newPath[200];
        sprintf(newPath, "/Users/eddyc/Desktop%s.hdf", dataSet);
        path = newPath;
    }
    
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2];
    herr_t status;
    
    fileID = H5Fcreate(path,
                       H5F_ACC_TRUNC,
                       H5P_DEFAULT,
                       H5P_DEFAULT);
    
    dimensions[0] = self->rowCount;
    dimensions[1] = self->columnCount;
    
    dataSpaceID = H5Screate_simple(2, dimensions, NULL);
    
    
    dataSetID = H5Dcreate(fileID,
                          dataSet,
                          H5T_NATIVE_DOUBLE,
                          dataSpaceID,
                          H5P_DEFAULT,
                          H5P_DEFAULT,
                          H5P_DEFAULT);
    
    H5Dwrite (dataSetID,
              H5T_NATIVE_DOUBLE,
              H5S_ALL,
              H5S_ALL,
              H5P_DEFAULT,
              self->data);
    
    status = H5Dclose(dataSetID);
    status = H5Sclose(dataSpaceID);
    status = H5Fclose(fileID);
    
}

void Matrix_addDataSetToHDF(Matrix *self, const char *path, const char *dataSet)
{
    
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2];
    herr_t status;
    
    fileID = H5Fopen(path,
                     H5F_ACC_RDWR,
                     H5P_DEFAULT);
    
    dimensions[0] = self->rowCount;
    dimensions[1] = self->columnCount;
    
    dataSpaceID = H5Screate_simple(2, dimensions, NULL);
    
    
    dataSetID = H5Dcreate(fileID,
                          dataSet,
                          H5T_NATIVE_DOUBLE,
                          dataSpaceID,
                          H5P_DEFAULT,
                          H5P_DEFAULT,
                          H5P_DEFAULT);
    
    H5Dwrite (dataSetID,
              H5T_NATIVE_DOUBLE,
              H5S_ALL,
              H5S_ALL,
              H5P_DEFAULT,
              self->data);
    
    status = H5Dclose(dataSetID);
    status = H5Sclose(dataSpaceID);
    status = H5Fclose(fileID);
    
}


#pragma mark Submatrix

Matrix *Matrix_submatrixView(Matrix *self,
                             size_t startRow,
                             size_t rowCount,
                             size_t startColumn,
                             size_t columnCount)
{
    
    if (startRow >= self->rowCount
        ||
        startColumn >= self->columnCount
        ||
        startRow + rowCount > self->rowCount
        ||
        startColumn + columnCount > self->columnCount) {
        
        printf("Matrix_submatrixView: Specified range illegal\nExiting\n");
        exit(-1);
    }
    
    Matrix *submatrixView = self->submatrixView;
    
    submatrixView->rowCount = rowCount;
    submatrixView->columnCount = columnCount;
    submatrixView->elementCount = submatrixView->rowCount * submatrixView->columnCount;
    
    vDSP_mmovD(&Matrix_getRow(self, startRow)[startColumn], submatrixView->data, submatrixView->columnCount, submatrixView->rowCount, self->columnCount, submatrixView->columnCount);
    
    return submatrixView;
}

void Matrix_subMatrixCopy(Matrix *source,
                          size_t sourceStartRow,
                          size_t sourceStartColumn,
                          size_t rowCount,
                          size_t columnCount,
                          Matrix *destination,
                          size_t destinationStartRow,
                          size_t destinationStartColumn)
{
    
    if (sourceStartRow >= source->rowCount
        ||
        sourceStartColumn >= source->columnCount
        ||
        sourceStartRow + rowCount > source->rowCount
        ||
        sourceStartColumn + columnCount > source->columnCount
        ||
        destinationStartRow >= destination->rowCount
        ||
        destinationStartColumn >= destination->columnCount
        ||
        destinationStartRow + rowCount > destination->rowCount
        ||
        destinationStartColumn + columnCount > destination->columnCount) {
        
        printf("Matrix_subMatrixCopy: Specified range illegal\nExiting\n");
        exit(-1);
    }
    
    vDSP_mmovD(&source->data[sourceStartColumn + (source->columnCount * sourceStartRow)],
               &destination->data[destinationStartColumn + (destination->columnCount * destinationStartRow)],
               columnCount,
               rowCount,
               source->columnCount,
               destination->columnCount);
}

#pragma mark - Arithmetic -
#pragma mark Addition

void Matrix_add(Matrix *inputA,
                Matrix *inputB,
                Matrix *result)
{
    vDSP_vaddD(inputA->data, 1, inputB->data, 1, result->data, 1, inputA->elementCount);
}

#pragma mark Subtraction

void Matrix_subtract(Matrix *inputA,
                     Matrix *inputB,
                     Matrix *result)
{
    vDSP_vsubD(inputA->data, 1, inputB->data, 1, result->data, 1, inputA->elementCount);
}

void Matrix_negate(Matrix *input, Matrix *output)
{
    vDSP_vnegD(input->data, 1, output->data, 1, input->elementCount);
}

#pragma mark Multiplication

void Matrix_multiply(Matrix *inputA,
                     Boolean transposeA,
                     Matrix *inputB,
                     Boolean transposeB,
                     Matrix *result)
{
    size_t ARowCount;
    size_t AColumnCount;
    size_t BRowCount;
    size_t BColumnCount;
    
    if (transposeA == false) {
        
        ARowCount = inputA->rowCount;
        AColumnCount = inputA->columnCount;
    }
    else {
        
        ARowCount = inputA->columnCount;
        AColumnCount = inputA->rowCount;
    }
    
    if (transposeB == false) {
        
        BRowCount = inputB->rowCount;
        BColumnCount = inputB->columnCount;
    }
    else {
        
        BRowCount = inputB->columnCount;
        BColumnCount = inputB->rowCount;
    }
    
    if (AColumnCount != BRowCount
        ||
        ARowCount != result->rowCount
        ||
        BColumnCount != result->columnCount
        ) {
        
        printf("Matrix dimensions incompatible for multiplication\nExiting\n");
        exit(-1);
    }
    
    if (transposeA == false && transposeB == false) {
        
        cblas_dgemm(CblasRowMajor,
                    CblasNoTrans,
                    CblasNoTrans,
                    (UInt32)ARowCount,
                    (UInt32)BColumnCount,
                    (UInt32)AColumnCount,
                    1.0,
                    inputA->data,
                    (UInt32)AColumnCount,
                    inputB->data,
                    (UInt32)BColumnCount,
                    0.,
                    result->data,
                    (UInt32)result->columnCount);
    }
    else if (transposeA == true && transposeB == false) {
        
        cblas_dgemm(CblasRowMajor,
                    CblasTrans,
                    CblasNoTrans,
                    (UInt32)ARowCount,
                    (UInt32)BColumnCount,
                    (UInt32)AColumnCount,
                    1.0,
                    inputA->data,
                    (UInt32)ARowCount,
                    inputB->data,
                    (UInt32)BColumnCount,
                    0.,
                    result->data,
                    (UInt32)result->columnCount);
    }
    else if (transposeA == false && transposeB == true) {
        
        cblas_dgemm(CblasRowMajor,
                    CblasNoTrans,
                    CblasTrans,
                    (UInt32)ARowCount,
                    (UInt32)BColumnCount,
                    (UInt32)AColumnCount,
                    1.0,
                    inputA->data,
                    (UInt32)AColumnCount,
                    inputB->data,
                    (UInt32)BRowCount,
                    0.,
                    result->data,
                    (UInt32)result->columnCount);
    }
    else if (transposeA == true && transposeB == true) {
        
        cblas_dgemm(CblasRowMajor,
                    CblasTrans,
                    CblasTrans,
                    (UInt32)ARowCount,
                    (UInt32)BColumnCount,
                    (UInt32)AColumnCount,
                    1.0,
                    inputA->data,
                    (UInt32)ARowCount,
                    inputB->data,
                    (UInt32)BRowCount,
                    0.,
                    result->data,
                    (UInt32)result->columnCount);
    }
}


void Matrix_elementWiseMultiply(Matrix *inputA,
                                Matrix *inputB,
                                Matrix *result)
{
    vDSP_vmulD(inputA->data, 1, inputB->data, 1, result->data, 1, result->elementCount);
}


void Matrix_scalarMultiply(Matrix *input,
                           Float64 scalar,
                           Matrix *result)
{
    vDSP_vsmulD(input->data, 1, &scalar, result->data, 1, input->elementCount);
}

#pragma mark Division

void Matrix_scalarDivide(Matrix *input,
                         Float64 scalar,
                         Matrix *result)
{
    vDSP_vsdivD(input->data, 1, &scalar, result->data, 1, input->elementCount);
}

void Matrix_elementWiseDivide(Matrix *inputA,
                              Matrix *inputB,
                              Matrix *result)
{
    vDSP_vdivD(inputA->data, 1, inputB->data, 1, result->data, 1, result->elementCount);
}

#pragma mark Factorisation

void Matrix_choleskyFactorisation(Matrix *input)
{
    for (size_t i = 0; i < input->rowCount; ++i) {
        
        Matrix_getRow(input, i)[i] = sqrt(Matrix_getRow(input, i)[i]);
        vDSP_vclrD(&Matrix_getRow(input, i)[i + 1], 1, input->rowCount - i - 1);
        vDSP_vsdivD(&Matrix_getRow(input, i + 1)[i], input->columnCount, &Matrix_getRow(input, i)[i], &Matrix_getRow(input, i + 1)[i], input->columnCount, input->rowCount - i - 1);
        
        for (size_t j = i + 1; j < input->rowCount; ++j) {
            
            for (size_t k = 0; k < input->rowCount - i - 1; ++k) {
                
                Float64 scalar = Matrix_getRow(input, j)[i];
                scalar *= Matrix_getRow(input, i + 1)[i + k * input->columnCount];
                Matrix_getRow(input, i + 1)[j + k * input->columnCount] -= scalar;
            }
        }
    }
}

#pragma mark Trigonometric

void Matrix_cosine(Matrix *input, Matrix *output)
{
    int elementCount = (int)input->elementCount;
    vvcos(input->data, output->data, &elementCount);
}

void Matrix_exponent(Matrix *input, Matrix *output)
{
    int elementCount = (int)input->elementCount;
    vvexp(input->data, output->data, &elementCount);
}


#pragma mark Other

void Matrix_normalise(Matrix *input, Matrix *output)
{
    Float64 max;
    
    vDSP_maxmgvD(input->data, 1, &max, input->elementCount);
    
    vDSP_vsdivD(input->data, 1, &max, output->data, 1, input->elementCount);
}

void Matrix_setElementsToScalar(Matrix *input, Float64 scalar)
{
    vDSP_vfillD(&scalar, input->data, 1, input->elementCount);
}

void Matrix_reverse(Matrix *input)
{
    vDSP_vrvrsD(input->data, 1, input->elementCount);
}

void Matrix_clear(const Matrix *const input)
{
    vDSP_vclrD(input->data, 1, input->elementCount);
}

void Matrix_identity(Matrix *input)
{
    if (input->rowCount != input->columnCount) {
        
        printf("Matrix_identity, Matrix not square exiting\n");
        exit(-1);
    }
    
    Matrix_clear(input);
    for (size_t i = 0; i < input->columnCount; ++i) {
        
        Matrix_getRow(input, i)[i] = 1.;
    }
}

void Matrix_transpose(Matrix *input, Matrix *output)
{
    vDSP_mtransD(input->data, 1, output->data, 1, input->columnCount, input->rowCount);
    
    size_t rowCount = input->rowCount;
    size_t columnCount = input->columnCount;
    output->rowCount= columnCount;
    output->columnCount = rowCount;
}

void Matrix_squareRoot(Matrix *input, Matrix *output)
{
    const int elementCount = (int)input->elementCount;
    vvsqrt(input->data, output->data, &elementCount);
}
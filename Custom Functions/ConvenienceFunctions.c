//
//  ConvenienceFunctions.c
//  Phase Vocoder
//
//  Created by Edward Costello on 05/12/2012.
//  Copyright (c) 2012 Edward Costello. All rights reserved.
//

#import "ConvenienceFunctions.h"
#import "hdf5.h"
#import <stdio.h>
#import <stdlib.h>
#import <string.h>

Float32 *Float32_newArray(size_t size)
{
    Float32 *newArray = calloc(size, sizeof(Float32));
    
    return newArray;
}

void Float32_printArray(Float32 *array, size_t size, char *name)
{
    printf("Float32_printArray:\n\nSize = %zd\n\n", size);
    
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %.10g\n", name, i, array[i]);
    }
    
    printf("\n\nEnd\n\n");
}

void Float32_printContiguous2DArray(Float32 *array, size_t rows, size_t columns, char *name)
{
    printf("Float32_printArray:%s\n\nrows = %zd, columns = %zd\n\n", name, rows, columns);
    
    for (size_t i = 0; i < rows; ++i) {
        
        for (size_t j = 0; j < columns; ++j) {
            
            printf("[%g]\t", array[i * columns + j]);
        }
        
        printf("\n");

    }
    
    printf("\n\nEnd\n\n");
}

void SInt32_printArray(SInt32 *array, size_t size, char *name)
{
    printf("SInt32_printArray:\n\nSize = %zd\n\n", size);
    
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %d\n", name, i, array[i]);
    }
    
    printf("\n\nEnd\n\n");
}

void UInt32_printArray(UInt32 *array, size_t size, char *name)
{
    printf("UInt32_printArray:\n\nSize = %zd\n\n", size);
    
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %d\n", name, i, array[i]);
    }
    
    printf("\n\nEnd\n\n");
}

void size_t_printArray(size_t *array, size_t size, char *name)
{
    printf("size_t_printArray:\n\nSize = %zd\n\n", size);
    
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %zd\n", name, i, array[i]);
    }
    
    printf("\n\nEnd\n\n");
}

Float64 *Float64_newArray(size_t size)
{
    Float64 *newArray = calloc(size, sizeof(Float64));
    
    return newArray;
}

void Float64_printArray(const Float64 *const array, size_t size, char *name)
{
    printf("Float64_printArray:\n\nSize = %zd\n", size);
    
    printf("%s:", name);

    for (size_t i = 0; i < size; ++i) {
        
        printf("[%.10g]", array[i]);
    }
    
    printf("\n");
}


BOOL Float32_arraysAreEqual(Float32 *a, Float32 *b, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        
        if (a[i] != b[i]) {
            
            printf("at index %zd\n", i);
            
            return NO;
        }
    }
    
    return YES;
}


Float32 **Float32_new2DArray(size_t rowCount, size_t columnCount)
{
    Float32 **array = calloc(rowCount, sizeof(Float32 *));
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        array[i] = calloc(columnCount, sizeof(Float32));
    }
    
    return array;
}

void Float32_delete2DArray(Float32 **array, size_t rowSize)
{
    for (size_t i = 0; i < rowSize; ++i) {
        
        free(array[i]);
    }
    
    free(array);
}

Float32 **Float32_newContiguous2DArray(size_t rowSize, size_t columnSize)
{
    Float32 **rows = calloc(rowSize, sizeof(Float32 *));
    Float32 *memory = calloc(rowSize * columnSize, sizeof(Float32));
    for (size_t i = 0; i < rowSize; ++i) {
        
        rows[i] = &memory[i * columnSize];
    }
    
    return rows;
}

void Float32_deleteContiguous2DArray(Float32 **array)
{
    free(array[0]);
    free(array);
}

Float64 **Float64_new2DArray(size_t rowSize, size_t columnSize)
{
    Float64 **newArray = calloc(rowSize, sizeof(Float64 *));
    
    for (size_t i = 0; i < rowSize; ++i) {
        
        newArray[i] = Float64_newArray(columnSize);
    }
    
    return newArray;
}


void Float64_delete2DArray(Float64 **array, size_t rowSize)
{
    for (size_t i = 0; i < rowSize; ++i) {
        
        free(array[i]);
    }
    
    free(array);
}


void Float32_print2DArray(Float32 **array, size_t rowSize, size_t columnSize, char *name)
{
    printf("Float32_print2DArray:\n\nSize = %zd, %zd\n\n", rowSize, columnSize);
    
    
    for (size_t i = 0; i < columnSize; ++i) {
        
        printf("%s", name);
        
        for (size_t j = 0; j < rowSize; ++j) {
            
            printf("[%.0f]\t",array[j][i]);
        }
        
        printf("\n");
    }
    
    printf("END\n\n");
}

void Float32_arrayToFile(Float32 *array, size_t count, char *filePath)
{
    FILE *fileHandle = fopen(filePath, "w");
    
    for (size_t i = 0; i < count; ++i) {
        
        fprintf(fileHandle, "%f,", array[i]);
    }
    
    fclose(fileHandle);
}


void Float32_2DarrayToFile(Float32 **array, size_t rows, size_t columns, char *filePath)
{
    FILE *fileHandle = fopen(filePath, "w");
    
    for (size_t i = 0; i < rows; ++i) {
        
        for (size_t j = 0; j < columns; ++j) {
            
            fprintf(fileHandle, "%f,", array[i][j]);
        }
        
        fprintf(fileHandle, "\n");
    }
    
    fclose(fileHandle);
}


BOOL *BOOL_newArray(size_t size)
{
    BOOL *newArray = calloc(size, sizeof(BOOL));
    
    return newArray;
}

BOOL **BOOL_new2DArray(size_t rowSize, size_t columnSize)
{
    BOOL **newArray = calloc(rowSize, sizeof(BOOL *));
    
    for (size_t i = 0; i < rowSize; ++i) {
        
        newArray[i] = BOOL_newArray(columnSize);
    }
    
    return newArray;
}

void BOOL_delete2DArray(BOOL **array, size_t rowSize)
{
    for (size_t i = 0; i < rowSize; ++i) {
        
        free(array[i]);
    }
    
    free(array);
}

DSPSplitComplex *DSPSplitComplex_new2DArray(size_t rowCount, size_t columnCount)
{
    DSPSplitComplex *self = calloc(rowCount, sizeof(DSPSplitComplex));
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        self[i].realp = calloc(columnCount, sizeof(Float32));
        self[i].imagp = calloc(columnCount, sizeof(Float32));
    }
    
    return self;
}

DSPDoubleSplitComplex *DSPDoubleSplitComplex_new2DArray(size_t rowCount, size_t columnCount)
{
    DSPDoubleSplitComplex *self = calloc(rowCount, sizeof(DSPDoubleSplitComplex));
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        self[i].realp = calloc(columnCount, sizeof(Float64));
        self[i].imagp = calloc(columnCount, sizeof(Float64));
    }
    
    return self;
}

void DSPSplitComplex_delete2DArray(DSPSplitComplex *self, size_t rowCount)
{
    for (size_t i = 0; i < rowCount; ++i) {
        
        free(self[i].realp);
        free(self[i].imagp);
    }
    
    free(self);
}

void DSPDoubleSplitComplex_delete2DArray(DSPDoubleSplitComplex *self, size_t rowCount)
{
    for (size_t i = 0; i < rowCount; ++i) {
        
        free(self[i].realp);
        free(self[i].imagp);
    }
    
    free(self);
}


void DSPSplitComplex_printArray(DSPSplitComplex array, size_t size, char *name)
{
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %f + %fi\n", name, i, array.realp[i], array.imagp[i]);
    }
}

void DSPDoubleSplitComplex_printArray(DSPDoubleSplitComplex array, size_t size, char *name)
{
    for (size_t i = 0; i < size; ++i) {
        
        printf("%s[%zd] = %f + %fi\n", name, i, array.realp[i], array.imagp[i]);
    }
}

void Float32_2DArrayToHDF(Float32 **array, size_t rowCount, size_t columnCount, char *path, char *dataSet)
{
    
    
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[2];
    herr_t      status;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = rowCount;
    dimensions[1] = columnCount;
    
    dataspace_id = H5Screate_simple(2, dimensions, NULL);
    
    Float32 *data = calloc(dimensions[0] * dimensions[1], sizeof(Float32));
    
    for (size_t i = 0; i < dimensions[0]; ++i) {
        
        for (size_t j = 0; j < dimensions[1]; ++j) {
            
            data[i * dimensions[1] + j] = array[i][j];
        }
    }
    
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dataSet, H5T_NATIVE_FLOAT, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);
    
    free(data);
}

void Float32_contiguous2DArrayToHDF(Float32 *array, size_t rowCount, size_t columnCount, char *path, char *dataSet)
{
    
    
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[2];
    herr_t      status;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = rowCount;
    dimensions[1] = columnCount;
    
    dataspace_id = H5Screate_simple(2, dimensions, NULL);
    
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dataSet, H5T_NATIVE_FLOAT, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);    
}


void Float64_2DArrayToHDF(Float64 **array, size_t rowCount, size_t columnCount, char *path, char *dataSet)
{
    
    
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[2];
    herr_t      status;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = rowCount;
    dimensions[1] = columnCount;
    
    dataspace_id = H5Screate_simple(2, dimensions, NULL);
    
    Float64 *data = calloc(dimensions[0] * dimensions[1], sizeof(Float64));
    
    for (size_t i = 0; i < dimensions[0]; ++i) {
        
        for (size_t j = 0; j < dimensions[1]; ++j) {
            
            data[i * dimensions[1] + j] = array[i][j];
        }
    }
    
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dataSet, H5T_NATIVE_DOUBLE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);
    
    free(data);
}


void DSPDoubleSplitComplex_2DArrayToHDF(DSPDoubleSplitComplex *array, size_t rowCount, size_t columnCount, char *path, char *dataSet)
{
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[2];
    herr_t      status;
    
    char *realSet = "real";
    char *imagSet = "imag";
    
    char *realSetPath = malloc(snprintf(NULL, 0, "%s%s", path, realSet) + 1);
    sprintf(realSetPath, "%s%s", dataSet, realSet);
    
    char *imagSetPath = malloc(snprintf(NULL, 0, "%s%s", path, imagSet) + 1);
    sprintf(imagSetPath, "%s%s", dataSet, imagSet);
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = rowCount;
    dimensions[1] = columnCount;
    
    dataspace_id = H5Screate_simple(2, dimensions, NULL);
    
    Float64 *realData = calloc(dimensions[0] * dimensions[1], sizeof(Float64));
    Float64 *imagData = calloc(dimensions[0] * dimensions[1], sizeof(Float64));
    
    for (size_t i = 0; i < dimensions[0]; ++i) {
        
        for (size_t j = 0; j < dimensions[1]; ++j) {
            
            realData[i * dimensions[1] + j] = array[i].realp[j];
            imagData[i * dimensions[1] + j] = array[i].imagp[j];
            
        }
    }
    
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, realSetPath, H5T_NATIVE_DOUBLE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, realData);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    dataset_id = H5Dcreate(file_id, imagSetPath, H5T_NATIVE_DOUBLE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, imagData);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);
    
    free(realData);
    free(imagData);
    
}


void Float32_arrayToHDF(Float32 *array, size_t size, char *path, char *dataSet)
{
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[1];
    herr_t      status;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = size;
    
    dataspace_id = H5Screate_simple(1, dimensions, NULL);
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dataSet, H5T_NATIVE_FLOAT_g, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_FLOAT_g, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);
}

void Float64_arrayToHDF(Float64 *array, size_t size, char *path, char *dataSet)
{
    hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
    hsize_t     dimensions[1];
    herr_t      status;
    
    /* Create a new file using default properties. */
    file_id = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    
    /* Create the data space for the dataset. */
    dimensions[0] = size;
    
    dataspace_id = H5Screate_simple(1, dimensions, NULL);
    
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dataSet, H5T_NATIVE_DOUBLE, dataspace_id,
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite (dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, array);
    
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    
    /* Terminate access to the data space. */
    status = H5Sclose(dataspace_id);
    
    /* Close the file. */
    status = H5Fclose(file_id);
}

void getFileNameFromPath(const char *filePath, char fileName[])
{
    size_t forwardSlashPosition = strlen(filePath);
    size_t dotPosition = forwardSlashPosition;
    while (dotPosition > 0) {
        
        if (filePath[dotPosition] == '.') {
            
            break;
        }
        dotPosition--;
    }
    while (forwardSlashPosition > 0) {
        
        if (filePath[forwardSlashPosition] == '/') {
            
            forwardSlashPosition++;
            break;
        }
        forwardSlashPosition--;
    }
    
    size_t i = 0;
    for (size_t j = forwardSlashPosition; j < dotPosition; ++i, ++j) {
        
        fileName[i] = filePath[j];
    }
    
    fileName[i] = '\0';
}

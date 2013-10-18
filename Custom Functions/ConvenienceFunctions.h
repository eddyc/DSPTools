//
//  ConvenienceFunctions.h
//  Phase Vocoder
//
//  Created by Edward Costello on 05/12/2012.
//  Copyright (c) 2012 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import <objc/objc.h>
#import <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    Float32 *Float32_newArray(size_t size);
    Float32 **Float32_new2DArray(size_t rowSize, size_t columnSize);
    void Float32_delete2DArray(Float32 **array, size_t rowSize);
    Float32 **Float32_newContiguous2DArray(size_t rowSize, size_t columnSize);
    void Float32_deleteContiguous2DArray(Float32 **array);
    void Float32_printContiguous2DArray(Float32 *array, size_t rows, size_t columns, char *name);


    void Float32_printArray(Float32 *array, size_t size, char *name);
    void Float32_print2DArray(Float32 **array, size_t rowSize, size_t columnSize, char *name);

    void Float32_arrayToFile(Float32 *array, size_t count, char *filePath);
    void Float32_2DarrayToFile(Float32 **array, size_t rows, size_t columns, char *filePath);
    
    void Float32_arrayToHDF(Float32 *array, size_t size, char *path, char *dataSet);
    void Float32_contiguous2DArrayToHDF(Float32 *array, size_t rowCount, size_t columnCount, char *path, char *dataSet);
    void Float32_2DArrayToHDF(Float32 **array, size_t rowCount, size_t columnCount, char *path, char *dataSet);

    Float64 *Float64_newArray(size_t size);
    Float64 **Float64_new2DArray(size_t rowSize, size_t columnSize);
    void Float64_delete2DArray(Float64 **array, size_t rowSize);

    void Float64_printArray(const Float64 *const array, size_t size, char *name);
    void Float64_arrayToHDF(Float64 *array, size_t size, char *path, char *dataSet);
    void Float64_2DArrayToHDF(Float64 **array, size_t rowCount, size_t columnCount, char *path, char *dataSet);

    BOOL *BOOL_newArray(size_t size);
    
    BOOL Float32_arraysAreEqual(Float32 *, Float32 *, size_t size);
    

    BOOL **BOOL_new2DArray(size_t rowSize, size_t columnSize);
    void BOOL_delete2DArray(BOOL **array, size_t rowSize);
    
    void DSPSplitComplex_printArray(DSPSplitComplex , size_t size, char *name);
    void DSPDoubleSplitComplex_printArray(DSPDoubleSplitComplex array, size_t size, char *name);

    DSPSplitComplex *DSPSplitComplex_new2DArray(size_t rowCount, size_t columnCount);
    void DSPSplitComplex_delete2DArray(DSPSplitComplex *self, size_t rowCount);

    DSPDoubleSplitComplex *DSPDoubleSplitComplex_new2DArray(size_t rowCount, size_t columnCount);
    void DSPDoubleSplitComplex_delete2DArray(DSPDoubleSplitComplex *, size_t rowCount);
    void DSPDoubleSplitComplex_2DArrayToHDF(DSPDoubleSplitComplex *array, size_t rowCount, size_t columnCount, char *path, char *dataSet);
    void SInt32_printArray(SInt32 *array, size_t size, char *name);
    void UInt32_printArray(UInt32 *array, size_t size, char *name);
    void size_t_printArray(size_t *array, size_t size, char *name);

    void MATLAB_2DArrayToMat();
    
    void getFileNameFromPath(const char *filePath, char fileName[]);

    
#ifdef __cplusplus
}
#endif
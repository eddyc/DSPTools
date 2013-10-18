//
//  MatrixCL.h
//  OpenCL
//
//  Created by Edward Costello on 06/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import "OpenCLObject.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct MatrixCLStruct
    {
        size_t rowCount;
        size_t columnCount;
        size_t elementCount;
        float *data;
        
    } MatrixCLStruct;
    
    typedef struct MatrixCL
    {
        cl_mem data;
        cl_mem structure;
    } MatrixCL;

    
    MatrixCL *MatrixCL_new(OpenCLObject *opencl, size_t rowCount, size_t columnCount);
    
    void MatrixCL_delete(MatrixCL *self);
    void MatrixCL_scopedDelete(MatrixCL **self);

#define _MatrixCL __attribute__((unused)) __attribute__((cleanup(MatrixCL_scopedDelete))) MatrixCL

    typedef struct MatrixArrayCL
    {
        MatrixCL **matrices;
        size_t matrixCount;
        cl_mem data;
    } MatrixArrayCL;
    
    MatrixArrayCL *MatrixArrayCL_new(OpenCLObject *opencl,
                                     size_t rowCount,
                                     size_t columnCount,
                                     size_t matrixCount);
    void MatrixArrayCL_delete(MatrixArrayCL *self);
    void MatrixArrayCL_scopedDelete(MatrixArrayCL **self);

#define _MatrixArrayCL __attribute__((unused)) __attribute__((cleanup(MatrixArrayCL_scopedDelete))) MatrixArrayCL

#ifdef __cplusplus
}
#endif
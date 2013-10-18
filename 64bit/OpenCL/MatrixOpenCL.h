//
//  MatrixOpenCL.h
//  AudioMosaicing
//
//  Created by Edward Costello on 09/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef struct MatrixGlobal {
    
    global double *data;
    size_t rowCount;
    size_t columnCount;
    size_t elementCount;
    
} MatrixGlobal;

MatrixGlobal MatrixGlobal_new(global double *data,
                              size_t rowCount,
                              size_t columnCount);

void MatrixGlobal_print(MatrixGlobal *self);

#define Matrix_getRow(self,row) (&(self)->data[(row) * (self)->columnCount])

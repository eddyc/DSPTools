//
//  MatrixCL.c
//  OpenCL
//
//  Created by Edward Costello on 06/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "MatrixCL.h"

MatrixCL *MatrixCL_new(OpenCLObject *opencl, size_t rowCount, size_t columnCount)
{
    MatrixCL *self = calloc(1, sizeof(MatrixCL));
    const char *constructorKernel = "MatrixCL_new";
    cl_kernel matrixKernel = OpenCLObject_createKernel(opencl, constructorKernel);

    size_t elementCount = rowCount * columnCount;
    self->data = OpenCLObject_allocateBuffer(opencl,
                                            elementCount * sizeof(float),
                                            CL_MEM_READ_WRITE);
    self->structure = OpenCLObject_allocateBuffer(opencl,
                                                 elementCount * sizeof(MatrixCL),
                                                 CL_MEM_READ_WRITE);
    
    OpenCLObject_setKernelArgument(opencl, matrixKernel, 0, sizeof(cl_mem), &self->structure);
    OpenCLObject_setKernelArgument(opencl, matrixKernel, 1, sizeof(cl_mem), &self->data);
    OpenCLObject_setKernelArgument(opencl, matrixKernel, 2, sizeof(size_t), &rowCount);
    OpenCLObject_setKernelArgument(opencl, matrixKernel, 3, sizeof(size_t), &columnCount);
    OpenCLObject_executeKernel(opencl, matrixKernel, 1);
    clReleaseKernel(matrixKernel);
    return self;
}

void MatrixCL_delete(MatrixCL *self)
{
    clReleaseMemObject(self->data);
    clReleaseMemObject(self->structure);
    free(self);
}

void MatrixCL_scopedDelete(MatrixCL **self)
{
    MatrixCL_delete(*self);
}

MatrixArrayCL *MatrixArrayCL_new(OpenCLObject *opencl,
                                 size_t rowCount,
                                 size_t columnCount,
                                 size_t matrixCount)
{
    MatrixArrayCL *self = calloc(1, sizeof(MatrixArrayCL));
    self->matrixCount = matrixCount;
    self->matrices = calloc(matrixCount, sizeof(MatrixCL *));
    const char *constructorKernel = "MatrixArrayCL_new";
    cl_kernel matrixKernel = OpenCLObject_createKernel(opencl, constructorKernel);
    self->data = OpenCLObject_allocateBuffer(opencl,
                                             matrixCount * sizeof(MatrixCL *),
                                             CL_MEM_READ_WRITE);
    OpenCLObject_setKernelArgument(opencl, matrixKernel, 0, sizeof(cl_mem), &self->data);

    for (size_t i = 0; i < matrixCount; ++i) {
        
        self->matrices[i] = MatrixCL_new(opencl, rowCount, columnCount);
        OpenCLObject_setKernelArgument(opencl, matrixKernel, 1, sizeof(cl_mem), &self->matrices[i]->structure);
        OpenCLObject_setKernelArgument(opencl, matrixKernel, 2, sizeof(size_t), &i);
        OpenCLObject_executeKernel(opencl, matrixKernel, 1);
    }
    
    
    clReleaseKernel(matrixKernel); 
    return self;
}

void MatrixArrayCL_delete(MatrixArrayCL *self)
{
    for (size_t i = 0; i < self->matrixCount; ++i) {
        
        MatrixCL_delete(self->matrices[i]);
    }
    
    free(self->matrices);
    clReleaseMemObject(self->data);
    free(self);
}

void MatrixArrayCL_scopedDelete(MatrixArrayCL **self)
{
    MatrixArrayCL_delete(*self);
}

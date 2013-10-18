//
//  OpenCLObjectTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 26/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "OpenCLObject.h"

typedef struct  OpenCLMatrix
{
    size_t rowCount;
    size_t columnCount;
    size_t elementCount;
    double *data;

} OpenCLMatrix;

void OpenCLObjectTests()
{
    OpenCLObject *opencl = OpenCLObject_new(kOpenCLObjectDevice_GPU, "/Users/eddyc/Dropbox/SoftwareProjects/64bit Tools/DSP/OpenCLObjectTests.cl", 2, 1);
    OpenCLObject_deviceStats(opencl->device);
    OpenCLObject_createKernel(opencl, 0, "Matrix_new");
    OpenCLObject_createKernel(opencl, 1, "Matrix_print");
    cl_mem openCLMatrix = OpenCLObject_allocateBuffer(opencl, sizeof(OpenCLMatrix), CL_MEM_READ_WRITE);
    Matrix *matrix = Matrix_newWithValues(1, 4, 4, 1., 2., 3., 4.);
    cl_mem matrixData = OpenCLObject_allocateFloat64Buffer(opencl, matrix->elementCount, CL_MEM_READ_WRITE);
    
    OpenCLObject_writeFloat64Buffer(opencl, matrixData, matrix->data, matrix->elementCount);
    
    OpenCLObject_setKernelArgument(opencl, 0, 0, sizeof(OpenCLMatrix *), &openCLMatrix);
    OpenCLObject_setKernelArgument(opencl, 0, 1, sizeof(size_t), &matrix->rowCount);
    OpenCLObject_setKernelArgument(opencl, 0, 2, sizeof(size_t), &matrix->columnCount);
    OpenCLObject_setKernelArgument(opencl, 0, 3, sizeof(cl_mem), &matrixData);
    OpenCLObject_setKernelArgument(opencl, 1, 0, sizeof(OpenCLMatrix *), &openCLMatrix);

    OpenCLObject_executeKernel(opencl, 0);
    OpenCLObject_executeKernel(opencl, 1);

    OpenCLObject_delete(opencl);
}
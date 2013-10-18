//
//  DTWOpenCL1D.c
//  AudioMosaicing
//
//  Created by Edward Costello on 09/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTWOpenCL1D.h"
#import "ConvenienceFunctions.h"

DTWOpenCL1D *DTWOpenCL1D_new(const Matrix *const paletteData,
                         const size_t maxTimeFrameCount,
                         const size_t hopSize)
{
    DTWOpenCL1D *self = calloc(1, sizeof(DTWOpenCL1D));
    self->globalWorkSize = 1 + ((paletteData->rowCount - maxTimeFrameCount) / hopSize);
    self->opencl = OpenCLObject_new(kOpenCLObjectDevice_CPU, "/Users/eddyc/Dropbox/SoftwareProjects/64bit Tools/DSP/DTWOpenCL1D.cl", self->globalWorkSize);
    self->paletteData = paletteData;
    self->maxTimeFrameCount = maxTimeFrameCount;
    self->paletteMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                             paletteData->elementCount,
                                                             CL_MEM_READ_ONLY);
    
    Matrix *timeCostMemoryMatrix = Matrix_new((maxTimeFrameCount + 1) * self->globalWorkSize, maxTimeFrameCount + 1);
    Matrix *frequencyCostMemoryMatrix = Matrix_new((paletteData->columnCount + 1) * self->globalWorkSize, (paletteData->columnCount + 1));
    DTWOpenCL1D_fillCostMatrix(timeCostMemoryMatrix, self);
    DTWOpenCL1D_fillCostMatrix(frequencyCostMemoryMatrix, self);
    
    self->timeCostScore = calloc(self->globalWorkSize, sizeof(Float64));
    self->frequencyCostScore = calloc(self->globalWorkSize, sizeof(Float64));
    self->timeCostScoreMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                                   self->globalWorkSize,
                                                                   CL_MEM_WRITE_ONLY);
    
    self->frequencyCostScoreMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                                        self->globalWorkSize,
                                                                        CL_MEM_WRITE_ONLY);
    self->timeCostMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                              timeCostMemoryMatrix->elementCount,
                                                              CL_MEM_READ_WRITE);
    
    self->frequencyCostMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                                   frequencyCostMemoryMatrix->elementCount,
                                                                   CL_MEM_READ_WRITE);
    
    OpenCLObject_writeFloat64Buffer(self->opencl, self->timeCostMemory, timeCostMemoryMatrix->data, timeCostMemoryMatrix->elementCount);
    OpenCLObject_writeFloat64Buffer(self->opencl, self->frequencyCostMemory, frequencyCostMemoryMatrix->data, frequencyCostMemoryMatrix->elementCount);
    
    self->currentFrameMemory = OpenCLObject_allocateFloat64Buffer(self->opencl,
                                                                  paletteData->columnCount,
                                                                  CL_MEM_READ_ONLY);
    
    OpenCLObject_writeFloat64Buffer(self->opencl, self->paletteMemory, paletteData->data, paletteData->elementCount);
    
    OpenCLObject_createKernel(self->opencl, "DTWOpenCL1D_process");
    OpenCLObject_setKernelArgument(self->opencl, 0, sizeof(size_t), &maxTimeFrameCount);
    OpenCLObject_setKernelArgument(self->opencl, 1, sizeof(size_t), &hopSize);
    OpenCLObject_setKernelArgument(self->opencl, 2, sizeof(size_t), &self->currentFrameNumber);
    OpenCLObject_setKernelArgument(self->opencl, 3, sizeof(cl_mem), &self->currentFrameMemory);
    OpenCLObject_setKernelArgument(self->opencl, 4, sizeof(cl_mem), &self->paletteMemory);
    OpenCLObject_setKernelArgument(self->opencl, 5, sizeof(size_t), &paletteData->rowCount);
    OpenCLObject_setKernelArgument(self->opencl, 6, sizeof(size_t), &paletteData->columnCount);
    OpenCLObject_setKernelArgument(self->opencl, 7, sizeof(cl_mem), &self->timeCostMemory);
    OpenCLObject_setKernelArgument(self->opencl, 8, sizeof(cl_mem), &self->frequencyCostMemory);
    OpenCLObject_setKernelArgument(self->opencl, 9, sizeof(cl_mem), &self->timeCostScoreMemory);
    OpenCLObject_setKernelArgument(self->opencl, 10, sizeof(cl_mem), &self->frequencyCostScoreMemory);
    
    self->inputFrameBuffer = Matrix_new(maxTimeFrameCount, paletteData->columnCount);
    self->bestMatchTimeCostMatrix = Matrix_new(maxTimeFrameCount, maxTimeFrameCount);
    self->bestMatchFrequencyCostMatrix = Matrix_new(paletteData->columnCount, paletteData->columnCount);
    self->tempTimeRow = calloc(maxTimeFrameCount, sizeof(Float64));
    self->tempFrequencyRow = calloc(paletteData->columnCount, sizeof(Float64));
    Matrix_delete(timeCostMemoryMatrix);
    Matrix_delete(frequencyCostMemoryMatrix);
    
    return self;
}

void DTWOpenCL1D_delete(DTWOpenCL1D *const self)
{
    free(self->timeCostScore);
    free(self->frequencyCostScore);
    free(self->tempTimeRow);
    free(self->tempFrequencyRow);
    
    Matrix_delete(self->inputFrameBuffer);
    Matrix_delete(self->bestMatchTimeCostMatrix);
    Matrix_delete(self->bestMatchFrequencyCostMatrix);

    OpenCLObject_deallocateFloat64Buffer(self->currentFrameMemory);
    OpenCLObject_deallocateFloat64Buffer(self->frequencyCostMemory);
    OpenCLObject_deallocateFloat64Buffer(self->frequencyCostScoreMemory);
    OpenCLObject_deallocateFloat64Buffer(self->paletteMemory);
    OpenCLObject_deallocateFloat64Buffer(self->timeCostMemory);
    OpenCLObject_deallocateFloat64Buffer(self->timeCostScoreMemory);
    
    free(self);
}

size_t DTWOpenCL1D_getBestMatch(const DTWOpenCL1D *const self)
{
    OpenCLObject_readFloat64Buffer(self->opencl, self->timeCostScoreMemory, self->timeCostScore, self->globalWorkSize, 0);
    OpenCLObject_readFloat64Buffer(self->opencl, self->frequencyCostScoreMemory, self->frequencyCostScore, self->globalWorkSize, 0);
    
    size_t minimumIndex;
    Float64 minimumValue;
    
    vDSP_minviD(self->timeCostScore, 1, &minimumValue, &minimumIndex, self->globalWorkSize);
    
    return minimumIndex;
}

void DTWOpenCL1D_getBestMatchWarpPaths(const DTWOpenCL1D *const self,
                                     const size_t bestMatch)
{
    for (size_t i = 0; i < self->inputFrameBuffer->rowCount; ++i) {
        
        
    }
}

bool DTWOpenCL1D_process(DTWOpenCL1D *const self,
                       const Float64 *const inputFrame)
{
    bool matchFound = false;
    
    cblas_dcopy((UInt32)self->paletteData->columnCount, inputFrame, 1, Matrix_getRow(self->inputFrameBuffer, self->currentFrameNumber), 1);
    OpenCLObject_writeFloat64Buffer(self->opencl, self->currentFrameMemory, inputFrame, self->paletteData->columnCount);
    OpenCLObject_execute(self->opencl);
    
    if (self->currentFrameNumber == self->maxTimeFrameCount - 1) {
        
        self->currentBestMatch = DTWOpenCL1D_getBestMatch(self);
        DTWOpenCL1D_getBestMatchWarpPaths(self, self->currentBestMatch);
        matchFound = true;
    }
    
    self->currentFrameNumber++;
    self->currentFrameNumber %= self->maxTimeFrameCount;
    OpenCLObject_setKernelArgument(self->opencl, 2, sizeof(size_t), &self->currentFrameNumber);
    
    return matchFound;
}


void DTWOpenCL1D_fillCostMatrix(const Matrix *const costMatrix,
                              const DTWOpenCL1D *const self)
{
    Float64 infinity = INFINITY;
    
    vDSP_vfillD(&infinity, costMatrix->data, 1, costMatrix->columnCount);
    vDSP_vfillD(&infinity, costMatrix->data, costMatrix->columnCount, costMatrix->columnCount);
    costMatrix->data[0] = 0;
    UInt32 costMatrixElementCount = (UInt32)costMatrix->columnCount * (UInt32)costMatrix->columnCount;
    for (size_t i = 1; i < self->globalWorkSize; ++i) {
        
        cblas_dcopy(costMatrixElementCount, costMatrix->data, 1, &costMatrix->data[costMatrixElementCount * i], 1);
    }
}


//
//  DTWOpenCL1D.h
//  AudioMosaicing
//
//  Created by Edward Costello on 08/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import "Matrix.h"
#import <OpenCL/OpenCL.h>
#import "OpenCLObject.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct DTWOpenCL1D
    {
        OpenCLObject *opencl;
        size_t globalWorkSize;
        size_t maxTimeFrameCount;
        size_t currentFrameNumber;
        size_t currentBestMatch;
        const Matrix *paletteData;
        cl_mem currentFrameMemory;
        cl_mem paletteMemory;
        cl_mem timeCostMemory;
        cl_mem frequencyCostMemory;
        cl_mem timeCostScoreMemory;
        cl_mem frequencyCostScoreMemory;
        
        Float64 *timeCostScore;
        Float64 *frequencyCostScore;
        Matrix *inputFrameBuffer;
        Matrix *bestMatchTimeCostMatrix;
        Matrix *bestMatchFrequencyCostMatrix;
        
        Float64 *tempTimeRow;
        Float64 *tempFrequencyRow;

    } DTWOpenCL1D;
    
    DTWOpenCL1D *DTWOpenCL1D_new(const Matrix *const paletteData,
                             const size_t maxTimeFrameCount,
                             const size_t hopSize);
    
    void DTWOpenCL1D_delete(DTWOpenCL1D *const self);
    bool DTWOpenCL1D_process(DTWOpenCL1D *const self, const Float64 *const inputFrame);
    void DTWOpenCL1D_fillCostMatrix(const Matrix *const costMatrix, const DTWOpenCL1D *const self);

    
#ifdef __cplusplus
}
#endif
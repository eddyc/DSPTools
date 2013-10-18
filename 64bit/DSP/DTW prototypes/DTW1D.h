//
//  DTW1D.h
//  AudioMosaicing
//
//  Created by Edward Costello on 06/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>
#import "Matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct DTW1D
    {
        size_t maxTimeFrameCount;
        size_t frequencyFrameCount;
        size_t hopSize;
        size_t segmentCount;
        size_t currentFrameNumber;
        Float64 *timeFrameTemp;
        Float64 *frequencyFrameTemp;
        Float64 *bestMatchComparisonArray;
        Matrix *frequencyCostMatrixTemp;
        Matrix *paletteData;
        Matrix **timeCostMatrices;
        Matrix **frequencyCostMatrices;
    } DTW1D;
    
    DTW1D *DTW1D_new(Matrix *const paletteData,
                     const size_t maxTimeFrameCount,
                     const size_t hopSize);
    void DTW1D_delete(DTW1D *const self);
    bool DTW1D_processFrame(DTW1D *const self, Float64 *const inputFrame);
    
#ifdef __cplusplus
}
#endif
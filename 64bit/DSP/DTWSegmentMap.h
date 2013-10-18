//
//  DTWSegmentMap.h
//  AudioMosaicing
//
//  Created by Edward Costello on 28/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "Matrix.h"
#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _DTWSegmentMap
    {
        size_t paletteFrameCount;
        size_t paletteFrequencyBinCount;
        size_t paletteSegmentCount;
        size_t paletteSegmentFrameCount;
        size_t paletteSegmentHopSize;
        size_t frequencySplitCount;
        
        Float64 *paletteSegmentStartIndices;
        Float64 *frequencyStartIndices;
        SInt32 *frequencyStartIndicesInt;
        Float64 *frequencySplitSizes;
        SInt32 *frequencySplitSizesInt;
        
    } DTWSegmentMap;
    
    OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(const Matrix *const paletteData);
    OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(size_t frameCount, size_t maximumFrequencyChannelCount);
    

    void DTWSegmentMap_delete(DTWSegmentMap *self);
    void DTWSegmentMap_scopedDelete(DTWSegmentMap **self);

#define _DTWSegmentMap __attribute__((unused)) __attribute__((cleanup(DTWSegmentMap_scopedDelete))) DTWSegmentMap

    void DTWSegmentMap_calculateSegmentMap(DTWSegmentMap *const self,
                                           const size_t paletteSegmentFrameCount,
                                           const size_t paletteSegmentHopSize,
                                           const size_t frequencySplitCount);
    

    
#ifdef __cplusplus
}
#endif
//
//  DTWAccelarate.h
//  AudioMosaicing
//
//  Created by Edward Costello on 20/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DTWSegmentMap.h"
#import "MatrixRingBuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct _DTWAccelerate
    {
        const Matrix *paletteData;
        const DTWSegmentMap *segmentMap;
        Matrix currentPaletteSegment;
        Matrix currentTestGlobalCostSegment;
        
        size_t maximumInputBufferFrameCount;
        size_t *bestMatchArray;
        Float64 *infinityRow;
        Float64 *tempLocalCostRow;
        Matrix *bestMatchMatrix;
        
        Matrix *globalCostMatrix;
        MatrixRingBuffer *costRingBuffer;
        
    } DTWAccelerate;
    
    
    DTWAccelerate *const DTWAccelerate_new(const Matrix *const paletteData,
                                           size_t maximumInputBufferFrameCount,
                                           const DTWSegmentMap *segmentMap);
    
    void DTWAccelerate_delete(DTWAccelerate *self);
    void DTWAccelerate_scopedDelete(DTWAccelerate **self);
    
#define _DTWAccelerate __attribute__((unused)) __attribute__((cleanup(DTWAccelerate_scopedDelete))) DTWAccelerate
    
    void DTWAccelerate_processFrame(DTWAccelerate *const self,
                                    const Float64 *const inputFrame,
                                    const bool reset);
    void DTWAccelerate_getBestMatch(DTWAccelerate *const self);
    
    
#ifdef __cplusplus
}
#endif
//
//  TriangleFilterBank.h
//  Audio-Mosaicing
//
//  Created by Edward Costello on 01/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct TriangleFilterBank32
    {
        size_t filterCount;
        size_t filterFrequencyCount;
        size_t magnitudeFrameSize;
        size_t samplerate;
        Float32 *filterFrequencies;
        Float32 *filterBank;

    } TriangleFilterBank32;
    
    TriangleFilterBank32 *TriangleFilterBank32_new(size_t filterCount,
                                                   size_t magnitudeFrameSize,
                                                   size_t samplerate);
    void TriangleFilterBank32_delete(TriangleFilterBank32 *self);
    void TriangleFilterBank32_process(TriangleFilterBank32 *self,
                                      Float32 *magnitudesIn,
                                      Float32 *filteredMagnitudesOut);

#ifdef __cplusplus
}
#endif
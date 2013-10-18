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
    
    typedef struct TriangleFilterBank
    {
        size_t filterCount;
        size_t filterFrequencyCount;
        size_t magnitudeFrameSize;
        size_t samplerate;
        Float64 *filterFrequencies;
        Float64 *filterBank;
        
    } TriangleFilterBank;
    
    TriangleFilterBank *TriangleFilterBank_new(size_t filterCount,
                                               size_t magnitudeFrameSize,
                                               size_t samplerate);
    void TriangleFilterBank_delete(TriangleFilterBank *self);
    void TriangleFilterBank_process(TriangleFilterBank *self,
                                    Float64 *magnitudesIn,
                                    Float64 *filteredMagnitudesOut);
    
#ifdef __cplusplus
}
#endif
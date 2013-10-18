//
//  OverlappingRingBuffer.h
//  AudioMosaicing
//
//  Created by Edward Costello on 02/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "MatrixRingBuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct OverlappingRingBuffer
    {
        MatrixRingBuffer *data;

    } OverlappingRingBuffer;
    
    OverlappingRingBuffer *OverlappingRingBuffer_new();
    void OverlappingRingBuffer_delete(OverlappingRingBuffer *self);
    void OverlappingRingBuffer_write(OverlappingRingBuffer *self, Float64 *input);
    void OverlappingRingBuffer_read(OverlappingRingBuffer *self, Float64 *output);

    
#ifdef __cplusplus
}
#endif
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
    
    /**
     * @class OverlappingRingBuffer
     *
     * @abstract This object is used to reconstruct inverse FFT or similar overlapped frames, NOTE: At the moment this is hardcoded to frame sizes of 1024, and hop sizes of 256.
     * @param data A MatrixRingBuffer object
     */
    typedef struct OverlappingRingBuffer
    {
        MatrixRingBuffer *data;

    } OverlappingRingBuffer;
    
    /**
     *  Construct a OverlappingRingBuffer object
     *
     *  @return An instance of OverlappingRingBuffer object
     */
    OverlappingRingBuffer *OverlappingRingBuffer_new();
    
    /**
     *  Destroy a OverlappingRingBuffer object
     *
     *  @param self An instance of a OverlappingRingBuffer object
     */
    void OverlappingRingBuffer_delete(OverlappingRingBuffer *self);
    
    /**
     *  Write a frame to a OverlappingRingBuffer object
     *
     *  @param self  An instance of a OverlappingRingBuffer object
     *  @param input An input frame
     */
    void OverlappingRingBuffer_write(OverlappingRingBuffer *self, Float64 *input);
    
    /**
     *  Read a frame from a OverlappingRingBuffer object
     *
     *  @param self   An instance of a OverlappingRingBuffer object
     *  @param output An output frame
     */
    void OverlappingRingBuffer_read(OverlappingRingBuffer *self, Float64 *output);

    
#ifdef __cplusplus
}
#endif
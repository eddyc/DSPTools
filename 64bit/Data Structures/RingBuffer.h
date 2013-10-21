//
//  RingBuffer.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 26/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class RingBuffer
     *
     * @abstract A simple RingBuffer object for audio samples
     * @param capacity The total amount of sample the buffer can hold
     * @param dataWrittenCount The amount of samples current in the buffer
     * @param dataFreeCount The amount of free space in the buffer
     * @param readPointer The current read position in the buffer
     * @param writePointer The current write position in the buffer
     * @param data The data buffer
     */
    typedef struct RingBuffer
    {
        size_t capacity;
        size_t dataWrittenCount;
        size_t dataFreeCount;
        size_t readPointer;
        size_t writePointer;
        Float64 *data;
        
    } RingBuffer;
    
    /**
     *  Construct a RingBuffer object
     *
     *  @param capacity The total number of samples the buffer can hold
     *
     *  @return A pointer to a RingBuffer instance
     */
    RingBuffer *RingBuffer_new(size_t capacity);
    
    /**
     *  Destroy a RingBuffer object
     *
     *  @param  A pointer to a RingBuffer instance
     */
    void RingBuffer_delete(RingBuffer *);
    
    /**
     *  Write samples to the RingBuffer object
     *
     *  @param self          A pointer to a RingBuffer instance
     *  @param inputData     The data to write
     *  @param inputDataSize The amount of samples to write
     */
    void RingBuffer_write(RingBuffer *self,
                          Float64 *__restrict inputData,
                          size_t inputDataSize);
    
    /**
     *  Copy the samples in a RingBuffer object to an output buffer
     *
     *  @param self           A pointer to a RingBuffer instance
     *  @param outputData     The output buffer where the samples are copied
     *  @param outputDataSize The amount of samples to copy
     */
    void RingBuffer_copy(RingBuffer *self,
                         Float64 *__restrict outputData,
                         size_t outputDataSize);
    
    /**
     *  Skip the write pointer a given amount of samples
     *
     *  @param self      A pointer to a RingBuffer instance
     *  @param skipCount The amount of samples to skip
     */
    void RingBuffer_skip(RingBuffer *self,
                         size_t skipCount);
    
    /**
     *  Peform the write, copy and skip methods in sequence
     *
     *  @param self           A pointer to a RingBuffer instance
     *  @param inputData      Input data to copy to the ring buffer
     *  @param inputDataSize  The amount of samples to copy to the buffer
     *  @param outputData     Output data buffer to copy samples from the ring buffer
     *  @param outputDataSize The amount of samples to copy from the ring buffer
     *  @param skipCount      The amount of samples to skip
     */
    void RingBuffer_writeCopySkip(RingBuffer *self,
                                  Float64 *__restrict inputData,
                                  size_t inputDataSize,
                                  Float64 *__restrict outputData,
                                  size_t outputDataSize,
                                  size_t skipCount);
    
    
#ifdef __cplusplus
}
#endif
//
//  RingBuffer.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 26/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import <math.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct RingBuffer
    {
        size_t capacity;
        
        size_t dataWrittenCount;
        size_t dataFreeCount;

        size_t readPointer;
        size_t writePointer;
        
        Float64 *data;
        
    } RingBuffer;
    
    RingBuffer *RingBuffer_new(size_t capacity);
    void RingBuffer_delete(RingBuffer *);
    
    extern inline void RingBuffer_write(RingBuffer *self,
                                               Float64 *__restrict inputData,
                                               size_t inputDataSize);
    
    extern inline void RingBuffer_copy(RingBuffer *self,
                                              Float64 *__restrict outputData,
                                              size_t outputDataSize);
    
    extern inline void RingBuffer_skip(RingBuffer *self,
                                              size_t skipCount);
    
    extern inline void RingBuffer_writeCopySkip(RingBuffer *self,
                                                       Float64 *__restrict inputData,
                                                       size_t inputDataSize,
                                                       Float64 *__restrict outputData,
                                                       size_t outputDataSize,
                                                       size_t skipCount);


#ifdef __cplusplus
}
#endif
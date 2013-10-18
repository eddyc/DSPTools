//
//  RingBuffer.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 26/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "RingBuffer.h"
#import <stdio.h>
#import <Accelerate/Accelerate.h>

size_t RingBuffer_countLeadingZeroes(size_t arg)
{
    if (arg == 0) {
        
        return 32;
    }
    
    return __builtin_clz((UInt32)arg);
    
}

size_t RingBuffer_log2Ceil(size_t x)
{
    return 32 - RingBuffer_countLeadingZeroes(x - 1);
}

size_t RingBuffer_nextPowerOfTwo(size_t x)
{
    return 1 << RingBuffer_log2Ceil(x);
}

RingBuffer *RingBuffer_new(size_t capacity)
{
    RingBuffer *self = calloc(1, sizeof(RingBuffer));
    self->capacity = RingBuffer_nextPowerOfTwo(capacity);
    self->dataFreeCount = self->capacity;
    self->data = calloc(self->capacity, sizeof(Float64));
    
    return self;
}

void RingBuffer_delete(RingBuffer *self)
{
    free(self->data);
    free(self);
    self = NULL;
}

static inline void RingBuffer_checkDataSize(RingBuffer *self, size_t dataSize)
{
    if (dataSize > self->capacity) {
        
        printf("RingBuffer error, requested data size is larger than buffer size, exiting\n");
        exit(-1);
    }
}

static void RingBuffer_setDataSize(RingBuffer *self, SInt64 dataSize)
{
    self->dataFreeCount -= dataSize;
    self->dataWrittenCount += dataSize;
}

inline static Boolean RingBuffer_checkForWrapAround(RingBuffer *self, size_t pointer, size_t dataSize)
{
    return ((dataSize + pointer) > self->capacity);
}

inline void RingBuffer_write(RingBuffer *self, Float64 *__restrict inputData, size_t inputDataSize)
{
    if (self->dataWrittenCount >= self->capacity) {
        
        return;
    }
    
    RingBuffer_checkDataSize(self, inputDataSize);
    
    if (RingBuffer_checkForWrapAround(self, self->writePointer, inputDataSize) == true) {
        
        size_t firstHalfSize = self->capacity - self->writePointer;
        size_t secondHalfSize = inputDataSize - firstHalfSize;
        
        cblas_dcopy((SInt32)firstHalfSize, inputData, 1, &self->data[self->writePointer], 1);
        self->writePointer = 0;
        cblas_dcopy((SInt32)secondHalfSize, &inputData[firstHalfSize], 1, &self->data[self->writePointer], 1);
        self->writePointer += secondHalfSize;
    }
    else {
        
        cblas_dcopy((SInt32)inputDataSize, inputData, 1, &self->data[self->writePointer], 1);
        self->writePointer += inputDataSize;
    }
    
    self->writePointer &= self->capacity - 1;
    self->readPointer += inputDataSize;
    self->readPointer &= self->capacity - 1;
    
    RingBuffer_setDataSize(self, inputDataSize);
}

inline void RingBuffer_copy(RingBuffer *self, Float64 *__restrict outputData, size_t outputDataSize)
{
    RingBuffer_checkDataSize(self, outputDataSize);
    
    if (RingBuffer_checkForWrapAround(self, self->readPointer, outputDataSize) == true) {
        
        size_t firstHalfSize = self->capacity - self->readPointer;
        size_t secondHalfSize = outputDataSize - firstHalfSize;
        
        cblas_dcopy((SInt32)firstHalfSize, &self->data[self->readPointer], 1, outputData, 1);
        cblas_dcopy((SInt32)secondHalfSize, &self->data[0], 1, &outputData[firstHalfSize], 1);
        
    }
    else {
        
        cblas_dcopy((SInt32)outputDataSize, &self->data[self->readPointer], 1, outputData, 1);
    }
}


inline void RingBuffer_skip(RingBuffer *self, size_t skipCount)
{
    if (self->dataFreeCount == self->capacity) {
        
        return;
    }
    
    RingBuffer_checkDataSize(self, skipCount);
    
    RingBuffer_setDataSize(self,-(SInt32)skipCount);
}

inline void RingBuffer_writeCopySkip(RingBuffer *self,
                                            Float64 *__restrict inputData,
                                            size_t inputDataSize,
                                            Float64 *__restrict outputData,
                                            size_t outputDataSize,
                                            size_t skipCount)
{
    RingBuffer_write(self, inputData, inputDataSize);
    RingBuffer_copy(self, outputData, outputDataSize);
    RingBuffer_skip(self, skipCount);
}


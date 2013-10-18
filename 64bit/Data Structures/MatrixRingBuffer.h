//
//  MatrixRingBuffer.h
//  AudioMosaicing
//
//  Created by Edward Costello on 21/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct MatrixRingBuffer
    {
        Float64 **data;
        size_t rowCount;
        size_t columnCount;

    } MatrixRingBuffer;
    
    MatrixRingBuffer *MatrixRingBuffer_new(const size_t rowCount, const size_t columnCount);
    void MatrixRingBuffer_delete(MatrixRingBuffer *self);
    
    void MatrixRingBuffer_write(const MatrixRingBuffer *const self, const Float64 *const inputFrame);
    void MatrixRingBuffer_rotate(const MatrixRingBuffer *const self);

    void MatrixRingBuffer_print(const MatrixRingBuffer *const self);
    
#define MatrixRingBuffer_getCurrentRow(self) ((self)->data[self->rowCount - 1])

#ifdef __cplusplus
}
#endif
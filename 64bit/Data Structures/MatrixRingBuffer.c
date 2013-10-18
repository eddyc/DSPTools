//
//  MatrixRingBuffer.c
//  AudioMosaicing
//
//  Created by Edward Costello on 21/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "MatrixRingBuffer.h"
#import <Accelerate/Accelerate.h>
#import <string.h>

MatrixRingBuffer *MatrixRingBuffer_new(const size_t rowCount, const size_t columnCount)
{
    MatrixRingBuffer *self = calloc(1, sizeof(MatrixRingBuffer));
    self->rowCount = rowCount;
    self->columnCount = columnCount;
    
    self->data = calloc(rowCount, sizeof(Float64 *));
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        self->data[i] = calloc(columnCount, sizeof(Float64));
    }
    
    return self;
}

void MatrixRingBuffer_delete(MatrixRingBuffer *self)
{
    
    for (size_t i = 0; i < self->rowCount; ++i) {
        
        free(self->data[i]);
    }
    
    free(self->data);
    free(self);
    self = NULL;
}

void MatrixRingBuffer_rotate(const MatrixRingBuffer *const self)
{
    Float64 *const temp = self->data[0];
    memcpy(&self->data[0], &self->data[1], (self->rowCount - 1) * sizeof(Float64 *));
    self->data[self->rowCount - 1] = temp;
}

void MatrixRingBuffer_write(const MatrixRingBuffer *const self, const Float64 *const inputFrame)
{
    MatrixRingBuffer_rotate(self);
    
    cblas_dcopy((UInt32)self->columnCount, inputFrame, 1, self->data[self->rowCount - 1], 1);
}

void MatrixRingBuffer_print(const MatrixRingBuffer *const self)
{
    printf("MatrixRingBuffer_print\n");

    for (size_t i = 0; i < self->rowCount; ++i) {
        
        for (size_t j = 0; j < self->columnCount; ++j) {
            
            printf("[%f]\t", self->data[i][j]);
        }
        
        printf("\n");
    }
    
    printf("\n");

}


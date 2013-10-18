//
//  OverlappingRingBuffer.c
//  AudioMosaicing
//
//  Created by Edward Costello on 02/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "OverlappingRingBuffer.h"
#import <Accelerate/Accelerate.h>
#import <string.h>
OverlappingRingBuffer *OverlappingRingBuffer_new()
{
    OverlappingRingBuffer *self = calloc(1, sizeof(OverlappingRingBuffer));
    
    self->data = MatrixRingBuffer_new(4, 1024);

    return self;
}

void OverlappingRingBuffer_delete(OverlappingRingBuffer *self)
{
    MatrixRingBuffer_delete(self->data);
    
    free(self);
    self = NULL;
}

void OverlappingRingBuffer_write(OverlappingRingBuffer *self, Float64 *input)
{
    cblas_dcopy(1024, input, 1, self->data->data[0], 1);
}

void OverlappingRingBuffer_read(OverlappingRingBuffer *self, Float64 *output)
{

    for (size_t i = 0; i < 4; ++i) {
        
        vDSP_vaddD(&self->data->data[i][i * 256], 1, output, 1, output, 1, 256);
    }
    
    Float64 *const temp = self->data->data[3];
    memcpy(&self->data->data[1], &self->data->data[0], 3 * sizeof(Float64 *));
    self->data->data[0] = temp;
}


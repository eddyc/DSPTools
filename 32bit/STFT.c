//
//  STFT.c
//  DSPTools
//
//  Created by Edward Costello on 19/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "STFT.h"

STFT32 *STFT32_new(size_t FFTFrameSize, size_t hopSize, FFTWindowType windowType)
{
    STFT32 *self = malloc(sizeof(STFT32));
    self->FFTFrameSize = FFTFrameSize;
    self->hopSize = hopSize;
    self->fft = FFT32_new(self->FFTFrameSize, windowType);
    
    return self;
}

void STFT32_delete(STFT32 *self)
{
    FFT32_delete(self->fft);
    free(self);
    self = NULL;
}

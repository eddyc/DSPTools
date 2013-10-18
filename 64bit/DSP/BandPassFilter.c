//
//  BandPassFilter.c
//  Kalman Filters
//
//  Created by Edward Costello on 16/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "BandPassFilter.h"
#import <math.h>

BandPassFilter *BandPassFilter_new(Float64 samplerate, Float64 frequency, Float64 bandwidth)
{
    BandPassFilter *self = calloc(1, sizeof(BandPassFilter));
    self->samplerate = samplerate;
    self->currentFrequency = frequency;
    self->currentBandwidth = bandwidth;
    self->feedbackBuffer = self->feedbackBufferA;
    return self;
}

void BandPassFilter_delete(BandPassFilter *self)
{
    free(self);
    self = NULL;
}

void BandPassFilter_scopedDelete(BandPassFilter **self)
{
    BandPassFilter_delete(*self);
}

void BandPassFilter_process(BandPassFilter *self, size_t inNumberFrames, Float64 *samples)
{
    Float64 *feedbackBuffer, c, d;
    
    feedbackBuffer = self->feedbackBuffer;
    
    if (self->currentBandwidth != self->lastBandwidth
        ||
        self->currentFrequency != self->lastFrequency) {
        
        self->lastFrequency = self->currentFrequency;
        self->lastBandwidth = self->currentBandwidth;
        
        c = 1.0 / tan((double)((M_PI / self->samplerate) * self->lastBandwidth));
        d = 2.0 * cos((double)(((2 * M_PI) / self->samplerate) * self->lastFrequency));
        self->feedbackBufferA[0] = self->feedbackBufferB[0] = 1.0 / (1.0 + c);
        self->feedbackBufferA[1] = self->feedbackBufferB[1] = 0.0;
        self->feedbackBufferA[2] = self->feedbackBufferB[2] = -feedbackBuffer[0];
        self->feedbackBufferA[3] = self->feedbackBufferB[3] = - c * d * feedbackBuffer[0];
        self->feedbackBufferA[4] = self->feedbackBufferB[4] = (c - 1.0) * feedbackBuffer[0];
    }
    
    for (size_t i = 0; i < inNumberFrames; i++) {
        
        double t = samples[i] - feedbackBuffer[3] * feedbackBuffer[5] - feedbackBuffer[4] * feedbackBuffer[6];
        double y = t * feedbackBuffer[0] + feedbackBuffer[1] * feedbackBuffer[5] + feedbackBuffer[2] * feedbackBuffer[6];
        feedbackBuffer[6] = feedbackBuffer[5];
        feedbackBuffer[5] = t;
        samples[i] = y;
    }
}

void BandPassFilter_processingCallback(AudioStream *audioStream,
                                       void *inRefCon,
                                       const UInt32 inNumberFrames,
                                       Float64 **audioData)
{
    BandPassFilter *self = inRefCon;
    
    printf("In:%f %f\n", audioData[0][0], audioData[1][0]);
    for (size_t i = 0; i < 2; ++i) {
        if (i == 0) {
            
            self->feedbackBuffer = self->feedbackBufferA;
        }
        else{
            
            self->feedbackBuffer = self->feedbackBufferB;
        }
        BandPassFilter_process(self, inNumberFrames, audioData[i]);
    }
    
    printf("Out:%f %f\n", audioData[0][0], audioData[1][0]);

}

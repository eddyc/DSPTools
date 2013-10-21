//
//  WavetableOscillator.c
//  Kalman Filters
//
//  Created by Edward Costello on 18/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "WavetableOscillator.h"

WavetableOscillator *WavetableOscillator_new(size_t samplerate)
{
    WavetableOscillator *self = calloc(1, sizeof(WavetableOscillator));
    self->samplerate = samplerate;
    self->nyquist = samplerate / 2;
    self->sineWaveTable = calloc(self->samplerate, sizeof(Float64));
    WavetableOscillator_createSineWavetable(self->sineWaveTable, samplerate, samplerate, 1);
    return self;
}

void WavetableOscillator_delete(WavetableOscillator *self)
{
    free(self->sineWaveTable);
    free(self);
    self = NULL;
}

void WavetableOscillator_scopedDelete(WavetableOscillator **self)
{
    WavetableOscillator_delete(*self);
}

void WavetableOscillator_createSineWavetable(Float64 *table,
                                             size_t tableSize,
                                             size_t samplerate,
                                             Float64 frequency)
{
    for (int i = 0; i < tableSize; ++i) {
        
        table[i] = sin(frequency * 2 * M_PI * ((Float64)i / samplerate));
        printf("%f\n", table[i]);
    }
}

void WavetableOscillator_readCallbackStereo(AudioStream *audioStream, void *inRefCon, const UInt32 inNumberFrames, Float64 **audioData)
{
    WavetableOscillator *self = inRefCon;
    
    for (size_t i = 0; i < 2; ++i) {
        

    }
}

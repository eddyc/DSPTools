//
//  WavetableOscillator.h
//  Kalman Filters
//
//  Created by Edward Costello on 18/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import "AudioStream.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct WavetableOscillator
    {
        Float64 phase;
        size_t samplerate;
        size_t nyquist;
        Float64 *currentWavetable;
        Float64 *sineWaveTable;
    } WavetableOscillator;
    
    WavetableOscillator *WavetableOscillator_new(size_t samplerate);
    void WavetableOscillator_delete(WavetableOscillator *self);
    void WavetableOscillator_scopedDelete(WavetableOscillator **self);
#define _WavetableOscillator __attribute__((unused)) __attribute__((cleanup(WavetableOscillator_scopedDelete))) WavetableOscillator
    void WavetableOscillator_readCallbackStereo(AudioStream *audioStream,
                                                void *inRefCon,
                                                const UInt32 inNumberFrames,
                                                Float64 **audioData);
    
    void WavetableOscillator_createSineWavetable(Float64 *table,
                                                 size_t tableSize,
                                                 size_t samplerate,
                                                 Float64 frequency);

#ifdef __cplusplus
}
#endif
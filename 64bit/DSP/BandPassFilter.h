//
//  BandPassFilter.h
//  Kalman Filters
//
//  Created by Edward Costello on 16/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"
#import "AudioStream.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct BandPassFilter
    {
        Float64 currentFrequency;
        Float64 currentBandwidth;
        Float64 lastFrequency;
        Float64 lastBandwidth;
        Float64 *feedbackBuffer;
        Float64 feedbackBufferA[7];
        Float64 feedbackBufferB[7];
        Float64 samplerate;
    } BandPassFilter;
    

#define _BandPassFilter __attribute__((unused)) __attribute__((cleanup(BandPassFilter_scopedDelete))) BandPassFilter

    BandPassFilter *BandPassFilter_new(Float64 samplerate, Float64 frequency, Float64 bandwidth);
    void BandPassFilter_delete(BandPassFilter *self);
    void BandPassFilter_scopedDelete(BandPassFilter **self);
    void BandPassFilter_process(BandPassFilter *self, size_t inNumberFrames, Float64 *samples);
    void BandPassFilter_processingCallback(AudioStream *self,
                                           void *inRefCon,
                                           const UInt32 inNumberFrames,
                                           Float64 **audioData);
#ifdef __cplusplus
}
#endif
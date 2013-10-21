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
    
    /**
     * @class BandPassFilter
     *
     * @abstract A butterworth bandpass filter based on the Csound 6 implementation. It uses two feedback buffers because of the AudioStream processing stereo callback.
     *
     * @param currentFrequency  The current centre frequency of the filter
     * @param currentBandwidth The current bandwidth of the filter
     * @param lastFrequency The previous centre frequency of the filter
     * @param lastBandwidth The previous bandwidth of the filter
     * @param feedbackBuffer Pointer to the currently used feedback buffer
     * @param feedbackBufferA Feedback buffer for IIR filter
     * @param feedbackBufferB Feedback buffer for IIR filter
     * @param samplerate The samplerate that processing is taking place at
     */
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

    /**
     *  Constructor for the BandPassFilter object
     *
     *  @param samplerate The samplerate that processing is taking place at
     *  @param frequency  The specified centre frequency
     *  @param bandwidth  The specified bandwidth
     *
     *  @return A BandPassFilter instance pointer
     */
    BandPassFilter *BandPassFilter_new(Float64 samplerate,
                                       Float64 frequency,
                                       Float64 bandwidth);
    
    /**
     *  Destructor
     *
     *  @param self A BandPassFilter instance pointer
     */
    void BandPassFilter_delete(BandPassFilter *self);
    
    /**
     *  Scoped destructor
     *
     *  @param self A BandPassFilter instance pointer
     */
    void BandPassFilter_scopedDelete(BandPassFilter **self);
    
    /**
     *  Used to declare a scoped BandPassFilter instance
     */
    #define _BandPassFilter __attribute__((unused)) __attribute__((cleanup(BandPassFilter_scopedDelete))) BandPassFilter
    
    /**
     *  Perform filtering of input audio
     *
     *  @param self           A BandPassFilter instance pointer
     *  @param inNumberFrames The number of samples to process
     *  @param samples        The samples buffer
     */
    void BandPassFilter_process(BandPassFilter *self, size_t inNumberFrames, Float64 *samples);
    
    /**
     *  Processing callback for an AudioStream object
     *
     *  @param self           A pointer to an AudioStream object
     *  @param inRefCon       Opaque pointer to BandPassFilter object
     *  @param inNumberFrames The amount of samples to process
     *  @param audioData      The audio data buffers
     */
    void BandPassFilter_processingCallback(AudioStream *self,
                                           void *inRefCon,
                                           const UInt32 inNumberFrames,
                                           Float64 **audioData);
#ifdef __cplusplus
}
#endif
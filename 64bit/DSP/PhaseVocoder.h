//
//  PhaseVocoder.h
//  AudioMosaicing
//
//  Created by Edward Costello on 25/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "AudioMatrix.h"
#import "FFT.h"
#import "AudioObject.h"

#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif
#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct _PhaseVocoder
    {
        
        size_t FFTFrameSize;
        size_t hopSize;
        size_t samplerate;
        FFT *fft;
        Float64 *window;
        Float64 *tempFrame;
        Float64 *tempSampleShiftFrame;
        Float64 *tempMagnitudes;
        Float64 *tempFrequencies;
        
        Float64 *currentPhases;
        Float64 *previousPhases;
        Float64 *twoPiArray;
        Float64 *forwardFrequencyScale;
        Float64 *inverseFrequencyScale;

        Float64 forwardFactor;
        Float64 inverseFactor;
        size_t phasePosition;
        
        
    } PhaseVocoder;
    
    typedef struct _PhaseVocoderData
    {
        Matrix *frequencies;
        Matrix *magnitudes;
        
    } PhaseVocoderData;
    
    PhaseVocoder *PhaseVocoder_new(size_t FFTFrameSize,
                                   size_t hopSize,
                                   size_t samplerate);
    
    void PhaseVocoder_delete(PhaseVocoder *const self);
    void PhaseVocoder_scopedDelete(PhaseVocoder **self);

#define _PhaseVocoder __attribute__((unused)) __attribute__((cleanup(PhaseVocoder_scopedDelete))) PhaseVocoder

    
    PhaseVocoderData *PhaseVocoder_analyseAudioMatrix(PhaseVocoder *const self,
                                                      const AudioMatrix *const audioMatrix);
    PhaseVocoderData *PhaseVocoder_analyseAudioObject(PhaseVocoder *const self,
                                                      const AudioObject *const audioObject);
    OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(PhaseVocoder *const self,
                                                               const char *filePath);
    
    OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(const char *filePath,
                                                               size_t FFTFrameSize,
                                                               size_t hopSize);
    
    void PhaseVocoder_synthesise(PhaseVocoder *self,
                                 Float64 *magnitudes,
                                 Float64 *frequencies,
                                 Float64 *output);

    void PhaseVocoder_synthesiseFrame(PhaseVocoder *const self,
                                      PhaseVocoderData *phaseVocoderData,
                                      Float64 frameNumber,
                                      Float64 *output);
    
    void PhaseVocoder_getPhaseVocoderDataForFrame(PhaseVocoder *const self,
                                                  PhaseVocoderData *phaseVocoderData,
                                                  Float64 frameNumber,
                                                  Float64 **magnitudesOut,
                                                  Float64 **frequenciesOut);
    
    void PhaseVocoder_inverse(PhaseVocoder *const self,
                              PhaseVocoderData *phaseVocoderDataIn,
                              AudioMatrix *audioMatrixOut,
                              Float64 increment);

    PhaseVocoderData *PhaseVocoderData_new(const size_t frameCount,
                                           const size_t FFTFrameSize);
    void PhaseVocoderData_delete(PhaseVocoderData *const self);
    void PhaseVocoderData_scopedDelete(PhaseVocoderData **self);

#define _PhaseVocoderData __attribute__((unused)) __attribute__((cleanup(PhaseVocoderData_scopedDelete))) PhaseVocoderData

    
#ifdef __cplusplus
}
#endif
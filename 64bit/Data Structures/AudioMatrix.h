//
//  AudioMatrix.h
//  Matrix
//
//  Created by Edward Costello on 05/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "Matrix.h"
#import "AudioObject.h"
#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct AudioMatrix
    {
        Matrix *matrix;
        size_t channelCount;
        size_t sampleCount;
        size_t samplerate;
        size_t currentSamplePosition;
        
    } AudioMatrix;
    
    OVERLOADED AudioMatrix *AudioMatrix_new(size_t channelCount,
                                            size_t sampleCount,
                                            size_t samplerate);
    
    OVERLOADED AudioMatrix *AudioMatrix_new(const char *filePath);
    OVERLOADED AudioMatrix *AudioMatrix_new(const AudioObject *const audioObject);
    
    void AudioMatrix_delete(AudioMatrix *self);
    void AudioMatrix_scopedDelete(AudioMatrix **self);
    
#define _AudioMatrix __attribute__((unused)) __attribute__((cleanup(AudioMatrix_scopedDelete))) AudioMatrix
    
#pragma mark Audio Playback
    
    OVERLOADED AudioMatrix *AudioMatrix_new(const AudioObject *const audioObject);
    OVERLOADED AudioMatrix *AudioMatrix_new(const char *audioFile);
    
    void AudioMatrix_readCallback(AudioStream *audioStream,
                                  void *inRefCon,
                                  UInt32 inNumberFrames,
                                  Float64 **audioData);
    
#ifdef __cplusplus
}
#endif
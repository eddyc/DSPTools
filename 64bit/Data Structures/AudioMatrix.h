//
//  AudioMatrix.h
//  Matrix
//
//  Created by Edward Costello on 05/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"
#import "Matrix.h"
#import "AudioObject.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /*!
     @class AudioMatrix
     @abstract Object for working with audio samples derives from Matrix.
     @param matrix The Matrix struct which stores the samples
     @param channelCount The number of channels in the audio matrix
     @param sampleCount The number of samples in the audio matrix
     @param samplerate The samplerate of the audio matrix
     @param currentSamplePosition The current position of the playback head
     */
    
    typedef struct AudioMatrix
    {
        Matrix *matrix;
        size_t channelCount;
        size_t sampleCount;
        size_t samplerate;
        size_t currentSamplePosition;
        
    } AudioMatrix;
    
    /**
     *  Constructor for AudioMatrix object
     *
     *  @param channelCount The specified number of channels
     *  @param sampleCount  The specified number of samples
     *  @param samplerate   The specified samplerate
     *
     *  @return A pointer to an AudioMatrix object
     */
    OVERLOADED AudioMatrix *AudioMatrix_new(size_t channelCount,
                                            size_t sampleCount,
                                            size_t samplerate);
    
    /**
     *  Constructor for AudioMatrix object
     *
     *  @param filePath The filepath of the audio file to open
     *
     *  @return A pointer to an AudioMatrix object
     */
    OVERLOADED AudioMatrix *AudioMatrix_new(const char *filePath);
    
    /**
     *  Constructor for AudioMatrix object
     *
     *  @param audioObject A pointer to an AudioObject instance to convert to an AudioMatrix
     *
     *  @return A pointer to an AudioMatrix object
     */
    OVERLOADED AudioMatrix *AudioMatrix_new(const AudioObject *const audioObject);
    
    /**
     *  Definition used to construct a scoped AudioMatrix pseudoclass
     */
#define _AudioMatrix __attribute__((unused)) __attribute__((cleanup(AudioMatrix_scopedDelete))) AudioMatrix

    /**
     *  Destroy an AudioMatrix instance
     *
     *  @param self Pointer to an AudioMatrix instance
     */
    void AudioMatrix_delete(AudioMatrix *self);
    
    /**
     *  Scoped destroy an AudioMatrix instance
     *
     *  @param self Pointer to an AudioMatrix instance
     */
    void AudioMatrix_scopedDelete(AudioMatrix **self);
    
    

#pragma mark Audio Playback

    /**
     *  Play callback for AudioStream object
     *
     *  @param audioStream    Instance of an AudioStream object
     *  @param inRefCon       Opaque pointer to AudioMatrix object
     *  @param inNumberFrames Amount of samples to process
     *  @param audioData      Sample buffers
     */
    void AudioMatrix_readCallback(AudioStream *audioStream,
                                  void *inRefCon,
                                  UInt32 inNumberFrames,
                                  Float64 **audioData);
    
#ifdef __cplusplus
}
#endif
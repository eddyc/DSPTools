//
//  AudioInput.h
//  Spectrogram
//
//  Created by Edward Costello on 16/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"
#import "AudioStream.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class AudioInput
     * @abstract Audio line input class for use with AudioStream object
     * @param timeStamp The time stamp which is used to correct ringbuffer offset.
     * @param inputUnit The audiounit which is used for playback
     * @param inputBuffer Buffer for audio input samples
     **/
    typedef struct AudioInput
    {
        AudioTimeStamp timeStamp;
        AudioUnit inputUnit;
        AudioBufferList *inputBuffer;
    } AudioInput;
    
    /**
     *  Constructor for AudioInput pseudoclass
     *
     *  @return A pointer to an instance of an AudioInput pseudoclass
     */
    AudioInput *AudioInput_new();
    
    /**
     *  Definition used to construct a scoped AudioInput pseudoclas
     *
     *  @return A scoped AudioInput pseudoclass
     */
#define _AudioInput __attribute__((unused)) __attribute__((cleanup(AudioInput_scopedDelete))) AudioInput
    
    /**
     *  Deletes an instance of an AudioInput pseudoclass
     *
     *  @param self A pointer to an instance of an AudioInput pseudoclass
     */
    void AudioInput_delete(AudioInput *self);
    
    /**
     *  Scoped destructor for AudioInput pseudoclass
     *
     *  @param self reference to a reference of an AudioInput pseudoclass
     */
    void AudioInput_scopedDelete(AudioInput **self);

    /**
     *  AudioInput_readCallback
     *
     *  @param audioStream    AudioStream pointer
     *  @param inRefCon       void pointer cast to AudioInput
     *  @param inNumberFrames Number of samples to process
     *  @param audioData      Audio samples buffers
     */
    void AudioInput_readCallback(AudioStream *audioStream,
                                 void *inRefCon,
                                 const UInt32 inNumberFrames,
                                 Float64 **audioData);
    
    /**
     *  AudioUnit render process for AudioInput object
     */
    OSStatus InputRenderProc(void *inRefCon,
                             AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp,
                             UInt32 inBusNumber,
                             UInt32 inNumberFrames,
                             AudioBufferList * ioData);
#ifdef __cplusplus
}
#endif
//
//  AudioInput.h
//  Spectrogram
//
//  Created by Edward Costello on 16/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import <CoreAudio/CoreAudio.h>
#import "AudioStream.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct AudioInput
    {

        AudioTimeStamp timeStamp;
        AudioUnit inputUnit;
        AudioBufferList *inputBuffer;
    } AudioInput;
    
    AudioInput *AudioInput_new();
    void AudioInput_delete(AudioInput *self);
    void AudioInput_scopedDelete(AudioInput **self);
    
#define _AudioInput __attribute__((unused)) __attribute__((cleanup(AudioInput_scopedDelete))) AudioInput
    
    void AudioInput_readCallback(AudioStream *audioStream,
                                 void *inRefCon,
                                 const UInt32 inNumberFrames,
                                 Float64 **audioData);
    
    OSStatus InputRenderProc(void *inRefCon,
                             AudioUnitRenderActionFlags *ioActionFlags,
                             const AudioTimeStamp *inTimeStamp,
                             UInt32 inBusNumber,
                             UInt32 inNumberFrames,
                             AudioBufferList * ioData);
#ifdef __cplusplus
}
#endif
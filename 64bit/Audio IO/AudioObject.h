//
//  AudioObject.h
//  AudioMosaicing
//
//  Created by Edward Costello on 18/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <AudioToolbox/AudioToolbox.h>
#import "AudioStream.h"
#define AudioObject_monoChannel -1

#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    
    /*!
     @class AudioObject
     @abstract Object for opening, storing and saving audio samples
     @var audioChannels
     Pointers to audio channels sample data.
     @var monoChannel
     Pointer to mono channel sample data.
     @var channelCount
     Number of channels of audio data.
     @var samplerate
     Sample rate of the audio data.
     @var frameCount
     Number of samples of audio data in each channel.
     @var filePath
     Path to audio file, if applicable, that the audio samples are opened from.
     @var currentReadPosition
     Acts as a play head for when reading audio, represents the current read position
     @var fileStreamDescription
     AudioStreamBasicDescription of audio file audio samples are opened from.
     @var processStreamDescription
     AudioStreamBasicDescription of audio samples when stored in memory.
     */
    
    typedef struct _AudioObject
    {
        Float64 **audioChannels;
        Float64 *monoChannel;
        
        UInt16 channelCount;
        size_t samplerate;
        size_t frameCount;
        char *filePath;
        size_t currentReadPosition;
        AudioStreamBasicDescription fileStreamDescription;
        AudioStreamBasicDescription processStreamDescription;
        
    } AudioObject;
    
    /*!
     @functiongroup Construct/Destruct
     */
    
    /*!
     Construct a AudioObject pseudoclass.
     @return
     Returns a AudioObject pseudoclass.
     */
    
    OVERLOADED AudioObject *AudioObject_new(void);

    OVERLOADED AudioObject *AudioObject_new(const char *const path);


    /*!
     Destroy an AudioObject pseudoclass.
     @param self
     Pointer to an AudioObject pseudoclass.
     */
    
    void AudioObject_delete(AudioObject *self);
    
    /*!
     Destroy an AudioObject pseudoclass using cleanup LLVM attribute.
     @param self
     Pointer to an AudioObject pseudoclass.
     */
    void AudioObject_scopedDelete(AudioObject **self);

#define _AudioObject __attribute__((unused)) __attribute__((cleanup(AudioObject_scopedDelete))) AudioObject

    /*!
     @functiongroup Open/Save Files
     */
    
    /*!
     Open an audio file and copy the file description and audio samples to an AudioObject pseudoclass.
     @param self
     Pointer to an AudioObject pseudoclass.
     @param path
     File system path to the audio file which is to be opened.
     */
    
    void AudioObject_openAudioFile(AudioObject *self,
                                   const char *path);
    
    void AudioObject_saveAudioFile(const AudioObject *self,
                                   const char *path);
    
    /*!
     @functiongroup Read/Write Samples
     */
    
    /*!
     Read samples from a specified channel in an AudioObject pseudoclass and copy to the output variable.
     @param self
     Pointer to an AudioObject pseudoclass.
     @param readOffset
     The sample offset in the AudioObject pseudoclass where reading begins.
     @param channel
     The AudioObject channel of samples to read.
     @param output
     Destination sample buffer where the read samples are copied.
     @param outputSize
     Size in samples of the destination sample buffer
     */
    

    void AudioObject_readSamples(const AudioObject *self,
                                 const SInt32 readOffset,
                                 const SInt32 channel,
                                 Float64 *output,
                                 const size_t outputSize);
    
    /*!
     @functiongroup Read Callbacks
     */
    
    /*!
     Read callback used for reading samples from a AudioObject pseudoclass in stereo which is called by an AudioStream32 pseudoclass. This callback uses the currentReadPosition member to provide the start reading position. This member is then incremented by inNumberFrames.
     @param inNumberFrames
     The specified number of frames to read from the file.
     @param audioData
     Pointers to the audio sample data.
     */
    
    void AudioObject_readCallbackStereo(AudioStream *audioStream,
                                        void *inRefCon,
                                        const UInt32 inNumberFrames,
                                        Float64 **audioData);
    
    /*!
     Read callback used for reading samples from a AudioObject pseudoclass in mono which is called by an AudioStream32 pseudoclass. This callback uses the currentReadPosition member to provide the start reading position. This member is then incremented by inNumberFrames.
     @param inNumberFrames
     The specified number of frames to read from the file.
     @param audioData
     Pointers to the audio sample data.
     */
    void AudioObject_readCallbackMono(AudioStream *audioStream,
                                      void *inRefCon,
                                      const UInt32 inNumberFrames,
                                      Float64 **audioData);
    
    
#ifdef __cplusplus
}
#endif
//
//  AudioStream.h
//  AudioMosaicing
//
//  Created by Edward Costello on 18/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <AudioToolbox/AudioToolbox.h>

#ifndef OVERLOADED
#define OVERLOADED __attribute__((overloadable))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    
    /*!
     @class AudioStream
     @abstract Object for playing back streams of audio samples.
     @var outputUnit
     The output AudioUnit which is used to play back the audio.
     @var stereoBuffer
     A sample buffer the same size as a callback inNumberFrames variable used to store samples.
     @var samplesPerCallback
     Number of samples that are processed in one callback
     @var readObject
     A pointer to the object storing the samples to be played back. If NULL is used as an argument the AudioStream pseudoclass will use a pointer to self as the argument.
     @var readCallback
     A pointer to the function used by the readObject variable to copy samples into the stereoBuffer. If NULL is used as an argument the AudioStream pseudoclass will use a pointer to AudioStream_emptyCallback as the argument.
     @var processingObject
     A pointer to the object that will process the current samples block. If NULL is used as an argument the AudioStream pseudoclass will use a pointer to self as the argument.
     @var processingCallback
     A pointer to the function used by the processingObject variable that performs the sample processing.
     */
    
    typedef struct AudioStream
    {
        AudioUnit outputUnit;
        Float64 **stereoBuffer;
        size_t samplesPerCallback;
        AudioUnitRenderActionFlags *ioActionFlags;
        const AudioTimeStamp *inTimeStamp;
        AudioBufferList *ioData;
        UInt32 inBusNumber;
        void *readObject;
        void (*readCallback)();
        void *processingObject;
        void (*processingCallback)();
        volatile bool isProcessing;
    } AudioStream;
    
    /*!
     @functiongroup Construct/Destruct
     */
    
    /*!
     Construct an AudioStream pseudoclass.
     @param readObject
     A pointer to the object storing the samples to be played back.
     @param readCallback
     A pointer to the function used by the readObject variable to copy samples into the stereoBuffer.
     @param processingObject
     A pointer to the object that will process the current samples block.
     @param processingCallback
     A pointer to the function used by the processingObject variable that performs the sample processing.
     @return
     Returns an AudioStream pseudoclass.
     */
    

    AudioStream *AudioStream_new(void *readObject,
                                 void (*readCallback)(),
                                 void *processingObject,
                                 void (*processingCallback)());
    
    
    /*!
     Destroy an AudioStream pseudoclass.
     @param self
     Pointer to an AudioStream pseudoclass.
     */
    
    void AudioStream_delete(AudioStream *self);
    void AudioStream_scopedDelete(AudioStream **self);

#define _AudioStream __attribute__((unused)) __attribute__((cleanup(AudioStream_scopedDelete))) AudioStream

    /*!
     Configure the AudioUnit structure used for audio output.
     @param self
     Pointer to an AudioStream pseudoclass.
     */
    
    void AudioStream_configureAudioUnit(AudioStream *self);
    
    /*!
     Start the audio processing callback.
     @param self
     Pointer to an AudioStream pseudoclass.
     @param time
     Audio processing time in seconds.
     */
    



    OVERLOADED void AudioStream_process(AudioStream *self,
                                        const Float64 time);
    
    /*!
     An empty audio processing callback used when specified processing callback in constructor is NULL.
     @param inRefCon
     Pointer to an AudioStream pseudoclass.
     @param inNumberFrames
     The specified number of frames to process.
     @param audioData
     Pointers to the audio sample data.
     */
    
    void AudioStream_emptyCallback(AudioStream *self,
                                          const void *inRefCon,
                                          const UInt32 inNumberFrames,
                                   const Float64 **audioData);
    
    /*!
     Write the contents of the audio buffer to the AudioBufferList provide by the AudioUnit callback.
     @param self
     Pointer to an AudioStream pseudoclass.
     @param audioData
     Pointers to the audio sample data.
     @param ioData
     Pointer to the AudioBufferList provided by the AudioUnit callback.
     */
    void AudioStream_writeBufferToOutput(const AudioStream *self,
                                                const AudioBufferList *ioData);
    
#ifdef __cplusplus
}
#endif
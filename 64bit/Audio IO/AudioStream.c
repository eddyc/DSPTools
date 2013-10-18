//
//  AudioStream.c
//  AudioMosaicing
//
//  Created by Edward Costello on 18/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "AudioStream.h"
#import <Accelerate/Accelerate.h>

AudioStream *AudioStream_new(void *readObject,
                             void (*readCallback)(),
                             void *processingObject,
                             void (*processingCallback)())
{
    AudioStream *self = calloc(1, sizeof(AudioStream));
    self->samplesPerCallback = 512;
    
    self->readObject = readObject == NULL ? self : readObject;
    self->readCallback = readCallback == NULL ? AudioStream_emptyCallback : readCallback;
    self->processingObject = processingObject == NULL ? self : processingObject;
    self->processingCallback = processingCallback == NULL ? AudioStream_emptyCallback : processingCallback;
    
    self->stereoBuffer = calloc(2, sizeof(Float64 *));
    
    for (size_t i = 0; i < 2; ++i) {
        
        self->stereoBuffer[i] = calloc(self->samplesPerCallback, sizeof(Float64));
    }
    
    AudioStream_configureAudioUnit(self);
    

    
    
    return self;
}


void AudioStream_delete(AudioStream *self)
{
    AudioOutputUnitStop(self->outputUnit);
    AudioComponentInstanceDispose(self->outputUnit);
    
    for (size_t i = 0; i < 2; ++i) {
        
        free(self->stereoBuffer[i]);
    }
    
    free(self->stereoBuffer);
    free(self);
    self = NULL;
}

void AudioStream_scopedDelete(AudioStream **self)
{
    AudioStream_delete(*self);
}


inline void AudioStream_writeBufferToOutput(const AudioStream *self,
                                                   const AudioBufferList *ioData)
{
    
    for (size_t channel = 0; channel < ioData->mNumberBuffers; ++channel) {
        
        Float64 *currentBuffer = ioData->mBuffers[channel].mData;
        
        cblas_dcopy((SInt32)self->samplesPerCallback,
                    self->stereoBuffer[channel],
                    1,
                    currentBuffer,
                    1);
    }
}


static OSStatus AudioStream_RenderProc(void *inRefCon,
                                       AudioUnitRenderActionFlags *ioActionFlags,
                                       const AudioTimeStamp *inTimeStamp,
                                       UInt32 inBusNumber,
                                       UInt32 inNumberFrames,
                                       AudioBufferList *ioData)
{
    AudioStream *self = (AudioStream *) inRefCon;
    self->ioActionFlags = ioActionFlags;
    self->inTimeStamp = inTimeStamp;
    self->inBusNumber = inBusNumber;
    self->ioData = ioData;
    self->readCallback(self, self->readObject, inNumberFrames, self->stereoBuffer);
    self->processingCallback(self, self->processingObject, inNumberFrames, self->stereoBuffer);
    
    AudioStream_writeBufferToOutput(self, ioData);
    
    return noErr;
}

static void AudioStream_checkError(OSStatus error, const char *operation)
{
	if (error == noErr) return;
	
	char str[20];
    
	*(UInt32 *)&str[1] = CFSwapInt32HostToBig(error);
    
	if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4])) {
        
		str[0] = str[5] = '\'';
		str[6] = '\0';
	}
    else {
        
		sprintf(str, "%d", (int)error);
    }
    printf("Error: %s failed (%s)\n", operation, str);
    
    exit(1);
}


void AudioStream_configureAudioUnit(AudioStream *self)
{
    OSStatus result = noErr;
    
    AudioComponentDescription outputComponentDescription = {
        
        .componentType = kAudioUnitType_Output,
        .componentSubType = kAudioUnitSubType_DefaultOutput,
        .componentManufacturer = kAudioUnitManufacturer_Apple
        
    };
    
    AudioComponent audioComponent = AudioComponentFindNext (NULL, &outputComponentDescription);
    
    if (audioComponent == NULL) {
        
        printf ("can't get output unit");
        exit (-1);
    }
    
    result = AudioComponentInstanceNew(audioComponent,
                                       &self->outputUnit);
    
    AudioStream_checkError(result, "Couldn't open component for outputUnit");
    
    AURenderCallbackStruct renderCallbackStruct = {
        
        .inputProc = AudioStream_RenderProc,
        .inputProcRefCon = self
    };
    
    result = AudioUnitSetProperty(self->outputUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Input,
                                  0,
                                  &renderCallbackStruct,
                                  sizeof(renderCallbackStruct));
    
    AudioStream_checkError(result, "AudioUnitSetProperty kAudioUnitProperty_SetRenderCallback failed");
    
    AudioStreamBasicDescription streamFormat = {
        
        .mFormatID = kAudioFormatLinearPCM,
        .mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved,
        .mBytesPerPacket = sizeof(Float64),
        .mBytesPerFrame = sizeof(Float64),
        .mFramesPerPacket = 1,
        .mBitsPerChannel = 64,
        .mChannelsPerFrame = 2,
        .mSampleRate = 44100.,
    };
    
    result = AudioUnitSetProperty(self->outputUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &streamFormat,
                                  sizeof(streamFormat));
    
    AudioStream_checkError(result, "AudioUnitSetProperty kAudioUnitProperty_StreamFormat failed");
    
    result = AudioUnitInitialize(self->outputUnit);
    
    AudioStream_checkError(result, "Couldn't initialize output unit");
}

OVERLOADED void AudioStream_process(AudioStream *self,
                                    const Float64 time)
{
    self->isProcessing = true;
    AudioOutputUnitStart(self->outputUnit);
    
    usleep(time * 1e+6);
    
    self->isProcessing = false;
    AudioOutputUnitStop(self->outputUnit);
}

void AudioStream_emptyCallback(AudioStream *self,
                               const void *inRefCon,
                               const UInt32 inNumberFrames,
                               const Float64 **audioData)
{
}


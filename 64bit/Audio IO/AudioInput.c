//
//  AudioInput.c
//  Spectrogram
//
//  Created by Edward Costello on 16/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "AudioInput.h"
#import <Accelerate/Accelerate.h>

static void CheckError(OSStatus error, const char *operation)
{
	if (error == noErr) return;
	
	char str[20];
	// see if it appears to be a 4-char-code
	*(UInt32 *)(str + 1) = CFSwapInt32HostToBig(error);
	if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4])) {
		str[0] = str[5] = '\'';
		str[6] = '\0';
	} else
		// no, format it as an integer
		sprintf(str, "%d", (int)error);
	
	fprintf(stderr, "Error: %s (%s)\n", operation, str);
	
	exit(1);
}

AudioInput *AudioInput_new()
{
    AudioInput *self = calloc(1, sizeof(AudioInput));
    
    AudioComponentDescription inputComponentDescription = {0};
    inputComponentDescription.componentType = kAudioUnitType_Output;
    inputComponentDescription.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
    inputComponentDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    AudioComponent audioComponent = AudioComponentFindNext(NULL,
                                                           &inputComponentDescription);
    if (audioComponent == NULL) {
        printf ("can't get output unit");
        exit (-1);
    }
    
    OSStatus result = AudioComponentInstanceNew(audioComponent,
                                                &self->inputUnit);
    
    CheckError(result, "Couldn't open component for inputUnit");
    
    UInt32 disableFlag = 0;
    UInt32 enableFlag = 1;
    AudioUnitScope outputBus = 0;
    AudioUnitScope inputBus = 1;
    
//    result = AudioUnitSetProperty(self->inputUnit,
//                                  kAudioOutputUnitProperty_EnableIO,
//                                  kAudioUnitScope_Global,
//                                  inputBus,
//                                  &enableFlag,
//                                  sizeof(enableFlag));
//    
//    CheckError (result, "Couldn't enable input on I/O unit");
//    
//    result = AudioUnitSetProperty(self->inputUnit,
//                                  kAudioOutputUnitProperty_EnableIO,
//                                  kAudioUnitScope_Output,
//                                  outputBus,
//                                  &enableFlag,	// well crap, have to disable
//                                  sizeof(enableFlag));
//    
//    CheckError (result, "Couldn't disable output on I/O unit");
//    
    
    AudioDeviceID defaultDevice = kAudioObjectUnknown;
    UInt32 propertySize = sizeof (defaultDevice);
    
    
    AudioObjectPropertyAddress defaultDeviceProperty;
    defaultDeviceProperty.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    defaultDeviceProperty.mScope = kAudioObjectPropertyScopeGlobal;
    defaultDeviceProperty.mElement = kAudioObjectPropertyElementMaster;
    
    result = AudioObjectGetPropertyData(kAudioObjectSystemObject,
                                        &defaultDeviceProperty,
                                        0,
                                        NULL,
                                        &propertySize,
                                        &defaultDevice);
    CheckError (result, "Couldn't get default input device");
    
    result = AudioUnitSetProperty(self->inputUnit,
                                  kAudioOutputUnitProperty_CurrentDevice,
                                  kAudioUnitScope_Global,
                                  outputBus,
                                  &defaultDevice,
                                  sizeof(defaultDevice));
    
    CheckError(result, "Couldn't set default device on I/O unit");
    
    propertySize = sizeof (AudioStreamBasicDescription);
    
    
    CheckError(result, "Couldn't get ASBD from input unit");
    
    AudioStreamBasicDescription deviceFormat;
    
    result = AudioUnitGetProperty(self->inputUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  inputBus,
                                  &deviceFormat,
                                  &propertySize);
    
    CheckError(result, "Couldn't get ASBD from input unit");
        
    propertySize = sizeof (AudioStreamBasicDescription);
    
    AudioStreamBasicDescription streamFormat = {
        
        .mFormatID = kAudioFormatLinearPCM,
        .mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved,
        .mBytesPerPacket = sizeof(Float32),
        .mBytesPerFrame = sizeof(Float32),
        .mFramesPerPacket = 1,
        .mBitsPerChannel = 32,
        .mChannelsPerFrame = 2,
        .mSampleRate = 44100.,
    };
    result =  AudioUnitSetProperty(self->inputUnit,
                                   kAudioUnitProperty_StreamFormat,
                                   kAudioUnitScope_Output,
                                   inputBus,
                                   &streamFormat,
                                   propertySize);
    
    CheckError(result, "Couldn't set ASBD on input unit");
    
    UInt32 bufferSizeFrames = 0;
    propertySize = sizeof(UInt32);
    
    result = AudioUnitGetProperty(self->inputUnit,
                                  kAudioDevicePropertyBufferFrameSize,
                                  kAudioUnitScope_Global,
                                  0,
                                  &bufferSizeFrames,
                                  &propertySize);
    
    CheckError (result, "Couldn't get buffer frame size from input unit");
    
    UInt32 bufferSizeBytes = bufferSizeFrames * sizeof(Float32);
    
    if (streamFormat.mFormatFlags & kAudioFormatFlagIsNonInterleaved) {
        
        UInt32 propsize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * streamFormat.mChannelsPerFrame);
        
        self->inputBuffer = (AudioBufferList *)malloc(propsize);
        self->inputBuffer->mNumberBuffers = streamFormat.mChannelsPerFrame;
        
        for(UInt32 i =0; i< self->inputBuffer->mNumberBuffers ; i++) {
            
            self->inputBuffer->mBuffers[i].mNumberChannels = 1;
            self->inputBuffer->mBuffers[i].mDataByteSize = bufferSizeBytes;
            self->inputBuffer->mBuffers[i].mData = malloc(bufferSizeBytes);
        }
        
    } else {
        
        UInt32 propsize = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * 1);
        
        self->inputBuffer = (AudioBufferList *)malloc(propsize);
        self->inputBuffer->mNumberBuffers = 1;
        
        self->inputBuffer->mBuffers[0].mNumberChannels = streamFormat.mChannelsPerFrame;
        self->inputBuffer->mBuffers[0].mDataByteSize = bufferSizeBytes;
        self->inputBuffer->mBuffers[0].mData = malloc(bufferSizeBytes);
    }
    
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = InputRenderProc;
    callbackStruct.inputProcRefCon = self;
    
    result = AudioUnitSetProperty(self->inputUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Global,
                                  0,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
//
    CheckError(result, "Couldn't set input callback");
    
    result = AudioUnitInitialize(self->inputUnit);
    result = AudioOutputUnitStart(self->inputUnit);
    return self;
}

void AudioInput_delete(AudioInput *self)
{
    AudioUnitUninitialize(self->inputUnit);
    AudioComponentInstanceDispose(self->inputUnit);
    
    for(UInt32 i = 0; i < self->inputBuffer->mNumberBuffers; ++i) {
        
        free(self->inputBuffer->mBuffers[i].mData);
    }
    free(self->inputBuffer);
    free(self);
    self = NULL;
}

void AudioInput_scopedDelete(AudioInput **self)
{
    AudioInput_delete(*self);
}


void AudioInput_readCallback(AudioStream *audioStream,
                             void *inRefCon,
                             const UInt32 inNumberFrames,
                             Float64 **audioData)
{
    AudioInput *self = inRefCon;

	OSStatus result = noErr;
    
	result = AudioUnitRender(self->inputUnit,
                             audioStream->ioActionFlags,
                             audioStream->inTimeStamp,
                             audioStream->inBusNumber,
                             inNumberFrames,
                             self->inputBuffer);
    
    Float64 *inputChannel =  self->inputBuffer->mBuffers[0].mData;
    printf("%f\n", inputChannel[0]);
    
}

OSStatus InputRenderProc(void *inRefCon,
						 AudioUnitRenderActionFlags *ioActionFlags,
						 const AudioTimeStamp *inTimeStamp,
						 UInt32 inBusNumber,
						 UInt32 inNumberFrames,
						 AudioBufferList *ioData)
{
    AudioInput *self = inRefCon;
    
	OSStatus result = noErr;
    
	result = AudioUnitRender(self->inputUnit,
                             ioActionFlags,
                             inTimeStamp,
                             inBusNumber,
                             inNumberFrames,
                             self->inputBuffer);
    
    Float32 *inputChannel =  self->inputBuffer->mBuffers[0].mData;
    printf("%f\n", inputChannel[0]);
    
    cblas_scopy(inNumberFrames, inputChannel, 1, ioData->mBuffers[0].mData, 1);
    
	return result;
}

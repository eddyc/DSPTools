//
//  AudioObject.c
//  AudioMosaicing
//
//  Created by Edward Costello on 18/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "AudioObject.h"
#import <Accelerate/Accelerate.h>
#import <string.h>

OVERLOADED AudioObject *AudioObject_new(void)
{
    AudioObject *self = calloc(1, sizeof(AudioObject));
    return self;
}

OVERLOADED AudioObject *AudioObject_new(const char *const path)
{
    AudioObject *self = AudioObject_new();
    AudioObject_openAudioFile(self, path);
    return self;
}


void AudioObject_delete(AudioObject *self)
{
    if (self->channelCount >= 2) {
        
        if (self->monoChannel != NULL) {
            
            free(self->monoChannel);
        }
    }
    
    for (size_t i = 0; i < self->channelCount; ++i) {
        
        free(self->audioChannels[i]);
    }
    
    free(self->audioChannels);
    free(self->filePath);
    free(self);
    self = NULL;
}

void AudioObject_scopedDelete(AudioObject **self)
{
    AudioObject_delete(*self);
}


static void AudioObject_checkError(OSStatus error, const char *operation)
{
	if (error == noErr) return;
	
	char str[20];
    // see if it appears to be a 4-char-code
	*(UInt32 *)&str[1] = CFSwapInt32HostToBig(error);
    
	if (isprint(str[1]) && isprint(str[2]) && isprint(str[3]) && isprint(str[4])) {
		str[0] = str[5] = '\'';
		str[6] = '\0';
	} else
        // no, format it as an integer
		sprintf(str, "%d", (int)error);
    
    printf("Error: %s failed (%s)\n", operation, str);
    
    exit(1);
}

void AudioObject_openAudioFile(AudioObject *self, const char *path)
{
    
    size_t stringLength = strlen(path);
    self->filePath = calloc(stringLength + 1, sizeof(char));
    strcpy(self->filePath, path);
    OSStatus result = noErr;
    ExtAudioFileRef inputAudioFileReference = {0};
    
    CFStringRef pathAsCFString = CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);
    
    CFURLRef fileURLRef = CFURLCreateWithFileSystemPath(NULL, pathAsCFString, kCFURLPOSIXPathStyle, false);
    
    result = ExtAudioFileOpenURL(fileURLRef,
                                 &inputAudioFileReference);
    
    AudioObject_checkError(result, "ExtAudioFileOpenURL");
    
    
    UInt32 audioStreamDescriptionSize = sizeof(self->fileStreamDescription);
    
    result = ExtAudioFileGetProperty(inputAudioFileReference,
                                     kExtAudioFileProperty_FileDataFormat,
                                     &audioStreamDescriptionSize,
                                     &self->fileStreamDescription);
    
    AudioObject_checkError(result, "ExtAudioFileGetProperty kExtAudioFileProperty_FileDataFormat");
    
    self->samplerate = self->fileStreamDescription.mSampleRate;
    
    AudioStreamBasicDescription processStreamDescription = {
        
        .mChannelsPerFrame = self->fileStreamDescription.mChannelsPerFrame,
        .mBytesPerFrame = sizeof(Float64),
        .mBytesPerPacket = sizeof(Float64),
        .mFramesPerPacket = 1,
        .mBitsPerChannel = 64,
        .mFormatID = kAudioFormatLinearPCM,
        .mSampleRate = self->samplerate,
        .mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved
    };
    
    self->processStreamDescription = processStreamDescription;
    
    
    result = ExtAudioFileSetProperty(inputAudioFileReference,
                                     kExtAudioFileProperty_ClientDataFormat,
                                     sizeof(processStreamDescription),
                                     &processStreamDescription);
    
    AudioObject_checkError(result, "ExtAudioFileSetProperty kExtAudioFileProperty_ClientDataFormat");
    
    
    SInt64 frameCount;
    UInt32 frameCountSize = sizeof(SInt64);
    
    result = ExtAudioFileGetProperty(inputAudioFileReference,
                                     kExtAudioFileProperty_FileLengthFrames,
                                     &frameCountSize,
                                     &frameCount);
    
    
    AudioObject_checkError(result, "ExtAudioFileGetProperty kExtAudioFileProperty_FileLengthFrames");
    
    
    self->channelCount = self->fileStreamDescription.mChannelsPerFrame;
    self->frameCount = frameCount;
    self->audioChannels = calloc(self->channelCount, sizeof(AudioSampleType *));
    
    AudioBufferList *audioBufferList = (AudioBufferList *)malloc(sizeof(AudioBufferList) + sizeof(AudioBuffer) * (self->fileStreamDescription.mChannelsPerFrame - 1));
    
    audioBufferList->mNumberBuffers = self->fileStreamDescription.mChannelsPerFrame;
    
    for (UInt32 i = 0; i < audioBufferList->mNumberBuffers; ++i) {
        
        audioBufferList->mBuffers[i].mDataByteSize = (UInt32) (sizeof(Float64) * frameCount);
        audioBufferList->mBuffers[i].mNumberChannels = 1;
        
        self->audioChannels[i] = (Float64 *) calloc(frameCount, sizeof(Float64));
        audioBufferList->mBuffers[i].mData = self->audioChannels[i];
    }
    
    
    
    UInt32 frameCountUInt32 = (UInt32) frameCount;
    
    result = ExtAudioFileRead(inputAudioFileReference,
                              &frameCountUInt32,
                              audioBufferList);
    
    AudioObject_checkError(result, "ExtAudioFileRead");
    
    if (self->channelCount >= 2) {
        
        self->monoChannel = (Float64 *) calloc(frameCount, sizeof(Float64));
        
        for (size_t i = 0; i < self->channelCount; ++i) {
            
            vDSP_vaddD(self->monoChannel, 1, self->audioChannels[i], 1, self->monoChannel, 1, self->frameCount);
        }
        
        Float64 divisor = self->channelCount;
        vDSP_vsdivD(self->monoChannel, 1, &divisor, self->monoChannel, 1, self->frameCount);
    }
    else {
        
        self->monoChannel = self->audioChannels[1] = self->audioChannels[0];
    }
    
    
    result = ExtAudioFileDispose(inputAudioFileReference);
    
    AudioObject_checkError(result, "ExtAudioFileDispose");
    
    free(audioBufferList);
    audioBufferList = NULL;
    
}

void AudioObject_saveAudioFile(const AudioObject *self, const char *path)
{
    OSStatus result = noErr;
    CFStringRef pathAsCFString = CFStringCreateWithCString(NULL, path, kCFStringEncodingUTF8);
    
    CFURLRef fileURLRef = CFURLCreateWithFileSystemPath(NULL, pathAsCFString, kCFURLPOSIXPathStyle, false);
    
    ExtAudioFileRef outputAudioFileReference;
    AudioFileTypeID outputAudioFileTypeID = kAudioFileWAVEType;
    
    
    result = ExtAudioFileCreateWithURL(fileURLRef,
                                       outputAudioFileTypeID,
                                       &self->fileStreamDescription,
                                       NULL,
                                       kAudioFileFlags_EraseFile,
                                       &outputAudioFileReference);
    
    
    AudioObject_checkError(result, "ExtAudioFileCreateWithURL");
    
    AudioStreamBasicDescription processStreamDescription = {
        
        .mChannelsPerFrame = self->fileStreamDescription.mChannelsPerFrame,
        .mBytesPerFrame = sizeof(Float64),
        .mBytesPerPacket = sizeof(Float64),
        .mFramesPerPacket = 1,
        .mBitsPerChannel = 64,
        .mFormatID = kAudioFormatLinearPCM,
        .mSampleRate = self->samplerate,
        .mFormatFlags = kLinearPCMFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved
    };
    
    
    result = ExtAudioFileSetProperty(outputAudioFileReference,
                                     kExtAudioFileProperty_ClientDataFormat,
                                     sizeof(processStreamDescription),
                                     &processStreamDescription);
    
    AudioObject_checkError(result, "ExtAudioFileSetProperty kExtAudioFileProperty_ClientDataFormat");
    
    
    
    
    AudioBufferList *audioBufferList = (AudioBufferList *)malloc(sizeof(AudioBufferList) + sizeof(AudioBuffer) * (processStreamDescription.mChannelsPerFrame - 1));
    
    audioBufferList->mNumberBuffers = processStreamDescription.mChannelsPerFrame;
    
    for (UInt32 i = 0; i < audioBufferList->mNumberBuffers; ++i) {
        
        audioBufferList->mBuffers[i].mDataByteSize = (UInt32) (sizeof(AudioSampleType) * self->frameCount);
        audioBufferList->mBuffers[i].mNumberChannels = 1;
        audioBufferList->mBuffers[i].mData = self->audioChannels[i];
        
    }
    
    result = ExtAudioFileWrite(outputAudioFileReference,
                               (UInt32)self->frameCount,
                               audioBufferList);
    
    AudioObject_checkError(result, "ExtAudioFileWrite");
    
    ExtAudioFileDispose(outputAudioFileReference);
    
    free(audioBufferList);
    
    audioBufferList = NULL;
}


void AudioObject_readSamples(const AudioObject *self,
                             const SInt32 readOffset,
                             const SInt32 channel,
                             Float64 *output,
                             const size_t outputSize)
{
    Float64 *audioChannel;
    
    if (channel < self->channelCount) {
        
        if (channel == AudioObject_monoChannel) {
            
            audioChannel = self->monoChannel;
        }
        else {
            
            audioChannel = self->audioChannels[channel];
        }
    }
    else {
        
        printf("AudioObject_readSamples specified channel is invalid\nExiting\n");
        exit(-1);
    }
    
    if (readOffset < 0) {
        
        if ((readOffset + (SInt32)outputSize) < 0) {
            
            vDSP_vclrD(output, 1, outputSize);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= 0
                 &&
                 (readOffset + (SInt32)outputSize) < self->frameCount) {
            
            SInt32 negativeCount = -readOffset;
            SInt32 positiveCount = (SInt32)outputSize - negativeCount;
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(positiveCount, audioChannel, 1, &output[negativeCount], 1);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= self->frameCount) {
            
            SInt32 negativeCount = -readOffset;
            SInt32 positiveCount = (SInt32)self->frameCount;
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(positiveCount, audioChannel, 1, &output[negativeCount], 1);
            
            return;
        }
    }
    else if (readOffset >= 0 && readOffset < self->frameCount) {
        
        if ((readOffset + (SInt32)outputSize) < self->frameCount) {
            
            cblas_dcopy((UInt32)outputSize, &audioChannel[readOffset], 1, output, 1);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= self->frameCount) {
            
            SInt32 samplesCount = (SInt32)(self->frameCount - readOffset);
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(samplesCount, &audioChannel[readOffset], 1, output, 1);
            
            return;
        }
    }
    else if (readOffset >= self->frameCount) {
        
        vDSP_vclrD(output, 1, outputSize);
        
        return;
    }
}

void AudioObject_readCallbackStereo(AudioStream *audioStream, void *inRefCon, const UInt32 inNumberFrames, Float64 **audioData)
{
    AudioObject *self = (AudioObject *)inRefCon;
    
    for (size_t i = 0; i < self->channelCount; ++i) {
        
        AudioObject_readSamples(self,
                                (SInt32)self->currentReadPosition,
                                (SInt32)i,
                                audioData[i],
                                inNumberFrames);
    }
    
    self->currentReadPosition += inNumberFrames;
}

void AudioObject_readCallbackMono(AudioStream *audioStream,
                                  void *inRefCon,
                                  const UInt32 inNumberFrames,
                                  Float64 **audioData)
{
    AudioObject *self = (AudioObject *)inRefCon;
    
    for (size_t i = 0; i < self->channelCount; ++i) {
        
        AudioObject_readSamples(self,
                                (SInt32)self->currentReadPosition,
                                (SInt32)AudioObject_monoChannel,
                                audioData[i],
                                inNumberFrames);
    }
    
    self->currentReadPosition += inNumberFrames;
}

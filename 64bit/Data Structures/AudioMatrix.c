//
//  AudioMatrix.c
//  Matrix
//
//  Created by Edward Costello on 05/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "AudioMatrix.h"

OVERLOADED AudioMatrix *AudioMatrix_new(size_t channelCount,
                                        size_t sampleCount,
                                        size_t samplerate)
{
    AudioMatrix *self = calloc(1, sizeof(AudioMatrix));
    self->matrix = Matrix_new(channelCount, sampleCount);
    self->channelCount = channelCount;
    self->sampleCount = sampleCount;
    self->samplerate = samplerate;
    return self;
}

OVERLOADED AudioMatrix *AudioMatrix_new(const AudioObject *const audioObject)
{
    AudioMatrix *self;
    
    if (audioObject->channelCount == 1) {
        
        self = AudioMatrix_new(1, audioObject->frameCount, audioObject->samplerate);
        cblas_dcopy((UInt32)audioObject->frameCount, audioObject->audioChannels[0], 1, self->matrix->data, 1);
    }
    else {
        
        self = AudioMatrix_new(3, audioObject->frameCount, audioObject->samplerate);
        cblas_dcopy((UInt32)audioObject->frameCount, audioObject->audioChannels[0], 1, Matrix_getRow(self->matrix, 0), 1);
        cblas_dcopy((UInt32)audioObject->frameCount, audioObject->audioChannels[1], 1, Matrix_getRow(self->matrix, 1), 1);
        cblas_dcopy((UInt32)audioObject->frameCount, audioObject->monoChannel, 1, Matrix_getRow(self->matrix, 2), 1);
    }
    return self;
}


OVERLOADED AudioMatrix *AudioMatrix_new(const char *filePath)
{
    _AudioObject *audioObject = AudioObject_new(filePath);
    AudioMatrix *self = AudioMatrix_new(audioObject);
    return self;
}


void AudioMatrix_delete(AudioMatrix *self)
{
    Matrix_delete(self->matrix);
    free(self);
    self = NULL;
}

void AudioMatrix_scopedDelete(AudioMatrix **self)
{
    AudioMatrix_delete(*self);
}

void AudioMatrix_readSamples(const AudioMatrix *self,
                             const SInt32 readOffset,
                             const SInt32 channel,
                             Float64 *output,
                             const size_t outputSize)
{
    Float64 *audioChannel;
    
    if (channel < self->channelCount) {
        
        audioChannel = Matrix_getRow(self->matrix, channel);
    }
    else {
        
        printf("Matrix_readSamples specified channel is invalid\nExiting\n");
        exit(-1);
    }
    
    if (readOffset < 0) {
        
        if ((readOffset + (SInt32)outputSize) < 0) {
            
            vDSP_vclrD(output, 1, outputSize);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= 0
                 &&
                 (readOffset + (SInt32)outputSize) < self->sampleCount) {
            
            SInt32 negativeCount = -readOffset;
            SInt32 positiveCount = (SInt32)outputSize - negativeCount;
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(positiveCount, audioChannel, 1, &output[negativeCount], 1);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= self->sampleCount) {
            
            SInt32 negativeCount = -readOffset;
            SInt32 positiveCount = (SInt32)self->sampleCount;
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(positiveCount, audioChannel, 1, &output[negativeCount], 1);
            
            return;
        }
    }
    else if (readOffset >= 0 && readOffset < self->sampleCount) {
        
        if ((readOffset + (SInt32)outputSize) < self->sampleCount) {
            
            cblas_dcopy((UInt32)outputSize, &audioChannel[readOffset], 1, output, 1);
            
            return;
        }
        else if ((readOffset + (SInt32)outputSize) >= self->sampleCount) {
            
            SInt32 samplesCount = (SInt32)(self->sampleCount - readOffset);
            vDSP_vclrD(output, 1, outputSize);
            cblas_dcopy(samplesCount, &audioChannel[readOffset], 1, output, 1);
            
            return;
        }
    }
    else if (readOffset >= self->sampleCount) {
        
        vDSP_vclrD(output, 1, outputSize);
        
        return;
    }
}


void AudioMatrix_readCallback(AudioStream *audioStream,
                              void *inRefCon,
                              UInt32 inNumberFrames,
                              Float64 **audioData)
{
    AudioMatrix *self = inRefCon;
    
    if (self->channelCount == 1) {
        
        for (size_t i = 0; i < 2; ++i) {
            
            AudioMatrix_readSamples(self,
                                    (UInt32)self->currentSamplePosition,
                                    0,
                                    audioData[i],
                                    inNumberFrames);
        }
    }
    else {
        
        for (size_t i = 0; i < 2; ++i) {
            
            AudioMatrix_readSamples(self,
                                    (UInt32)self->currentSamplePosition,
                                    (UInt32)i,
                                    audioData[i],
                                    inNumberFrames);
        }
    }
    
    
    self->currentSamplePosition += inNumberFrames;
    
    if (self->currentSamplePosition >= self->sampleCount) {
        
        self->currentSamplePosition = 0;
    }
}


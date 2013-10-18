//
//  AudioAnalyser.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 28/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "AudioAnalyser.h"
#import "MathematicalFunctions.h"
#import "ConvenienceFunctions.h"

#pragma mark AudioAnalyser32

AudioAnalyser32 *AudioAnalyser32_new(size_t samplerate,
                                     size_t FFTFrameSize,
                                     size_t hopSize)
{
    AudioAnalyser32 *self = calloc(1, sizeof(AudioAnalyser32));
    self->FFTFrameSize = nextPowerOfTwo(FFTFrameSize);
    self->FFTFrameSizeOver2 = self->FFTFrameSize / 2;
    self->hopSize = hopSize;
    
    self->fft = FFT32_new(self->FFTFrameSize, kFFTWindowType_Hanning);
    self->mfcc = MFCC32_new(self->FFTFrameSize, samplerate);
    self->chromagram = Chromagram32_new(self->FFTFrameSize, samplerate);
    self->magnitudeBuffer = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->frameBuffer = calloc(self->FFTFrameSize, sizeof(Float32));
    self->mfccBuffer = calloc(self->mfcc->cepstralCoefficients, sizeof(Float32));
    self->chromagramBuffer = calloc(self->chromagram->nchr, sizeof(Float32));
    
    
    return self;
}

void AudioAnalyser32_delete(AudioAnalyser32 *self)
{
    FFT32_delete(self->fft);
    MFCC32_delete(self->mfcc);
    Chromagram32_delete(self->chromagram);

    free(self->magnitudeBuffer);
    free(self->frameBuffer);
    free(self->mfccBuffer);
    free(self->chromagramBuffer);
    free(self);
    self = NULL;
}

static void AudioAnalyser32_analyseBeats(AudioAnalyser32 *self,
                                         AudioObject *audioObject,
                                         AudioAnalysisData32 *analysisData,
                                         size_t maximumSegmentCount)
{
    BeatDetect32 *beatDetect = BeatDetect32_new(audioObject->samplerate, audioObject->frameCount);
    
    BeatDetect32_process(beatDetect, audioObject->channelMono, analysisData->beats->data, &analysisData->beats->columnCount);
    analysisData->beats->elementCount = analysisData->beats->columnCount * 2;
    
    Float32 *beatPositions = Matrix_getRow(analysisData->beats, 0);
    Float32 *beatLengths = Matrix_getRow(analysisData->beats, 1);
    size_t beatsCount = analysisData->beats->columnCount;
    
    for (size_t i = 0; i < analysisData->beats->columnCount - 1; ++i) {
        
        beatLengths[i] = beatPositions[i + 1] - beatPositions[i];
    }
    
    beatLengths[beatsCount - 1] = analysisData->hopCount - beatPositions[beatsCount - 1];
    
    Float32 maximumSongBeatSize;
    Float32 subDiviser = 0;
    vDSP_maxv(beatLengths, 1, &maximumSongBeatSize, beatsCount);
    
    if (maximumSongBeatSize < maximumSegmentCount) {
        
        BeatDetect32_delete(beatDetect);
        return;
    }
    
    Float32 *beatsWithEnd = calloc(beatsCount + 1, sizeof(Float32));
    cblas_scopy((SInt32)beatsCount, beatPositions, 1, beatsWithEnd, 1);
    beatsWithEnd[beatsCount] = analysisData->hopCount;

    while (maximumSongBeatSize > maximumSegmentCount) {
        
        maximumSongBeatSize = roundf(maximumSongBeatSize / 2);
        subDiviser++;
    }

    subDiviser = powf(2, subDiviser);
    UInt32 newBeatsCount = beatsCount * subDiviser;
    
    analysisData->beats->elementCount = newBeatsCount * 2;
    analysisData->beats->columnCount = newBeatsCount;
    beatPositions = Matrix_getRow(analysisData->beats, 0);
    beatLengths = Matrix_getRow(analysisData->beats, 1);
    
    Float32 *indexes = calloc(newBeatsCount, sizeof(Float32));
    Float32 *newBeats = calloc(newBeatsCount, sizeof(Float32));
    SInt32 *newBeatsAsInt = calloc(newBeatsCount, sizeof(SInt32));
    
    Float32 zero = 0;
    Float32 increment = 1./subDiviser;
    vDSP_vramp(&zero, &increment, indexes, 1, newBeatsCount);
    
    vDSP_vlint(beatsWithEnd, indexes, 1, newBeats, 1, newBeatsCount, beatsCount + 1);
    vDSP_vfixr32(newBeats, 1, newBeatsAsInt, 1, newBeatsCount);
    vDSP_vflt32(newBeatsAsInt, 1, newBeats, 1, newBeatsCount);
    
    cblas_scopy(newBeatsCount, newBeats, 1, beatPositions, 1);


    for (size_t i = 0; i < newBeatsCount - 1; ++i) {
        
        beatLengths[i] = beatPositions[i + 1] - beatPositions[i];
    }
    
    beatLengths[newBeatsCount - 1] = analysisData->hopCount - beatPositions[newBeatsCount - 1];

    free(newBeats);
    free(newBeatsAsInt);
    free(beatsWithEnd);
    free(indexes);

    BeatDetect32_delete(beatDetect);

}

void AudioAnalyser32_analyseAudioObject(AudioAnalyser32 *self,
                                        AudioObject *audioObject,
                                        AudioAnalysisData32 *analysisData,
                                        size_t maximumSegmentCount)
{
    
    for (size_t i = 0; i < analysisData->hopCount; ++i) {
        
        AudioObject_readSamples(audioObject, (SInt32)(i * self->hopSize), 0, self->frameBuffer, self->FFTFrameSize);
        FFT32_forwardMagnitudesWindowed(self->fft, self->frameBuffer, self->magnitudeBuffer);
        MFCC32_process(self->mfcc, self->magnitudeBuffer, Matrix_getRow(analysisData->mfccs, i));
        Chromagram32_process(self->chromagram, self->frameBuffer, Matrix_getRow(analysisData->chromagram, i));
    }
    
    AudioAnalyser32_analyseBeats(self, audioObject, analysisData, maximumSegmentCount);
}

void AudioAnalyser32_analyseAudioFrameToQueue(AudioAnalyser32 *self,
                                              Float32 *audioFrame,
                                              AudioAnalysisQueue32 *analysisQueue)
{
    size_t currentQueueFrame = analysisQueue->currentFrame;
    Float32 *currentMFCCFrame = Matrix_getRow(analysisQueue->mfccs, currentQueueFrame);
    Float32 *currentChromagramFrame = Matrix_getRow(analysisQueue->chromagram, currentQueueFrame);

    FFT32_forwardMagnitudesWindowed(self->fft, audioFrame, self->magnitudeBuffer);
    MFCC32_process(self->mfcc, self->magnitudeBuffer, currentMFCCFrame);
    Chromagram32_process(self->chromagram, audioFrame, currentChromagramFrame);

    analysisQueue->currentFrame = (analysisQueue->currentFrame + 1) % analysisQueue->frameCount;
//    printf("%zd\n", analysisQueue->currentFrame);
}
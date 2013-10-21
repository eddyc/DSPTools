//
//  PhaseVocoder.c
//  AudioMosaicing
//
//  Created by Edward Costello on 25/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "PhaseVocoder.h"
#import "../../Custom Functions/ConvenienceFunctions.h"

PhaseVocoder *PhaseVocoder_new(size_t FFTFrameSize,
                               size_t hopSize,
                               size_t samplerate)
{
    PhaseVocoder *self = calloc(1, sizeof(PhaseVocoder));
    
    self->samplerate = samplerate;
    self->FFTFrameSize = FFTFrameSize;
    self->hopSize = hopSize;
    self->fft = FFT_new(self->FFTFrameSize);
    self->window = calloc(self->FFTFrameSize, sizeof(Float64));
    self->tempFrame = calloc(self->FFTFrameSize, sizeof(Float64));
    self->tempSampleShiftFrame = calloc(self->FFTFrameSize, sizeof(Float64));
    self->currentPhases = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->previousPhases = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->twoPiArray = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->forwardFrequencyScale = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->inverseFrequencyScale = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->tempMagnitudes = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->tempFrequencies = calloc(self->FFTFrameSize / 2, sizeof(Float64));
    self->phasePosition = 0;
    Float64 twoPi = 2 * M_PI;
    vDSP_vfillD(&twoPi, self->twoPiArray, 1, self->FFTFrameSize / 2);
    vDSP_hann_windowD(self->window, self->FFTFrameSize, vDSP_HANN_DENORM);
    Float64 one = 1;
    Float64 halfFFTFrameSize = self->FFTFrameSize / 2;
    vDSP_vgenD(&one, &halfFFTFrameSize, self->forwardFrequencyScale, 1, self->FFTFrameSize / 2);
    vDSP_vgenD(&one, &halfFFTFrameSize, self->inverseFrequencyScale, 1, self->FFTFrameSize / 2);
    
    Float64 forwardScale = twoPi * self->hopSize / self->FFTFrameSize;
    vDSP_vsmulD(self->forwardFrequencyScale, 1, &forwardScale, self->forwardFrequencyScale, 1, self->FFTFrameSize / 2);
    
    Float64 inverseScale = (Float64)samplerate / self->FFTFrameSize;
    vDSP_vsmulD(self->inverseFrequencyScale, 1, &inverseScale, self->inverseFrequencyScale, 1, self->FFTFrameSize / 2);
    
    self->forwardFactor = (Float64)samplerate / ((Float64)self->hopSize * twoPi);
    self->inverseFactor = (Float64)self->hopSize * twoPi / (Float64)samplerate;
    
    return self;
}

void PhaseVocoder_delete(PhaseVocoder *self)
{
    free(self->window);
    free(self->tempFrame);
    free(self->tempSampleShiftFrame);
    free(self->currentPhases);
    free(self->previousPhases);
    free(self->forwardFrequencyScale);
    free(self->inverseFrequencyScale);
    free(self->tempFrequencies);
    free(self->tempMagnitudes);
    free(self->twoPiArray);
    FFT_delete(self->fft);
    free(self);
    self = NULL;
}

void PhaseVocoder_scopedDelete(PhaseVocoder **self)
{
    PhaseVocoder_delete(*self);
}


void PhaseVocoder_rotateTempFrame(PhaseVocoder *const self, bool forward)
{
    size_t sampleShift = self->phasePosition % self->FFTFrameSize;
    
    self->phasePosition += self->hopSize;
    self->phasePosition %= self->FFTFrameSize;
    
    if (sampleShift == 0) {
        
        return;
    }
    
    Float64 *pointerFlipTemp = self->tempFrame;
    self->tempFrame = self->tempSampleShiftFrame;
    self->tempSampleShiftFrame = pointerFlipTemp;
    
    size_t firstSectionSize = sampleShift;
    size_t secondSectionSize = self->FFTFrameSize - sampleShift;
    
    if (forward == true) {
        
        cblas_dcopy((UInt32)secondSectionSize, self->tempSampleShiftFrame, 1, &self->tempFrame[firstSectionSize], 1);
        cblas_dcopy((UInt32)firstSectionSize, &self->tempSampleShiftFrame[secondSectionSize], 1, self->tempFrame, 1);
    }
    else {
        
        cblas_dcopy((UInt32)secondSectionSize, &self->tempSampleShiftFrame[firstSectionSize], 1, self->tempFrame, 1);
        cblas_dcopy((UInt32)firstSectionSize, self->tempSampleShiftFrame, 1, &self->tempFrame[secondSectionSize], 1);
    }
}


PhaseVocoderData *PhaseVocoder_analyseAudioMatrix(PhaseVocoder *const self,
                                                  const AudioMatrix *const audioMatrixIn)
{
    size_t analysisFramesCount = (audioMatrixIn->sampleCount - self->FFTFrameSize) / self->hopSize;
    PhaseVocoderData *phaseVocoderDataOut = PhaseVocoderData_new(analysisFramesCount,
                                                                 self->FFTFrameSize);
    Float64 *input = audioMatrixIn->channelCount == 1 ? Matrix_getRow(audioMatrixIn->matrix, 0) : Matrix_getRow(audioMatrixIn->matrix, 2);
    
    vDSP_vclrD(self->previousPhases, 1, self->FFTFrameSize / 2);
    
    Float64 pi = M_PI;
    Float64 twoPi = M_PI * 2;
    
    for (size_t sampleNumber = 0, phaseVocoderFrame = 0; phaseVocoderFrame < phaseVocoderDataOut->frequencies->rowCount; phaseVocoderFrame++, sampleNumber += self->hopSize) {
        
        
        vDSP_vmulD(self->window, 1, &input[sampleNumber], 1, self->tempFrame, 1, self->FFTFrameSize);
        PhaseVocoder_rotateTempFrame(self, true);
        
        Float64 *magnitudes = Matrix_getRow(phaseVocoderDataOut->magnitudes, phaseVocoderFrame);
        Float64 *phases = Matrix_getRow(phaseVocoderDataOut->frequencies, phaseVocoderFrame);
        FFT_forwardPolar(self->fft, self->tempFrame, magnitudes, phases);
        
        vDSP_vsubD(self->previousPhases, 1, phases, 1, self->tempFrequencies, 1, self->FFTFrameSize / 2);
        cblas_dcopy((SInt32)self->FFTFrameSize / 2, phases, 1, self->previousPhases, 1);
        
        
        for (size_t i = 0; i < self->FFTFrameSize / 2; ++i) {
            
            while(self->tempFrequencies[i] > pi) self->tempFrequencies[i] -= twoPi;
            while(self->tempFrequencies[i] < -pi) self->tempFrequencies[i] += twoPi;
        }
        
        vDSP_vaddD(self->forwardFrequencyScale, 1, self->tempFrequencies,  1, phases, 1, self->FFTFrameSize / 2);
        vDSP_vsmulD(phases, 1, &self->forwardFactor, phases, 1, self->FFTFrameSize / 2);
    }
    
    return phaseVocoderDataOut;
}

PhaseVocoderData *PhaseVocoder_analyseAudioObject(PhaseVocoder *const self,
                                                  const AudioObject *const audioObject)
{
    _AudioMatrix *audioMatrix = AudioMatrix_new(audioObject);
    PhaseVocoderData *phaseVocoderData = PhaseVocoder_analyseAudioMatrix(self, audioMatrix);
    
    return phaseVocoderData;
}

OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(PhaseVocoder *const self,
                                                           const char *filePath)
{
    _AudioObject *audioObject = AudioObject_new(filePath);
    PhaseVocoderData *phaseVocoderData = PhaseVocoder_analyseAudioObject(self, audioObject);
    
    return phaseVocoderData;
}

OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(const char *filePath,
                                                           size_t FFTFrameSize,
                                                           size_t hopSize)
{
    _AudioObject *audioObject = AudioObject_new(filePath);
    _PhaseVocoder *phaseVocoder = PhaseVocoder_new(FFTFrameSize, hopSize, audioObject->samplerate);
    PhaseVocoderData *output = PhaseVocoder_analyseAudioFile(phaseVocoder, filePath);
    return output;
}

void PhaseVocoder_synthesiseFrame(PhaseVocoder *const self,
                                  PhaseVocoderData *phaseVocoderData,
                                  Float64 frameNumber,
                                  Float64 *output)
{
    if (frameNumber < 0
        ||
        frameNumber >= phaseVocoderData->frequencies->rowCount) {
        
        printf("PhaseVocoder_synthesiseFrame, Illegal phase vocoder frame number, exiting\n");
        exit(-1);
    }
    
    if (fmod(frameNumber, 1) == 0) {
        
        size_t phaseVocoderFrame = (size_t) frameNumber;
        Float64 *magnitudes = Matrix_getRow(phaseVocoderData->magnitudes, phaseVocoderFrame);
        Float64 *frequencies = Matrix_getRow(phaseVocoderData->frequencies, phaseVocoderFrame);
        PhaseVocoder_synthesise(self, magnitudes, frequencies, output);
    }
    else {
        
        size_t firstFrameNumber = floor(frameNumber);
        size_t secondFrameNumber = ceil(frameNumber);
        Float64 interpolation = fmod(frameNumber, 1);
        
        Float64 *firstMagnitudes = Matrix_getRow(phaseVocoderData->magnitudes, firstFrameNumber);
        Float64 *firstFrequencies = Matrix_getRow(phaseVocoderData->frequencies, firstFrameNumber);
        Float64 *secondMagnitudes = Matrix_getRow(phaseVocoderData->magnitudes, secondFrameNumber);
        Float64 *secondFrequencies = Matrix_getRow(phaseVocoderData->frequencies, secondFrameNumber);
        
        vDSP_vintbD(firstMagnitudes, 1, secondMagnitudes, 1, &interpolation, self->tempMagnitudes, 1, self->FFTFrameSize / 2);
        vDSP_vintbD(firstFrequencies, 1, secondFrequencies, 1, &interpolation, self->tempFrequencies, 1, self->FFTFrameSize / 2);
        
        PhaseVocoder_synthesise(self, self->tempMagnitudes, self->tempFrequencies, output);
    }
}

void PhaseVocoder_getPhaseVocoderDataForFrame(PhaseVocoder *const self,
                                              PhaseVocoderData *phaseVocoderData,
                                              Float64 frameNumber,
                                              Float64 **magnitudesOut,
                                              Float64 **frequenciesOut)
{
    if (frameNumber < 0
        ||
        frameNumber >= phaseVocoderData->frequencies->rowCount) {
        
        printf("PhaseVocoder_synthesiseFrame, Illegal phase vocoder frame number, exiting\n");
        exit(-1);
    }
    
    if (fmod(frameNumber, 1) == 0) {
        
        size_t phaseVocoderFrame = (size_t)frameNumber;
        *magnitudesOut = Matrix_getRow(phaseVocoderData->magnitudes, phaseVocoderFrame);
        *frequenciesOut = Matrix_getRow(phaseVocoderData->frequencies, phaseVocoderFrame);
        return;
    }
    else {
        
        size_t firstFrameNumber = floor(frameNumber);
        size_t secondFrameNumber = ceil(frameNumber);
        Float64 interpolation = fmod(frameNumber, 1);
        
        Float64 *firstMagnitudes = Matrix_getRow(phaseVocoderData->magnitudes, firstFrameNumber);
        Float64 *firstFrequencies = Matrix_getRow(phaseVocoderData->frequencies, firstFrameNumber);
        Float64 *secondMagnitudes = Matrix_getRow(phaseVocoderData->magnitudes, secondFrameNumber);
        Float64 *secondFrequencies = Matrix_getRow(phaseVocoderData->frequencies, secondFrameNumber);
        
        vDSP_vintbD(firstMagnitudes, 1, secondMagnitudes, 1, &interpolation, self->tempMagnitudes, 1, self->FFTFrameSize / 2);
        vDSP_vintbD(firstFrequencies, 1, secondFrequencies, 1, &interpolation, self->tempFrequencies, 1, self->FFTFrameSize / 2);
        
        *magnitudesOut = self->tempMagnitudes;
        *frequenciesOut = self->tempFrequencies;
    }
}


void PhaseVocoder_synthesise(PhaseVocoder *self,
                             Float64 *magnitudes,
                             Float64 *frequencies,
                             Float64 *output)
{
    vDSP_vsubD(self->inverseFrequencyScale, 1, frequencies,  1, self->tempFrequencies, 1, self->FFTFrameSize / 2);
    vDSP_vsmulD(self->tempFrequencies, 1, &self->inverseFactor, self->tempFrequencies, 1, self->FFTFrameSize / 2);
    vDSP_vaddD(self->tempFrequencies, 1, self->previousPhases, 1, self->tempFrequencies, 1, self->FFTFrameSize / 2);
    cblas_dcopy((UInt32)self->FFTFrameSize / 2, self->tempFrequencies, 1, self->previousPhases, 1);
    
    FFT_inversePolar(self->fft, magnitudes, self->tempFrequencies, self->tempFrame);
    
    PhaseVocoder_rotateTempFrame(self, false);
    Float64 twoThirds = 2./3.;
    vDSP_vmulD(self->tempFrame, 1, self->window, 1, self->tempFrame, 1, self->FFTFrameSize);
    vDSP_vsmulD(self->tempFrame, 1, &twoThirds, output, 1, self->FFTFrameSize);
}


void PhaseVocoder_inverse(PhaseVocoder *const self,
                          PhaseVocoderData *phaseVocoderDataIn,
                          AudioMatrix *audioMatrixOut,
                          Float64 increment)
{
    size_t frameCount = phaseVocoderDataIn->frequencies->rowCount;
    
    Float64 *output = audioMatrixOut->matrix->data;
    Float64 phaseVocoderFrame = 0;
    for (size_t sampleNumber = 0; phaseVocoderFrame < frameCount; phaseVocoderFrame += increment, sampleNumber += self->hopSize) {
        
        PhaseVocoder_synthesiseFrame(self, phaseVocoderDataIn, phaseVocoderFrame, self->tempFrame);
        
        vDSP_vaddD(self->tempFrame, 1, &output[sampleNumber], 1, &output[sampleNumber], 1, self->FFTFrameSize);
    }
}

PhaseVocoderData *PhaseVocoderData_new(size_t frameCount, size_t FFTFrameSize)
{
    PhaseVocoderData *self = calloc(1, sizeof(PhaseVocoderData));
    self->frequencies = Matrix_new(frameCount, FFTFrameSize / 2);
    self->magnitudes = Matrix_new(frameCount, FFTFrameSize / 2);
    return self;
}

void PhaseVocoderData_delete(PhaseVocoderData *const self)
{
    Matrix_delete(self->frequencies);
    Matrix_delete(self->magnitudes);
    free(self);
}

void PhaseVocoderData_scopedDelete(PhaseVocoderData **self)
{
    PhaseVocoderData_delete(*self);
}

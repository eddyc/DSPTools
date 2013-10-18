//
//  BeatDetect.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 06/03/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "BeatDetect.h"
#import "MathematicalFunctions.h"
#import "ConvenienceFunctions.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

BeatDetect32 *BeatDetect32_new(size_t samplerate, size_t frameCount, Float32 tempoMean)
{
    BeatDetect32 *self = calloc(1, sizeof(BeatDetect32));
    self->samplerate = samplerate;
    self->frameCount = frameCount;
    self->FFTFrameSize = 1024;
    self->FFTFrameSizeOver2 = self->FFTFrameSize / 2;
    self->hopSize = self->FFTFrameSize / 4;
    self->spectralFluxFFT = FFT32_new(self->FFTFrameSize, kFFTWindowType_None);
    self->hopCount = 1 + floor((self->frameCount - self->FFTFrameSize)/self->hopSize);
    self->tempomean = tempoMean;
    self->temposd = 1;
    self->tightness = 400;
    
    self->currentMagnitudes = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->previousMagnitudes = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->spectralFluxVector = calloc(self->hopCount * 2, sizeof(Float32));
    self->temp1_hopCountSize = calloc(self->hopCount, sizeof(Float32));
    self->temp2_hopCountSize = calloc(self->hopCount, sizeof(Float32));
    

    self->oesr = (Float32)self->samplerate/(Float32)self->hopSize;
    self->acmax = roundf(4 * self->oesr);
    self->crossCorrelationFrameSize = self->acmax + 1;
    self->crossCorrelationFrame = calloc(self->hopCount * 2, sizeof(Float32));
    
    self->xcrwin = calloc(self->crossCorrelationFrameSize, sizeof(Float32));
    self->bpms = calloc(self->crossCorrelationFrameSize, sizeof(Float32));
    self->xpks = calloc(self->crossCorrelationFrameSize, sizeof(Float32));
    
    for (size_t i = 0; i < self->crossCorrelationFrameSize; ++i) {
        
        self->bpms[i] = (60 * self->oesr) / ((Float32)i + 0.1);
    }
    
    vDSP_vsdiv(self->bpms, 1, &self->tempomean, self->bpms, 1, self->crossCorrelationFrameSize);
    SInt32 bpmsSize = (SInt32)self->crossCorrelationFrameSize;
    vvlogf(self->bpms, self->bpms, &bpmsSize);
    Float32 logTwo = logf(2);
    vDSP_vsdiv(self->bpms, 1, &logTwo, self->bpms, 1, self->crossCorrelationFrameSize);
    vDSP_vmul(self->bpms, 1, self->bpms, 1, self->bpms, 1, bpmsSize);
    Float32 minusPointFive = -.5;
    vDSP_vsmul(self->bpms, 1, &minusPointFive, self->bpms, 1, self->crossCorrelationFrameSize);
    vvexpf(self->xcrwin, self->bpms, &bpmsSize);
    
    
    self->xcr00Size = self->crossCorrelationFrameSize + 2;
    self->xcr00 = calloc(self->xcr00Size, sizeof(Float32));
    
    self->xcr2Size = ceilf((Float32)self->crossCorrelationFrameSize/2);
    self->xcr2 = calloc(self->xcr2Size, sizeof(Float32));
    
    self->xcr3Size = ceilf((Float32)self->crossCorrelationFrameSize/3);
    self->xcr3 = calloc(self->xcr3Size, sizeof(Float32));
    self->convolutionVectorCount = self->hopCount * 2;
    self->convolutionVector = calloc(self->convolutionVectorCount, sizeof(Float32));
    self->backlink = calloc(self->hopCount, sizeof(Float32));
    self->cumulatedScore = calloc(self->hopCount, sizeof(Float32));
    self->prange = calloc(self->hopCount, sizeof(Float32));
    self->txwt = calloc(self->hopCount, sizeof(Float32));
    self->timerange = calloc(self->hopCount, sizeof(Float32));
    self->scorecands = calloc(self->hopCount, sizeof(Float32));
    self->timeRangeIndexes = calloc(self->hopCount, sizeof(size_t));
    self->beats = calloc(self->hopCount, sizeof(Float32));
    
    return  self;
}

void BeatDetect32_delete(BeatDetect32 *self)
{
    FFT32_delete(self->spectralFluxFFT);
    free(self->currentMagnitudes);
    free(self->previousMagnitudes);
    free(self->spectralFluxVector);
    free(self->xcr00);
    free(self->xcr2);
    free(self->xcr3);
    free(self->xcrwin);
    free(self->bpms);
    free(self->xpks);
    free(self->temp1_hopCountSize);
    free(self->temp2_hopCountSize);
    free(self->convolutionVector);
    free(self->backlink);
    free(self->cumulatedScore);
    free(self->prange);
    free(self->txwt);
    free(self->timerange);
    free(self->scorecands);
    free(self->timeRangeIndexes);
    free(self->beats);
    free(self);
}

void BeatDetect32_getSpectralDifference(BeatDetect32 *self, Float32 *samplesIn)
{
    Float32 previousX = 0;
    Float32 previousY = 0;
    
    for (size_t i = 0; i < self->hopCount; ++i) {
        
        FFT32_forwardMagnitudesWindowed(self->spectralFluxFFT, &samplesIn[i * self->hopSize], self->currentMagnitudes);
        vDSP_vsub(self->currentMagnitudes, 1, self->previousMagnitudes, 1, self->previousMagnitudes, 1, self->FFTFrameSizeOver2);
        vDSP_vabs(self->previousMagnitudes, 1, self->previousMagnitudes, 1, self->FFTFrameSizeOver2);
        
        vDSP_sve(self->previousMagnitudes, 1, &self->spectralFluxVector[i], self->FFTFrameSizeOver2);
        
        Float32 currentY = self->spectralFluxVector[i] + (previousX * -1) + (previousY * .99);
        previousX = self->spectralFluxVector[i];
        previousY = currentY;
        
        self->spectralFluxVector[i] = isnan(currentY) ? 0 : currentY;
        self->spectralFluxVector[i] = isinf(self->spectralFluxVector[i]) ? 0 : self->spectralFluxVector[i];

        cblas_scopy((SInt32)self->FFTFrameSizeOver2, self->currentMagnitudes, 1, self->previousMagnitudes, 1);
    }
}

void BeatDetect32_getTempo(BeatDetect32 *self, Float32 *samplesIn)
{
    vDSP_conv(self->spectralFluxVector, 1, self->spectralFluxVector, 1, self->crossCorrelationFrame, 1, self->hopCount, self->hopCount);

    vDSP_vmul(self->xcrwin, 1, self->crossCorrelationFrame, 1, self->crossCorrelationFrame, 1, self->crossCorrelationFrameSize);

    Float32 maxPeak = 0;
    SInt32 minimumIndex = -1;
    
    for (size_t i = 1; i < self->crossCorrelationFrameSize - 1; ++i) {
        
        if (minimumIndex == -1 && self->crossCorrelationFrame[i] < 0) {
            
            minimumIndex = (SInt32)i;
        }
        
        if (self->crossCorrelationFrame[i] > self->crossCorrelationFrame[i - 1]
            &&
            self->crossCorrelationFrame[i] >= self->crossCorrelationFrame[i + 1]
            &&
            minimumIndex >= 0) {
            
            self->xpks[i] = 1;
            
            
            if (self->crossCorrelationFrame[i] > maxPeak) {
                
                maxPeak = self->crossCorrelationFrame[i];
            }
        }
    }

    cblas_scopy((SInt32)self->crossCorrelationFrameSize, self->crossCorrelationFrame, 1, &self->xcr00[1], 1);
    
    Float32 pointFive = 0.5;
    vDSP_vsmul(self->xcr00, 2, &pointFive, self->xcr2, 1, self->xcr2Size);
    vDSP_vadd(self->xcr2, 1, &self->xcr00[1], 2, self->xcr2, 1, self->xcr2Size);
    vDSP_vsma(&self->xcr00[2], 2, &pointFive, self->xcr2, 1, self->xcr2, 1, self->xcr2Size);
    vDSP_vsmul(self->xcr2, 1, &pointFive, self->xcr2, 1, self->xcr2Size);
    vDSP_vadd(self->xcr2, 1, self->crossCorrelationFrame, 1, self->xcr2, 1, self->xcr2Size);
    
    Float32 pointThreeThree = 0.33;
    vDSP_vadd(self->xcr00, 3, self->xcr3, 1, self->xcr3, 1, self->xcr3Size);
    vDSP_vadd(&self->xcr00[1], 3, self->xcr3, 1, self->xcr3, 1, self->xcr3Size);
    vDSP_vadd(&self->xcr00[2], 3, self->xcr3, 1, self->xcr3, 1, self->xcr3Size);
    vDSP_vsmul(self->xcr3, 1, &pointThreeThree, self->xcr3, 1, self->xcr3Size);
    vDSP_vadd(self->xcr3, 1, self->crossCorrelationFrame, 1, self->xcr3, 1, self->xcr3Size);
    
    Float32 xcr2Max;
    size_t xcr2MaxIndex;
    vDSP_maxvi(self->xcr2, 1, &xcr2Max, &xcr2MaxIndex, self->xcr2Size);
    Float32 xcr3Max;
    size_t xcr3MaxIndex;
    vDSP_maxvi(self->xcr3, 1, &xcr3Max, &xcr3MaxIndex, self->xcr3Size);
    
    size_t startpd, startpd2;
    if (xcr2Max > xcr3Max) {
        
        startpd = xcr2MaxIndex - 1;
        startpd2 = xcr2MaxIndex * 2 - 1;
    }
    else {
        
        startpd = xcr3MaxIndex - 1;
        startpd2 = xcr3MaxIndex * 3 - 1;
        
    }
    
    Float32 test = self->crossCorrelationFrame[startpd + 1];
    test = self->crossCorrelationFrame[startpd2 + 1];
    
    Float32 pratio = self->crossCorrelationFrame[startpd + 1]/ (self->crossCorrelationFrame[startpd + 1] + self->crossCorrelationFrame[startpd2 + 1]);
    
    
    self->tempos[0] = 60./((startpd + 1)/self->oesr);
    self->tempos[1] = 60./((startpd2 + 1)/self->oesr);
    self->tempos[2] = pratio;
    
    if (self->tempos[1] < self->tempos[0]) {
        
        Float32 temp = self->tempos[1];
        self->tempos[1] = self->tempos[0];
        self->tempos[0] = temp;
        self->tempos[2] = 1 - self->tempos[2];
    }
}

void BeatDetect32_getBeatPositions(BeatDetect32 *self, Float32 *beatsOut, size_t *beatsCount)
{
    Float32 startBPM;
    if (self->tempos[2] > 0.5) {
        
        startBPM = self->tempos[0];
    }
    else {
        
        startBPM = self->tempos[1];
    }
    
    Float32 mean;
    
    vDSP_meanv(self->spectralFluxVector, 1, &mean, self->hopCount);
    mean *= -1;
    vDSP_vsadd(self->spectralFluxVector, 1, &mean, self->temp1_hopCountSize, 1, self->hopCount);
    vDSP_vmul(self->temp1_hopCountSize, 1, self->temp1_hopCountSize, 1, self->temp1_hopCountSize, 1, self->hopCount);
    vDSP_meanv(self->temp1_hopCountSize, 1, &mean, self->hopCount);
    Float32 standardDeviation = sqrtf(mean);
    
    vDSP_vsdiv(self->spectralFluxVector, 1, &standardDeviation, self->spectralFluxVector, 1, self->hopCount);
        
    Float32 startpd = (60 * self->oesr)/startBPM;
    Float32 pd = startpd;
    size_t templdSize = pd * 2 + 1;
    Float32 minusPd = -pd;
    Float32 one = 1;
    Float32 pdOver32 = pd / 32;
    
    vDSP_vramp(&minusPd, &one, self->temp1_hopCountSize, 1, templdSize);
    vDSP_vsdiv(self->temp1_hopCountSize, 1, &pdOver32, self->temp1_hopCountSize, 1, templdSize);
    vDSP_vsq(self->temp1_hopCountSize, 1, self->temp1_hopCountSize, 1, templdSize);
    Float32 minusPoint5 = -0.5;
    vDSP_vsmul(self->temp1_hopCountSize, 1, &minusPoint5, self->temp1_hopCountSize, 1, templdSize);
    SInt32 templdSizeAsInt = (SInt32)templdSize;
    vvexpf(self->temp1_hopCountSize, self->temp1_hopCountSize, &templdSizeAsInt);
    
    size_t halftemplt = roundf((Float32)templdSize) / 2.;
    cblas_scopy((SInt32)self->hopCount, self->spectralFluxVector, 1, &self->convolutionVector[halftemplt - 1], 1);
    vDSP_conv(self->convolutionVector, 1, &self->temp1_hopCountSize[templdSize - 1], -1, self->convolutionVector, 1, self->hopCount + templdSize - 1, templdSize);
    
    Float32 prangeStart = roundf(-2 * pd);
    Float32 prangeEnd = -roundf(pd / 2);
    SInt32 prangeSize = abs(prangeStart - prangeEnd) + 1;
    
    vDSP_vgen(&prangeStart, &prangeEnd, self->prange, 1, prangeSize);
    vDSP_vsdiv(self->prange, 1, &minusPd, self->txwt, 1, prangeSize);
    vvlogf(self->txwt, self->txwt, &prangeSize);
    vDSP_vsq(self->txwt, 1, self->txwt, 1, prangeSize);
    vDSP_vabs(self->txwt, 1, self->txwt, 1, prangeSize);
    Float32 minusTightness = -self->tightness;
    vDSP_vsmul(self->txwt, 1, &minusTightness, self->txwt, 1, prangeSize);
    
    Float32 starting = 1;
    
    SInt32 startIndex = self->prange[0];
    Float32 convolutionMax;
    vDSP_maxv(self->convolutionVector, 1, &convolutionMax, self->hopCount);
    
    for (size_t i = 0; i < self->hopCount; ++i, ++startIndex) {
        
        Float32 iPlusOneAsFloat = (Float32)i + 1;
        vDSP_vsadd(self->prange, 1, &iPlusOneAsFloat, self->timerange, 1, prangeSize);
        Float32 zpad = MAX(0, MIN(1 - self->timerange[0], prangeSize));
        vDSP_vclr(self->scorecands, 1, zpad);
        size_t timeSize =  abs(zpad - prangeSize);
        size_t timeRangeIndex = (size_t) zpad;
        
        for (size_t j = 0; j < timeSize; ++j) {
            
            self->timeRangeIndexes[j] = self->timerange[timeRangeIndex + j];
        }
        
        vDSP_vgathr(self->cumulatedScore, self->timeRangeIndexes, 1, &self->scorecands[prangeSize - timeSize], 1, timeSize);
        vDSP_vadd(self->txwt, 1, self->scorecands, 1, self->scorecands, 1, prangeSize);
        Float32 scorecandsMax;
        size_t scorecandsMaxIndex;
        vDSP_maxvi(self->scorecands, 1, &scorecandsMax, &scorecandsMaxIndex, prangeSize);
        
        self->cumulatedScore[i] = scorecandsMax + self->convolutionVector[i];
        
        if (starting == 1 && self->convolutionVector[i] < 0.01 * convolutionMax) {
            
            self->backlink[i] = -1;
        }
        else {
            
            self->backlink[i] = self->timerange[scorecandsMaxIndex];
            starting = 0;
        }
    }
        
    size_t peakCount = 0;
    for (size_t i = 1; i < self->hopCount - 1; ++i) {
        
        if (self->cumulatedScore[i] > self->cumulatedScore[i - 1]
            &&
            self->cumulatedScore[i] >= self->cumulatedScore[i + 1]) {
            
            self->temp1_hopCountSize[peakCount] = self->cumulatedScore[i];
            peakCount++;
        }
    }
    
    vDSP_vsort(self->temp1_hopCountSize, peakCount, 1);
    Float32 median;
    if (peakCount % 2 == 0) {
        
        median = 0.5 * (self->temp1_hopCountSize[peakCount / 2 - 1] + self->temp1_hopCountSize[peakCount / 2]);
    }
    else {
        
        median = self->temp1_hopCountSize[peakCount / 2];
    }
    
    self->temp1_hopCountSize[0] = self->temp1_hopCountSize[self->hopCount - 1] = 0;
    
    for (size_t i = 1; i < self->hopCount - 1; ++i) {
        
        self->temp1_hopCountSize[i] = 0;
        
        if (self->cumulatedScore[i] > self->cumulatedScore[i - 1]
            &&
            self->cumulatedScore[i] >= self->cumulatedScore[i + 1]
            &&
            self->cumulatedScore[i] > (0.5 * median)) {
            
            self->temp1_hopCountSize[i] = i + 1;
        }
    }
    
    Float32 bestendx;
    vDSP_maxv(self->temp1_hopCountSize, 1, &bestendx, self->hopCount);
    beatsOut[0] = bestendx;
    
    for (size_t i = 1; i < self->hopCount && beatsOut[i - 1] > 0; ++i, self->beatsCount++) {
        
        beatsOut[i] = roundf(self->backlink[(size_t)beatsOut[i - 1]]);
    }
    
    Float32 minusOne = -1;
    vDSP_vsadd(beatsOut, 1, &minusOne, beatsOut, 1, self->beatsCount);
    vDSP_vrvrs(beatsOut, 1, self->beatsCount);
    *beatsCount = self->beatsCount;
}

void BeatDetect32_process(BeatDetect32 *self, Float32 *samplesIn, Float32 *beatsOut, size_t *beatsCount)
{
    BeatDetect32_getSpectralDifference(self, samplesIn);
    BeatDetect32_getTempo(self, samplesIn);
    BeatDetect32_getBeatPositions(self, beatsOut, beatsCount);
}




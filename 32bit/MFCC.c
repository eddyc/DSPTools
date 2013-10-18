    //
//  MFCC.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 23/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "MFCC.h"
#import <math.h>
#import <Accelerate/Accelerate.h>

static void MFCC32_configure(MFCC32 *self);

MFCC32 *MFCC32_new(size_t FFTFrameSize,
                   size_t samplerate)
{
    MFCC32 *self = malloc(sizeof(MFCC32));
    
    self->FFTFrameSize = FFTFrameSize;
    self->FFTFrameSizeOver2 = FFTFrameSize / 2;
    self->samplerate = samplerate;
    self->lowestFrequency = 133.3333f;
    self->linearFilters = 13;
    self->linearSpacing = 66.66666666f;
    self->logFilters = 27;
    self->logSpacing = 1.0711703f;
    self->cepstralCoefficients = 13;
    self->totalFilters = self->linearFilters + self->logFilters;
    self->mfccFilterWeights = calloc(self->totalFilters, sizeof(Float32 *));
    
    for (size_t i = 0; i < self->totalFilters; ++i) {
        
        self->mfccFilterWeights[i] = calloc(self->FFTFrameSize, sizeof(Float32));
    }
    
    self->mfccDCTMatrix = calloc(self->linearFilters * self->totalFilters, sizeof(Float32 *));
    
    self->earMagnitudes = calloc(self->totalFilters, sizeof(Float32));
    self->logEarMagnitudes = calloc(self->totalFilters, sizeof(Float32));

    
    MFCC32_configure(self);
    
    return self;
}

void MFCC32_delete(MFCC32 *self)
{
    for (size_t i = 0; i < self->totalFilters; ++i) {
        
        free(self->mfccFilterWeights[i]);
    }
    
    free(self->mfccFilterWeights);
    free(self->mfccDCTMatrix);
    free(self->earMagnitudes);
    free(self->logEarMagnitudes);
    free(self);
    self = NULL;
}


static void MFCC32_configure(MFCC32 *self)
{
    
    Float32 *filterFrequencys = calloc(self->totalFilters + 2, sizeof(Float32));
    Float32 *lower = calloc(self->totalFilters, sizeof(Float32));
    Float32 *center = calloc(self->totalFilters, sizeof(Float32));
    Float32 *upper = calloc(self->totalFilters, sizeof(Float32));
    Float32 *triangleHeight = calloc(self->totalFilters, sizeof(Float32));
    
    Float32 *fftFreqs = calloc(self->FFTFrameSize, sizeof(Float32));
    Float32 *temp1 = (Float32 *)calloc(self->FFTFrameSize, sizeof(Float32));
    Float32 *temp2 = (Float32 *)calloc(self->FFTFrameSize, sizeof(Float32));
    
    for (size_t i = 0; i < self->linearFilters; ++i) {
        
        filterFrequencys[i] = i * self->linearSpacing + self->lowestFrequency;
    }
    
    for (size_t i = self->linearFilters, j = 1; i < self->totalFilters + 2; ++i, ++j) {
        
        filterFrequencys[i] = filterFrequencys[self->linearFilters - 1] * pow(self->logSpacing, j);
    }
    
    cblas_scopy(self->totalFilters, &filterFrequencys[0], 1, lower, 1);
    cblas_scopy(self->totalFilters, &filterFrequencys[1], 1, center, 1);
    cblas_scopy(self->totalFilters, &filterFrequencys[2], 1, upper, 1);
    
    for (size_t i = 0; i < self->totalFilters; ++i) {
        
        triangleHeight[i] = 2.f/(upper[i] - lower[i]);
    }
    
    
    for (size_t i = 0; i < self->FFTFrameSize; ++i) {
        
        fftFreqs[i] = (Float32)i /(Float32)self->FFTFrameSize * self->samplerate;
    }
    
    
    for (size_t i = 0; i < self->totalFilters; ++i) {
        
        Float32 negateLowerValue = -lower[i];
        vDSP_vsadd(fftFreqs, 1, &negateLowerValue, temp1, 1, self->FFTFrameSize);
        Float32 divider = center[i] - lower[i];
        vDSP_vsdiv(temp1, 1, &divider, temp1, 1, self->FFTFrameSize);
        vDSP_vsmul(temp1, 1, &triangleHeight[i], temp1, 1, self->FFTFrameSize);
        
        for (size_t j = 0; j < self->FFTFrameSize; ++j) {
            
            if (!(fftFreqs[j] > lower[i] && fftFreqs[j] <= center[i])) {
                
                temp1[j] = 0;
            }
        }
        
        Float32 minusOne = -1;
        vDSP_vsmul(fftFreqs, 1, &minusOne, temp2, 1, self->FFTFrameSize);
        vDSP_vsadd(temp2, 1, &upper[i], temp2, 1, self->FFTFrameSize);
        divider = upper[i] - center[i];
        vDSP_vsdiv(temp2, 1, &divider, temp2, 1, self->FFTFrameSize);
        vDSP_vsmul(temp2, 1, &triangleHeight[i], temp2, 1, self->FFTFrameSize);
        
        for (size_t j = 0; j < self->FFTFrameSize; ++j) {
            
            if (!(fftFreqs[j] > center[i] && fftFreqs[j] <= upper[i])) {
                
                temp2[j] = 0;
            }
        }
        
        vDSP_vadd(temp1, 1, temp2, 1, self->mfccFilterWeights[i], 1, self->FFTFrameSize);
    }
    
    for (size_t i = 0; i < self->linearFilters; ++i) {
        
        for (size_t j = 0; j < self->totalFilters; ++j) {
            
            self->mfccDCTMatrix[i * self->totalFilters + j] = 1./sqrt(self->totalFilters / 2.) * cosf(((j * 2 + 1) * M_PI / 2.f / self->totalFilters) * i);
        }
    }
    
    Float32 sqrtTwoOverTwo = sqrtf(2.)/ 2.;
    
    vDSP_vsmul(self->mfccDCTMatrix, 1, &sqrtTwoOverTwo, self->mfccDCTMatrix, 1, self->totalFilters);
    
    free(filterFrequencys);
    free(lower);
    free(center);
    free(upper);
    free(triangleHeight);
    free(fftFreqs);
    free(temp1);
    free(temp2);
    
}

inline void MFCC32_process(MFCC32 *self,
                           Float32 *__restrict inputMagnitudes,
                           Float32 *__restrict outputMFCCs)
{
    
    Float32 maximum;
    vDSP_maxv(inputMagnitudes, 1, &maximum, self->FFTFrameSizeOver2);
    
    if (maximum <= 0) {
        
        return;
    }
    
    for (size_t i = 0; i < self->totalFilters; ++i) {
        
        vDSP_dotpr(inputMagnitudes,
                   1,
                   self->mfccFilterWeights[i],
                   1,
                   &self->earMagnitudes[i],
                   self->FFTFrameSizeOver2);
        
    }
    
    vvlog10f(self->logEarMagnitudes, self->earMagnitudes, &self->totalFilters);
    
    cblas_sgemm(CblasRowMajor,
                CblasNoTrans,
                CblasTrans,
                (SInt32)self->cepstralCoefficients,
                1,
                (SInt32)self->totalFilters,
                1.0f,
                self->mfccDCTMatrix,
                (SInt32)self->totalFilters,
                self->logEarMagnitudes,
                (SInt32)self->totalFilters,
                0.0f,
                outputMFCCs,
                1);
    
    outputMFCCs[0] = 0;
    
}




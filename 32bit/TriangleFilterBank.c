//
//  TriangleFilterBank.c
//  Audio-Mosaicing
//
//  Created by Edward Costello on 01/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "TriangleFilterBank.h"

TriangleFilterBank32 *TriangleFilterBank32_new(size_t filterCount, size_t magnitudeFrameSize, size_t samplerate)
{
    TriangleFilterBank32 *self = calloc(1, sizeof(TriangleFilterBank32));
    self->filterCount = filterCount;
    self->filterFrequencyCount = filterCount + 2;
    self->magnitudeFrameSize = magnitudeFrameSize;
    self->samplerate = samplerate;
    self->filterFrequencies = calloc(self->filterFrequencyCount, sizeof(Float32));
    self->filterBank = calloc(self->magnitudeFrameSize * self->filterCount, sizeof(Float32));
    Float32 *filterTemp = calloc(self->magnitudeFrameSize * self->filterCount, sizeof(Float32));

    Float32 one = 2;
    Float32 zero = 0;
    
    vDSP_vgen(&zero, &one, self->filterFrequencies, 1, self->filterFrequencyCount);

    for (size_t i = 0; i < self->filterFrequencyCount; ++i) {
        
        self->filterFrequencies[i] = powf(10, self->filterFrequencies[i]);

    }
    
    Float32 minusFirstElement = -self->filterFrequencies[0];
    vDSP_vsadd(self->filterFrequencies, 1, &minusFirstElement, self->filterFrequencies, 1, self->filterFrequencyCount);
    Float32 maximum = self->filterFrequencies[self->filterFrequencyCount - 1];

    vDSP_vsdiv(self->filterFrequencies, 1, &maximum, self->filterFrequencies, 1, self->filterFrequencyCount);
    
    Float32 nyquist = self->samplerate / 2;
    
    vDSP_vsmul(self->filterFrequencies, 1, &nyquist, self->filterFrequencies, 1, self->filterFrequencyCount);
    
    Float32 *magnitudeFrequencies = calloc(self->magnitudeFrameSize, sizeof(Float32));

    vDSP_vgen(&zero, &nyquist, magnitudeFrequencies, 1, self->magnitudeFrameSize);
    
    Float32 *lower = calloc(self->filterCount, sizeof(Float32));
    Float32 *center = calloc(self->filterCount, sizeof(Float32));
    Float32 *upper = calloc(self->filterCount, sizeof(Float32));
    
    Float32 *temp1 = (Float32 *)calloc(self->magnitudeFrameSize, sizeof(Float32));
    Float32 *temp2 = (Float32 *)calloc(self->magnitudeFrameSize, sizeof(Float32));
    

    cblas_scopy((SInt32)self->filterCount, &self->filterFrequencies[0], 1, lower, 1);
    cblas_scopy((SInt32)self->filterCount, &self->filterFrequencies[1], 1, center, 1);
    cblas_scopy((SInt32)self->filterCount, &self->filterFrequencies[2], 1, upper, 1);

   
    for (size_t i = 0; i < self->filterCount; ++i) {
        
        Float32 negateLowerValue = -lower[i];
        vDSP_vsadd(magnitudeFrequencies, 1, &negateLowerValue, temp1, 1, self->magnitudeFrameSize);
        Float32 divider = center[i] - lower[i];
        vDSP_vsdiv(temp1, 1, &divider, temp1, 1, self->magnitudeFrameSize);
        
        for (size_t j = 0; j < self->magnitudeFrameSize; ++j) {
            
            if (!(magnitudeFrequencies[j] > lower[i] && magnitudeFrequencies[j] <= center[i])) {
                
                temp1[j] = 0;
            }
        }
        
        Float32 minusOne = -1;
        vDSP_vsmul(magnitudeFrequencies, 1, &minusOne, temp2, 1, self->magnitudeFrameSize);
        vDSP_vsadd(temp2, 1, &upper[i], temp2, 1, self->magnitudeFrameSize);
        divider = upper[i] - center[i];
        vDSP_vsdiv(temp2, 1, &divider, temp2, 1, self->magnitudeFrameSize);
        
        for (size_t j = 0; j < self->magnitudeFrameSize; ++j) {
            
            if (!(magnitudeFrequencies[j] > center[i] && magnitudeFrequencies[j] <= upper[i])) {
                
                temp2[j] = 0;
            }
        }
        
        vDSP_vadd(temp1, 1, temp2, 1, &filterTemp[i * self->magnitudeFrameSize], 1, self->magnitudeFrameSize);
    }
    
    vDSP_mtrans(filterTemp, 1, self->filterBank, 1, self->magnitudeFrameSize, self->filterCount);
    
    free(lower);
    free(center);
    free(upper);
    free(temp1);
    free(temp2);
    free(magnitudeFrequencies);
    free(filterTemp);
    
    return self;
}

void TriangleFilterBank32_delete(TriangleFilterBank32 *self)
{
    free(self->filterFrequencies);
    free(self);
    self = NULL;
}

void TriangleFilterBank32_process(TriangleFilterBank32 *self,
                                  Float32 *magnitudesIn,
                                  Float32 *filteredMagnitudesOut)
{
    vDSP_mmul(magnitudesIn, 1, self->filterBank, 1, filteredMagnitudesOut, 1, 1, self->filterCount,self->magnitudeFrameSize);
}
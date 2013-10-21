//
//  TriangleFilterBank.c
//  Audio-Mosaicing
//
//  Created by Edward Costello on 01/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "TriangleFilterBank.h"
#import "../../Custom Functions/ConvenienceFunctions.h" 
TriangleFilterBank *TriangleFilterBank_new(size_t filterCount,
                                           size_t magnitudeFrameSize,
                                           size_t samplerate)
{
    TriangleFilterBank *self = calloc(1, sizeof(TriangleFilterBank));
    self->filterCount = filterCount;
    self->filterFrequencyCount = filterCount + 2;
    self->magnitudeFrameSize = magnitudeFrameSize;
    self->samplerate = samplerate;
    self->filterFrequencies = calloc(self->filterFrequencyCount, sizeof(Float64));
    self->filterBank = calloc(self->magnitudeFrameSize * self->filterCount, sizeof(Float64));
    Float64 *filterTemp = calloc(self->magnitudeFrameSize * self->filterCount, sizeof(Float64));
    
    Float64 zero = 0;
    Float64 one = 1;
    Float64 frequencyCount = self->filterFrequencyCount;
    vDSP_vgenD(&one, &frequencyCount, self->filterFrequencies, 1, self->filterFrequencyCount);
    vDSP_vsdivD(self->filterFrequencies, 1, &frequencyCount, self->filterFrequencies, 1, self->filterFrequencyCount);

    for (size_t i = 0; i < self->filterFrequencyCount; ++i) {
        
        self->filterFrequencies[i] = powf(10, self->filterFrequencies[i]);        
    }
    
    Float64 minusFirstElement = -self->filterFrequencies[0];
    vDSP_vsaddD(self->filterFrequencies, 1, &minusFirstElement, self->filterFrequencies, 1, self->filterFrequencyCount);
    
    Float64 maximum = self->filterFrequencies[self->filterFrequencyCount - 1];
    
    vDSP_vsdivD(self->filterFrequencies, 1, &maximum, self->filterFrequencies, 1, self->filterFrequencyCount);
    

    Float64 nyquist = self->samplerate / 2;
    
    vDSP_vsmulD(self->filterFrequencies, 1, &nyquist, self->filterFrequencies, 1, self->filterFrequencyCount);

    Float64 *magnitudeFrequencies = calloc(self->magnitudeFrameSize, sizeof(Float64));
    
    vDSP_vgenD(&zero, &nyquist, magnitudeFrequencies, 1, self->magnitudeFrameSize);
    

    Float64 *lower = calloc(self->filterCount, sizeof(Float64));
    Float64 *center = calloc(self->filterCount, sizeof(Float64));
    Float64 *upper = calloc(self->filterCount, sizeof(Float64));
    
    Float64 *temp1 = calloc(self->magnitudeFrameSize, sizeof(Float64));
    Float64 *temp2 = calloc(self->magnitudeFrameSize, sizeof(Float64));
    
    
    cblas_dcopy((UInt32)self->filterCount, &self->filterFrequencies[0], 1, lower, 1);
    cblas_dcopy((UInt32)self->filterCount, &self->filterFrequencies[1], 1, center, 1);
    cblas_dcopy((UInt32)self->filterCount, &self->filterFrequencies[2], 1, upper, 1);
    
    
    for (size_t i = 0; i < self->filterCount; ++i) {
        
        Float64 negateLowerValue = -lower[i];
        vDSP_vsaddD(magnitudeFrequencies, 1, &negateLowerValue, temp1, 1, self->magnitudeFrameSize);
        Float64 divider = center[i] - lower[i];
        vDSP_vsdivD(temp1, 1, &divider, temp1, 1, self->magnitudeFrameSize);
        
        for (size_t j = 0; j < self->magnitudeFrameSize; ++j) {
            
            if (!(magnitudeFrequencies[j] > lower[i] && magnitudeFrequencies[j] <= center[i])) {
                
                temp1[j] = 0.;
            }
        }
        
        Float64 minusOne = -1.;
        vDSP_vsmulD(magnitudeFrequencies, 1, &minusOne, temp2, 1, self->magnitudeFrameSize);
        vDSP_vsaddD(temp2, 1, &upper[i], temp2, 1, self->magnitudeFrameSize);
        divider = upper[i] - center[i];
        vDSP_vsdivD(temp2, 1, &divider, temp2, 1, self->magnitudeFrameSize);
        
        for (size_t j = 0; j < self->magnitudeFrameSize; ++j) {
            
            if (!(magnitudeFrequencies[j] > center[i] && magnitudeFrequencies[j] <= upper[i])) {
                
                temp2[j] = 0.;
            }
        }
        
        vDSP_vaddD(temp1, 1, temp2, 1, &filterTemp[i * self->magnitudeFrameSize], 1, self->magnitudeFrameSize);
    }
    
    vDSP_mtransD(filterTemp, 1, self->filterBank, 1, self->magnitudeFrameSize, self->filterCount);
    
    free(lower);
    free(center);
    free(upper);
    free(temp1);
    free(temp2);
    free(magnitudeFrequencies);
    free(filterTemp);
    
    return self;
}

void TriangleFilterBank_delete(TriangleFilterBank *self)
{
    free(self->filterFrequencies);
    free(self);
    self = NULL;
}

void TriangleFilterBank_process(TriangleFilterBank *self,
                                Float64 *magnitudesIn,
                                Float64 *filteredMagnitudesOut)
{
    
    
    vDSP_mmulD(magnitudesIn, 1, self->filterBank, 1, filteredMagnitudesOut, 1, 1, self->filterCount,self->magnitudeFrameSize);
}
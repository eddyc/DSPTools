//
//  ODF.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 22/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "ODF.h"
#import <stdio.h>
#import <Accelerate/Accelerate.h>

ODF32 *ODF32_new(size_t magnitudesSize,
                 size_t filterSize,
                 Float32 filterScale)
{
    ODF32 *self = calloc(1, sizeof(ODF32));
    
    self->magnitudesSize = magnitudesSize;
    self->filterSize = filterSize;
    self->filterScale = filterScale;
    
    self->previousMagnitudes = calloc(self->magnitudesSize, sizeof(Float32));
    self->magnitudeDifference = calloc(self->magnitudesSize, sizeof(Float32));
    self->filterBuffer = calloc(self->filterSize, sizeof(Float32));
    
    return self;
}

void ODF32_delete(ODF32 *self)
{
    free(self->previousMagnitudes);
    free(self->magnitudeDifference);
    free(self->filterBuffer);
    free(self);
    self = NULL;
}

void ODF32_reset(ODF32 *self)
{
    self->filterIndex = 0;
    self->filterValue = 0.f;
    self->spectralFluxValue = 0.f;
        
    vDSP_vclr(self->previousMagnitudes, 1, self->magnitudesSize);
    vDSP_vclr(self->magnitudeDifference, 1, self->magnitudesSize);
    vDSP_vclr(self->filterBuffer, 1, self->filterSize);

    self->onsetState = kODFOnsetState_None;
}

inline ODFOnsetState ODF32_process(ODF32 *self, Float32 *__restrict inputMagnitudes)
{    
    vDSP_vsub(inputMagnitudes, 1, self->previousMagnitudes, 1, self->magnitudeDifference, 1, self->magnitudesSize);
    
    vDSP_vabs(self->magnitudeDifference, 1, self->magnitudeDifference, 1, self->magnitudesSize);
    
    vDSP_sve(self->magnitudeDifference, 1, &self->spectralFluxValue, self->magnitudesSize);
    
    cblas_scopy((SInt32)self->magnitudesSize, inputMagnitudes, 1, self->previousMagnitudes, 1);
        
    self->filterBuffer[self->filterIndex] = self->spectralFluxValue;
    
    self->filterIndex = (self->filterIndex + 1) % self->filterSize;
    
    vDSP_sve(self->filterBuffer, 1, &self->filterValue, self->filterSize);
    
    self->filterValue = (self->filterValue / self->filterSize) * self->filterScale;

    if ((self->onsetState == kODFOnsetState_None
         ||
         self->onsetState == kODFOnsetState_OnsetEnded)
        &&
        self->spectralFluxValue > self->filterValue) {
        
        self->onsetState = kODFOnsetState_OnsetBeginning;
    }
    else if (self->onsetState == kODFOnsetState_OnsetBeginning
             &&
             self->spectralFluxValue > self->filterValue) {
        
        self->onsetState = kODFOnsetState_OnsetDuration;
    }
    else if ((self->onsetState == kODFOnsetState_OnsetDuration
              ||
              self->onsetState == kODFOnsetState_OnsetBeginning)
             &&
             self->spectralFluxValue < self->filterValue) {
        
        self->onsetState = kODFOnsetState_OnsetEnded;
    }
    else if (self->onsetState == kODFOnsetState_OnsetEnded
             &&
             self->spectralFluxValue < self->filterValue) {
        
        self->onsetState = kODFOnsetState_None;
    }
        
    return self->onsetState;
}

char *ODFOnsetState_asString(ODFOnsetState onsetState)
{
    char *string = onsetState == kODFOnsetState_None ? "none" : onsetState == kODFOnsetState_OnsetBeginning ? "begin" : onsetState == kODFOnsetState_OnsetDuration ? "duration" : "ended";
    
    return string;
}


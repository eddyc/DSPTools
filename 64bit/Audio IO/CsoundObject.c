//
//  CsoundObject.c
//  Audio-Mosaicing
//
//  Created by Edward Costello on 27/03/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "CsoundObject.h"
#import <Accelerate/Accelerate.h>

CsoundObject *CsoundObject_new(char *csdPath)
{
    CsoundObject *self = calloc(1, sizeof(CsoundObject));
    
    self->csound = csoundCreate(NULL);
    
    csoundSetHostImplementedAudioIO(self->csound, 1, 0);
    csoundSetHostData(self->csound, self);
    self->channelsCount = 2;
    
    char *argv[2] = {
        "csound",
        csdPath
    };
    
    self->csoundResult = csoundCompile(self->csound, 2, argv);
    
    if (self->csoundResult == 0) {
        printf("success\n");
    }
    
    return self;
}

void CsoundObject_delete(CsoundObject *self)
{
    csoundStop(self->csound);
    csoundDestroy(self->csound);
    free(self);
    self = NULL;
}

void CsoundObject_inputMessage(CsoundObject *self, const char *score)
{
    csoundInputMessage(self->csound, (char *)score);
}

void CsoundObject_performControlCycle(CsoundObject *self)
{
    csoundPerformKsmps(self->csound);
}

void CsoundObject_readSamplesBlock(CsoundObject *self,
                                   Float64 **audioData)
{
    Float64 *csoundOut = (Float64 *) csoundGetSpout(self->csound);
    size_t Ksmps = csoundGetKsmps(self->csound);
    
    for (size_t channel = 0; channel < self->channelsCount; ++channel) {
        
        cblas_dcopy((SInt32)Ksmps,
                    &csoundOut[channel],
                    2,
                    audioData[channel],
                    1);
    }
}

void CsoundObject_readCallback(void *inRefCon,
                               UInt32 inNumberFrames,
                               Float64 **audioData)
{
    CsoundObject *self = (CsoundObject *) inRefCon;
    Float64 *csoundOut = (Float64 *) csoundGetSpout(self->csound);
    csoundPerformKsmps(self->csound);
    
    for (size_t channel = 0; channel < self->channelsCount; ++channel) {
        
        cblas_dcopy((SInt32)inNumberFrames,
                    &csoundOut[channel],
                    2,
                    audioData[channel],
                    1);
    }
}

void CsoundObject_processingCallback(void *inRefCon,
                                     UInt32 inNumberFrames,
                                     Float64 **audioData)
{
    CsoundObject *self = (CsoundObject *) inRefCon;
    Float64 *csoundOut = (Float64 *) csoundGetSpout(self->csound);
    csoundPerformKsmps(self->csound);
    
    for (size_t channel = 0; channel < self->channelsCount; ++channel) {
        
        cblas_dcopy((SInt32)inNumberFrames,
                    &csoundOut[channel],
                    2,
                    audioData[channel],
                    1);
    }
}


    //
    //  CsoundObject.c
    //  Audio-Mosaicing
    //
    //  Created by Edward Costello on 27/03/2013.
    //  Copyright (c) 2013 Edward Costello. All rights reserved.
    //

#import "CsoundObject.h"
#import <Accelerate/Accelerate.h>
#import "ConvenienceFunctions.h"

static const UInt32 channelOneTableNumber = 100;
static const UInt32 channelTwoTableNumber = 101;
static const UInt32 warpPathTableBaseNumber = 200;
static const UInt32 bandGainTableBaseNumber = 300;

CsoundObject *CsoundObject_new(char *csdPath, size_t analysisSegmentFramesCount)
{
    CsoundObject *self = calloc(1, sizeof(CsoundObject));
    self->analysisSegmentFramesCount = analysisSegmentFramesCount;
    self->csound = csoundCreate(NULL);
    self->frameTimes = calloc(analysisSegmentFramesCount, sizeof(Float32));
    
    for (size_t i = 0; i < analysisSegmentFramesCount; ++i) {
        
        self->frameTimes[i] = ((1. / 44100) * 256) * i;
    }
    
    csoundPreCompile(self->csound);
    csoundSetHostImplementedAudioIO(self->csound, 1, 0);
    csoundSetHostData(self->csound, self);
    self->channelsCount = 2;
    char *parserFlag = "--new-parser";
    char *argv[3] = {
        "csound",
        parserFlag,
        csdPath
    };
    
    self->csoundResult = csoundCompile(self->csound, 3, argv);
    
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

void CsoundObject_readCallback(void *inRefCon, UInt32 inNumberFrames, Float32 **audioData)
{
    CsoundObject *self = (CsoundObject *) inRefCon;
    MYFLT *csoundOut = csoundGetSpout(self->csound);
    csoundPerformKsmps(self->csound);
    
    for (size_t channel = 0; channel < self->channelsCount; ++channel) {
        
        cblas_scopy((SInt32)inNumberFrames, &csoundOut[channel], 2, audioData[channel], 1);
    }
}

void CsoundObject_writeDataToTable(CsoundObject *self, UInt32 tableNumber, Float32 *data, UInt32 dataCount)
{
    self->csoundScore[0] = tableNumber;
    self->csoundScore[1] = 0;
    self->csoundScore[2] = -((Float32)dataCount);
    self->csoundScore[3] = 2;
    self->csoundScore[4] = 0;
    
    csoundScoreEvent(self->csound, 'f', self->csoundScore, 5);
    csoundPerformKsmps(self->csound);
    
    Float32 *tablePointer;
    
    csoundGetTable(self->csound, &tablePointer, tableNumber);
    
    cblas_scopy((UInt32)dataCount, data, 1, tablePointer, 1);
    
}

void CsoundObject_turnOnPhaseVocoder(CsoundObject *self, size_t phaseVocoderInstances)
{
    self->csoundScore[0] = 1;
    self->csoundScore[1] = 0;
    self->csoundScore[2] = -1;
    self->csoundScore[3] = phaseVocoderInstances;
    self->csoundScore[4] = self->analysisSegmentFramesCount;
    
    csoundScoreEvent(self->csound, 'i', self->csoundScore, 5);
    csoundPerformKsmps(self->csound);
}

void CsoundObject_createPVSFile(CsoundObject *self, char *path)
{
    char *command[100];
    char *pvanalPath = "/usr/local/bin/pvanal";
    sprintf((char *)command, "%s %s Audio-Mosaicing/PVSInput.pvoc ", pvanalPath, path);
    while(system((const char *)command));
    
}



void CsoundObject_turnOnPhasor(CsoundObject *self)
{
    self->csoundScore[0] = 3;
    self->csoundScore[1] = 0;
    self->csoundScore[2] = -1;
    
    
    csoundScoreEvent(self->csound, 'i', self->csoundScore, 3);
}


void CsoundObject_writePVSReadPath(CsoundObject *self,
                                   size_t *segmentStartFrames,
                                   size_t triangleFilterBandsCount,
                                   Matrix32 *warpPaths)


{
    
    Float32 frameLengthInSeconds = 1./44100. * 256.;
    
    for (size_t i = 0; i < triangleFilterBandsCount; ++i) {
        
        Float32 startFrameInSeconds = (Float32)segmentStartFrames[i] * frameLengthInSeconds;
        Float32 *tablePointer;
        
        csoundGetTable(self->csound, &tablePointer, (SInt32)(warpPathTableBaseNumber + i));
        vDSP_vsadd(Matrix_getRow(warpPaths, i), 1, &startFrameInSeconds, tablePointer, 1, self->analysisSegmentFramesCount);
        
    }
}

void CsoundObject_writeOpenCLPVSReadPath(CsoundObject *self,
                                         size_t triangleFilterBandsCount,
                                         size_t *bestTriangleBandMatches,
                                         Float32 *paletteSegmentFramesCounts,
                                         Float32 *paletteMagnitudeDifferences,
                                         Matrix32 *triangleBandGains)



{
    
    Float32 frameLengthInSeconds = 1./44100. * 256.;
    for (size_t i = 0; i < triangleFilterBandsCount; ++i) {
        
        Float32 paletteSegmentLengthInSeconds = frameLengthInSeconds * paletteSegmentFramesCounts[i];
        Float32 startFrameInSeconds = (Float32)bestTriangleBandMatches[i] * frameLengthInSeconds;
        Float32 endFrameInSeconds = startFrameInSeconds + paletteSegmentLengthInSeconds;

        Float32 *warpTablePointer, *bandGainTablePointer;
        
        csoundGetTable(self->csound, &warpTablePointer, (SInt32)(warpPathTableBaseNumber + i));
        csoundGetTable(self->csound, &bandGainTablePointer, (SInt32)(bandGainTableBaseNumber + i));

        vDSP_vgen(&startFrameInSeconds, &endFrameInSeconds, warpTablePointer, 1, self->analysisSegmentFramesCount);
        
        vDSP_vsmul(Matrix_getRow(triangleBandGains, i), 1, &paletteMagnitudeDifferences[i], bandGainTablePointer, 1, triangleBandGains->columnCount);
//        vDSP_vsadd(self->frameTimes, 1, &startFrameInSeconds, tablePointer, 1, self->analysisSegmentFramesCount);        
    }
}

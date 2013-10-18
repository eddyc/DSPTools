    //
    //  CsoundObject.h
    //  Audio-Mosaicing
    //
    //  Created by Edward Costello on 27/03/2013.
    //  Copyright (c) 2013 Edward Costello. All rights reserved.
    //


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <CsoundLib/csound.h>
#import "Matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct CsoundObject
    {
        CSOUND *csound;
        SInt32 csoundResult;
        UInt32 channelsCount;
        Float32 csoundScore[10];
        Float32 *frameTimes;
        size_t analysisSegmentFramesCount;
        
    } CsoundObject;
    
    CsoundObject *CsoundObject_new(char *csdPath, size_t analysisSegmentFramesCount);
    void CsoundObject_delete(CsoundObject *self);
    void CsoundObject_readCallback(void *inRefCon, UInt32 inNumberFrames, Float32 **audioData);
    void CsoundObject_writeDataToTable(CsoundObject *self, UInt32 tableNumber, Float32 *data, UInt32 dataCount);
    void CsoundObject_turnOnPhaseVocoder(CsoundObject *self, size_t phaseVocoderInstances);
    void CsoundObject_writePVSReadPath(CsoundObject *self,
                                       size_t *segmentStartFrames,
                                       size_t triangleFilterBandsCount,
                                       Matrix32 *warpPaths);
    void CsoundObject_turnOnPhasor(CsoundObject *self);
    void CsoundObject_createPVSFile(CsoundObject *self, char *path);
    void CsoundObject_writeOpenCLPVSReadPath(CsoundObject *self,
                                             size_t triangleFilterBandsCount,
                                             size_t *bestTriangleBandMatches,
                                             Float32 *paletteSegmentFramesCounts,
                                             Float32 *paletteMagnitudeDifferences,
                                             Matrix32 *triangleBandGains);

#ifdef __cplusplus
}
#endif
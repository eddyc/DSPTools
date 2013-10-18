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
#import <CsoundLib64/csound.h>
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
        Float64 csoundScore[10];
        
    } CsoundObject;
    
    CsoundObject *CsoundObject_new(char *csdPath);
    
    void CsoundObject_delete(CsoundObject *self);

    void CsoundObject_inputMessage(CsoundObject *self, const char *score);
    void CsoundObject_performControlCycle(CsoundObject *self);
    void CsoundObject_readSamplesBlock(CsoundObject *self,
                                       Float64 **audioData);
    void CsoundObject_readCallback(void *inRefCon,
                                   UInt32 inNumberFrames,
                                   Float64 **audioData);
    void CsoundObject_processingCallback(void *inRefCon,
                                         UInt32 inNumberFrames,
                                         Float64 **audioData);
    #ifdef __cplusplus
}
#endif
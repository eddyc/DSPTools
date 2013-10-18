//
//  BeatDetect.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 06/03/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>
#import "Matrix.h"
#import "FFT.h"
#import "Chromagram.h"
#import "MFCC.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct BeatDetect32
    {
        size_t samplerate;
        size_t frameCount;
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;

        size_t hopSize;
        size_t hopCount;
        FFT32 *spectralFluxFFT;
        
        Float32 tempomean;
        Float32 temposd;
        Float32 tightness;
        
        Float32 *currentMagnitudes;
        Float32 *previousMagnitudes;
        Float32 *spectralFluxVector;
        Float32 oesr;
        size_t acmax;
        size_t crossCorrelationFrameSize;
        Float32 *crossCorrelationFrame;

        Float32 *xcrwin;
        Float32 *bpms;
        Float32 *xpks;
        
        Float32 *xcr00;
        Float32 *xcr2;
        Float32 *xcr3;
        size_t xcr00Size;
        size_t xcr2Size;
        size_t xcr3Size;        
        Float32 tempos[3];
        
        Float32 *temp1_hopCountSize;
        Float32 *temp2_hopCountSize;

        Float32 convolutionVectorCount;
        Float32 *convolutionVector;
        Float32 *backlink;
        Float32 *cumulatedScore;
        
        Float32 *prange;
        Float32 *txwt;
        Float32 *timerange;
        Float32 *scorecands;
        size_t *timeRangeIndexes;
        Float32 *beats;
        size_t beatsCount;

    } BeatDetect32;
    
    BeatDetect32 *BeatDetect32_new(size_t samplerate, size_t frameCount, Float32 tempoMean);
    void BeatDetect32_delete(BeatDetect32 *);
    void BeatDetect32_getTempo(BeatDetect32 *, Float32 *samplesIn);
    void BeatDetect32_getSpectralDifference(BeatDetect32 *, Float32 *samplesIn);
    void BeatDetect32_getBeatPositions(BeatDetect32 *, Float32 *beatsOut, size_t *beatsCount);
    void BeatDetect32_process(BeatDetect32 *, Float32 *samplesIn, Float32 *beatsOut, size_t *beatsCount);
    
    
#ifdef __cplusplus
}
#endif
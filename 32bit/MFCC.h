//
//  MFCC.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 23/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct MFCC32
    {
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        size_t samplerate;
        SInt32 totalFilters;
        size_t cepstralCoefficients;
        Float32 lowestFrequency;
        UInt32 linearFilters;
        Float32 linearSpacing;
        UInt32 logFilters;
        Float32 logSpacing;
        
        Float32 **mfccFilterWeights;
        Float32 *mfccDCTMatrix;
        Float32 *earMagnitudes;
        Float32 *logEarMagnitudes;
        
    } MFCC32;
    
    MFCC32 *MFCC32_new(size_t FFTFrameSize,
                       size_t samplerate);
    
    void MFCC32_delete(MFCC32 *);

    extern inline void MFCC32_process(MFCC32 *,
                                      Float32 *__restrict inputMagnitudes,
                                      Float32 *__restrict outputMFCCs);

    
#ifdef __cplusplus
}
#endif
//
//  STFT.h
//  DSPTools
//
//  Created by Edward Costello on 19/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import "FFT.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct STFTData32 STFTData32;
    typedef struct STFT32 STFT32;
    
    struct STFT32
    {
        FFT32 *fft;
        size_t FFTFrameSize;
        size_t hopSize;
    };
    
    
    STFT32 *STFT32_new(size_t FFTFrameSize, size_t hopSize, FFTWindowType windowType);
    void STFT32_delete(STFT32 *);
    void STFT32_forwardPolar(STFT32 *, Float32 *inputSamples, size_t inputSamplesCount, STFTData32 *outputSTFTData);

    struct STFTData32
    {
        size_t FFTFrameSize;
        size_t hopSize;
        size_t frameCount;
        Float32 **magnitudes;
        Float32 **phases;
    };
    
    STFTData32 *STFTData32_new(size_t FFTFrameSize, size_t hopSize, size_t samplesCount);
    void STFTData32_delete(STFTData32 *);
    
#ifdef __cplusplus
}
#endif
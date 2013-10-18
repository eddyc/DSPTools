//
//  FFT.h
//  AudioMosaicing
//
//  Created by Edward Costello on 15/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct FFT
    {
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        size_t log2FFTFrameSize;
        DSPDoubleSplitComplex complexData;
        Float64 *interlacedPolar;
        Float64 *temp;
        FFTSetupD FFTSetup;
        
    } FFT;
    
    FFT *FFT_new(size_t FFTFrameSize);
    void FFT_delete(FFT *self);
    
    void FFT_forwardComplex(FFT *self,
                            Float64 *samplesIn,
                            Float64 *realOut,
                            Float64 *imaginaryOut);
    
    void FFT_inverseComplex(FFT *self,
                            Float64 *realIn,
                            Float64 *imaginaryIn,
                            Float64 *samplesOut);
    
    void FFT_forwardPolar(FFT *self,
                          Float64 *samplesIn,
                          Float64 *magnitudesOut,
                          Float64 *phasesOut);
    
    void FFT_inversePolar(FFT *self,
                          Float64 *magnitudesIn,
                          Float64 *phasesIn,
                          Float64 *samplesOut);
    
    void FFT_forwardMagnitudes(FFT *self,
                               Float64 *samplesIn,
                               Float64 *magnitudesOut);
    
#ifdef __cplusplus
}
#endif
//
//  FFT.h
//  DSPTools
//
//  Created by Edward Costello on 17/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>
#import "MathematicalFunctions.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef enum FFTWindowType
    {
        kFFTWindowType_None,
        kFFTWindowType_Hanning,
        kFFTWindowType_Hamming,
        kFFTWindowType_Blackman
    } FFTWindowType;
    
#pragma mark 32Bit
    
    typedef struct FFT32 FFT32;
    struct FFT32
    {
        Float32 *window;
        Float32 *interlacedPolar;
        Float32 *frameBuffer;
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        Float32 log2n;
        
        FFTSetup FFTData;
        DSPSplitComplex splitComplex;
    };
    
    FFT32 *FFT32_new(size_t FFTFrameSize, FFTWindowType windowType);
    void FFT32_delete(FFT32 *);
    
    void FFT32_configureWindow(FFT32 *, FFTWindowType windowType);
    
    void FFT32_forwardPolarWindowed(FFT32 *,
                                    Float32 *__restrict inputFrame,
                                    Float32 *__restrict outputMagnitudes,
                                    Float32 *__restrict outputPhases);
    
    extern inline void FFT32_forwardComplex(FFT32 *,
                                            Float32 *__restrict inputFrame,
                                            Float32 *__restrict outputReal,
                                            Float32 *__restrict outputImaginary);
    
    extern inline void FFT32_forwardPolar(FFT32 *,
                                          Float32 *__restrict inputFrame,
                                          Float32 *__restrict outputMagnitudes,
                                          Float32 *__restrict outputPhases);
    
    extern inline void FFT32_forwardComplex(FFT32 *,
                                            Float32 *__restrict inputFrame,
                                            Float32 *__restrict outputReal,
                                            Float32 *__restrict outputImaginary);
    
    extern inline void FFT32_forwardComplexWindowed(FFT32 *self,
                                                    Float32 *__restrict inputFrame,
                                                    Float32 *__restrict outputReal,
                                                    Float32 *__restrict outputImaginary);
    
    extern inline void FFT32_forwardMagnitudes(FFT32 *,
                                               Float32 *__restrict inputFrame,
                                               Float32 *__restrict outputMagnitudes);
    
    extern inline void FFT32_forwardMagnitudesWindowed(FFT32 *,
                                                       Float32 *__restrict inputFrame,
                                                       Float32 *__restrict outputMagnitudes);
    
    extern inline void FFT32_inverseComplex(FFT32 *,
                                            Float32 *__restrict inputReal,
                                            Float32 *__restrict inputImaginary,
                                            Float32 *__restrict outputFrame);
    
    extern inline void FFT32_inversePolar(FFT32 *,
                                          Float32 *__restrict inputMagnitudes,
                                          Float32 *__restrict inputPhases,
                                          Float32 *__restrict outputFrame);
    
    
#pragma mark 64Bit
    
    
    typedef struct FFT64
    {
        Float64 *window;
        Float64 *interlacedPolar;
        
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        Float64 log2n;
        
        FFTSetupD FFTData;
        DSPDoubleSplitComplex splitComplex;
        
        
    } FFT64;
    
    FFT64 *FFT64_new(size_t FFTFrameSize, FFTWindowType windowType);
    void FFT64_delete(FFT64 *);
    
    void FFT64_configureWindow(FFT64 *, FFTWindowType windowType);
    
    void FFT64_forwardComplex(FFT64 *self,
                              Float64 *__restrict inputFrame,
                              Float64 *__restrict outputReal,
                              Float64 *__restrict outputImaginary);
    void FFT64_forwardPolar(FFT64 *,
                            Float64 *__restrict inputFrame,
                            Float64 *__restrict outputMagnitudes,
                            Float64 *__restrict outputPhases);
    
    void FFT64_forwardPolarWindowed(FFT64 *,
                                    Float64 *__restrict inputFrame,
                                    Float64 *__restrict outputMagnitudes,
                                    Float64 *__restrict outputPhases);
    
    void FFT64_inversePolar(FFT64 *,
                            Float64 *__restrict inputMagnitudes,
                            Float64 *__restrict inputPhases,
                            Float64 *__restrict outputFrame);
    
#ifdef __cplusplus
}
#endif
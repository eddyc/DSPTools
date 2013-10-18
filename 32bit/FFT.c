//
//  FFT.c
//  DSPTools
//
//  Created by Edward Costello on 17/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "FFT.h"
#import <stdio.h>

const static size_t FFT_minimumFFTFrameSize = 128;

#pragma mark 32Bit

FFT32 *FFT32_new(size_t FFTFrameSize, FFTWindowType windowType)
{
    FFT32 *self = calloc(1, sizeof(FFT32));
    
    if (FFTFrameSize < FFT_minimumFFTFrameSize) {
        
        printf("FFTFrameSize specified is lower than minimum, defaulting to minimum\n");
        FFTFrameSize = FFT_minimumFFTFrameSize;
    }
    
    self->FFTFrameSize = nextPowerOfTwo(FFTFrameSize);
    self->FFTFrameSizeOver2 = self->FFTFrameSize / 2;
    self->log2n = log2f((Float32)FFTFrameSize);
    self->frameBuffer = calloc(self->FFTFrameSize, sizeof(Float32));
    self->window = calloc(self->FFTFrameSize, sizeof(Float32));
    self->interlacedPolar = calloc(self->FFTFrameSize, sizeof(Float32));
    self->splitComplex.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->splitComplex.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->FFTData = vDSP_create_fftsetup(self->log2n, FFT_RADIX2);
    
    FFT32_configureWindow(self, windowType);
    
    return self;
}

void FFT32_delete(FFT32 *self)
{
    free(self->window);
    free(self->frameBuffer);
    free(self->interlacedPolar);
    free(self->splitComplex.realp);
    free(self->splitComplex.imagp);
    vDSP_destroy_fftsetup(self->FFTData);
    
    free(self);
    self = NULL;
}

void FFT32_configureWindow(FFT32 *self, FFTWindowType windowType)
{
    Float32 one = 1;
    
    switch (windowType) {
            
        case kFFTWindowType_None:
            
            vDSP_vfill(&one, self->window, 1, self->FFTFrameSize);
            break;
            
        case kFFTWindowType_Hanning:
            
            vDSP_hann_window(self->window, self->FFTFrameSize, vDSP_HANN_DENORM);
            break;
            
        case kFFTWindowType_Hamming:
            
            vDSP_hamm_window(self->window, self->FFTFrameSize, 0);
            break;
            
        default:
            printf("Valid window not specified, default to none\n");
            vDSP_vfill(&one, self->window, 1, self->FFTFrameSize);
            break;
    }
}


static inline void FFT32_forward(FFT32 *self, Float32 *__restrict inputFrame)
{
    vDSP_ctoz((DSPComplex *)inputFrame, 2, &self->splitComplex, 1, self->FFTFrameSizeOver2);
    vDSP_fft_zrip(self->FFTData, &self->splitComplex, 1, self->log2n, kFFTDirection_Forward);
    
    //    self->splitComplex.realp[self->FFTFrameSizeOver2 - 1] = self->splitComplex.imagp[0];// move real Nyquist term to where it belongs
    //    self->splitComplex.imagp[self->FFTFrameSizeOver2 - 1] = 0.0f;// this is zero
    self->splitComplex.imagp[0] = 0.0f;
    
}

inline void FFT32_forwardComplex(FFT32 *self,
                                 Float32 *__restrict inputFrame,
                                 Float32 *__restrict outputReal,
                                 Float32 *__restrict outputImaginary)
{
    FFT32_forward(self, inputFrame);
    Float32 zeroPointFive = 0.5f;
    
    vDSP_vsmul(self->splitComplex.realp, 1, &zeroPointFive, outputReal, 1, self->FFTFrameSizeOver2);
    vDSP_vsmul(self->splitComplex.imagp, 1, &zeroPointFive, outputImaginary, 1, self->FFTFrameSizeOver2);
}

inline void FFT32_forwardComplexWindowed(FFT32 *self,
                                         Float32 *__restrict inputFrame,
                                         Float32 *__restrict outputReal,
                                         Float32 *__restrict outputImaginary)
{
    vDSP_vmul(inputFrame, 1, self->window, 1, self->frameBuffer, 1, self->FFTFrameSize);

    FFT32_forward(self, self->frameBuffer);
    Float32 zeroPointFive = 0.5f;
    
    vDSP_vsmul(self->splitComplex.realp, 1, &zeroPointFive, outputReal, 1, self->FFTFrameSizeOver2);
    vDSP_vsmul(self->splitComplex.imagp, 1, &zeroPointFive, outputImaginary, 1, self->FFTFrameSizeOver2);
}

inline void FFT32_forwardPolar(FFT32 *self,
                               Float32 *__restrict inputFrame,
                               Float32 *__restrict outputMagnitudes,
                               Float32 *__restrict outputPhases)
{
    
    FFT32_forward(self, inputFrame);
    
    vDSP_ztoc(&self->splitComplex, 1, (DSPComplex *)self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_polar(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    
    Float32 zeroPointFive = 0.5f;
    
    vDSP_vsmul(&self->interlacedPolar[0], 2, &zeroPointFive, outputMagnitudes, 1, self->FFTFrameSizeOver2);
    vDSP_vsmul(&self->interlacedPolar[1], 2, &zeroPointFive, outputPhases, 1, self->FFTFrameSizeOver2);
}

inline void FFT32_forwardPolarWindowed(FFT32 *self,
                                       Float32 *__restrict inputFrame,
                                       Float32 *__restrict outputMagnitudes,
                                       Float32 *__restrict outputPhases)
{
    vDSP_vmul(inputFrame, 1, self->window, 1, self->frameBuffer, 1, self->FFTFrameSize);
    
    FFT32_forwardPolar(self, self->frameBuffer, outputMagnitudes, outputPhases);
}


inline void FFT32_forwardMagnitudes(FFT32 *self,
                                    Float32 *__restrict inputFrame,
                                    Float32 *__restrict outputMagnitudes)
{
    FFT32_forward(self, inputFrame);
    
    vDSP_ztoc(&self->splitComplex, 1, (DSPComplex *)self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_polar(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    
    Float32 zeroPointFive = 0.5f;
    
    vDSP_vsmul(&self->interlacedPolar[0], 2, &zeroPointFive, outputMagnitudes, 1, self->FFTFrameSizeOver2);
}

inline void FFT32_forwardMagnitudesWindowed(FFT32 *self,
                                            Float32 *__restrict inputFrame,
                                            Float32 *__restrict outputMagnitudes)
{
    vDSP_vmul(inputFrame, 1, self->window, 1, self->frameBuffer, 1, self->FFTFrameSize);
    
    FFT32_forwardMagnitudes(self, self->frameBuffer, outputMagnitudes);
}


inline void FFT32_inverseComplex(FFT32 *self,
                                 Float32 *__restrict inputReal,
                                 Float32 *__restrict inputImaginary,
                                 Float32 *__restrict outputFrame)
{
    cblas_scopy((SInt32)self->FFTFrameSizeOver2, inputReal, 1, self->splitComplex.realp, 1);
    cblas_scopy((SInt32)self->FFTFrameSizeOver2, inputImaginary, 1, self->splitComplex.imagp, 1);
    
    vDSP_fft_zrip (self->FFTData, &self->splitComplex, 1, self->log2n, kFFTDirection_Inverse);
    vDSP_ztoc(&self->splitComplex, 1, (DSPComplex *) outputFrame, 2, self->FFTFrameSizeOver2);
    
    Float32 scalar = 1.f/((Float32)self->FFTFrameSize);
    
    vDSP_vsmul(outputFrame, 1, &scalar, outputFrame, 1, self->FFTFrameSize);
}

inline void FFT32_inversePolar(FFT32 *self,
                               Float32 *__restrict inputMagnitudes,
                               Float32 *__restrict inputPhases,
                               Float32 *__restrict outputFrame)
{
    Float32 two = 2.0f;
    
    vDSP_vsmul(inputMagnitudes, 1, &two, &self->interlacedPolar[0], 2, self->FFTFrameSizeOver2);
    vDSP_vsmul(inputPhases, 1, &two, &self->interlacedPolar[1], 2, self->FFTFrameSizeOver2);
    
    vDSP_rect(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_ctoz((DSPComplex *) self->interlacedPolar, 2, &self->splitComplex, 1, self->FFTFrameSizeOver2);
    
    vDSP_fft_zrip (self->FFTData, &self->splitComplex, 1, self->log2n, kFFTDirection_Inverse);
    vDSP_ztoc(&self->splitComplex, 1, (DSPComplex *) outputFrame, 2, self->FFTFrameSizeOver2);
    
    Float32 scalar = 1.f/((Float32)self->FFTFrameSize * 2);
    
    vDSP_vsmul(outputFrame, 1, &scalar, outputFrame, 1, self->FFTFrameSize);
}

#pragma mark 64Bit

FFT64 *FFT64_new(size_t FFTFrameSize, FFTWindowType windowType)
{
    FFT64 *self = calloc(1, sizeof(FFT64));
    
    if (FFTFrameSize < FFT_minimumFFTFrameSize) {
        
        printf("FFTFrameSize specified is lower than minimum, defaulting to minimum\n");
        FFTFrameSize = FFT_minimumFFTFrameSize;
    }
    
    self->FFTFrameSize = nextPowerOfTwo(FFTFrameSize);
    self->FFTFrameSizeOver2 = self->FFTFrameSize / 2;
    self->log2n = log2((Float64)FFTFrameSize);
    
    self->window = calloc(self->FFTFrameSize, sizeof(Float64));
    self->interlacedPolar = calloc(self->FFTFrameSize, sizeof(Float64));
    self->splitComplex.realp = calloc(self->FFTFrameSize, sizeof(Float64));
    self->splitComplex.imagp = calloc(self->FFTFrameSize, sizeof(Float64));
    self->FFTData = vDSP_create_fftsetupD(self->log2n, FFT_RADIX2);
    
    FFT64_configureWindow(self, windowType);
    
    return self;
}

void FFT64_delete(FFT64 *self)
{
    free(self->window);
    free(self->interlacedPolar);
    free(self->splitComplex.realp);
    free(self->splitComplex.imagp);
    vDSP_destroy_fftsetupD(self->FFTData);
    
    free(self);
    self = NULL;
}

void FFT64_configureWindow(FFT64 *self, FFTWindowType windowType)
{
    Float64 one = 1;
    
    switch (windowType) {
            
        case kFFTWindowType_None:
            
            vDSP_vfillD(&one, self->window, 1, self->FFTFrameSize);
            break;
            
        case kFFTWindowType_Hanning:
            
            vDSP_hann_windowD(self->window, self->FFTFrameSize, vDSP_HANN_DENORM);
            break;
            
        default:
            printf("Valid window not specified, default to none\n");
            vDSP_vfillD(&one, self->window, 1, self->FFTFrameSize);
            break;
    }
}

static inline void FFT64_forward(FFT64 *self, Float64 *__restrict inputFrame)
{
    vDSP_ctozD((DSPDoubleComplex *)inputFrame, 2, &self->splitComplex, 1, self->FFTFrameSizeOver2);
    vDSP_fft_zripD(self->FFTData, &self->splitComplex, 1, self->log2n, kFFTDirection_Forward);
    
    self->splitComplex.realp[self->FFTFrameSizeOver2] = self->splitComplex.imagp[0];   // move real Nyquist term to where it belongs
    self->splitComplex.imagp[self->FFTFrameSizeOver2] = 0.0f;          // this is zero
    self->splitComplex.imagp[0] = 0.0f;
}

inline void FFT64_forwardComplex(FFT64 *self,
                                 Float64 *__restrict inputFrame,
                                 Float64 *__restrict outputReal,
                                 Float64 *__restrict outputImaginary)
{
    FFT64_forward(self, inputFrame);
    cblas_dcopy((SInt32)self->FFTFrameSize, self->splitComplex.realp, 1, outputReal, 1);
    cblas_dcopy((SInt32)self->FFTFrameSize, self->splitComplex.imagp, 1, outputImaginary, 1);
    
    Float64 zeroPointFive = 0.5f;
    
    vDSP_vsmulD(outputReal, 1, &zeroPointFive, outputReal, 1, self->FFTFrameSize);
    vDSP_vsmulD(outputImaginary, 1, &zeroPointFive, outputImaginary, 1, self->FFTFrameSize);
}


inline void FFT64_forwardPolar(FFT64 *self,
                               Float64 *__restrict inputFrame,
                               Float64 *__restrict outputMagnitudes,
                               Float64 *__restrict outputPhases)
{
    
    FFT64_forward(self, inputFrame);
    
    vDSP_ztocD(&self->splitComplex, 1, (DSPDoubleComplex *)self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_polarD(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    
    Float64 zeroPointFive = 0.5;
    
    vDSP_vsmulD(&self->interlacedPolar[0], 2, &zeroPointFive, outputMagnitudes, 1, self->FFTFrameSizeOver2);
    vDSP_vsmulD(&self->interlacedPolar[1], 2, &zeroPointFive, outputPhases, 1, self->FFTFrameSizeOver2);
}

inline void FFT64_forwardPolarWindowed(FFT64 *self,
                                       Float64 *__restrict inputFrame,
                                       Float64 *__restrict outputMagnitudes,
                                       Float64 *__restrict outputPhases)
{
    vDSP_vmulD(inputFrame, 1, self->window, 1, inputFrame, 1, self->FFTFrameSize);
    
    FFT64_forwardPolar(self, inputFrame, outputMagnitudes, outputPhases);
    
}


inline void FFT64_inversePolar(FFT64 *self,
                               Float64 *__restrict inputMagnitudes,
                               Float64 *__restrict inputPhases,
                               Float64 *__restrict outputFrame)
{
    Float64 two = 2.0f;
    
    vDSP_vsmulD(inputMagnitudes, 1, &two, &self->interlacedPolar[0], 2, self->FFTFrameSizeOver2);
    vDSP_vsmulD(inputPhases, 1, &two, &self->interlacedPolar[1], 2, self->FFTFrameSizeOver2);
    
    vDSP_rectD(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_ctozD((DSPDoubleComplex *) self->interlacedPolar, 2, &self->splitComplex, 1, self->FFTFrameSizeOver2);
    
    vDSP_fft_zripD (self->FFTData, &self->splitComplex, 1, self->log2n, kFFTDirection_Inverse);
    vDSP_ztocD(&self->splitComplex, 1, (DSPDoubleComplex *) outputFrame, 2, self->FFTFrameSizeOver2);
    
    Float64 scalar = 1.f/((Float64)self->FFTFrameSize * 2);
    
    vDSP_vsmulD(outputFrame, 1, &scalar, outputFrame, 1, self->FFTFrameSize);
    
}

//
//  FFT.c
//  AudioMosaicing
//
//  Created by Edward Costello on 15/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "FFT.h"

FFT *FFT_new(size_t FFTFrameSize)
{
    FFT *self = calloc(1, sizeof(FFT));
    
    self->FFTFrameSize = FFTFrameSize;
    self->FFTFrameSizeOver2 = self->FFTFrameSize / 2;
    self->log2FFTFrameSize = (size_t)log2(FFTFrameSize);
    
    self->interlacedPolar = calloc(self->FFTFrameSize, sizeof(Float64));
    self->temp = calloc(self->FFTFrameSize, sizeof(Float64));
    self->FFTSetup = vDSP_create_fftsetupD(self->log2FFTFrameSize, kFFTRadix2);
    
    self->complexData.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float64));
    self->complexData.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float64));
    
    return self;
}

void FFT_delete(FFT *self)
{
    vDSP_destroy_fftsetupD(self->FFTSetup);
    free(self->complexData.realp);
    free(self->complexData.imagp);
    free(self->interlacedPolar);
    free(self->temp);
    free(self);
    self = NULL;
}

void FFT_forwardComplex(FFT *self,
                        Float64 *samplesIn,
                        Float64 *realOut,
                        Float64 *imaginaryOut)
{
    
    vDSP_ctozD((DSPDoubleComplex *) samplesIn, 2, &self->complexData, 1, self->FFTFrameSizeOver2);
    vDSP_fft_zripD(self->FFTSetup, &self->complexData, 1, self->log2FFTFrameSize, kFFTDirection_Forward);
    Float64 pointFive = 0.5;
    vDSP_vsmulD(self->complexData.realp, 1, &pointFive, realOut, 1, self->FFTFrameSizeOver2);
    vDSP_vsmulD(self->complexData.imagp, 1, &pointFive, imaginaryOut, 1, self->FFTFrameSizeOver2);
    imaginaryOut[0] = 0;
}

void FFT_inverseComplex(FFT *self,
                        Float64 *realIn,
                        Float64 *imaginaryIn,
                        Float64 *samplesOut)
{
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, realIn, 1, self->complexData.realp, 1);
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, imaginaryIn, 1, self->complexData.imagp, 1);
    
    self->complexData.imagp[0] = realIn[self->FFTFrameSizeOver2 - 1];
    vDSP_fft_zripD(self->FFTSetup, &self->complexData, 1, self->log2FFTFrameSize, kFFTDirection_Inverse);
    
    Float64 scale = (Float64) 1.0 / (self->FFTFrameSize);
    
    vDSP_vsmulD(self->complexData.realp, 1, &scale, self->complexData.realp, 1, self->FFTFrameSizeOver2);
    vDSP_vsmulD(self->complexData.imagp, 1, &scale, self->complexData.imagp, 1, self->FFTFrameSizeOver2);
    
    vDSP_ztocD(&self->complexData, 1, (DSPDoubleComplex *) samplesOut, 2, self->FFTFrameSizeOver2);
}

void FFT_forwardPolar(FFT *self,
                      Float64 *samplesIn,
                      Float64 *magnitudesOut,
                      Float64 *phasesOut)
{
    
    FFT_forwardComplex(self, samplesIn, self->complexData.realp, self->complexData.imagp);
    
    vDSP_ztocD(&self->complexData, 1, (DSPDoubleComplex *)self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    vDSP_polarD(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, &self->interlacedPolar[0], 2, magnitudesOut, 1);
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, &self->interlacedPolar[1], 2, phasesOut, 1);
}

void FFT_forwardMagnitudes(FFT *self,
                           Float64 *samplesIn,
                           Float64 *magnitudesOut)
{
    
    FFT_forwardComplex(self, samplesIn, self->complexData.realp, self->complexData.imagp);
    vDSP_vdistD(self->complexData.realp, 1, self->complexData.imagp, 1, magnitudesOut, 1, self->FFTFrameSizeOver2);
}

void FFT_inversePolar(FFT *self,
                      Float64 *magnitudesIn,
                      Float64 *phasesIn,
                      Float64 *samplesOut)
{
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, magnitudesIn, 1, &self->interlacedPolar[0], 2);
    cblas_dcopy((UInt32)self->FFTFrameSizeOver2, phasesIn, 1, &self->interlacedPolar[1], 2);
    vDSP_rectD(self->interlacedPolar, 2, self->interlacedPolar, 2, self->FFTFrameSizeOver2);
    
    vDSP_ctozD((DSPDoubleComplex *) self->interlacedPolar, 2, &self->complexData, 1, self->FFTFrameSizeOver2);
    
    FFT_inverseComplex(self, self->complexData.realp, self->complexData.imagp, samplesOut);
}
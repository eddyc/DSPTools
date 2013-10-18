//
//  DFT.c
//  Audio Concatenator Rewrite
//
//  Created by Edward Costello on 02/06/2012.
//  Copyright (c) 2012 NUI Maynooth. All rights reserved.
//

#import "DFT.h"


DFT *DFT_new(UInt32 FFTFrameSize,
             DFT_WINDOW windowType)
{
    DFT *self = (DFT *)malloc(sizeof(DFT));
    
    self->windowType = windowType;
    self->FFTFrameSize = FFTFrameSize;
    self->FFTFrameSizeOver2 = FFTFrameSize / 2;
    self->log2FFTFrameSize = log2(FFTFrameSize);
    self->window = (Float32 *)calloc(FFTFrameSize, sizeof(Float32));
    self->inReal = (Float32 *)calloc(FFTFrameSize, sizeof(Float32));
    self->outReal = (Float32 *)calloc(FFTFrameSize, sizeof(Float32));
    
    self->FFTDataReal = (Float32 *)calloc(FFTFrameSize / 2, sizeof(Float32));
    self->FFTDataImag = (Float32 *)calloc(FFTFrameSize / 2, sizeof(Float32));
    
    self->FFTSplitComplex.realp = self->FFTDataReal;
    self->FFTSplitComplex.imagp = self->FFTDataImag;
    
    self->FFTWeights = vDSP_create_fftsetup(self->log2FFTFrameSize, kFFTRadix2);
    self->FFTWeightsCepstral = vDSP_create_fftsetup(self->log2FFTFrameSize, kFFTRadix2);

    
    if (self->windowType == DFT_WINDOW_HANNING) {
        
        vDSP_hann_window(self->window, self->FFTFrameSize, vDSP_HANN_DENORM);
        
    }
    else if (self->windowType == DFT_WINDOW_HAMMING) {
        
        vDSP_hamm_window(self->window, self->FFTFrameSize, 0);
    }
    else if (self->windowType == DFT_WINDOW_NONE) {
        
        Float32 one = 1;
        vDSP_vfill(&one, self->window, 1, self->FFTFrameSize);
    }
    else {
        
        printf("Valid window not specified, default to none");
        Float32 one = 1;
        vDSP_vfill(&one, self->window, 1, self->FFTFrameSize);
    }
    
    
    
    self->scale = 1. /  (self->FFTFrameSize * 2);
    
    return self;
}

void DFT_delete(DFT *self)
{
    free(self->window);
    free(self->inReal);
    free(self->outReal);
    free(self->FFTDataReal);
    free(self->FFTDataImag);
    vDSP_destroy_fftsetup(self->FFTWeights);
    free(self);
    self = NULL;
}

void DFT_forward(const DFT *self,
                 const AudioSampleType *inputReal,
                 Float32 *outputReal,
                 Float32 *outputImaginary)
{
    cblas_scopy(self->FFTFrameSize,
                inputReal,
                1,
                self->inReal,
                1);
    
    
    //    //Multiply by window function
    vDSP_vmul(self->inReal,
              1,
              self->window,
              1,
              self->inReal,
              1,
              self->FFTFrameSize);
    
    
    
    //Cast to split complex
    vDSP_ctoz((DSPComplex *)self->inReal,
              2,
              (DSPSplitComplex *)&self->FFTSplitComplex,
              1,
              self->FFTFrameSizeOver2);
    
    
    
    //Perform FFT
    vDSP_fft_zrip(self->FFTWeights,
                  (DSPSplitComplex *)&self->FFTSplitComplex,
                  1,
                  self->log2FFTFrameSize,
                  FFT_FORWARD);
    
    
    
    self->FFTSplitComplex.imagp[0] = 0.; //Zero the Nyquist
    
    cblas_scopy(self->FFTFrameSizeOver2, self->FFTSplitComplex.realp, 1, outputReal, 1);
    
//    Float32 scale = 0.5;
//    
//    vDSP_vsmul(outputReal,
//               1,
//               &scale,
//               outputReal,
//               1,
//               self->FFTFrameSizeOver2);
    
    cblas_scopy(self->FFTFrameSizeOver2, self->FFTSplitComplex.imagp, 1, outputImaginary, 1);
//    
//    vDSP_vsmul(outputImaginary,
//               1,
//               &scale,
//               outputImaginary,
//               1,
//               self->FFTFrameSizeOver2);
}

void DFT_forwardPolar(const DFT *self,
                      const AudioSampleType *inputReal,
                      Float32 *outputMagnitudes,
                      Float32 *outputPhases)
{
    cblas_scopy(self->FFTFrameSize,
                inputReal,
                1,
                self->inReal,
                1);
    
    
    //    //Multiply by window function
    vDSP_vmul(self->inReal,
              1,
              self->window,
              1,
              self->inReal,
              1,
              self->FFTFrameSize);
    
    
    
    //Cast to split complex
    vDSP_ctoz((DSPComplex *)self->inReal,
              2,
              (DSPSplitComplex *)&self->FFTSplitComplex,
              1,
              self->FFTFrameSizeOver2);
    
    
    
    //Perform FFT
    vDSP_fft_zrip(self->FFTWeights,
                  (DSPSplitComplex *)&self->FFTSplitComplex,
                  1,
                  self->log2FFTFrameSize,
                  FFT_FORWARD);
    
    
    
    self->FFTSplitComplex.imagp[0] = 0.; //Zero the Nyquist
    
    
    
    vDSP_ztoc(&self->FFTSplitComplex,
              1,
              (COMPLEX *) self->inReal,
              2,
              self->FFTFrameSizeOver2);
    
    vDSP_polar(self->inReal,
               2,
               self->outReal,
               2,
               self->FFTFrameSizeOver2);
    
    cblas_scopy(self->FFTFrameSizeOver2,
                self->outReal,
                2,
                outputMagnitudes,
                1);
    
    Float32 scale = 0.5;
    
//    vDSP_vsmul(outputMagnitudes,
//               1,
//               &scale,
//               outputMagnitudes,
//               1,
//               self->FFTFrameSizeOver2);
//
    cblas_scopy(self->FFTFrameSizeOver2,
                self->outReal + 1,
                2,
                outputPhases,
                1);
}


void DFT_forwardMagnitudes(const DFT *self,
                           const Float32 *inputReal,
                           Float32 *outputMagnitudes)
{
    cblas_scopy(self->FFTFrameSize,
                inputReal,
                1,
                self->inReal,
                1);
    
    
    //    //Multiply by window function
    vDSP_vmul(self->inReal,
              1,
              self->window,
              1,
              self->inReal,
              1,
              self->FFTFrameSize);
    
    //Cast to split complex
    vDSP_ctoz((DSPComplex *)self->inReal,
              2,
              (DSPSplitComplex *)&self->FFTSplitComplex,
              1,
              self->FFTFrameSizeOver2);
    
    
    
    //Perform FFT
    vDSP_fft_zrip(self->FFTWeights,
                  (DSPSplitComplex *)&self->FFTSplitComplex,
                  1,
                  self->log2FFTFrameSize,
                  FFT_FORWARD);
    
    
    
    self->FFTSplitComplex.imagp[0] = 0.; //Zero the Nyquist
    
    
    
    vDSP_ztoc(&self->FFTSplitComplex,
              1,
              (COMPLEX *) self->inReal,
              2,
              self->FFTFrameSizeOver2);
    
    vDSP_polar(self->inReal,
               2,
               self->outReal,
               2,
               self->FFTFrameSizeOver2);
    
    cblas_scopy(self->FFTFrameSizeOver2,
                self->outReal,
                2,
                outputMagnitudes,
                1);
    
//    Float32 scale = 0.5;
//    
//    vDSP_vsmul(outputMagnitudes,
//               1,
//               &scale,
//               outputMagnitudes,
//               1,
//               self->FFTFrameSizeOver2);
    
}


void DFT_inversePolar(const DFT *self,
                      const Float32 *inputMagnitudes,
                      const Float32 *inputPhases,
                      AudioSampleType *outputFrame)
{
    cblas_scopy(self->FFTFrameSizeOver2,
                inputMagnitudes,
                1,
                self->inReal,
                2);
    
    cblas_scopy(self->FFTFrameSizeOver2,
                inputPhases,
                1,
                self->inReal + 1,
                2);
    
    vDSP_rect(self->inReal,
              2,
              self->outReal,
              2,
              self->FFTFrameSizeOver2);
    
    DSPSplitComplex FFTSplitComplex = self->FFTSplitComplex;
    
    vDSP_ctoz((COMPLEX *) self->outReal,
              2,
              &FFTSplitComplex,
              1,
              self->FFTFrameSizeOver2);
    
    vDSP_fft_zrip(self->FFTWeights,
                  &FFTSplitComplex,
                  1,
                  self->log2FFTFrameSize,
                  FFT_INVERSE);
    
    vDSP_ztoc(&self->FFTSplitComplex,
              1,
              (COMPLEX*) self->outReal,
              2,
              self->FFTFrameSizeOver2);
    
    vDSP_vsmul(self->outReal,
               1,
               &self->scale,
               self->outReal,
               1,
               self->FFTFrameSize);
    
    vDSP_vmul(self->outReal,
              1,
              self->window,
              1,
              outputFrame,
              1,
              self->FFTFrameSize);
    
    cblas_scopy(self->FFTFrameSize,
                self->outReal,
                1,
                outputFrame,
                1);
}

void DFT_inverseCepstral(const DFT *self,
                         const Float32 *inputMagnitudes,
                         Float32 *outputCepstrum)
{

    cblas_scopy(self->FFTFrameSizeOver2,
                inputMagnitudes,
                1,
                self->inReal,
                1);
    
    const SInt32 FFTFrameSizeOver2Const = self->FFTFrameSizeOver2;
    
    vvlogf(self->FFTSplitComplex.realp,
           self->inReal,
           &FFTFrameSizeOver2Const);
    
    vDSP_vclr(self->FFTSplitComplex.imagp, 1, self->FFTFrameSizeOver2);
        
    vDSP_fft_zrip(self->FFTWeights,
                  &self->FFTSplitComplex,
                  1,
                  self->log2FFTFrameSize,
                  FFT_INVERSE);
    
    vDSP_ztoc(&self->FFTSplitComplex,
              1,
              (COMPLEX *)outputCepstrum,
              2,
              self->FFTFrameSizeOver2);
    
    vDSP_vsmul(outputCepstrum,
               1,
               &self->scale,
               outputCepstrum,
               1,
               self->FFTFrameSize);
  
}


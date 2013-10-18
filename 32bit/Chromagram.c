//
//  Chromagram.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 18/02/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "Chromagram.h"
#import <string.h>
#import <stdio.h>
#import "ConvenienceFunctions.h"

Chromagram32 *Chromagram32_new(size_t FFTFrameSize, size_t samplerate)
{
    Chromagram32 *self = calloc(1, sizeof(Chromagram32));
    self->FFTFrameSize = FFTFrameSize;
    self->FFTFrameSizeOver2 = self->FFTFrameSize/2;
    self->FFTFrameSizeOver4 = self->FFTFrameSizeOver2/2;

    self->samplerate = samplerate;
    self->differentiationWindow = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    Chromagram32_configureDifferentiationWindow(self);
    
    self->hanningWindow = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    vDSP_hann_window(self->hanningWindow, self->FFTFrameSizeOver2, vDSP_HANN_DENORM);
    Float32 hanningWindowSum = 0;
    vDSP_sve(self->hanningWindow, 1, &hanningWindowSum, self->FFTFrameSizeOver2);
    self->normalisationFactor = 2./hanningWindowSum;
    
    
    self->fft = FFT32_new(FFTFrameSize, kFFTWindowType_None);
    self->differentiationFrame = calloc(self->FFTFrameSize, sizeof(Float32));
    self->hanningFrame = calloc(self->FFTFrameSize, sizeof(Float32));
    self->FFTShiftBuffer = calloc(self->FFTFrameSize, sizeof(Float32));
    self->zeroToPiLine = calloc(self->FFTFrameSize, sizeof(Float32));

    for (size_t i = 0; i < self->FFTFrameSize; ++i) {
        
        self->zeroToPiLine[i] = (2 * M_PI * (Float32)i * self->samplerate)/(Float32)self->FFTFrameSize;
    }

    
    self->instFreqTemp.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->instFreqTemp.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->instFreqSpec.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->instFreqSpec.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->spectrumTemp.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->spectrumTemp.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->tempComplex.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->tempComplex.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    
    self->spectrum.realp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->spectrum.imagp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));

    self->magnitudesTemp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->frequenciesTemp = calloc(self->FFTFrameSizeOver2, sizeof(Float32));
    self->frequencies = calloc(self->FFTFrameSizeOver2, sizeof(Float32));

    self->f_ctr = 1000;
    self->f_sd = 1;
    self->fminl = octs2hz(hz2octs(self->f_ctr)- 2 * self->f_sd);
    self->fminu = octs2hz(hz2octs(self->f_ctr) - self->f_sd);
    self->fmaxl = octs2hz(hz2octs(self->f_ctr) + self->f_sd);
    self->fmaxu = octs2hz(hz2octs(self->f_ctr) + 2 * self->f_sd);
    self->nchr = 12;
    
    self->maxbin = roundf(self->fmaxu * self->FFTFrameSize/self->samplerate);
    self->tempFloat1_maxbinSize = calloc(self->maxbin, sizeof(Float32));
    self->tempFloat2_maxbinSize = calloc(self->maxbin, sizeof(Float32));
    self->octavesBinary = calloc(self->maxbin, sizeof(Float32));

    self->ddif = calloc(self->maxbin, sizeof(Float32));
    self->dgood = calloc(self->maxbin, sizeof(Float32));
    self->st = calloc(self->maxbin, sizeof(Float32));
    self->en = calloc(self->maxbin, sizeof(Float32));
    self->roundingArray = calloc(self->maxbin, sizeof(SInt32));
    self->noteNumbers = calloc(self->maxbin * self->nchr, sizeof(Float32));
    self->notesPresent = calloc(self->maxbin * self->nchr, sizeof(Float32));

    Float32 one = 1;
    
    for (size_t i = 0; i < self->maxbin; ++i) {
        
        vDSP_vramp(&one, &one, &self->noteNumbers[i], self->maxbin, self->nchr);

    }

    self->histogramTemp = calloc(101, sizeof(Float32));
    
    self->pitches = calloc(self->maxbin, sizeof(Float32));
    self->magnitudes = calloc(self->maxbin, sizeof(Float32));
    self->octaves = calloc(self->maxbin, sizeof(Float32));

    return self;
}

void Chromagram32_delete(Chromagram32 *self)
{
    free(self->noteNumbers);
    free(self->notesPresent);
    free(self->differentiationWindow);
    free(self->hanningWindow);
    free(self->differentiationFrame);
    free(self->hanningFrame);
    free(self->FFTShiftBuffer);
    free(self->instFreqTemp.realp);
    free(self->instFreqTemp.imagp);
    free(self->instFreqSpec.realp);
    free(self->instFreqSpec.imagp);
    free(self->spectrumTemp.realp);
    free(self->spectrumTemp.imagp);
    free(self->spectrum.realp);
    free(self->spectrum.imagp);
    free(self->tempComplex.realp);
    free(self->tempComplex.imagp);
    free(self->zeroToPiLine);
    free(self->magnitudesTemp);
    free(self->frequenciesTemp);
    free(self->frequencies);
    free(self->octavesBinary);
    free(self->roundingArray);
    FFT32_delete(self->fft);
    
    free(self->tempFloat1_maxbinSize);
    free(self->tempFloat2_maxbinSize);
    free(self->ddif);
    free(self->dgood);
    free(self->st);
    free(self->en);
    free(self->histogramTemp);
    free(self->pitches);
    free(self->magnitudes);
    free(self->octaves);
    free(self);
    self = NULL;
}

void Chromagram32_process(Chromagram32 *self, Float32 *samplesIn, Float32 *chromagramOut)
{
    Chromagram32_getFrequenciesAndSpectrum(self, samplesIn);    
    Chromagram32_getPitchesAndMagnitudes(self);
    
    Chromagram32_chromaQuantizedSinusoids(self);
    
    Chromagram32_calculateChromagram(self, chromagramOut);
}


Float32 octs2hz(Float32 octs)
{
    Float32 A440 = 440;
    
    return (A440 / 16) * powf(2, octs);
}

Float32 hz2octs(Float32 freq)
{
    Float32 A440 = 440;
    
    return logf(freq / (A440 / 16)) / logf(2);
}

void Chromagram32_configureDifferentiationWindow(Chromagram32 *self)
{
    Float32 delta = (Float32)(self->FFTFrameSizeOver2)/(Float32)(self->samplerate);
    
    for (size_t i = 0; i < self->FFTFrameSizeOver2; ++i) {
        
        self->differentiationWindow[i] = -M_PI / delta * sinf((Float32)(i) / (Float32)(self->FFTFrameSizeOver2) * 2 * M_PI);
    }
}

static void Chromagram32_FFTShift32(Chromagram32 *self, Float32 *input)
{    
    memcpy(self->FFTShiftBuffer, input, sizeof(Float32) * self->FFTFrameSize);
    memcpy(&input[self->FFTFrameSizeOver2], self->FFTShiftBuffer, sizeof(Float32) * (self->FFTFrameSizeOver2));
    memcpy(input, &self->FFTShiftBuffer[self->FFTFrameSizeOver2], sizeof(Float32) * (self->FFTFrameSizeOver2));
    
}


void Chromagram32_getFrequenciesAndSpectrum(Chromagram32 *self,
                                            Float32 *samplesIn)
{
    vDSP_vclr(self->differentiationFrame, 1, self->FFTFrameSize);
    vDSP_vclr(self->hanningFrame, 1, self->FFTFrameSize);
    vDSP_vmul(samplesIn, 1, self->hanningWindow, 1, &self->hanningFrame[self->FFTFrameSizeOver4], 1, self->FFTFrameSizeOver2);
    vDSP_vmul(samplesIn, 1, self->differentiationWindow, 1, &self->differentiationFrame[self->FFTFrameSizeOver4], 1, self->FFTFrameSizeOver2);


    Chromagram32_FFTShift32(self, self->differentiationFrame);
    Chromagram32_FFTShift32(self, self->hanningFrame);
        
    FFT32_forwardComplex(self->fft, self->hanningFrame, self->spectrumTemp.realp, self->spectrumTemp.imagp);
    FFT32_forwardComplex(self->fft, self->differentiationFrame, self->instFreqSpec.realp, self->instFreqSpec.imagp);

    vDSP_vsmul(self->spectrumTemp.realp, 1, &self->normalisationFactor, self->spectrum.realp, 1, self->FFTFrameSizeOver2);
    vDSP_vsmul(self->spectrumTemp.imagp, 1, &self->normalisationFactor, self->spectrum.imagp, 1, self->FFTFrameSizeOver2);
    Float32 minusOne = -1;
    vDSP_vsmul(self->spectrum.imagp, 1, &minusOne, self->spectrum.imagp, 1, self->FFTFrameSizeOver2);
    
    vDSP_zrvmul(&self->spectrumTemp, 1, self->zeroToPiLine, 1, &self->instFreqTemp, 1, self->FFTFrameSizeOver2);

    Float32 *tempPointer = self->instFreqTemp.realp;
    self->instFreqTemp.realp = self->instFreqTemp.imagp;
    self->instFreqTemp.imagp = tempPointer;
    
    vDSP_vsub(self->instFreqTemp.realp, 1, self->instFreqSpec.realp, 1, self->instFreqSpec.realp, 1, self->FFTFrameSizeOver2);
    vDSP_vadd(self->instFreqSpec.imagp, 1, self->instFreqTemp.imagp, 1, self->instFreqSpec.imagp, 1, self->FFTFrameSizeOver2);

    vDSP_zvabs(&self->spectrumTemp, 1, self->magnitudesTemp, 1, self->FFTFrameSizeOver2);

    
    for (size_t i = 0; i < self->FFTFrameSizeOver2; ++i) {
        
        self->magnitudesTemp[i] = self->magnitudesTemp[i] == 0 ? 1 : 0;
    }
    
    vDSP_zvmgsa(&self->spectrumTemp, 1, self->magnitudesTemp, 1, self->magnitudesTemp, 1, self->FFTFrameSizeOver2);
    
    vDSP_vmmsb(self->spectrumTemp.realp, 1, self->instFreqSpec.imagp, 1, self->spectrumTemp.imagp, 1, self->instFreqSpec.realp, 1, self->frequencies, 1, self->FFTFrameSizeOver2);
    
    Float32 reciprocalTwoPi = 1./(2. * M_PI);
    
    vDSP_vsmul(self->frequencies, 1, &reciprocalTwoPi, self->frequencies, 1, self->FFTFrameSizeOver2);
    vDSP_vdiv(self->magnitudesTemp, 1, self->frequencies, 1, self->frequencies, 1, self->FFTFrameSizeOver2);
}


void Chromagram32_getPitchesAndMagnitudes(Chromagram32 *self)
{
    cblas_scopy((SInt32)self->maxbin, &self->frequencies[1], 1, self->tempFloat1_maxbinSize, 1);
    self->tempFloat1_maxbinSize[self->maxbin - 1] = self->tempFloat1_maxbinSize[self->maxbin - 2];
    
    cblas_scopy((SInt32)self->maxbin, self->frequencies, 1, &self->tempFloat2_maxbinSize[1], 1);
    self->tempFloat2_maxbinSize[0] = self->tempFloat2_maxbinSize[1];
    
    vDSP_vsub(self->tempFloat2_maxbinSize, 1, self->tempFloat1_maxbinSize, 1, self->ddif, 1, self->maxbin);
    
    vDSP_vabs(self->ddif, 1, self->dgood, 1, self->maxbin);
    
    Float32 threshold = .75 * (Float32)self->samplerate/(Float32)self->FFTFrameSize;
    Float32 limitValue = -1;
    Float32 zero = 0;
    vDSP_vlim(self->dgood, 1, &threshold, &limitValue, self->dgood, 1, self->maxbin);
    vDSP_vthres(self->dgood, 1, &zero, self->dgood, 1, self->maxbin);
    
    cblas_scopy((SInt32)self->maxbin, &self->dgood[1], 1, self->tempFloat1_maxbinSize, 1);
    self->tempFloat1_maxbinSize[self->maxbin - 1] = self->tempFloat1_maxbinSize[self->maxbin - 2];
    
    cblas_scopy((SInt32)self->maxbin, self->dgood, 1, &self->tempFloat2_maxbinSize[1], 1);
    self->tempFloat2_maxbinSize[0] = self->tempFloat2_maxbinSize[1];
    vDSP_vsub(self->tempFloat2_maxbinSize, 1, self->tempFloat1_maxbinSize, 1, self->tempFloat1_maxbinSize, 1, self->maxbin);
    
    for (size_t i = 0; i < self->maxbin; ++i) {
        
        self->dgood[i] = self->dgood[i] * (self->tempFloat1_maxbinSize[i] > 0 || self->tempFloat2_maxbinSize[i] > 0 ? 1 : 0);
    }
    
    
    vDSP_vclr(self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vclr(self->tempFloat2_maxbinSize, 1, self->maxbin);
    
    cblas_scopy((SInt32)self->maxbin - 1, self->dgood , 1, &self->tempFloat1_maxbinSize[1], 1);
    cblas_scopy((SInt32)self->maxbin - 1, &self->dgood[1] , 1, self->tempFloat2_maxbinSize, 1);
    
    UInt32 st_length = 0;
    UInt32 en_length = 0;
    
    for (size_t i = 0; i < self->maxbin; ++i) {
        
        self->tempFloat1_maxbinSize[i] = self->tempFloat1_maxbinSize[i] == 0 && self->dgood[i] > 0 ? 1 : 0;
        self->tempFloat2_maxbinSize[i] = self->tempFloat2_maxbinSize[i] == 0 && self->dgood[i] > 0 ? 1 : 0;
        
        if (self->tempFloat1_maxbinSize[i] == 1) {
            
            self->st[st_length] = (UInt32)i;
            st_length++;
        }
        if (self->tempFloat2_maxbinSize[i] == 1) {
            
            self->en[en_length] = (UInt32)i;
            en_length++;
        }
    }
    
    vDSP_vclr(self->frequenciesTemp, 1, self->maxbin);
    vDSP_vclr(self->magnitudesTemp, 1, self->maxbin);
    
    SInt32 copyLength = 0;
    
    for (size_t i = 0; i < st_length; ++i) {
        
        copyLength = (SInt32)self->en[i] - (SInt32)self->st[i] + 1;
        cblas_scopy(copyLength, &self->spectrum.realp[(SInt32)self->st[i]], 1, self->tempComplex.realp, 1);
        cblas_scopy(copyLength, &self->spectrum.imagp[(SInt32)self->st[i]], 1, self->tempComplex.imagp, 1);
        vDSP_zvabs(&self->tempComplex, 1, self->tempFloat1_maxbinSize, 1, copyLength);
        cblas_scopy(copyLength, &self->frequencies[(SInt32)self->st[i]], 1, self->tempFloat2_maxbinSize, 1);
        Float32 dotProduct;
        vDSP_dotpr(self->tempFloat1_maxbinSize, 1, self->tempFloat2_maxbinSize, 1, &dotProduct, copyLength);
        Float32 sum;
        vDSP_sve(self->tempFloat1_maxbinSize, 1, &sum, copyLength);
        
        self->magnitudesTemp[i] = sum;
        
        sum += sum == 0 ? 1 : 0;
        
        self->frequenciesTemp[i] = dotProduct/sum;
        
        if (self->frequenciesTemp[i] > self->fmaxu) {
            
            self->magnitudesTemp[i] = 0;
            self->frequenciesTemp[i] = 0;
        }
        else if (self->frequenciesTemp[i] > self->fmaxl) {
            
            Float32 comparison = (self->fmaxu - self->frequenciesTemp[i])/(self->fmaxu - self->fmaxl);
            self->magnitudesTemp[i] = self->magnitudesTemp[i] * (0 > comparison ? 0 : comparison);
            
        }
        
        if (self->frequenciesTemp[i] < self->fminl) {
            
            self->magnitudesTemp[i] = 0;
            self->frequenciesTemp[i] = 0;
        }
        else if (self->frequenciesTemp[i] < self->fminu) {
            
            self->magnitudesTemp[i] = self->magnitudesTemp[i] * (self->frequenciesTemp[i] - self->fminl)/(self->fminu - self->fminl);
        }
        
        if (self->frequenciesTemp[i] < 0) {
            
            self->magnitudesTemp[i] = 0;
            self->frequenciesTemp[i] = 0;
        }
    }

    vDSP_vadd(self->st, 1, self->en, 1, self->tempFloat1_maxbinSize, 1, st_length);
    Float32 two = 2;
    vDSP_vsdiv(self->tempFloat1_maxbinSize, 1, &two, self->tempFloat1_maxbinSize, 1, st_length);
    vDSP_vclr(self->magnitudes, 1, self->maxbin);

    for (size_t i = 0; i < st_length; ++i) {
        
        UInt32 currentIndex = fmodf(self->tempFloat1_maxbinSize[i], 1.) >= 0.5 ? self->tempFloat1_maxbinSize[i] + 0.5f : self->tempFloat1_maxbinSize[i];
        
        self->pitches[currentIndex] = self->frequenciesTemp[i];
        self->magnitudes[currentIndex] = self->magnitudesTemp[i];
    }
}

void Chromagram32_chromaQuantizedSinusoids(Chromagram32 *self)
{
    Float32 zero = 0, one = 1;
    vDSP_vneg(self->pitches, 1, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vthrsc(self->tempFloat1_maxbinSize, 1, &zero, &one, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vthres(self->tempFloat1_maxbinSize, 1, &zero, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vadd(self->tempFloat1_maxbinSize, 1, self->pitches, 1, self->octaves, 1, self->maxbin);

    Float32 scalar = 440./16.;
    vDSP_vsdiv(self->octaves, 1, &scalar, self->octaves, 1, self->maxbin);
    const int maxbinConst = (int)self->maxbin;
    vvlogf(self->octaves, self->octaves, &maxbinConst);
    
    scalar = log(2);
    vDSP_vsdiv(self->octaves, 1, &scalar, self->octaves, 1, self->maxbin);
    
    vDSP_vneg(self->tempFloat1_maxbinSize, 1, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vthrsc(self->tempFloat1_maxbinSize, 1, &zero, &one, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vthres(self->tempFloat1_maxbinSize, 1, &zero, self->tempFloat1_maxbinSize, 1, self->maxbin);    
    vDSP_vmul(self->tempFloat1_maxbinSize, 1, self->octaves, 1, self->octaves, 1, self->maxbin);
}

void Chromagram32_getHistogram(Chromagram32 *self, Float32 *inputValues, size_t inputSize, size_t outputSize, Float32 *outputValues, Float32 *outputCounts)
{
    Float32 minInput, maxInput;

    vDSP_maxv(inputValues, 1, &maxInput, inputSize);
    vDSP_minv(inputValues, 1, &minInput, inputSize);
    
    Float32 range = maxInput - minInput;
    Float32 binWidth = range / (Float32)outputSize;
    Float32 halfBinWidth = binWidth/2;
    Float32 one = 1;
    vDSP_vramp(&minInput, &binWidth, self->histogramTemp, 1, outputSize + 1);
    vDSP_vsadd(self->histogramTemp, 1, &halfBinWidth, outputValues, 1, outputSize);
    vDSP_vsadd(outputCounts, 1, &one, outputCounts, 1, outputSize);    
    vDSP_vsort(inputValues, inputSize, 1);
    
    size_t histogramCounter = 0;
    for (size_t i = 0; i < inputSize; ++i) {
        
        if (inputValues[i] >= self->histogramTemp[histogramCounter]
            &&
            inputValues[i] < self->histogramTemp[histogramCounter + 1]) {
            
            outputCounts[histogramCounter]++;
        }
        else {
            
            histogramCounter++;
        }
    }
}

void Chromagram32_calculateChromagram(Chromagram32 *self, Float32 *chromagramOut)
{
    Float32 zero = 0;
    Float32 minusOne = -1;
    vDSP_vneg(self->octaves, 1, self->octavesBinary, 1, self->maxbin);
    vDSP_vthrsc(self->octavesBinary, 1, &zero, &minusOne, self->octavesBinary, 1, self->maxbin);
    vDSP_vthres(self->octavesBinary, 1, &zero, self->octavesBinary, 1, self->maxbin);
    
    vDSP_vsmul(self->octaves, 1, &self->nchr, self->tempFloat1_maxbinSize, 1, self->maxbin);
    vDSP_vfixr32(self->tempFloat1_maxbinSize, 1, self->roundingArray, 1, self->maxbin);
    vDSP_vflt32(self->roundingArray, 1, self->tempFloat2_maxbinSize, 1, self->maxbin);
    vDSP_vsdiv(self->tempFloat2_maxbinSize, 1, &self->nchr, self->tempFloat2_maxbinSize, 1, self->maxbin);

    vDSP_vfix32(self->tempFloat2_maxbinSize, 1, self->roundingArray, 1, self->maxbin);
    vDSP_vflt32(self->roundingArray, 1, self->tempFloat1_maxbinSize, 1,  self->maxbin);
    vDSP_vsub(self->tempFloat1_maxbinSize, 1, self->tempFloat2_maxbinSize, 1, self->tempFloat2_maxbinSize, 1,  self->maxbin);
    vDSP_vsmul(self->tempFloat2_maxbinSize, 1, &self->nchr, self->tempFloat2_maxbinSize, 1, self->maxbin);
    vDSP_vfixr32(self->tempFloat2_maxbinSize, 1, self->roundingArray, 1, self->maxbin);
    vDSP_vflt32(self->roundingArray, 1, self->tempFloat2_maxbinSize, 1, self->maxbin);

    vDSP_vsadd(self->tempFloat2_maxbinSize, 1, &minusOne, self->tempFloat2_maxbinSize, 1, self->maxbin);
    vDSP_vadd(self->tempFloat2_maxbinSize, 1, self->octavesBinary, 1, self->tempFloat2_maxbinSize, 1, self->maxbin);
    
    vDSP_vneg(self->tempFloat2_maxbinSize, 1, self->tempFloat2_maxbinSize, 1, self->maxbin);
    Float32 minusEleven = -11;
    vDSP_vthres(self->tempFloat2_maxbinSize, 1, &minusEleven, self->tempFloat2_maxbinSize, 1, self->maxbin);
    vDSP_vneg(self->tempFloat2_maxbinSize, 1, self->tempFloat2_maxbinSize, 1, self->maxbin);
    
    for (size_t j = 0; j < self->maxbin; ++j) {
        
        Float32 scalar = self->tempFloat2_maxbinSize[j];
        vDSP_vfill(&scalar, &self->notesPresent[j], self->maxbin, self->nchr);
    }
    

    size_t elementCount = self->maxbin * self->nchr;
    Float32 one = 1;
    vDSP_vsadd(self->notesPresent, 1, &one, self->notesPresent, 1, elementCount);
    vDSP_vdiv(self->notesPresent, 1, self->noteNumbers, 1, self->notesPresent, 1, elementCount);

    vDSP_vsadd(self->notesPresent, 1, &minusOne, self->notesPresent, 1, elementCount);
    vDSP_vabs(self->notesPresent, 1, self->notesPresent, 1, elementCount);
    vDSP_vneg(self->notesPresent, 1, self->notesPresent, 1, elementCount);
    vDSP_vthrsc(self->notesPresent, 1, &zero, &one, self->notesPresent, 1, elementCount);
    vDSP_vthres(self->notesPresent, 1, &one, self->notesPresent, 1, elementCount);

    cblas_sgemm(CblasRowMajor,
                CblasNoTrans,
                CblasTrans,
                self->nchr,
                1,
                (SInt32)self->maxbin,
                1.0f,
                self->notesPresent,
                (SInt32)self->maxbin,
                self->magnitudes,
                (SInt32)self->maxbin,
                0.0f,
                chromagramOut,
                1);
}

// Code for finding better tuning alignment:
//
//    This happens just after the quantised sinusoids method
//
//    vDSP_vsmul(octaves->data, 1, &numberOfChroma, tempMatrix1->data, 1, octaves->elementCount);
//    vDSP_vfixr32(tempMatrix1->data, 1, roundingArray, 1, tempMatrix1->elementCount);
//    vDSP_vflt32(roundingArray, 1, tempMatrix2->data, 1, tempMatrix2->elementCount);
//    vDSP_vsub(tempMatrix2->data, 1, tempMatrix1->data, 1, tempMatrix2->data, 1, tempMatrix2->elementCount);
//    vDSP_vclr(tempMatrix1->data, 1, tempMatrix1->elementCount);
//    vDSP_vcmprs(tempMatrix2->data, 1, tempMatrix2->data, 1, tempMatrix1->data, 1, tempMatrix2->elementCount);
//    Float32 dummy;
//    size_t index;
//    vDSP_minmgvi(tempMatrix1->data, 1, &dummy, &index, tempMatrix1->elementCount);
//
//    Chromagram32_getHistogram(chromagram, tempMatrix1->data, index, histogramSize, histogramValues, histogramCounts);
//
//    Float32 maxHistogramValue;
//    size_t maxHistogramIndex;
//
//    vDSP_maxvi(histogramCounts, 1, &maxHistogramValue, &maxHistogramIndex, histogramSize);
//    maxHistogramValue = -(histogramValues[maxHistogramIndex] / numberOfChroma);
//
//    The maxHistogramValue is then added to octaves using:
//
//    vDSP_vsadd(octaves->data, 1, &maxHistogramValue, octaves->data, 1, octaves->elementCount);
//    vDSP_vmul(octaves->data, 1, octavesBinary->data, 1, octaves->data, 1, octaves->elementCount);

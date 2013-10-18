//
//  Chromagram.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 18/02/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

//
//  DTW.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 25/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import "Matrix.h"
#import "FFT.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct Chromagram32
    {
        size_t samplerate;
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        size_t FFTFrameSizeOver4;
        Float32 *differentiationWindow;
        Float32 *hanningWindow;
        Float32 *differentiationFrame;
        Float32 *hanningFrame;
        Float32 *FFTShiftBuffer;
        Float32 *zeroToPiLine;
        Float32 *magnitudesTemp;
        Float32 *frequenciesTemp;
        SInt32 *roundingArray;
        Float32 *noteNumbers;
        Float32 *notesPresent;
        
        Float32 *pitches;
        Float32 *magnitudes;
        Float32 *octaves;
        Float32 *frequencies;
        DSPSplitComplex spectrum;
        

        Float32 normalisationFactor;
        DSPSplitComplex instFreqTemp;
        DSPSplitComplex instFreqSpec;
        DSPSplitComplex spectrumTemp;
        
        Float32 f_ctr;
        Float32 f_sd;
        Float32 fminl;
        Float32 fminu;
        Float32 fmaxl;
        Float32 fmaxu;
        Float32 nchr;
        size_t maxbin;
        
        Float32 *tempFloat1_maxbinSize;
        Float32 *tempFloat2_maxbinSize;
        
        Float32 *ddif;
        Float32 *dgood;
        Float32 *st;
        Float32 *en;
        Float32 *octavesBinary;
        DSPSplitComplex tempComplex;

        Float32 *histogramTemp;
        FFT32 *fft;
    } Chromagram32;
    
    Chromagram32 *Chromagram32_new(size_t FFTFrameSize, size_t samplerate);
    void Chromagram32_delete(Chromagram32 *);
    void Chromagram32_process(Chromagram32 *, Float32 *samplesIn, Float32 *chromagramOut);

    void Chromagram32_getFrequenciesAndSpectrum(Chromagram32 *, Float32 *samplesIn);
    void Chromagram32_getPitchesAndMagnitudes(Chromagram32 *);
    void Chromagram32_getHistogram(Chromagram32 *, Float32 *inputValues, size_t inputSize, size_t outputSize, Float32 *outputValues, Float32 *outputCounts);
    
    void Chromagram32_chromaQuantizedSinusoids(Chromagram32 *);
    void Chromagram32_calculateChromagram(Chromagram32 *, Float32 *chromagramOut);
    void Chromagram32_configureDifferentiationWindow(Chromagram32 *);
    Float32 octs2hz(Float32 octs);
    Float32 hz2octs(Float32 freq);


#ifdef __cplusplus
}
#endif
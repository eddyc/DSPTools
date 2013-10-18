//
//  FFTTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 05/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <Accelerate/Accelerate.h>
#import "ConvenienceFunctions.h"
#import "FFT.h"

// Note when doing an iSTFT the produced audio frame is multiplied by 0.5 then added on to the rest of the samples when using a hann window

void FFTTests()
{
    size_t samplerate = 44100;
    Float64 frequency = 440;
    
    size_t FFTFrameSize = 1024;
    FFT *fft = FFT_new(FFTFrameSize);
    
    size_t sampleCount = FFTFrameSize;
    
    Float64 *inputSamples = calloc(sampleCount, sizeof(Float64));
    Float64 *outputSamples = calloc(sampleCount, sizeof(Float64));
    Float64 *phase = calloc(FFTFrameSize/2, sizeof(Float64));
    Float64 *magnitude = calloc(FFTFrameSize/2, sizeof(Float64));
    Float64 *magnitude2 = calloc(FFTFrameSize/2, sizeof(Float64));

    
    for (size_t i = 0; i < sampleCount; ++i) {
        
        inputSamples[i] = cos(2 * M_PI * frequency * (Float64)i/(Float64)samplerate);
    }
    
    FFT_forwardPolar(fft, inputSamples, magnitude, phase);
    FFT_forwardMagnitudes(fft, inputSamples, magnitude2);
    
    vDSP_vsubD(magnitude, 1, magnitude2, 1, magnitude, 1, 512);
    
    Float64_printArray(magnitude, 512, "");
//    for (size_t i= 0; i < FFTFrameSize; ++i) {
//        
//        FFT_forwardPolar(fft, inputSamples, magnitude, phase);
//        FFT_inversePolar(fft, magnitude, phase, outputSamples);
//        
//        printf("%f\n%f\n", inputSamples[i], outputSamples[i]);
//    }
    
    FFT_delete(fft);
    free(phase);
    free(magnitude);
    free(outputSamples);
    free(inputSamples);

}
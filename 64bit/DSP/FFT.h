//
//  FFT.h
//  AudioMosaicing
//
//  Created by Edward Costello on 15/07/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class FFT
     *
     * @abstract A class for performing forward / inverse 1D fast fourier transforms
     *
     * @param FFTFrameSize The size of the FFT frame
     * @param FFTFrameSizeOver2 The size of the FFT frame over two
     * @param log2FFTFrameSize The log2 size of the FFT frame
     * @param complexData Buffer for complex data
     * @param interlacedPolar Buffer for interlaced polar data
     * @param temp Temporary buffer
     * @param FFTSetup Setup structure for Accelerates FFT
     */
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
    
    /**
     *  Construct an FFT object
     *
     *  @param FFTFrameSize The FFT frame size
     *
     *  @return An FFT object
     */
    FFT *FFT_new(size_t FFTFrameSize);
    
    /**
     *  Destroy an FFT object
     *
     *  @param self An FFT object
     */
    void FFT_delete(FFT *self);
    
    /**
     *  Peform a forward FFT to complex results
     *
     *  @param self         An FFT instance
     *  @param samplesIn    Real samples in
     *  @param realOut      Real FFT data out
     *  @param imaginaryOut Imaginary FFT data out
     */
    void FFT_forwardComplex(FFT *self,
                            Float64 *samplesIn,
                            Float64 *realOut,
                            Float64 *imaginaryOut);
    
    /**
     *  Perform an inverse complex FFT to real results
     *
     *  @param self        An FFT instance
     *  @param realIn      Real FFT data in
     *  @param imaginaryIn Imaginary FFT data in
     *  @param samplesOut  Real samples out
     */
    void FFT_inverseComplex(FFT *self,
                            Float64 *realIn,
                            Float64 *imaginaryIn,
                            Float64 *samplesOut);
    
    /**
     *  Perform a forward FFT to polar results
     *
     *  @param self          An FFT instance
     *  @param samplesIn     Real samples In
     *  @param magnitudesOut Magnitudes out
     *  @param phasesOut     Phases out
     */
    void FFT_forwardPolar(FFT *self,
                          Float64 *samplesIn,
                          Float64 *magnitudesOut,
                          Float64 *phasesOut);
    
    /**
     *  Peform an inverse polar FFT to real results
     *
     *  @param self         An FFT instance
     *  @param magnitudesIn Magnitudes In
     *  @param phasesIn     Phases In
     *  @param samplesOut   Real samples out
     */
    void FFT_inversePolar(FFT *self,
                          Float64 *magnitudesIn,
                          Float64 *phasesIn,
                          Float64 *samplesOut);
    
    /**
     *  Perform a forward FFT to magnitude results
     *
     *  @param self          An FFT instance
     *  @param samplesIn     Real samples In
     *  @param magnitudesOut Magnitudes out
     */
    void FFT_forwardMagnitudes(FFT *self,
                               Float64 *samplesIn,
                               Float64 *magnitudesOut);
    
#ifdef __cplusplus
}
#endif
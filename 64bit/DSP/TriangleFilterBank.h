//
//  TriangleFilterBank.h
//  Audio-Mosaicing
//
//  Created by Edward Costello on 01/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class TriangleFilterBank
     *
     * @abstract A triangle filter bank similar to that used in MFCC analysis
     *
     * @param filterCount The amount of triangle filters
     * @param filterFrequencyCount The amount of frequencies used to calculate the filters
     * @param magnitudeFrameSize The size of FFT analysis frame
     * @param samplerate The samplerate
     * @param filterFrequencies The center frequencies of the filters
     * @param filterBank The filter bank matrix
     */
    typedef struct TriangleFilterBank
    {
        size_t filterCount;
        size_t filterFrequencyCount;
        size_t magnitudeFrameSize;
        size_t samplerate;
        Float64 *filterFrequencies;
        Float64 *filterBank;
        
    } TriangleFilterBank;
    
    /**
     *  Construct a TriangleFilterBank object
     *
     *  @param filterCount        The amount of triangle filters
     *  @param magnitudeFrameSize The size of the FFT analysis frame
     *  @param samplerate         The samplerate
     *
     *  @return A TriangleFilterBank object
     */
    TriangleFilterBank *TriangleFilterBank_new(size_t filterCount,
                                               size_t magnitudeFrameSize,
                                               size_t samplerate);
    
    /**
     *  Destroy a TriangleFilterBank object
     *
     *  @param self A TriangleFilterBank object
     */
    void TriangleFilterBank_delete(TriangleFilterBank *self);
    
    /**
     *  Filter spectral magnitudes using TriangleFilterBank object
     *
     *  @param self                  A TriangleFilterBank object
     *  @param magnitudesIn          Input spectral magnitudes
     *  @param filteredMagnitudesOut Output filtered magnitudes
     */
    void TriangleFilterBank_process(TriangleFilterBank *self,
                                    Float64 *magnitudesIn,
                                    Float64 *filteredMagnitudesOut);
    
#ifdef __cplusplus
}
#endif
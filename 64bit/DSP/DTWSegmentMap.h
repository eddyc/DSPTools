//
//  DTWSegmentMap.h
//  AudioMosaicing
//
//  Created by Edward Costello on 28/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"
#import "Matrix.h"
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @class DTWSegmentMap
     *
     * @abstract A class for specifying the segmentation of palette data for DTW comparisons
     *
     * @param paletteFrameCount The amount of frames in the palette data
     * @param paletteFrequencyBinCount The amount of frequency bins in the palette data
     * @param paletteSegmentCount The amount segments the palette is split into
     * @param paletteSegmentFrameCount The amount of frames in each palettes segments
     * @param paletteSegmentHopSize The hop size at which to create segments
     * @param frequencySplitCount The amount of bands the frequency bins are split into
     * @param paletteSegmentStartIndices The starting indices of each palette segment
     * @param frequencyStartIndices The starting indices of each palette band
     * @param frequencyStartIndicesInt The starting indices of each palette band as an int array
     * @param frequencySplitSizes The sizes in bins of each frequency band
     * @param frequencySplitSizesInt The sizes in bins of each frequency band as an int array
     */
    typedef struct _DTWSegmentMap
    {
        size_t paletteFrameCount;
        size_t paletteFrequencyBinCount;
        size_t paletteSegmentCount;
        size_t paletteSegmentFrameCount;
        size_t paletteSegmentHopSize;
        size_t frequencySplitCount;
        
        Float64 *paletteSegmentStartIndices;
        Float64 *frequencyStartIndices;
        SInt32 *frequencyStartIndicesInt;
        Float64 *frequencySplitSizes;
        SInt32 *frequencySplitSizesInt;
        
    } DTWSegmentMap;
    
    /**
     *  Construct a DTWSegmentMap object
     *
     *  @param paletteData The palette data this map applies to
     *
     *  @return A DTWSegmentMap object
     */
    OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(const Matrix *const paletteData);
    
    /**
     *  Construct a DTWSegmentMap object
     *
     *  @param frameCount                   The amount of frames in the palette data
     *  @param maximumFrequencyChannelCount The maximum amount of frequency bins in the palette data
     *
     *  @return A DTWSegmentMap object
     */
    OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(size_t frameCount, size_t maximumFrequencyChannelCount);
    
    /**
     *  Destroy a DTWSegmentMap object
     *
     *  @param self A reference to a DTWSegmentMap object
     */
    void DTWSegmentMap_delete(DTWSegmentMap *self);
    
    /**
     *  Scoped destruction of a DTWSegmentMap object
     *
     *  @param self A reference to a DTWSegmentMap object
     */
    void DTWSegmentMap_scopedDelete(DTWSegmentMap **self);

    /**
     *  Used to specify a DTWSegmentMap as scoped
     *
     */
#define _DTWSegmentMap __attribute__((unused)) __attribute__((cleanup(DTWSegmentMap_scopedDelete))) DTWSegmentMap

    /**
     *  Calculate the segment map for palette data
     *
     *  @param self A reference to a DTWSegmentMap object
     *  @param paletteFrameCount The amount of frames in the palette data
     *  @param paletteSegmentHopSize The hop size at which to create segments
     *  @param frequencySplitCount The amount of bands the frequency bins are split into
     */
    void DTWSegmentMap_calculateSegmentMap(DTWSegmentMap *const self,
                                           const size_t paletteSegmentFrameCount,
                                           const size_t paletteSegmentHopSize,
                                           const size_t frequencySplitCount);
    

    
#ifdef __cplusplus
}
#endif
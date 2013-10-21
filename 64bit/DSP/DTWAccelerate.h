//
//  DTWAccelarate.h
//  AudioMosaicing
//
//  Created by Edward Costello on 20/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DTWSegmentMap.h"
#import "MatrixRingBuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class DTWAccelerate
     *
     * @abstract A class for performing dynamic time warping in real time using Apples Accelerate framework.
     *
     * @param paletteData A weak pointer to the palette data which will be compared
     * @param segmentMap A weak pointer to a DTWSegmentMap object
     * @param currentPaletteSegment A scoped reference to the current palette data segment
     * @param currentTestGlobalCostSegment A scoped reference to the current global cost segment
     * @param maximumInputBufferFrameCount The maximum frame count in the DTW input buffer
     * @param bestMatchArray An array that stores the scores of each DTW comparison
     * @param infinityRow A row containing the value INF
     * @param tempLocalCostRow A temporary buffer for the local DTW cost
     * @param bestMatchMatrix A matrix containing the best matching segment
     * @param globalCostMatrix A matrix containing the global DTW cost
     * @param costRingBuffer A ringbuffer for storing the global cost of each consecutive frame
     */
    typedef struct _DTWAccelerate
    {
        const Matrix *paletteData;
        const DTWSegmentMap *segmentMap;
        Matrix currentPaletteSegment;
        Matrix currentTestGlobalCostSegment;
        
        size_t maximumInputBufferFrameCount;
        size_t *bestMatchArray;
        Float64 *infinityRow;
        Float64 *tempLocalCostRow;
        Matrix *bestMatchMatrix;
        
        Matrix *globalCostMatrix;
        MatrixRingBuffer *costRingBuffer;
        
    } DTWAccelerate;
    
    /**
     *  Construct a DTWAccelerate instance
     *
     *  @param paletteData                  A reference to the palette data to compare
     *  @param maximumInputBufferFrameCount The maximum amount of frames in the DTWAccelerate input buffer
     *  @param segmentMap                   A reference to the segment map to use for comparisons
     *
     *  @return A DTWAccelerate instance
     */
    DTWAccelerate *const DTWAccelerate_new(const Matrix *const paletteData,
                                           size_t maximumInputBufferFrameCount,
                                           const DTWSegmentMap *segmentMap);
    
    /**
     *  Destroy a DTWAccelerate instance
     *
     *  @param self A pointer to a DTWAccelerate instance
     */
    void DTWAccelerate_delete(DTWAccelerate *self);
    
    /**
     *  Scoped destruction of a DTWAccelerate instance
     *
     *  @param self A pointer to a DTWAccelerate instance
     */
    void DTWAccelerate_scopedDelete(DTWAccelerate **self);
    
    
    /**
     *  Used to create a scoped instance of a DTWAccelerate object
     */
#define _DTWAccelerate __attribute__((unused)) __attribute__((cleanup(DTWAccelerate_scopedDelete))) DTWAccelerate
    
    /**
     *  Process an incoming frame using DTW
     *
     *  @param self       An instance of a DTWAccelerate object
     *  @param inputFrame The input frame
     *  @param reset      Set true to reset the DTW comparison frame count
     */
    void DTWAccelerate_processFrame(DTWAccelerate *const self,
                                    const Float64 *const inputFrame,
                                    const bool reset);
    
    /**
     *  Find the best matching palette segment
     *
     *  @param self An instance of a DTWAccelerate object
     */
    void DTWAccelerate_getBestMatch(DTWAccelerate *const self);
    
    
#ifdef __cplusplus
}
#endif
//
//  ODF.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 22/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef enum ODFOnsetState
    {
        kODFOnsetState_None = 0, //spectral flux value < filter value
        kODFOnsetState_OnsetBeginning, //spectral flux value > filter value, first frame
        kODFOnsetState_OnsetDuration, //spectral flux value > filter value, evert consecutive frame
        kODFOnsetState_OnsetEnded, //spectral flux value < filter value, first frame after kODFOnsetState_OnsetDuration
        
    } ODFOnsetState;
    
    char *ODFOnsetState_asString(ODFOnsetState);
    
    typedef struct ODF32
    {
        size_t filterSize;
        size_t filterIndex;
        size_t magnitudesSize;
        
        Float32 filterScale;
        Float32 filterValue;
        Float32 spectralFluxValue;
        
        Float32 *previousMagnitudes;
        Float32 *magnitudeDifference;
        Float32 *filterBuffer;
        
        ODFOnsetState onsetState;
        
    } ODF32;
    
    ODF32 *ODF32_new(size_t magnitudesSize,
                     size_t filterSize,
                     Float32 filterScale);
    
    void ODF32_delete(ODF32 *);
    void ODF32_reset(ODF32 *);
    extern inline ODFOnsetState ODF32_process(ODF32 *, Float32 *__restrict inputMagnitudes);


    
#ifdef __cplusplus
}
#endif
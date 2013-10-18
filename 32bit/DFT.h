//
//  DFT.h
//  Audio Concatenator Rewrite
//
//  Created by Edward Costello on 02/06/2012.
//  Copyright (c) 2012 NUI Maynooth. All rights reserved.
//

#import <Accelerate/Accelerate.h>
#import <AudioToolbox/AudioToolbox.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef enum DFT_WINDOW
    {
        DFT_WINDOW_NONE,
        DFT_WINDOW_HANNING,
        DFT_WINDOW_HAMMING,
        
    } DFT_WINDOW;
    
    typedef struct DFT
    {
        DFT_WINDOW windowType;
        UInt32  FFTFrameSize,
        FFTFrameSizeOver2,
        log2FFTFrameSize;
        
        DSPSplitComplex FFTSplitComplex;
        
        FFTSetup FFTWeights;
        FFTSetup FFTWeightsCepstral;
        
        AudioSampleType *window,
        *inReal,
        *outReal,
        *FFTDataReal,
        *FFTDataImag;
        Float32 scale;
        
    } DFT;
    
    
    DFT *DFT_new(UInt32 FFTFrameSize,
                 DFT_WINDOW window);
    
    void DFT_delete(DFT *);
    
    inline void DFT_forward(const DFT *,
                            const AudioSampleType *inputReal,
                            Float32 *outputReal,
                            Float32 *outputImaginary);
    
    inline void DFT_forwardPolar(const DFT *,
                                 const AudioSampleType *inputReal,
                                 Float32 *outputMagnitudes,
                                 Float32 *outputPhases);
    
    inline void DFT_forwardMagnitudes(const DFT *,
                                      const Float32 *inputReal,
                                      Float32 *outputMagnitudes);
    
    inline void DFT_inversePolar(const DFT *,
                                 const Float32 *inputMagnitudes,
                                 const Float32 *inputPhases,
                                 AudioSampleType *outputFrame);
    
    inline void DFT_inverseCepstral(const DFT *,
                                    const Float32 *inputMagnitudes,
                                    Float32 *outputCepstrum);
    
    
        
#ifdef __cplusplus
}
#endif
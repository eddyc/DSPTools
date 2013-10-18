//
//  AudioAnalyser.h
//  ConcatenationRewrite
//
//  Created by Edward Costello on 28/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import "FFT.h"
#import "BeatDetect.h"
#import "MFCC.h"
#import "DTW.h"
#import "Chromagram.h"
#import "AudioAnalysisData.h"
#import "AudioAnalysisQueue.h"
#import "AudioObject.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
        
    typedef struct AudioAnalyser32
    {
        size_t FFTFrameSize;
        size_t FFTFrameSizeOver2;
        size_t hopSize;
        
        FFT32 *fft;
        MFCC32 *mfcc;
        DTW32 *dtw;
        Chromagram32 *chromagram;
        
        Float32 *magnitudeBuffer;
        Float32 *frameBuffer;
        Float32 *mfccBuffer;
        Float32 *chromagramBuffer;
                
        size_t minimumSegmentFrameCount;
        size_t maximumSegmentFrameCount;
                
    } AudioAnalyser32;
    
    AudioAnalyser32 *AudioAnalyser32_new(size_t samplerate,
                                         size_t FFTFrameSize,
                                         size_t hopSize);
    
    void AudioAnalyser32_delete(AudioAnalyser32 *);
    
    void AudioAnalyser32_analyseAudioObject(AudioAnalyser32 *, AudioObject *audioObject, AudioAnalysisData32 *analysisData, size_t maximumSegmentCount);
    void AudioAnalyser32_analyseAudioFrameToQueue(AudioAnalyser32 *, Float32 *audioFrame, AudioAnalysisQueue32 *analysisQueue);

#ifdef __cplusplus
}
#endif
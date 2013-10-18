//
//  DTWOpenCL1DTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 09/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTWOpenCL1D.h"
#import "Matrix.h"
#import "AudioAnalyser.h"

void DTWOpenCL1DTests()
{
    size_t analysisFrameCount = 8;
    AudioObject *audioObject = AudioObject_new();
    AudioObject_openAudioFile(audioObject, "/Users/eddyc/Dropbox/SoftwareProjects/AudioMosaicing/AudioMosaicing/input.wav");
    AudioAnalyser *audioAnalyser = AudioAnalyser_new(16);
    Matrix *paletteData = AudioAnalyser_analyseAudioObject(audioAnalyser, audioObject);
    DTWOpenCL1D *DTWOpenCL1D = DTWOpenCL1D_new(paletteData, analysisFrameCount, analysisFrameCount);
    
    for (size_t i = 0; i < paletteData->rowCount; ++i) {
        
        bool bestMatchReady = DTWOpenCL1D_process(DTWOpenCL1D, Matrix_getRow(paletteData, i));
        
        if (bestMatchReady == true) {
            
            printf("t = %zd\n", DTWOpenCL1D->currentBestMatch);
        }
    }
    
    DTWOpenCL1D_delete(DTWOpenCL1D);
    Matrix_delete(paletteData);
}
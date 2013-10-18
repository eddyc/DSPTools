//
//  DTWTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 08/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <Accelerate/Accelerate.h>
#import "ConvenienceFunctions.h"
#import "DTW1D.h"
#import "DTWOpenCL1D.h"

void DTW1DTests()
{
    size_t analysisFrameCount = 12;
    size_t segmentFrameCount = 4;
    size_t columnCount = 4;
    size_t paletteFrameCount = 8;
    Matrix *analysisFrame = Matrix_new(analysisFrameCount, columnCount);
    Matrix *paletteData = Matrix_new(paletteFrameCount, columnCount);
    Float64 zero = 0;
    Float64 lastValue = analysisFrame->elementCount - 1;
    vDSP_vgenD(&zero, &lastValue, analysisFrame->data, 1, analysisFrame->elementCount);
    
    lastValue = paletteData->elementCount - 1;
    vDSP_vgenD(&zero, &lastValue, paletteData->data, 1, paletteData->elementCount);
    
    Matrix_print(analysisFrame);
    Matrix_print(paletteData);
    
    DTW1D *dtw = DTW1D_new(paletteData, segmentFrameCount, 4);
    DTWOpenCL1D *DTWOpenCL1D = DTWOpenCL1D_new(paletteData, segmentFrameCount, 4);
    
    for (size_t i = 0; i < analysisFrameCount; ++i) {
        
        DTW1D_processFrame(dtw, Matrix_getRow(analysisFrame, i));
        DTWOpenCL1D_process(DTWOpenCL1D, Matrix_getRow(analysisFrame, i));
    }
    
    Matrix_print(dtw->frequencyCostMatrices[0]);
    Matrix_print(dtw->frequencyCostMatrices[1]);

    DTW1D_delete(dtw);
    DTWOpenCL1D_delete(DTWOpenCL1D);
    Matrix_delete(paletteData);
    Matrix_delete(analysisFrame);
}
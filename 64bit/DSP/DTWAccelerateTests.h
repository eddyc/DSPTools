//
//  DTWTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 20/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTWAccelerate.h"
#import "AudioAnalyser.h"
#import "MatrixRingBuffer.h"
#import "ConvenienceFunctions.h"


void DTWAccelerate()
{
    const size_t segmentHopSize = 8;
    const size_t segmentFrameCount = 8;
    
    Float64 triangleFiltersCount = 8;
    const size_t frequencySplitCount = 8;
    const size_t maximumInputBufferFrameCount = 8;
    const size_t analysisFramesCount = 9;
    _Matrix *paletteData = AudioAnalyser_analyseAudioFile(triangleFiltersCount,
                                                          "AudioMosaicing/input.wav");
    _Matrix *analysisData = Matrix_new(paletteData);
    _DTWAccelerate *dtw = DTWAccelerate_new(paletteData,
                        maximumInputBufferFrameCount,
                        segmentFrameCount,
                        segmentHopSize,
                        frequencySplitCount);
    
    for (size_t i = 0; i < analysisFramesCount; ++i) {
        
        Float64 *currentFrame = Matrix_getRow(analysisData, i);
        DTWAccelerate_processFrame(dtw, currentFrame);
        
    }
    
}


void DTWTestsCreateMap()
{
    const size_t segmentHopSize = 7;
    const size_t segmentFrameCount = 6;

    Float64 triangleFiltersCount = 22;
    const size_t frequencySplitCount = 22;
    
    _Matrix *paletteData = AudioAnalyser_analyseAudioFile(triangleFiltersCount, "/Users/eddyc/Dropbox/SoftwareProjects/AudioMosaicing/AudioMosaicing/input.wav");
    
    _DTWAccelerate *dtw = DTWAccelerate_new(paletteData, 10, segmentFrameCount, segmentHopSize, frequencySplitCount);

    Float64_printArray(dtw->segmentMap->frequencySplitSizes, frequencySplitCount, "");
    Float64_printArray(dtw->segmentMap->frequencyStartIndices, frequencySplitCount, "");
    Float64_printArray(dtw->segmentMap->paletteSegmentStartIndices, dtw->segmentMap->paletteSegmentCount, "");

}

void DTWAccelerateTests_getTimeCost(Matrix *input)
{
    Float64 comparisonMatrix[3] = {0};
    for (size_t i = 1; i < input->rowCount; ++i) {
        
        for (size_t j = 1; j < input->columnCount; ++j) {
            
            comparisonMatrix[0] = Matrix_getRow(input, i - 1)[j - 1];
            comparisonMatrix[1] = Matrix_getRow(input, i)[j - 1];
            comparisonMatrix[2] = Matrix_getRow(input, i - 1)[j];
            Float64 min = 0;
            vDSP_minvD(comparisonMatrix, 1, &min, 3);
            
            Matrix_getRow(input, i)[j] += min;
        }
    }
}

void DTWAccelerateTests_testSlicedTimeCost()
{
    size_t rowCount = 4;
    size_t columnCount = 4;
    _Matrix *palette = Matrix_newWithValues(rowCount, columnCount,
                                           rowCount * columnCount,
                                           0., 1., 2., 3.,
                                           1., 2., 2., 1.,
                                           4., 1., 1., 2.,
                                           5., 3., 2., 2.);
    
    _Matrix *inputFrame = Matrix_newWithValues(rowCount, columnCount,
                                              rowCount * columnCount,
                                              3., 0., 1., 2.,
                                              1., 3., 3., 4.,
                                              5., 3., 1., 3.,
                                              0., 4., 2., 4.);
    
    Matrix3DRingBuffer *squaredDifferenceSlices = Matrix3DRingBuffer_new(rowCount, rowCount, columnCount);
    Matrix **timeCostSlices = calloc(columnCount, sizeof(Matrix *));
    _Matrix *normalTimeCost = Matrix_new(rowCount + 1, rowCount + 1);
    Float64 infinity = INFINITY;
    
    vDSP_vfillD(&infinity, normalTimeCost->data, 1, normalTimeCost->columnCount);
    vDSP_vfillD(&infinity, normalTimeCost->data, normalTimeCost->columnCount, normalTimeCost->rowCount);
    normalTimeCost->data[0] = 0;

    for (size_t i = 0; i < columnCount; ++i) {
        
        timeCostSlices[i] = Matrix_new(rowCount + 1, rowCount + 1);
                
        vDSP_vfillD(&infinity, timeCostSlices[i]->data, 1, timeCostSlices[i]->columnCount);
        vDSP_vfillD(&infinity, timeCostSlices[i]->data, timeCostSlices[i]->columnCount, timeCostSlices[i]->rowCount);
        timeCostSlices[i]->data[0] = 0;
        Matrix_print(timeCostSlices[i]);

    }
    
    for (size_t row = 0; row < rowCount; ++row) {
        
        Float64 *currentInputFrame = Matrix_getRow(inputFrame, row);
//        Float64_printArray(currentInputFrame, columnCount, "input");
        Matrix3DRingBuffer_rotate(squaredDifferenceSlices);
        for (size_t column = 0; column < rowCount; ++column) {
            
            Float64 *currentPaletteFrame = Matrix_getRow(palette, column);
//            Float64_printArray(currentPaletteFrame, columnCount, "palette");
            
            Float64 *currentSquaredDifferenceElement = Matrix3DRingBuffer_getCurrentRow(squaredDifferenceSlices)[column];
            
            vDSP_vsubD(currentInputFrame, 1, currentPaletteFrame, 1, currentSquaredDifferenceElement, 1, columnCount);
            vDSP_vsqD(currentSquaredDifferenceElement, 1, currentSquaredDifferenceElement, 1, columnCount);
            vDSP_sveD(currentSquaredDifferenceElement, 1, &Matrix_getRow(normalTimeCost, row + 1)[column + 1], columnCount);
        }
        
        Matrix3DRingBuffer_print(squaredDifferenceSlices);
    }
    
    Matrix_print(normalTimeCost);
    DTWAccelerateTests_getTimeCost(normalTimeCost);
    Matrix_print(normalTimeCost);
    Matrix_clear(normalTimeCost);
    for (size_t i = 0; i < columnCount; ++i) {
        
        Matrix3DRingBuffer_copyLayerToMatrix(squaredDifferenceSlices, i, timeCostSlices[i], 1, 1);
//        Matrix_print(timeCostSlices[i]);
        DTWAccelerateTests_getTimeCost(timeCostSlices[i]);

        //        Matrix_print(timeCostSlices[i]);
        Matrix_add(timeCostSlices[i], normalTimeCost, normalTimeCost);
    }
    Matrix_print(normalTimeCost);

    Matrix_print(palette);
    Matrix_print(inputFrame);
    

    for (size_t i = 0; i < columnCount; ++i) {
        
        Matrix_delete(timeCostSlices[i]);
    }
    
    free(timeCostSlices);
    Matrix3DRingBuffer_delete(squaredDifferenceSlices);
}


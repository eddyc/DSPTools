//
//  DTW1D.c
//  AudioMosaicing
//
//  Created by Edward Costello on 06/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTW1D.h"
#import "ConvenienceFunctions.h"

DTW1D *DTW1D_new(Matrix *const paletteData, const size_t maxTimeFrameCount, const size_t hopSize)
{
    DTW1D *self = calloc(1, sizeof(DTW1D));
    
    self->paletteData = paletteData;
    self->maxTimeFrameCount = maxTimeFrameCount;
    self->hopSize = hopSize;
    
    self->segmentCount = 1 + ((paletteData->rowCount - maxTimeFrameCount) / hopSize);
    self->bestMatchComparisonArray = calloc(self->segmentCount, sizeof(Float64));
    self->timeCostMatrices = calloc(self->segmentCount, sizeof(Matrix *));
    self->frequencyCostMatrices = calloc(self->segmentCount, sizeof(Matrix *));
    self->timeFrameTemp = calloc(paletteData->columnCount, sizeof(Float64));
    self->frequencyFrameTemp = calloc(paletteData->columnCount, sizeof(Float64));
    self->frequencyCostMatrixTemp = Matrix_new(paletteData->columnCount, paletteData->columnCount);
    Float64 infinity = INFINITY;
    
    for (size_t i = 0; i < self->segmentCount; ++i) {
        
        self->timeCostMatrices[i] = Matrix_new(maxTimeFrameCount + 1, maxTimeFrameCount + 1);
        self->frequencyCostMatrices[i] = Matrix_new(paletteData->columnCount + 1, paletteData->columnCount + 1);
        
        vDSP_vfillD(&infinity, Matrix_getRow(self->timeCostMatrices[i], 0), 1, self->timeCostMatrices[i]->columnCount);
        vDSP_vfillD(&infinity, Matrix_getRow(self->timeCostMatrices[i], 0), self->timeCostMatrices[i]->columnCount, self->timeCostMatrices[i]->columnCount);
        self->timeCostMatrices[i]->data[0] = 0;
        
        vDSP_vfillD(&infinity, Matrix_getRow(self->frequencyCostMatrices[i], 0), 1, self->frequencyCostMatrices[i]->columnCount);
        vDSP_vfillD(&infinity, Matrix_getRow(self->frequencyCostMatrices[i], 0), self->frequencyCostMatrices[i]->columnCount, self->frequencyCostMatrices[i]->columnCount);
        self->frequencyCostMatrices[i]->data[0] = 0;
    }
    
    return self;
}

void DTW1D_delete(DTW1D *const self)
{
    for (size_t i = 0; i < self->segmentCount; ++i) {
        
        Matrix_delete(self->timeCostMatrices[i]);
        Matrix_delete(self->frequencyCostMatrices[i]);
    }
    
    free(self->bestMatchComparisonArray);
    free(self->timeFrameTemp);
    free(self->frequencyFrameTemp);
    Matrix_delete(self->frequencyCostMatrixTemp);
    free(self->timeCostMatrices);
    free(self->frequencyCostMatrices);
    free(self);
}

void DTW1D_processFrameForSegment(DTW1D *const self, Float64 *const inputFrame, const size_t segmentNumber)
{
    const Matrix *const timeCostMatrix = self->timeCostMatrices[segmentNumber];
    const Matrix *const frequencyCostMatrix = self->frequencyCostMatrices[segmentNumber];
    size_t paletteRow = segmentNumber * self->hopSize;
    const int columnCount = (int)timeCostMatrix->columnCount;
    
    
    // Calculate local cost for current row of time cost matrix
    
    for (size_t i = 0; i < timeCostMatrix->rowCount - 1; ++i) {

        vDSP_vsubD(Matrix_getRow(self->paletteData, paletteRow + i), 1, inputFrame, 1, self->timeFrameTemp, 1, self->paletteData->columnCount);

        vDSP_svesqD(self->timeFrameTemp, 1, &Matrix_getRow(timeCostMatrix, self->currentFrameNumber + 1)[i + 1], self->paletteData->columnCount);
    }
    
    vvsqrt(&Matrix_getRow(timeCostMatrix, self->currentFrameNumber + 1)[1], &Matrix_getRow(timeCostMatrix, self->currentFrameNumber + 1)[1], &columnCount);
    
    Float64 globalCostComparisonArray[3] = {0};
    
    // Calculate global cost for current row of time cost matrix
    
    for (size_t i = 0; i < timeCostMatrix->rowCount - 1; ++i) {
        
        globalCostComparisonArray[0] = Matrix_getRow(timeCostMatrix, self->currentFrameNumber)[i];
        globalCostComparisonArray[1] = Matrix_getRow(timeCostMatrix, self->currentFrameNumber + 1)[i];
        globalCostComparisonArray[2] = Matrix_getRow(timeCostMatrix, self->currentFrameNumber)[i + 1];
        
        Float64 minimum;
        vDSP_minvD(globalCostComparisonArray, 1, &minimum, 3);
        Matrix_getRow(timeCostMatrix, self->currentFrameNumber + 1)[i + 1] += minimum;
    }
    
    // If frame is 0 clear frequency cost matrix then put back infinity values
    
    if (self->currentFrameNumber == 0) {
        
        Matrix_clear(frequencyCostMatrix);
        Float64 infinity = INFINITY;

        vDSP_vfillD(&infinity, Matrix_getRow(frequencyCostMatrix, 0), 1, frequencyCostMatrix->columnCount);
        vDSP_vfillD(&infinity, Matrix_getRow(frequencyCostMatrix, 0), frequencyCostMatrix->columnCount, frequencyCostMatrix->columnCount);
        frequencyCostMatrix->data[0] = 0;
    }
    
    // Get current frequency local cost matrix values and add them on to the previous accumulated values
    
    for (size_t i = 0; i < frequencyCostMatrix->columnCount - 1; ++i) {
        
        Float64 negatedValue = -inputFrame[i];
        vDSP_vsaddD(Matrix_getRow(self->paletteData, paletteRow + self->currentFrameNumber), 1, &negatedValue, Matrix_getRow(self->frequencyCostMatrixTemp, i), 1, self->paletteData->columnCount);
        vDSP_vsqD(Matrix_getRow(self->frequencyCostMatrixTemp, i), 1, Matrix_getRow(self->frequencyCostMatrixTemp, i), 1, self->paletteData->columnCount);
        vDSP_vaddD(Matrix_getRow(self->frequencyCostMatrixTemp, i), 1, &Matrix_getRow(frequencyCostMatrix, i + 1)[1], 1, &Matrix_getRow(frequencyCostMatrix, i + 1)[1], 1, self->paletteData->columnCount);
    }
    
    // If its the final frame of the analysis, square root the frequency cost matrix, then find global cost values
    
    if (self->currentFrameNumber + 1 == self->maxTimeFrameCount) {
        
        int elementCount = (int)frequencyCostMatrix->elementCount;
        vvsqrt(frequencyCostMatrix->data, frequencyCostMatrix->data, &elementCount);
        
        for (size_t row = 0; row < frequencyCostMatrix->rowCount - 1; ++row) {
            
            for (size_t column = 0; column < frequencyCostMatrix->columnCount - 1; ++column) {
                
                globalCostComparisonArray[0] = Matrix_getRow(frequencyCostMatrix, row)[column];
                globalCostComparisonArray[1] = Matrix_getRow(frequencyCostMatrix, row + 1)[column];
                globalCostComparisonArray[2] = Matrix_getRow(frequencyCostMatrix, row)[column + 1];
                
                Float64 minimum;
                vDSP_minvD(globalCostComparisonArray, 1, &minimum, 3);
                Matrix_getRow(frequencyCostMatrix, row + 1)[column + 1] += minimum;
            }
        }
    }
}

size_t DTW1D_findBestMatch(DTW1D *const self)
{
    size_t bestMatch = 0;
    
    for (size_t i = 0; i < self->segmentCount; ++i) {
        
        self->bestMatchComparisonArray[i] = Matrix_getRow(self->timeCostMatrices[i], self->timeCostMatrices[i]->rowCount - 1)[self->timeCostMatrices[i]->columnCount - 1];
    }
    
    Float64 globalCost;
    vDSP_minviD(self->bestMatchComparisonArray, 1, &globalCost, &bestMatch, self->segmentCount);
    
    for (size_t i = 0; i < self->segmentCount; ++i) {
        
        self->bestMatchComparisonArray[i] = Matrix_getRow(self->frequencyCostMatrices[i], self->frequencyCostMatrices[i]->rowCount - 1)[self->frequencyCostMatrices[i]->columnCount - 1];
    }
    
    size_t bestFreqMatch;
    vDSP_minviD(self->bestMatchComparisonArray, 1, &globalCost, &bestFreqMatch, self->segmentCount);
    
    printf("time = %zd, freq = %zd\n", bestMatch, bestFreqMatch);
    return bestMatch;
}

bool DTW1D_processFrame(DTW1D *const self, Float64 *const inputFrame)
{

    for (size_t i = 0; i < self->segmentCount; ++i) {
        
        DTW1D_processFrameForSegment(self, inputFrame, i);
    }
    
    self->currentFrameNumber++;
    self->currentFrameNumber %= self->maxTimeFrameCount;
    
    if (self->currentFrameNumber == 0) {
        
//        size_t bestMatch = DTW1D_findBestMatch(self);
        return true;
    }
    else {
        
        return false;
    }
}

//
//  DTWAccelerate.c
//  AudioMosaicing
//
//  Created by Edward Costello on 20/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTWAccelerate.h"
#import "ConvenienceFunctions.h"

DTWAccelerate *const DTWAccelerate_new(const Matrix *const paletteData,
                                       size_t maximumInputBufferFrameCount,
                                       const DTWSegmentMap *segmentMap)
{
    DTWAccelerate *const self = calloc(1, sizeof(DTWAccelerate));
    self->segmentMap = segmentMap;
    self->paletteData = paletteData;
    self->bestMatchArray = calloc(paletteData->rowCount, sizeof(size_t));
    self->maximumInputBufferFrameCount = maximumInputBufferFrameCount;

    self->bestMatchMatrix = Matrix_new(paletteData->columnCount,
                                       paletteData->rowCount);
    self->infinityRow = calloc(paletteData->columnCount, sizeof(Float64));
    self->tempLocalCostRow = calloc(paletteData->columnCount, sizeof(Float64));

    Float64 infinity = INFINITY;
    vDSP_vfillD(&infinity, self->infinityRow, 1, paletteData->columnCount);
    self->currentPaletteSegment.columnCount = paletteData->columnCount;
    self->currentTestGlobalCostSegment.columnCount = paletteData->columnCount;

    self->globalCostMatrix = Matrix_new(paletteData->rowCount,
                                        paletteData->columnCount);
    
    self->costRingBuffer = MatrixRingBuffer_new(2, paletteData->columnCount);
    
    return self;
}

void DTWAccelerate_delete(DTWAccelerate *self)
{
    free(self->bestMatchArray);
    free(self->infinityRow);
    free(self->tempLocalCostRow);    
    MatrixRingBuffer_delete(self->costRingBuffer);
    Matrix_delete(self->globalCostMatrix);
  
    free(self);
}

void DTWAccelerate_scopedDelete(DTWAccelerate **self)
{
    DTWAccelerate_delete(*self);
}

void DTWAccelerate_getEuclideanDistance(const Float64 *inputFrameA,
                                        const Float64 *inputFrameB,
                                        Float64 *result,
                                        size_t columnCount)
{
    vDSP_vsubD(inputFrameA, 1, inputFrameB, 1, result, 1, columnCount);
    vDSP_vsqD(result, 1, result, 1, columnCount);
}

void DTWAccelerate_getLocalSegmentCost(DTWAccelerate *const self,
                                       const Float64 *const inputFrame,
                                       Matrix *segment,
                                       Matrix *localCostMatrix)
{
    for (size_t i = 0; i < segment->rowCount; ++i) {
        
        Float64 *currentSegmentFrame = Matrix_getRow(segment, i);
        Float64 *currentLocalCostFrame = Matrix_getRow(localCostMatrix, i);
        
        DTWAccelerate_getEuclideanDistance(inputFrame,
                                           currentSegmentFrame,
                                           currentLocalCostFrame,
                                           segment->columnCount);
    }
}

void DTWAccelerate_calculateLowestCost(Float64 *currentLocal,
                                       Float64 *currentGlobal,
                                       Float64 *currentLocalBack,
                                       Float64 *previousGlobalTopBack,
                                       Float64 *previousGlobalTop,
                                       size_t columnCount)
{
    Float64 comparisonArray[3] = {0};
    for (size_t i = 0; i < columnCount; ++i) {
        
        comparisonArray[0] = previousGlobalTop[i];
        comparisonArray[1] = previousGlobalTopBack[i];
        comparisonArray[2] = currentLocalBack[i];
        Float64 lowest;
        vDSP_minvD(comparisonArray, 1, &lowest, 3);
        currentGlobal[i] = currentLocal[i] + lowest;
    }
}

static void DTWAccelerate_getCostForSegment(DTWAccelerate *const self,
                                            Matrix *currentSegment,
                                            Matrix *testGlobalCostMatrix,
                                            const Float64 *const inputFrame,
                                            const bool reset)
{
    
    
    if (reset == true) {
                
        Float64 *currentSegmentFrame = Matrix_getRow(currentSegment, 0);
        Float64 *currentTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, 0);

        DTWAccelerate_getEuclideanDistance(inputFrame,
                                           currentSegmentFrame,
                                           currentTestGlobalFrame,
                                           currentSegment->columnCount);

        for (size_t i = 1; i < currentSegment->rowCount; ++i) {
            

            currentSegmentFrame = Matrix_getRow(currentSegment, i);
            currentTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, i);
            DTWAccelerate_getEuclideanDistance(inputFrame,
                                               currentSegmentFrame,
                                               currentTestGlobalFrame,
                                               currentSegment->columnCount);
            
            Float64 *previousTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, i - 1);
            vDSP_vaddD(previousTestGlobalFrame, 1, currentTestGlobalFrame, 1, currentTestGlobalFrame, 1, currentSegment->columnCount);
            
        }

        return;
    }
    

    Float64 *currentSegmentFrame = Matrix_getRow(currentSegment, 0);
    Float64 *currentTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, 0);
    DTWAccelerate_getEuclideanDistance(inputFrame,
                                       currentSegmentFrame,
                                       self->tempLocalCostRow,
                                       currentSegment->columnCount);
    cblas_dcopy((UInt32)currentSegment->columnCount, self->infinityRow, 1, self->costRingBuffer->data[0], 1);
    cblas_dcopy((UInt32)currentSegment->columnCount, currentTestGlobalFrame, 1, self->costRingBuffer->data[1], 1);

    DTWAccelerate_calculateLowestCost(self->tempLocalCostRow, currentTestGlobalFrame, self->infinityRow, self->costRingBuffer->data[0], self->costRingBuffer->data[1], currentSegment->columnCount);
    
    for (size_t i = 1; i < currentSegment->rowCount; ++i) {
        
        Float64 *currentSegmentFrame = Matrix_getRow(currentSegment, i);
        Float64 *currentTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, i);
        Float64 *previousTestGlobalFrame = Matrix_getRow(testGlobalCostMatrix, i - 1);

        DTWAccelerate_getEuclideanDistance(inputFrame,
                                           currentSegmentFrame,
                                           self->tempLocalCostRow,
                                           currentSegment->columnCount);
        MatrixRingBuffer_rotate(self->costRingBuffer);
        cblas_dcopy((UInt32)currentSegment->columnCount, currentTestGlobalFrame, 1, self->costRingBuffer->data[1], 1);
        
        DTWAccelerate_calculateLowestCost(self->tempLocalCostRow, currentTestGlobalFrame, previousTestGlobalFrame, self->costRingBuffer->data[0], self->costRingBuffer->data[1], currentSegment->columnCount);
    }
}

static void DTWAccelerate_sumGlobalCostForSegment(DTWAccelerate *const self,
                                                  size_t segmentNumber,
                                                  Matrix *const globalCost)
{
    Float64 *globalCostFrame = Matrix_getRow(globalCost, globalCost->rowCount - 1);

    for (size_t i = 0; i < self->segmentMap->frequencySplitCount; ++i) {
        
        Float64 *bestMatchFrequencyBand = Matrix_getRow(self->bestMatchMatrix, i);

        size_t size = (size_t)self->segmentMap->frequencySplitSizes[i];
        size_t index = (size_t)self->segmentMap->frequencyStartIndices[i];
        vDSP_sveD(&globalCostFrame[index], 1, &bestMatchFrequencyBand[segmentNumber], size);
    }
}

static void DTWAccelerate_getCostForInputFrame(DTWAccelerate *const self,
                                               const Float64 *const inputFrame,
                                               const bool reset)
{
    for (size_t i = 0; i < self->segmentMap->paletteSegmentCount; ++i) {

        self->currentPaletteSegment.rowCount = self->segmentMap->paletteSegmentFrameCount;
        self->currentTestGlobalCostSegment.rowCount = self->segmentMap->paletteSegmentFrameCount;

        size_t currentIndex = (size_t)self->segmentMap->paletteSegmentStartIndices[i];
        self->currentPaletteSegment.data = Matrix_getRow(self->paletteData, currentIndex);
        self->currentTestGlobalCostSegment.data = Matrix_getRow(self->globalCostMatrix, currentIndex);

        
        DTWAccelerate_getCostForSegment(self,
                                        &self->currentPaletteSegment,
                                        &self->currentTestGlobalCostSegment,
                                        inputFrame,
                                        reset);

        
        DTWAccelerate_sumGlobalCostForSegment(self, i, &self->currentTestGlobalCostSegment);
    }
}

void DTWAccelerate_getBestMatch(DTWAccelerate *const self)
{
    size_t index;
    Float64 bestMatch;
    
    for (size_t i = 0; i < self->segmentMap->frequencySplitCount; ++i) {
        
        Float64 *bestMatchFrequencyBand = Matrix_getRow(self->bestMatchMatrix, i);
        vDSP_minviD(bestMatchFrequencyBand, 1, &bestMatch, &index, self->segmentMap->paletteSegmentCount);
        self->bestMatchArray[i] = index;
    }
}

void DTWAccelerate_processFrame(DTWAccelerate *const self,
                                const Float64 *const inputFrame,
                                const bool reset)
{
    DTWAccelerate_getCostForInputFrame(self, inputFrame, reset);    
}


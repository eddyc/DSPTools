
//
//  DTW.c
//  ConcatenationRewrite
//
//  Created by Edward Costello on 25/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTW.h"
#import <stdio.h>
#import <stdlib.h>
#import <Accelerate/Accelerate.h>
#import "ConvenienceFunctions.h"

DTW32 *DTW32_new(size_t rowCount, size_t maximumColumnCount)
{
    DTW32 *self = calloc(1, sizeof(DTW32));
    
    if (rowCount < maximumColumnCount) {
        
        rowCount = maximumColumnCount;
    }
    
    self->maximumRowCount = rowCount;
    self->maximumColumnCount = maximumColumnCount;
    self->maximumElementCount = rowCount * rowCount;
    
    self->ones = calloc(self->maximumElementCount, sizeof(Float32));
    Float32 one = 1;
    vDSP_vfill(&one, self->ones, 1, self->maximumElementCount);
    
    self->distanceMatrix = calloc(self->maximumElementCount, sizeof(Float32));
    
    size_t globalDistanceElementCount = (rowCount + 1) * (rowCount + 1);
    self->globalDistanceMatrix = calloc(globalDistanceElementCount, sizeof(Float32));
    self->multiplicationTemp = calloc(self->maximumElementCount, sizeof(Float32));
    self->inputTemp = calloc(self->maximumElementCount, sizeof(Float32));
    self->comparisonDataTemp = calloc(self->maximumElementCount, sizeof(Float32));
    
    self->normalisationMatrix = calloc(self->maximumElementCount, sizeof(Float32));
    self->phi = calloc(self->maximumElementCount, sizeof(Float32));;
    self->p = calloc(self->maximumElementCount, sizeof(Float32));
    self->q = calloc(self->maximumElementCount, sizeof(Float32));
    return self;
}

void DTW32_delete(DTW32 *self)
{
    free(self->globalDistanceMatrix);
    free(self->ones);
    free(self->distanceMatrix);
    free(self->multiplicationTemp);
    free(self->phi);
    free(self->normalisationMatrix);
    free(self->p);
    free(self->q);
    free(self);
    self = NULL;
}

static inline void DTW32_checkBounds(DTW32 *self,
                                     size_t inputARowCount,
                                     size_t inputBRowCount)
{
    if (inputARowCount > self->maximumRowCount
        ||
        inputBRowCount > self->maximumRowCount) {
        
        printf("DTW32_getSimilarityScore, input row count is larger than allocated comparison matrix, exiting\n");
        exit(-1);
    }
}

Float32 DTW32_getSimilarityScore(DTW32 *self,
                                 Float32 *inputData,
                                 size_t inputRowCount,
                                 Float32 *comparisonData,
                                 size_t comparisonDataRowCount,
                                 size_t currentColumnCount)
{
    
//    Float32_printContiguous2DArray(inputData, inputRowCount, currentColumnCount, "input");
//    Float32_printContiguous2DArray(comparisonData, comparisonDataRowCount, currentColumnCount, "compare");

    Float32 similarity = INFINITY;
    self->currentInputRowCount = inputRowCount;
    self->currentComparisonDataRowCount = comparisonDataRowCount;
    vDSP_vsq(inputData, 1, self->inputTemp, 1, inputRowCount * currentColumnCount);
    vDSP_vsq(comparisonData, 1, self->comparisonDataTemp, 1, comparisonDataRowCount * currentColumnCount);
    

    
    vDSP_mtrans(self->inputTemp, 1, self->multiplicationTemp, 1, currentColumnCount, inputRowCount);
    vDSP_mmul(self->ones, 1, self->multiplicationTemp, 1, self->inputTemp, 1, 1, inputRowCount, currentColumnCount);
    
    vDSP_mtrans(self->comparisonDataTemp, 1, self->multiplicationTemp, 1, currentColumnCount, comparisonDataRowCount);
    vDSP_mmul(self->ones, 1, self->multiplicationTemp, 1, self->comparisonDataTemp, 1, 1, comparisonDataRowCount, currentColumnCount);
    
    const int elementCountInput = (int)inputRowCount;
    const int elementCountComparisonData = (int)comparisonDataRowCount;
    
    vvsqrtf(self->inputTemp, self->inputTemp, &elementCountInput);
    vvsqrtf(self->comparisonDataTemp, self->comparisonDataTemp, &elementCountComparisonData);
 
    cblas_sgemm(CblasRowMajor,
                CblasNoTrans,
                CblasTrans,
                (SInt32)inputRowCount,
                (SInt32)comparisonDataRowCount,
                1,
                1.0,
                self->inputTemp,
                1,
                self->comparisonDataTemp,
                1,
                0.,
                self->normalisationMatrix,
                (SInt32)comparisonDataRowCount);
    
    cblas_sgemm(CblasRowMajor,
                CblasNoTrans,
                CblasTrans,
                (SInt32)inputRowCount,
                (SInt32)comparisonDataRowCount,
                (SInt32)currentColumnCount,
                1.0,
                inputData,
                (SInt32)currentColumnCount,
                comparisonData,
                (SInt32)currentColumnCount,
                0.,
                self->distanceMatrix,
                (SInt32)comparisonDataRowCount);
    
    vDSP_vdiv(self->normalisationMatrix, 1, self->distanceMatrix, 1, self->distanceMatrix, 1, inputRowCount * comparisonDataRowCount);
    
//    Float32_printContiguous2DArray(self->distanceMatrix, inputRowCount, comparisonDataRowCount, "norm");

    
    Float32 minusOne =  -1.f;

    vDSP_vsma(self->distanceMatrix, 1, &minusOne, self->ones, 1, self->distanceMatrix, 1, inputRowCount * comparisonDataRowCount);

//    Float32_printContiguous2DArray(self->distanceMatrix, inputRowCount, comparisonDataRowCount, "norm");

    
    vDSP_mmov(self->distanceMatrix, &self->globalDistanceMatrix[comparisonDataRowCount + 2], comparisonDataRowCount, inputRowCount, comparisonDataRowCount, comparisonDataRowCount + 1);


    Float32 nan = NAN;
    
    vDSP_vfill(&nan, &self->globalDistanceMatrix[1], 1, comparisonDataRowCount);
    vDSP_vfill(&nan, &self->globalDistanceMatrix[comparisonDataRowCount + 1], comparisonDataRowCount + 1, inputRowCount);
    
    Float32 comparisonArray[3] = {0};
    

    
    for (size_t i = 0; i < inputRowCount; ++i) {
        
        for (size_t j = 0; j < comparisonDataRowCount; ++j) {
            
            comparisonArray[0] = self->globalDistanceMatrix[i * (comparisonDataRowCount + 1) + j];
            comparisonArray[1] = self->globalDistanceMatrix[i * (comparisonDataRowCount + 1) + (j + 1)];
            comparisonArray[2] = self->globalDistanceMatrix[(i + 1) * (comparisonDataRowCount + 1) + j];
            
            
            Float32 dmax;
            size_t index;
            vDSP_minvi(comparisonArray, 1, &dmax, &index, 3);
            self->phi[(i * comparisonDataRowCount) + j] = index + 1;
            
            
            self->globalDistanceMatrix[(i + 1) * (comparisonDataRowCount + 1) + (j + 1)] = self->globalDistanceMatrix[(i + 1) * (comparisonDataRowCount + 1) + (j + 1)] + dmax;
            
        }
    }
    

    
    similarity = self->globalDistanceMatrix[(comparisonDataRowCount + 1) * (inputRowCount + 1) - 1];
        
    return similarity;
}


void DTW32_traceWarpPath(DTW32 *self,
                         size_t *warpPath)
{
    SInt32 i = (SInt32)self->currentInputRowCount - 1;
    SInt32 j = (SInt32)self->currentComparisonDataRowCount - 1;
    
    size_t index = 0;
    self->p[index] = i;
    self->q[index] = j;
    index++;
    while (i > 0 && j > 0) {
        
        size_t tb = self->phi[i * self->currentComparisonDataRowCount + j];
        
        switch (tb) {
            case 1:{
                
                i--;
                j--;
                break;
            }
            case 2:{
                
                i--;
                break;
            }
            case 3:{
                
                j--;
                break;
            }
        }
        
        self->p[index] = i;
        self->q[index] = j;
        index++;
    }
    
    vDSP_vrvrs(self->p, 1, index);
    vDSP_vrvrs(self->q, 1, index);
    
    for (size_t i = 0; i < self->currentInputRowCount; ++i) {
        
        for (size_t j = 0; j < index; ++j) {
            
            if (self->p[j] >= i) {
                
                warpPath[i] = (size_t)self->q[j] + 1;
                break;
            }
        }
    }
}


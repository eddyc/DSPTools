//
//  DTWSegmentMap.c
//  AudioMosaicing
//
//  Created by Edward Costello on 28/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "DTWSegmentMap.h"

OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(const Matrix *const paletteData)
{
    DTWSegmentMap *self = DTWSegmentMap_new(paletteData->rowCount, paletteData->columnCount);
    return self;
}

OVERLOADED DTWSegmentMap *const DTWSegmentMap_new(size_t frameCount, size_t maximumFrequencyChannelCount)
{
    DTWSegmentMap *self = calloc(1, sizeof(DTWSegmentMap));
    
    self->paletteFrequencyBinCount = maximumFrequencyChannelCount;
    self->paletteFrameCount = frameCount;

    self->paletteSegmentStartIndices = calloc(frameCount, sizeof(Float64));
    self->frequencySplitSizes = calloc(maximumFrequencyChannelCount + 1, sizeof(Float64));
    self->frequencyStartIndices = calloc(maximumFrequencyChannelCount + 1, sizeof(Float64));
    self->frequencyStartIndicesInt = calloc(maximumFrequencyChannelCount + 1, sizeof(SInt32));
    self->frequencySplitSizesInt = calloc(maximumFrequencyChannelCount + 1, sizeof(SInt32));
    
    return self;
}

void DTWSegmentMap_delete(DTWSegmentMap *self)
{
    free(self->paletteSegmentStartIndices);
    free(self->frequencySplitSizes);
    free(self->frequencyStartIndices);
    free(self->frequencyStartIndicesInt);
    free(self->frequencySplitSizesInt);
    free(self);
}

void DTWSegmentMap_scopedDelete(DTWSegmentMap **self)
{
    DTWSegmentMap_delete(*self);
}


void DTWSegmentMap_calculateSegmentMap(DTWSegmentMap *const self,
                                       const size_t paletteSegmentFrameCount,
                                       const size_t paletteSegmentHopSize,
                                       const size_t frequencySplitCount)
{
    if (paletteSegmentFrameCount > paletteSegmentHopSize
        ||
        paletteSegmentFrameCount > self->paletteFrameCount
        ||
        paletteSegmentHopSize > self->paletteFrameCount
        ||
        frequencySplitCount > self->paletteFrequencyBinCount) {
        
        printf("DTWAccelerate_calculateSegmentMap: error\nexiting.\n");
        exit(-1);
    }
    
    self->paletteSegmentFrameCount = paletteSegmentFrameCount;
    self->paletteSegmentHopSize = paletteSegmentHopSize;
    self->frequencySplitCount = frequencySplitCount;
    
    size_t remainder = self->paletteFrameCount % self->paletteSegmentHopSize;
    self->paletteSegmentCount = (self->paletteFrameCount - remainder) / self->paletteSegmentHopSize;
    Float64 zero = 0.;
    Float64 ramp = self->paletteSegmentHopSize;
    Float64 triangleFilterCount = self->paletteFrequencyBinCount;
    vDSP_vrampD(&zero, &ramp, self->paletteSegmentStartIndices, 1, self->paletteSegmentCount);
    
    vDSP_vgenD(&zero, &triangleFilterCount, self->frequencyStartIndices, 1,  self->frequencySplitCount + 1);
    vDSP_vfixr32D(self->frequencyStartIndices, 1, self->frequencyStartIndicesInt, 1,  self->frequencySplitCount);
    vDSP_vflt32D(self->frequencyStartIndicesInt, 1, self->frequencyStartIndices, 1,  self->frequencySplitCount);
    vDSP_vsubD(&self->frequencyStartIndices[0], 1, &self->frequencyStartIndices[1], 1, self->frequencySplitSizes, 1, self->frequencySplitCount);
    vDSP_vfixr32D(self->frequencySplitSizes, 1, self->frequencySplitSizesInt, 1,  self->frequencySplitCount);
}

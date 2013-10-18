//
//  FormantSynthesiser.c
//  Kalman Filters
//
//  Created by Edward Costello on 17/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "FormantSynthesiser.h"

FormantSynthesiser *FormantSynthesiser_new(size_t samplerate, size_t formantCount)
{
    FormantSynthesiser *self = calloc(1, sizeof(FormantSynthesiser));
    self->formantCount = formantCount;
    self->filters = calloc(self->formantCount, sizeof(BandPassFilter *));

    for (size_t i = 0; i < self->formantCount; ++i) {
        
        self->filters[i] = BandPassFilter_new(samplerate, 0, 0);
    }
    
    return self;
}

void FormantSynthesiser_delete(FormantSynthesiser *self)
{
    for (size_t i = 0; i < self->formantCount; ++i) {
        
        BandPassFilter_delete(self->filters[i]);
    }
    free(self->filters);
    free(self);
    self = NULL;
}
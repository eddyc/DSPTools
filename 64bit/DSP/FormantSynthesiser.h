//
//  FormantSynthesiser.h
//  Kalman Filters
//
//  Created by Edward Costello on 17/10/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"
#import "BandPassFilter.h"
#import "Matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef struct FormantSynthesiser
    {
        size_t formantCount;
        BandPassFilter **filters;
        Matrix *frequencies;
        Matrix *bandwidths;
    } FormantSynthesiser;
    
    FormantSynthesiser *FormantSynthesiser_new(size_t samplerate, size_t formantCount);
    void FormantSynthesiser_delete(FormantSynthesiser *self);
    
#ifdef __cplusplus
}
#endif
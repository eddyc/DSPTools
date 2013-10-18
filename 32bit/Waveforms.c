//
//  Waveforms.h
//
//  Created by Edward Costello on 29/11/2012.
//  Copyright (c) 2012 Edward Costello. All rights reserved.
//

#import "Waveforms.h"
#import <math.h>

void Waveform_Sine32(Float32 frequency,
                     Float32 amplitude,
                     size_t samplerate,
                     size_t sampleCount,
                     Float32 *data)
{
    for (size_t i = 0; i < sampleCount; ++i) {
        
        data[i] = (Float32)(amplitude * sin((2 * M_PI * i * frequency) / samplerate));
    }
}

void Waveform_Sine64(Float64 frequency, Float64 amplitude, size_t samplerate, size_t sampleCount, Float64 *data)
{
    for (size_t i = 0; i < sampleCount; ++i) {
        
        data[i] = (Float64)(amplitude * sin((2 * M_PI * i * frequency) / samplerate));
    }
}

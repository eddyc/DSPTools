//
//  Waveforms.h
//
//  Created by Edward Costello on 29/11/2012.
//  Copyright (c) 2012 Edward Costello. All rights reserved.
//

#import <MacTypes.h>
#import <stdlib.h>
#import <AudioToolbox/AudioToolbox.h>
#import <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    void Waveform_Sine32(Float32 frequency, Float32 amplitude, size_t samplerate, size_t sampleCount, Float32 *data);
    void Waveform_Sine64(Float64 frequency, Float64 amplitude, size_t samplerate, size_t sampleCount, Float64 *data);

#ifdef __cplusplus
}
#endif
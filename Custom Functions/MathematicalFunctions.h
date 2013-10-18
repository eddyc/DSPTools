//
//  MathematicalFunctions.h
//  DSPTools
//
//  Created by Edward Costello on 17/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <MacTypes.h>

#ifdef __cplusplus
extern "C"
{
#endif
    
    size_t nextPowerOfTwo(size_t x);
    size_t log2Ceil(size_t x);
    size_t countLeadingZeroes(size_t arg);
    
    Float32 manhattanDistance(Float32 a, Float32 b);
    
#ifdef __cplusplus
}
#endif
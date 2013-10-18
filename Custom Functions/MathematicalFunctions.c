//
//  MathematicalFunctions.c
//  DSPTools
//
//  Created by Edward Costello on 17/01/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "MathematicalFunctions.h"
#import <math.h>
inline size_t countLeadingZeroes(size_t arg)
{
	if (arg == 0) {
        
        return 32;
    }
    
	return __builtin_clz((UInt32)arg);
    
}
inline size_t log2Ceil(size_t x)
{
	return 32 - countLeadingZeroes(x - 1);
}

inline size_t nextPowerOfTwo(size_t x)
{
	return 1 << log2Ceil(x);
}

Float32 manhattanDistance(Float32 a, Float32 b)
{
    return powf(a - b, 2);
}

//
//  TriangleFilterBankTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 05/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import <Accelerate/Accelerate.h>
#import "ConvenienceFunctions.h"
#import "TriangleFilterBank.h"
#import "Matrix.h"

void TriangleFilterBankTests()
{
    TriangleFilterBank *filterBank = TriangleFilterBank_new(40, 512, 44100);
    Matrix *filters = Matrix_newWithArray(512, 40, filterBank->filterBank);

    Matrix_saveAsHDF(filters, "desktop", "/tri");

    TriangleFilterBank_delete(filterBank);
}
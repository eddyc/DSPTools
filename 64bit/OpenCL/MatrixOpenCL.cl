//
//  MatrixOpenCL.cl
//  AudioMosaicing
//
//  Created by Edward Costello on 09/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#include "MatrixOpenCL.h"

MatrixGlobal MatrixGlobal_new(global double *data,
                              size_t rowCount,
                              size_t columnCount)
{
    MatrixGlobal self = {
        
        .data = data,
        .rowCount = rowCount,
        .columnCount = columnCount,
        .elementCount = rowCount * columnCount,
    };
    
    return self;
}

void MatrixGlobal_print(MatrixGlobal *self)
{
    printf("MatrixGlobal_print:\n\nrows = %d, columns = %d\n\n", self->rowCount, self->columnCount);
    
    for (size_t i = 0; i < self->rowCount; ++i) {
        
        for (size_t j = 0; j < self->columnCount; ++j) {
            
            printf("[%g]\t", Matrix_getRow(self, i)[j]);
        }
        printf("\n");
    }
    
    printf("\n\nEnd\n\n");
}

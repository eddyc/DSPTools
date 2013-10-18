typedef struct MatrixCL
    {
        size_t rowCount;
        size_t columnCount;
        size_t elementCount;
        global float *data;
        
    } MatrixCL;

kernel void MatrixCL_new(global MatrixCL *self,
                         global float *data,
                         size_t rowCount,
                         size_t columnCount)
{
    self->data = data;
    self->rowCount = rowCount;
    self->columnCount = columnCount;
    self->elementCount = rowCount * columnCount;
}

void MatrixCL_print(global MatrixCL *self)
{
    printf("MatrixCL_print:\n\nrows = %d, columns = %d, elements = %d\n\n",
           self->rowCount,
           self->columnCount,
           self->elementCount);
    
    for (size_t i = 0; i < self->rowCount; ++i) {
        
        for (size_t j = 0; j < self->columnCount; ++j) {
            
            printf("[%g]\t",
                   self->data[i * self->columnCount + j]);
        }
        printf("\n");
    }
    
    printf("\n\nEnd\n\n");
}

kernel void MatrixArrayCL_new(global MatrixCL *global *self,
                              global MatrixCL *currentMatrix,
                              size_t currentMatrixNumber)
{
    self[currentMatrixNumber] = currentMatrix;
    float number = 1.f;

    for (size_t i = 0; i < currentMatrixNumber; ++i) {
        
        number += 1.0;
    }
    
    self[currentMatrixNumber]->data[0] = number;
}

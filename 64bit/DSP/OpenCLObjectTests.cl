 #pragma OPENCL EXTENSION cl_khr_fp64 : enable

typedef struct Matrix
{
    size_t rowCount;
    size_t columnCount;
    size_t elementCount;
    global double *data;
} Matrix;

kernel void Matrix_new(global Matrix *self,
                       size_t rowCount,
                       size_t columnCount,
                       global double *data)
{
    self->rowCount = rowCount;
    self->columnCount = columnCount;
    self->elementCount = rowCount * columnCount;
    self->data = data;
    printf("rows = %d, columns = %d\n", self->rowCount, self->columnCount);
}

kernel void Matrix_print(global Matrix *self)
{
    printf("Hurrah\n");
    printf("rows = %d, columns = %d\n", self->rowCount, self->columnCount);
    
    for (size_t i = 0; i < self->rowCount; ++i) {
        
        for (size_t j = 0; j < self->columnCount; ++j) {
            
            printf("[%f]\t", self->data[i * self->columnCount + j]);
        }
    }
}
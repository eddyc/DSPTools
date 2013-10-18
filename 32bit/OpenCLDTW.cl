#include "/Users/eddyc/Dropbox/SoftwareProjects/Tools/C-Data-Structures/Matrix/OpenCLMatrix.cl"

float OpenCLDTW_euclidianDistance(global float *vectorA, global float *vectorB, size_t length)
{
    float sum = 0;
    
    for (size_t i = 0; i < length; ++i) {
        
        float temp = vectorA[i] - vectorB[i];
        
        temp *= temp;
        sum += temp;
    }
    
    return sum;
}


__kernel void OpenCLDTW_noBeats(__global float *analysisData,
                                size_t analysisRowCount,
                                __global float *paletteData,
                                size_t paletteRowCount,
                                size_t columnCount,
                                __global float *resultData)

{
    int globalID = get_global_id(0);
    
    MatrixFloatGlobal analysisMatrix = MatrixFloatGlobal_new(analysisData, analysisRowCount, columnCount);
    MatrixFloatGlobal paletteMatrix = MatrixFloatGlobal_new(&paletteData[globalID * columnCount], paletteRowCount, columnCount);
    
    size_t distanceRowCount = paletteRowCount;
    size_t distanceColumnCount = analysisRowCount;
    size_t globalDistanceRowCount = paletteRowCount + 1;
    size_t globalDistanceColumnCount = analysisRowCount + 1;
    float top, middle, bottom, cheapest;
    
    float distanceData[distanceRowCount * distanceColumnCount];
    MatrixFloatPrivate distanceMatrix = MatrixFloatPrivate_new(distanceData, distanceRowCount, distanceColumnCount);
    
    float globalDistanceData[globalDistanceRowCount * globalDistanceColumnCount];
    MatrixFloatPrivate globalDistanceMatrix = MatrixFloatPrivate_new(globalDistanceData, globalDistanceRowCount, globalDistanceColumnCount);
    
    for (size_t i = 0; i < paletteRowCount; ++i) {
        
        for (size_t j = 0; j < analysisRowCount; ++j) {
            
            Matrix_getElement(distanceMatrix, i, j) = OpenCLDTW_euclidianDistance(Matrix_getRow(analysisMatrix, j), Matrix_getRow(paletteMatrix, i), columnCount);
        }
    }
    
    Matrix_getElement(globalDistanceMatrix, 0, 0) = Matrix_getElement(distanceMatrix, 0, 0);
    Matrix_getElement(globalDistanceMatrix, 0, 1) = INFINITY;
    
    for (size_t i = 2; i < globalDistanceMatrix.columnCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, 0, i) = INFINITY;
    }
    
    for (size_t i = 1; i < globalDistanceMatrix.rowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, i, 0) = INFINITY;
    }
    
    Matrix_getElement(globalDistanceMatrix, 1, 1) = Matrix_getElement(distanceMatrix, 0, 0) + Matrix_getElement(distanceMatrix, 1, 1);
    
    
    for (size_t i = 2; i < globalDistanceMatrix.rowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, i, 1) = INFINITY;
    }
    
    for (size_t i = 2; i < analysisRowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, 1, i) = Matrix_getElement(globalDistanceMatrix, 0, i - 1) + Matrix_getElement(distanceMatrix, 1, i);
        
        for (size_t j = 2; j < paletteRowCount; j++) {
            
            top = Matrix_getElement(globalDistanceMatrix, j - 1, i - 2) +  Matrix_getElement(distanceMatrix, j, i - 1) + Matrix_getElement(distanceMatrix, j, i);
            middle = Matrix_getElement(globalDistanceMatrix, j - 1, i - 1) + Matrix_getElement(distanceMatrix, j, i);
            bottom = Matrix_getElement(globalDistanceMatrix, j - 2, i - 1) + Matrix_getElement(distanceMatrix, j - 1, i) + Matrix_getElement(distanceMatrix, j, i);
            
            if ((top < middle) && (top < bottom)){
                
                cheapest = top;
            }
            else if (middle < bottom){
                
                cheapest = middle;
            }
            else {
                
                cheapest = bottom;
            }
            
            Matrix_getElement(globalDistanceMatrix, j, i) = cheapest;
        }
    }
    
    resultData[globalID] = Matrix_getElement(globalDistanceMatrix, paletteRowCount - 1, analysisRowCount - 1);
}

__kernel void OpenCLDTW_beats(__global float *analysisData,
                              size_t analysisRowCount,
                              __global float *paletteData,
                              __global float *paletteRowCounts,
                              __global float *paletteRowIndexes,
                              size_t columnCount,
                              __global float *resultData)

{
    int globalID = get_global_id(0);
    
    MatrixFloatGlobal analysisMatrix = MatrixFloatGlobal_new(analysisData, analysisRowCount, columnCount);
    MatrixFloatGlobal paletteMatrix = MatrixFloatGlobal_new(&paletteData[(int)paletteRowIndexes[globalID]], (int)paletteRowCounts[globalID], columnCount);
    
    size_t distanceRowCount = paletteRowCounts[globalID];
    size_t distanceColumnCount = analysisRowCount;
    size_t globalDistanceRowCount = paletteRowCounts[globalID] + 1;
    size_t globalDistanceColumnCount = analysisRowCount + 1;
    float top, middle, bottom, cheapest;
    
    float distanceData[distanceRowCount * distanceColumnCount];
    MatrixFloatPrivate distanceMatrix = MatrixFloatPrivate_new(distanceData, distanceRowCount, distanceColumnCount);
    
    float globalDistanceData[globalDistanceRowCount * globalDistanceColumnCount];
    MatrixFloatPrivate globalDistanceMatrix = MatrixFloatPrivate_new(globalDistanceData, globalDistanceRowCount, globalDistanceColumnCount);
    
    for (size_t i = 0; i < (int)paletteRowCounts[globalID]; ++i) {
        
        for (size_t j = 0; j < analysisRowCount; ++j) {
            
            Matrix_getElement(distanceMatrix, i, j) = OpenCLDTW_euclidianDistance(Matrix_getRow(analysisMatrix, j), Matrix_getRow(paletteMatrix, i), columnCount);
        }
    }
    
    Matrix_getElement(globalDistanceMatrix, 0, 0) = Matrix_getElement(distanceMatrix, 0, 0);
    Matrix_getElement(globalDistanceMatrix, 0, 1) = INFINITY;
    
    for (size_t i = 2; i < globalDistanceMatrix.columnCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, 0, i) = INFINITY;
    }
    
    for (size_t i = 1; i < globalDistanceMatrix.rowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, i, 0) = INFINITY;
    }
    
    Matrix_getElement(globalDistanceMatrix, 1, 1) = Matrix_getElement(distanceMatrix, 0, 0) + Matrix_getElement(distanceMatrix, 1, 1);
    
    
    for (size_t i = 2; i < globalDistanceMatrix.rowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, i, 1) = INFINITY;
    }
    
    for (size_t i = 2; i < analysisRowCount; i++) {
        
        Matrix_getElement(globalDistanceMatrix, 1, i) = Matrix_getElement(globalDistanceMatrix, 0, i - 1) + Matrix_getElement(distanceMatrix, 1, i);
        
        for (size_t j = 2; j < (int)paletteRowCounts[globalID]; j++) {
            
            top = Matrix_getElement(globalDistanceMatrix, j - 1, i - 2) +  Matrix_getElement(distanceMatrix, j, i - 1) + Matrix_getElement(distanceMatrix, j, i);
            middle = Matrix_getElement(globalDistanceMatrix, j - 1, i - 1) + Matrix_getElement(distanceMatrix, j, i);
            bottom = Matrix_getElement(globalDistanceMatrix, j - 2, i - 1) + Matrix_getElement(distanceMatrix, j - 1, i) + Matrix_getElement(distanceMatrix, j, i);
            
            if ((top < middle) && (top < bottom)){
                
                cheapest = top;
            }
            else if (middle < bottom){
                
                cheapest = middle;
            }
            else {
                
                cheapest = bottom;
            }
            
            Matrix_getElement(globalDistanceMatrix, j, i) = cheapest;
        }
    }
    
    resultData[globalID] = Matrix_getElement(globalDistanceMatrix, (int)paletteRowCounts[globalID] - 1, analysisRowCount - 1);
}
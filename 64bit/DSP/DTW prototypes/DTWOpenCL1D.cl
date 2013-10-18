
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#include "/Users/eddyc/Dropbox/SoftwareProjects/64bit Tools/OpenCL/MatrixOpenCL.cl"

double DTWOpenCL1D_getVectorDistance(const size_t vectorCount,
                                   global double *inputA,
                                   global double *inputB)
{
    double returnValue = 0;
    
    for (size_t i = 0; i < vectorCount; ++i) {
        
//        returnValue += pow((inputA[i] - inputB[i]), 2);
    }
    
    return returnValue;
}


void DTWOpenCL1D_processTimeCostFrame(MatrixGlobal *paletteSegment,
                                    MatrixGlobal *timeCost,
                                    global double *inputFrame,
                                    size_t currentFrameNumber)
{
    
    for (size_t i = 0; i < paletteSegment->rowCount; ++i) {
        
        double distance = DTWOpenCL1D_getVectorDistance(paletteSegment->columnCount, inputFrame, Matrix_getRow(paletteSegment, i));
//        Matrix_getRow(timeCost, currentFrameNumber + 1)[i + 1] = sqrt(distance);
        
        double lowest;
//        lowest = HUGE_VAL;
        lowest = Matrix_getRow(timeCost, currentFrameNumber)[i] < lowest ? Matrix_getRow(timeCost, currentFrameNumber)[i] : lowest;
        lowest = Matrix_getRow(timeCost, currentFrameNumber + 1)[i] < lowest ? Matrix_getRow(timeCost, currentFrameNumber + 1)[i] : lowest;
        lowest = Matrix_getRow(timeCost, currentFrameNumber)[i + 1] < lowest ? Matrix_getRow(timeCost, currentFrameNumber)[i + 1] : lowest;
        Matrix_getRow(timeCost, currentFrameNumber + 1)[i + 1] += lowest;
    }
}


void DTWOpenCL1D_clearFrequencyCostFrame(MatrixGlobal *frequencyCost)
{
    for (size_t i = 1; i < frequencyCost->rowCount; ++i) {
        
        for (size_t j = 1; j < frequencyCost->columnCount; ++j) {
            
            Matrix_getRow(frequencyCost, i)[j] = 0;
        }
    }
}

void DTWOpenCL1D_processFrequencyCostFrame(MatrixGlobal *paletteSegment,
                                         MatrixGlobal *frequencyCost,
                                         global double *inputFrame,
                                         size_t currentFrameNumber)
{
    if (currentFrameNumber == 0) {
        
        DTWOpenCL1D_clearFrequencyCostFrame(frequencyCost);
    }
    
    for (size_t i = 0; i < paletteSegment->columnCount; ++i) {
                
        for (size_t j = 0; j < paletteSegment->columnCount; ++j) {
            
            double temp = Matrix_getRow(paletteSegment, currentFrameNumber)[j] - inputFrame[i];
            temp *= temp;
            Matrix_getRow(frequencyCost, i + 1)[j + 1] += temp;
        }
    }
    
    if (currentFrameNumber == paletteSegment->rowCount - 1) {
        
        for (size_t i = 0; i < paletteSegment->columnCount; ++i) {
            
            for (size_t j = 0; j < paletteSegment->columnCount; ++j) {
                
//                Matrix_getRow(frequencyCost, i + 1)[j + 1] = sqrt(Matrix_getRow(frequencyCost, i + 1)[j + 1]);
                
                double lowest;
//                lowest = HUGE_VAL;
                lowest = Matrix_getRow(frequencyCost, i)[j] < lowest ? Matrix_getRow(frequencyCost, i)[j] : lowest;
                lowest = Matrix_getRow(frequencyCost, i + 1)[j] < lowest ? Matrix_getRow(frequencyCost, i + 1)[j] : lowest;
                lowest = Matrix_getRow(frequencyCost, i)[j + 1] < lowest ? Matrix_getRow(frequencyCost, i)[j + 1] : lowest;
                Matrix_getRow(frequencyCost, i + 1)[j + 1] += lowest;
            }
        }
    }
}

void DTWOpenCL1D_getCostScores(MatrixGlobal *timeCost,
                             MatrixGlobal *frequencyCost,
                             global double *timeCostScore,
                             global double *frequencyCostScore)
{
    timeCostScore[get_global_id(0)] = Matrix_getRow(timeCost, timeCost->rowCount - 1)[timeCost->columnCount - 1];
    frequencyCostScore[get_global_id(0)] = Matrix_getRow(frequencyCost, frequencyCost->rowCount - 1)[frequencyCost->columnCount - 1];
}

kernel void DTWOpenCL1D_process(size_t totalFrameCount,
                              size_t hopSize,
                              size_t currentFrameNumber,
                              global double *inputFrame,
                              global double *paletteData,
                              size_t paletteRowCount,
                              size_t paletteColumnCount,
                              global double *timeCostData,
                              global double *frequencyCostData,
                              global double *timeCostScore,
                              global double *frequencyCostScore)

{
    size_t timeCostDimension = totalFrameCount + 1;
    size_t frequencyCostDimension = paletteColumnCount + 1;
    
    MatrixGlobal paletteSegment = MatrixGlobal_new(&paletteData[hopSize * paletteColumnCount * get_global_id(0)], totalFrameCount, paletteColumnCount);
    MatrixGlobal timeCost = MatrixGlobal_new(&timeCostData[timeCostDimension * timeCostDimension * get_global_id(0)], timeCostDimension, timeCostDimension);
    MatrixGlobal frequencyCost = MatrixGlobal_new(&frequencyCostData[frequencyCostDimension * frequencyCostDimension * get_global_id(0)], frequencyCostDimension, frequencyCostDimension);

    DTWOpenCL1D_processTimeCostFrame(&paletteSegment, &timeCost, inputFrame, currentFrameNumber);
    DTWOpenCL1D_processFrequencyCostFrame(&paletteSegment, &frequencyCost, inputFrame, currentFrameNumber);
    
    if (currentFrameNumber == totalFrameCount - 1) {
        
        DTWOpenCL1D_getCostScores(&timeCost, &frequencyCost, timeCostScore, frequencyCostScore);
    }
}
    //
    //  DTW.h
    //  ConcatenationRewrite
    //
    //  Created by Edward Costello on 25/01/2013.
    //  Copyright (c) 2013 Edward Costello. All rights reserved.
    //

#import <MacTypes.h>
#import "Matrix.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
    /*!
     @class DTW32
     @abstract Dynamic time warping DSP module
     @var rowCount
     The maximum number of rows in the comparison matrix.
     @var columnCount
     The maximum number of columns in the comparison matrix.
     @var distanceMatrix
     A pointer to the distance matrix.
     @var globalDistanceMatrix
     A pointer to the global distance matrix.
     @var ones
     A ones vector of columnCount in size used for doing matrix column addition by multiplication.
     @var temp
     A pointer to temporary memory used for calculation.
     */
    typedef struct DTW32
    {
        size_t currentInputRowCount;
        size_t currentComparisonDataRowCount;
        size_t maximumRowCount;
        size_t maximumColumnCount;
        size_t maximumElementCount;
        
        Float32 *ones;
        
        Float32 *distanceMatrix;
        Float32 *globalDistanceMatrix;
        Float32 *multiplicationTemp;
        Float32 *inputTemp;
        Float32 *comparisonDataTemp;
        Float32 *normalisationMatrix;
        
        Float32 *phi;
        Float32 *p;
        Float32 *q;
        
    } DTW32;
    /*!
     @functiongroup Construct/Destruct
     */
    /*!
     Construct a DTW32 structure.
     @param rowCount
     Specify the maximum number of rows in the comparison matrix.
     @param columnCount
     Specify the maximum number of columns in the comparison matrix.
     @return
     Returns a DTW32 pseudoclass.
     */
    DTW32 *DTW32_new(size_t rowCount, size_t maximumColumnCount);
    /*!
     Destruct a DTW32 structure.
     @param self
     A pointer to the pseudoclass instance to be destroyed.
     */
    void DTW32_delete(DTW32 *self);
    
    /*!
     @functiongroup Processing
     */
    
    /*!
     Process two input matrices and return a similarity value.
     @param self
     A pointer to a DTW32 pseudoclass instance.
     @param inputA
     A pointer to an input matrix.
     @param inputB
     A pointer to an input matrix.
     @return
     A similarity value for the two input matrices, lower values indicate greater similarity.
     */
    Float32 DTW32_getSimilarityScore(DTW32 *self,
                                     Float32 *inputData,
                                     size_t inputDataRowCount,
                                     Float32 *comparisonData,
                                     size_t comparisonDataRowCount,
                                     size_t currentColumnCount);
    
    
    
    void DTW32_traceWarpPath(DTW32 *self,
                             size_t *warpPath);

    
#ifdef __cplusplus
}
#endif
//
//  MatrixRingBuffer.h
//  AudioMosaicing
//
//  Created by Edward Costello on 21/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class MatrixRingBuffer
     *
     * @abstract A matrix data structure that can rotate its rows
     *
     * @param data The data in the matrix
     * @param rowCount The number of rows in the matrix
     * @param columnCount The number of columns in the matrix
     */
    typedef struct MatrixRingBuffer
    {
        Float64 **data;
        size_t rowCount;
        size_t columnCount;
        
    } MatrixRingBuffer;
    
    /**
     *  Construct a MatrixRingBuffer
     *
     *  @param rowCount The number of rows in the matrix
     *  @param columnCount The number of columns in the matrix
     *
     *  @return A pointer to a new MatrixRingBuffer object
     */
    MatrixRingBuffer *MatrixRingBuffer_new(const size_t rowCount, const size_t columnCount);
    
    /**
     *  Delete a MatrixRingBuffer
     *
     *  @param self A pointer to a MatrixRingBuffer object
     */
    void MatrixRingBuffer_delete(MatrixRingBuffer *self);
    
    /**
     *  Writes a vector to a MatrixRingBuffer row
     *
     *  @param self       A pointer to a MatrixRingBuffer object
     *  @param inputFrame A pointer to the input vector
     */
    void MatrixRingBuffer_write(const MatrixRingBuffer *const self, const Float64 *const inputFrame);
    
    /**
     *  Rotate the rows of the MatrixRingBuffer object
     *
     *  @param self A pointer to a MatrixRingBuffer
     */
    void MatrixRingBuffer_rotate(const MatrixRingBuffer *const self);
    
    /**
     *  Print the contents of a MatrixRingBuffer object
     *
     *  @param self A pointer to a MatrixRingBuffer
     */
    void MatrixRingBuffer_print(const MatrixRingBuffer *const self);
    
    /**
     *  Returns the current row to be written in the MatrixRingBuffer object
     */
#define MatrixRingBuffer_getCurrentRow(self) ((self)->data[self->rowCount - 1])
    
#ifdef __cplusplus
}
#endif
//
//  MatrixCL.h
//  OpenCL
//
//  Created by Edward Costello on 06/09/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import "OpenCLObject.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class MatrixCLStruct
     *
     * @abstract A prototype struct for working with matrices and vectors using OpenCL, this is just a declaration to calculate memory size and is not used directly
     *
     * @param rowCount    The number of rows in the matrix
     * @param columnCount The number of columns in the matrix
     * @param elementCount The number of elements in the matrix, rowCount * columnCount
     * @param data The data store buffer
     */
    typedef struct MatrixCLStruct
    {
        size_t rowCount;
        size_t columnCount;
        size_t elementCount;
        float *data;
        
    } MatrixCLStruct;
    
    /**
     * @class MatrixCL
     *
     * @abstract A struct that contains pointers to OpenCL allocated memory for a MatrixCLStruct
     * @param data The data store buffer of the MatrixCLStruct allocated in OpenCL memory
     * @param structure The memory allocated for the MatrixCLStruct itself in OpenCL memory
     */
    typedef struct MatrixCL
    {
        cl_mem data;
        cl_mem structure;
    } MatrixCL;

    /**
     *  Construct a MatrixCL object
     *
     *  @param opencl      An OpenCLObject instance
     *  @param rowCount    The number of rows in the matrix
     *  @param columnCount The number of columns in the matrix
     *
     *  @return An instance of a MatrixCL struct
     */
    MatrixCL *MatrixCL_new(OpenCLObject *opencl,
                           size_t rowCount,
                           size_t columnCount);
    
    /**
     *  Destroys a MatrixCL object
     *
     *  @param self A pointer to a MatrixCL object
     */
    void MatrixCL_delete(MatrixCL *self);
    
    /**
     *  A scoped destructor function for a MatrixCL object
     *
     *  @param self A pointer to a MatrixCL object
     */
    void MatrixCL_scopedDelete(MatrixCL **self);

    /**
     *  Used to declare a scoped version of a MatrixCL object
     */
#define _MatrixCL __attribute__((unused)) __attribute__((cleanup(MatrixCL_scopedDelete))) MatrixCL

    /**
     * @class MatrixArrayCL
     *
     * @abstract A struct that contains pointers to OpenCL allocated memory for an array of MatrixCLStruct objects
     * @param matrixCount The amount of MatrixCL objects allocated
     * @param data The memory allocated for the MatrixArrayCL itself in OpenCL memory
     */
    typedef struct MatrixArrayCL
    {
        MatrixCL **matrices;
        size_t matrixCount;
        cl_mem data;
    } MatrixArrayCL;
    
    /**
     *  Construct an array of MatrixCL objects
     *
     *  @param opencl      An OpenCLObject instance
     *  @param rowCount    The number of rows in each matrix
     *  @param columnCount The number of columns in each matrix
     *  @param matrixCount The number of matrices to allocate
     *  @return An instance of a MatrixCL struct
     */
    MatrixArrayCL *MatrixArrayCL_new(OpenCLObject *opencl,
                                     size_t rowCount,
                                     size_t columnCount,
                                     size_t matrixCount);
    
    /**
     *  Destroys a MatrixArrayCL object
     *
     *  @param self A pointer to a MatrixArrayCL object
     */
    void MatrixArrayCL_delete(MatrixArrayCL *self);
    
    /**
     *  A scoped destructor function for a MatrixArrayCL object
     *
     *  @param self A pointer to a MatrixArrayCL object
     */
    void MatrixArrayCL_scopedDelete(MatrixArrayCL **self);

    /**
     *  Used to declare a scoped version of a MatrixArrayCL object
     */
#define _MatrixArrayCL __attribute__((unused)) __attribute__((cleanup(MatrixArrayCL_scopedDelete))) MatrixArrayCL

#ifdef __cplusplus
}
#endif
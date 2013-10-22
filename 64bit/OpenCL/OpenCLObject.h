//
//  OpenCLObject.h
//  AudioMosaicing
//
//  Created by Edward Costello on 08/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <OpenCL/opencl.h>
#import "DSPTools.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     *  Specify which OpenCL device to use for processing, CPU or GPU
     *  @param kOpenCLObjectDevice_CPU Select CPU
     *  @param kOpenCLObjectDevice_GPU Select GPU
     */
    typedef enum OpenCLObjectDevice
    {
        kOpenCLObjectDevice_CPU,
        kOpenCLObjectDevice_GPU
    } OpenCLObjectDevice;
    
    /**
     * @class OpenCLObject
     *
     * @abstract An object for simplifying using the Apples OpenCL framework
     * @param program Handle for the open cl program code
     * @param commandQueue Handle for the open cl command queue
     * @param context Handle for the open cl context
     * @param device Handle for the open cl device used for execution
     */
    typedef struct OpenCLObject
    {
        cl_program program;
        cl_command_queue commandQueue;
        cl_context context;
        cl_device_id device;
    } OpenCLObject;
    
    /**
     *  Construct a new OpenCLObject
     *
     *  @param device          The specified execution device
     *  @param programFileName The path of the open cl program code
     *
     *  @return An OpenCLObject instance
     */
    OpenCLObject *const OpenCLObject_new(const OpenCLObjectDevice device,
                                         const char *programFileName);
    
    /**
     *  Destroy an OpenCLObject instance
     *
     *  @param self A pointer to an OpenCLObject instance
     */
    void OpenCLObject_delete(OpenCLObject *self);
    
    /**
     *  Scoped destuction of an OpenCLObject instance
     *
     *  @param self A pointer to an OpenCLObject instance
     */
    void OpenCLObject_scopedDelete(OpenCLObject **self);
    
    /**
     *  Definition used to construct a scoped OpenCLObject pseudoclass
     */
#define _OpenCLObject __attribute__((unused)) __attribute__((cleanup(OpenCLObject_scopedDelete))) OpenCLObject
    
    /**
     *  Allocate a buffer of 64bit floats in open cl memory
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param size The number of floats to allocate
     *  @param flag The type of memory allocation
     *
     *  @return A handle to the open cl memory
     */
    cl_mem OpenCLObject_allocateFloat64Buffer(const OpenCLObject *const self,
                                              const size_t size,
                                              const cl_mem_flags flag);
    
    /**
     *  Deallocate a buffer of 64bit floats in open cl memory
     *
     *  @param buffer A handle to the open cl memory
     */
    void OpenCLObject_deallocateFloat64Buffer(const cl_mem buffer);
    
    /**
     *  Allocate a buffer of open cl memory
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param bufferSizeInBytes The size of the buffer in bytes
     *  @param flag The type of memory allocation
     *
     *  @return A handle to the open cl memory
     */
    cl_mem OpenCLObject_allocateBuffer(const OpenCLObject *const self,
                                       const size_t bufferSizeInBytes,
                                       const cl_mem_flags flag);
    
    
    /**
     *  Write 64bit float data to an open cl memory buffer
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param clBuffer A handle to the open cl memory buffer
     *  @param data     The 64bit float data to write
     *  @param size     The number of 64bit float values to write
     */
    void OpenCLObject_writeFloat64Buffer(const OpenCLObject *self,
                                         const cl_mem clBuffer,
                                         const Float64 *const data,
                                         const size_t size);
    
    /**
     *  Write data to an open cl memory buffer
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param clBuffer A handle to the open cl memory buffer
     *  @param data     The data to write
     *  @param bufferSizeInBytes The size of the buffer in bytes
     */
    void OpenCLObject_writeBuffer(const OpenCLObject *const self,
                                  const cl_mem clBuffer,
                                  const void *const data,
                                  const size_t sizeInBytes);
    
    /**
     *  Read data from a 64bit float buffer in open cl memory
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param clBuffer A handle to the open cl memory buffer
     *  @param data     The data to write
     *  @param size     The number of 64bit float values to read
     *  @param offset   The offest into the 64bit float buffer to start reading from
     */
    void OpenCLObject_readFloat64Buffer(const OpenCLObject *const self,
                                        const cl_mem clBuffer,
                                        Float64 *const data,
                                        const size_t size,
                                        const size_t offset);
    
    /**
     *  Read data from a buffer in open cl memory
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param clBuffer A handle to the open cl memory buffer
     *  @param data     The data to write
     *  @param size     The size of the data to read in bytes
     *  @param offset   The offset into the buffer to start reading from in bytes
     */
    void OpenCLObject_readBuffer(const OpenCLObject *const self,
                                 const cl_mem clBuffer,
                                 void *const data,
                                 const size_t size,
                                 const size_t offset);
    
    /**
     *  Load the open cl program source from a source code file
     *
     *  @param fileName The path to the file
     *
     *  @return A string containing the contents of the source code file
     */
    char *OpenCLObject_loadProgramSource(const char *fileName);
    
    /**
     *  Print the open cl device stats for the chosen compute device
     *
     *  @param device_id The id of the chosen compute device
     *
     */
    void OpenCLObject_deviceStats(const cl_device_id device_id);
    
    /**
     *  Execute an open cl kernel
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param kernel         Handle to the kernel to execute
     *  @param globalWorkSize The amount of threads this kernel is to execute
     */
    void OpenCLObject_executeKernel(const OpenCLObject *const self,
                                    cl_kernel kernel,
                                    size_t globalWorkSize);
    
    /**
     *  Create an open cl kernel
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param kernelName The function name used in the open cl source code
     *
     *  @return Handle to open cl kernel
     */
    cl_kernel OpenCLObject_createKernel(OpenCLObject *const self,
                                        const char *kernelName);
    
    /**
     *  Set one of the arguments in an open cl kernel
     *
     *  @param self A pointer to an OpenCLObject instance
     *  @param kernel         Handle to the kernel
     *  @param argumentNumber  The number of the argument to set
     *  @param sizeInBytes     The size of the argument data in bytes
     *  @param argumentPointer A pointer to the data to set as the argument
     */
    void OpenCLObject_setKernelArgument(const OpenCLObject *const self,
                                        const cl_kernel kernel,
                                        const UInt32 argumentNumber,
                                        size_t sizeInBytes,
                                        const void *const argumentPointer);
    
#ifdef __cplusplus
}
#endif
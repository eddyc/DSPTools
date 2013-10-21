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
    
    typedef enum OpenCLObjectDevice
    {
        kOpenCLObjectDevice_CPU,
        kOpenCLObjectDevice_GPU
    } OpenCLObjectDevice;
    
    typedef struct OpenCLObject
    {
        cl_program program;
        cl_command_queue commandQueue;
        cl_context context;
        cl_device_id device;
    } OpenCLObject;
    
    OpenCLObject *const OpenCLObject_new(const OpenCLObjectDevice device,
                                         const char *programFileName);
    
    void OpenCLObject_delete(OpenCLObject *self);
    void OpenCLObject_scopedDelete(OpenCLObject **self);
    
#define _OpenCLObject __attribute__((unused)) __attribute__((cleanup(OpenCLObject_scopedDelete))) OpenCLObject
    
    
    cl_mem OpenCLObject_allocateFloat64Buffer(const OpenCLObject *const self,
                                              const size_t size,
                                              const cl_mem_flags flag);
    
    void OpenCLObject_deallocateFloat64Buffer(const cl_mem buffer);
    
    cl_mem OpenCLObject_allocateBuffer(const OpenCLObject *const self,
                                       const size_t bufferSizeInBytes,
                                       const cl_mem_flags flag);
    void OpenCLObject_writeFloat64Buffer(const OpenCLObject *self,
                                         const cl_mem clBuffer,
                                         const Float64 *const data,
                                         const size_t size);
    
    void OpenCLObject_writeBuffer(const OpenCLObject *const self,
                                  const cl_mem clBuffer,
                                  const void *const data,
                                  const size_t sizeInBytes);
    
    void OpenCLObject_readFloat64Buffer(const OpenCLObject *const self,
                                        const cl_mem clBuffer,
                                        Float64 *const data,
                                        const size_t size,
                                        const size_t offset);
    
    void OpenCLObject_readBuffer(const OpenCLObject *const self,
                                 const cl_mem clBuffer,
                                 void *const data,
                                 const size_t size,
                                 const size_t offset);
    
    char *OpenCLObject_loadProgramSource(const char *fileName);
    
    int OpenCLObject_deviceStats(const cl_device_id device_id);
    
    void OpenCLObject_executeKernel(const OpenCLObject *const self,                                 cl_kernel kernel, size_t globalWorkSize);
    
    
    cl_kernel OpenCLObject_createKernel(OpenCLObject *const self,
                                        const char *kernelName);
    
    void OpenCLObject_setKernelArgument(const OpenCLObject *const self,
                                        const cl_kernel kernel,
                                        const UInt32 argumentNumber,
                                        size_t sizeInBytes,
                                        const void *const argumentPointer);
    
#ifdef __cplusplus
}
#endif
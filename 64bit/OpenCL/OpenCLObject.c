//
//  OpenCLObject.c
//  AudioMosaicing
//
//  Created by Edward Costello on 08/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "OpenCLObject.h"
#import <assert.h>
#import <sys/stat.h>
#import <stdio.h>
#import <string.h>

OpenCLObject *const OpenCLObject_new(const OpenCLObjectDevice device,
                                     const char *programFilePath)
{
    OpenCLObject *const self = calloc(1, sizeof(OpenCLObject));

    cl_int error;
    
    if (device == kOpenCLObjectDevice_CPU) {
        
        error = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1, &self->device, NULL);
    }
    else {
        
        error = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &self->device, NULL);
    }
    assert(error == CL_SUCCESS);
    assert(self->device);
    
    self->context = clCreateContext(0, 1, &self->device, NULL, NULL, &error);
    assert(error == CL_SUCCESS);
    
    self->commandQueue = clCreateCommandQueue(self->context, self->device, 0, NULL);
    
    char *programSource = OpenCLObject_loadProgramSource(programFilePath);
    self->program = clCreateProgramWithSource(self->context,
                                              1,
                                              (const char **)&programSource,
                                              NULL,
                                              &error);
    
    assert(error == CL_SUCCESS);
    
    error = clBuildProgram(self->program, 0, NULL,  NULL, NULL, NULL);
    
    if (error == CL_BUILD_PROGRAM_FAILURE) {
        
        size_t logSize;
        clGetProgramBuildInfo(self->program, self->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
        char *log = (char *) malloc(logSize);
        clGetProgramBuildInfo(self->program, self->device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);
        printf("%s\n", log);
    }
    
    return self;
}

void OpenCLObject_delete(OpenCLObject *self)
{
    clReleaseProgram(self->program);
    clReleaseContext(self->context);
    clReleaseCommandQueue(self->commandQueue);
    clReleaseDevice(self->device);
    free(self);
    self = NULL;
}

void OpenCLObject_scopedDelete(OpenCLObject **self)
{
    OpenCLObject_delete(*self);
}

char *OpenCLObject_loadProgramSource(const char *fileName)
{
    struct stat statbuf;
	FILE *fh;
	char *source;
	
	fh = fopen(fileName, "r");
	if (fh == 0)
		return 0;
	
	stat(fileName, &statbuf);
	source = (char *) malloc(statbuf.st_size + 1);
	fread(source, statbuf.st_size, 1, fh);
	source[statbuf.st_size] = '\0';
	
	return source;
}

cl_kernel OpenCLObject_createKernel(OpenCLObject *const self,
                               const char *kernelName)
{
    cl_int error;
    cl_kernel kernel = clCreateKernel(self->program, kernelName, &error);
    assert(error == CL_SUCCESS);
    return kernel;
    
}

void OpenCLObject_executeKernel(const OpenCLObject *const self,
                                cl_kernel kernel,
                                size_t globalWorkSize)
{
    cl_int error = clEnqueueNDRangeKernel(self->commandQueue,
                                          kernel,
                                          1,
                                          NULL,
                                          &globalWorkSize,
                                          NULL,
                                          0,
                                          NULL,
                                          NULL);
    assert(error == CL_SUCCESS);
    clFinish(self->commandQueue);
}

void OpenCLObject_setKernelArgument(const OpenCLObject *const self,
                                    const cl_kernel kernel,
                                    const UInt32 argumentNumber,
                                    size_t sizeInBytes,
                                    const void *const argumentPointer)
{
    cl_int error;
    error = clSetKernelArg(kernel, argumentNumber, sizeInBytes, argumentPointer);
    assert(error == CL_SUCCESS);
}

cl_mem OpenCLObject_allocateBuffer(const OpenCLObject *const self,
                                   const size_t bufferSizeInBytes,
                                   const cl_mem_flags flag)
{
    cl_int error;
    cl_mem bufferHandle = clCreateBuffer(self->context,
                                         flag,
                                         bufferSizeInBytes,
                                         NULL,
                                         &error);
    assert(error == CL_SUCCESS);
    
    return bufferHandle;
}

cl_mem OpenCLObject_allocateFloat64Buffer(const OpenCLObject *const self,
                                          const size_t size,
                                          const cl_mem_flags flag)
{
    cl_int error;
    size_t bufferSize = sizeof(Float64) * size;
    cl_mem bufferHandle = clCreateBuffer(self->context,
                                         flag,
                                         bufferSize,
                                         NULL,
                                         &error);
    assert(error == CL_SUCCESS);
    
    return bufferHandle;
}

void OpenCLObject_deallocateFloat64Buffer(const cl_mem buffer)
{
    clReleaseMemObject(buffer);
}


void OpenCLObject_writeBuffer(const OpenCLObject *const self,
                                     const cl_mem clBuffer,
                                     const void *const data,
                                     const size_t sizeInBytes)
{
    
    cl_int error = clEnqueueWriteBuffer(self->commandQueue,
                                        clBuffer,
                                        CL_TRUE,
                                        0,
                                        sizeInBytes,
                                        (void *)data,
                                        0,
                                        NULL,
                                        NULL);
    
    clFinish(self->commandQueue);
    
    assert(error == CL_SUCCESS);
}

void OpenCLObject_writeFloat64Buffer(const OpenCLObject *const self,
                                     const cl_mem clBuffer,
                                     const Float64 *const data,
                                     const size_t size)
{
    size_t bufferSize = sizeof(Float64) * size;
    
    cl_int error = clEnqueueWriteBuffer(self->commandQueue,
                                        clBuffer,
                                        CL_TRUE,
                                        0,
                                        bufferSize,
                                        (void *)data,
                                        0,
                                        NULL,
                                        NULL);
    
    clFinish(self->commandQueue);
    
    assert(error == CL_SUCCESS);
}

void OpenCLObject_readFloat64Buffer(const OpenCLObject *const self,
                                    const cl_mem clBuffer,
                                    Float64 *const data,
                                    const size_t size,
                                    const size_t offset)
{
    cl_int error;
    error = clEnqueueReadBuffer(self->commandQueue,
                                clBuffer,
                                CL_TRUE,
                                sizeof(Float64) * offset,
                                sizeof(Float64) * size,
                                data,
                                0,
                                NULL,
                                NULL);
    assert(error == CL_SUCCESS);
    clFinish(self->commandQueue);
    
}

void OpenCLObject_readBuffer(const OpenCLObject *const self,
                             const cl_mem clBuffer,
                             void *const data,
                             const size_t sizeInBytes,
                             const size_t offsetInBytes)
{
    cl_int error;
    error = clEnqueueReadBuffer(self->commandQueue,
                                clBuffer,
                                CL_TRUE,
                                offsetInBytes,
                                sizeInBytes,
                                data,
                                0,
                                NULL,
                                NULL);
    assert(error == CL_SUCCESS);
    clFinish(self->commandQueue);
}


void OpenCLObject_deviceStats(const cl_device_id device_id)
{
    int err;
	size_t returned_size;
    
    cl_char vendor_name[1024] = {0};
    cl_char device_name[1024] = {0};
	cl_char device_profile[1024] = {0};
	cl_char device_extensions[1024] = {0};
	cl_device_local_mem_type local_mem_type;
	
    cl_ulong global_mem_size, global_mem_cache_size;
	cl_ulong max_mem_alloc_size;
	
	cl_uint clock_frequency, vector_width, max_compute_units;
	
	size_t max_work_item_dims,max_work_group_size, max_work_item_sizes[3];
	
	cl_uint vector_types[] = {CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE};
	char *vector_type_names[] = {"char","short","int","long","float","double"};
	
	err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, &returned_size);
    err|= clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name, &returned_size);
	err|= clGetDeviceInfo(device_id, CL_DEVICE_PROFILE, sizeof(device_profile), device_profile, &returned_size);
	err|= clGetDeviceInfo(device_id, CL_DEVICE_EXTENSIONS, sizeof(device_extensions), device_extensions, &returned_size);
	err|= clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, &returned_size);
	err|= clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(global_mem_cache_size), &global_mem_cache_size, &returned_size);
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dims), &max_work_item_dims, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(max_work_item_sizes), max_work_item_sizes, &returned_size);
	
	err|= clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, &returned_size);
	
	printf("Vendor: %s\n", vendor_name);
	printf("Device Name: %s\n", device_name);
	printf("Profile: %s\n", device_profile);
	printf("Supported Extensions: %s\n\n", device_extensions);
	printf("Local Mem Type (Local=1, Global=2): %i\n",(int)local_mem_type);
	printf("Global Mem Size (MB): %i\n",(int)global_mem_size/(1024*1024));
	printf("Global Mem Cache Size (Bytes): %i\n",(int)global_mem_cache_size);
	printf("Max Mem Alloc Size (MB): %ld\n",(long int)max_mem_alloc_size/(1024*1024));
	
	printf("Clock Frequency (MHz): %i\n\n",clock_frequency);
	
	for (int i = 0; i < 6; i++) {
        
		err|= clGetDeviceInfo(device_id, vector_types[i], sizeof(clock_frequency), &vector_width, &returned_size);
		printf("Vector type width for: %s = %i\n",vector_type_names[i],vector_width);
	}
	
	printf("\nMax Work Group Size: %lu\n",max_work_group_size);
    //printf("Max Work Item Dims: %lu\n",max_work_item_dims);
    //for(size_t i=0;i<max_work_item_dims;i++)
    //	printf("Max Work Items in Dim %lu: %lu\n",(long unsigned)(i+1),(long unsigned)max_work_item_sizes[i]);
	
	printf("Max Compute Units: %i\n",max_compute_units);
	printf("\n");
}


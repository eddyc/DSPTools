    //
    //  OpenCLDTW.c
    //  Audio-Mosaicing
    //
    //  Created by Edward Costello on 05/04/2013.
    //  Copyright (c) 2013 Edward Costello. All rights reserved.
    //

#import "OpenCLDTW.h"
#import <assert.h>
#import <sys/stat.h>



OpenCLDTW *OpenCLDTW_new(OpenCLDTW_Device device,
                         size_t maximumRowCount,
                         size_t maximumColumnCount,
                         Float32 *paletteData,
                         size_t paletteRowCount,
                         Matrix32 *beats,
                         Boolean useBeats)
{
    OpenCLDTW *self = calloc(1, sizeof(OpenCLDTW));
    
    self->maximumRowCount = maximumRowCount;
    self->maximumColumnCount = maximumColumnCount;
    self->maximumElementCount = self->maximumRowCount * self->maximumColumnCount;
    self->paletteRowCount = paletteRowCount;
    
    if (useBeats == false) {
        
        self->globalWorkSize  = paletteRowCount / maximumRowCount;

    }
    else {
        
        self->globalWorkSize = beats->columnCount;
    }
    
    self->paletteData = paletteData;
    self->useBeats = useBeats;
    self->paletteBeatIndexes = Matrix_getRow(beats, 0);
    self->paletteBeatCounts = Matrix_getRow(beats, 1);
    self->beatsCount = beats->columnCount;
    cl_int error;
    
    if (device == OpenCLDTW_useCPU) {
        
        error = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1, &self->device, NULL);
        assert(error == CL_SUCCESS);
    }
    else {
        
        error = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &self->device, NULL);
        assert(error == CL_SUCCESS);
    }
    
    assert(self->device);
    
    self->context = clCreateContext(0, 1, &self->device, NULL, NULL, &error);
    assert(error == CL_SUCCESS);
    
    self->commandQueue = clCreateCommandQueue(self->context, self->device, 0, NULL);
    
    const char *programFileName = "/Users/eddyc/Dropbox/SoftwareProjects/Tools/DSP/OpenCLDTW.cl";
    char *programSource = OpenCLDTW_loadProgramSource(programFileName);
    
    self->program = clCreateProgramWithSource(self->context,
                                              1,
                                              (const char **)&programSource,
                                              NULL,
                                              &error);
    assert(error == CL_SUCCESS);
    
    error = clBuildProgram(self->program, 0, NULL,  NULL, NULL, NULL);
    
    if (error == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(self->program, self->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        
            // Allocate memory for the log
        char *log = (char *) malloc(log_size);
        
            // Get the log
        clGetProgramBuildInfo(self->program, self->device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        
            // Print the log
        printf("%s\n", log);
    }
    assert(error == CL_SUCCESS);
    
    self->analysisMemory = OpenCLDTW_allocateFloatBuffer(self,
                                                         self->maximumElementCount,
                                                         CL_MEM_READ_ONLY);
    
    
    self->paletteMemory = OpenCLDTW_allocateFloatBuffer(self,
                                                        self->paletteRowCount * self->maximumColumnCount,
                                                        CL_MEM_READ_ONLY);
    
    OpenCLDTW_writeFloatBuffer(self, self->paletteMemory, self->paletteData, self->paletteRowCount * self->maximumColumnCount);
    
    self->resultMemory = OpenCLDTW_allocateFloatBuffer(self,
                                                       self->globalWorkSize,
                                                       CL_MEM_WRITE_ONLY);
    
    if (useBeats == false) {
        
        OpenCLDTW_configureNoBeats(self);
    }
    else if (useBeats == true) {
        
        OpenCLDTW_configureBeats(self);
    }
    
    return self;
}

void OpenCLDTW_delete(OpenCLDTW *self)
{
    clReleaseMemObject(self->paletteMemory);
    clReleaseMemObject(self->analysisMemory);
    clReleaseMemObject(self->resultMemory);
    
    if (self->useBeats == true) {
        
        clReleaseMemObject(self->paletteRowIndexesMemory);
        clReleaseMemObject(self->paletteRowCountsMemory);
    }
    
    free(self);
    self = NULL;
}

void OpenCLDTW_configureNoBeats(OpenCLDTW *self)
{
    cl_int error;
    self->kernel = clCreateKernel(self->program, "OpenCLDTW_noBeats", &error);
    assert(error == CL_SUCCESS);
    
    error   = clSetKernelArg(self->kernel,  0, sizeof(cl_mem), &self->analysisMemory);
    error  |= clSetKernelArg(self->kernel,  1, sizeof(size_t), &self->maximumRowCount);
    error  |= clSetKernelArg(self->kernel,  2, sizeof(cl_mem), &self->paletteMemory);
    error  |= clSetKernelArg(self->kernel,  3, sizeof(size_t), &self->maximumRowCount);
    error  |= clSetKernelArg(self->kernel,  4, sizeof(size_t), &self->maximumColumnCount);
    error  |= clSetKernelArg(self->kernel,  5, sizeof(cl_mem), &self->resultMemory);
    
    
    assert(error == CL_SUCCESS);
}

void OpenCLDTW_configureBeats(OpenCLDTW *self)
{
    cl_int error;
    self->kernel = clCreateKernel(self->program, "OpenCLDTW_beats", &error);
    assert(error == CL_SUCCESS);
    
    self->paletteRowIndexesMemory = OpenCLDTW_allocateFloatBuffer(self,
                                                                  self->beatsCount,
                                                                  CL_MEM_READ_ONLY);
    
    OpenCLDTW_writeFloatBuffer(self,
                               self->paletteRowIndexesMemory,
                               self->paletteBeatIndexes,
                               self->beatsCount);
    
    self->paletteRowCountsMemory = OpenCLDTW_allocateFloatBuffer(self,
                                                                 self->beatsCount,
                                                                 CL_MEM_READ_ONLY);
    OpenCLDTW_writeFloatBuffer(self,
                               self->paletteRowCountsMemory,
                               self->paletteBeatCounts,
                               self->beatsCount);
    
    error   = clSetKernelArg(self->kernel,  0, sizeof(cl_mem), &self->analysisMemory);
    error  |= clSetKernelArg(self->kernel,  1, sizeof(size_t), &self->maximumRowCount);
    error  |= clSetKernelArg(self->kernel,  2, sizeof(cl_mem), &self->paletteMemory);
    error  |= clSetKernelArg(self->kernel,  3, sizeof(cl_mem), &self->paletteRowCountsMemory);
    error  |= clSetKernelArg(self->kernel,  4, sizeof(cl_mem), &self->paletteRowIndexesMemory);
    error  |= clSetKernelArg(self->kernel,  5, sizeof(size_t), &self->maximumColumnCount);
    error  |= clSetKernelArg(self->kernel,  6, sizeof(cl_mem), &self->resultMemory);
    
    
    assert(error == CL_SUCCESS);
}


cl_mem OpenCLDTW_allocateFloatBuffer(OpenCLDTW *self, size_t size, cl_mem_flags flag)
{
    cl_int error;
    size_t bufferSize = sizeof(Float32) * size;
    cl_mem bufferHandle = clCreateBuffer(self->context,
                                         flag,
                                         bufferSize,
                                         NULL,
                                         &error);
    assert(error == CL_SUCCESS);
    
    return bufferHandle;
}

void OpenCLDTW_writeFloatBuffer(OpenCLDTW *self, cl_mem clBuffer, Float32 *data, size_t size)
{
    size_t bufferSize = sizeof(Float32) * size;
    
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

void OpenCLDTW_process(OpenCLDTW *self, Float32 *analysisData, Float32 *result)
{
    OpenCLDTW_writeFloatBuffer(self, self->analysisMemory, analysisData, self->maximumElementCount);
    
    
    cl_int error = clEnqueueNDRangeKernel(self->commandQueue,
                                          self->kernel,
                                          1,
                                          NULL,
                                          &self->globalWorkSize,
                                          NULL,
                                          0,
                                          NULL,
                                          NULL);
    assert(error == CL_SUCCESS);
    clFinish(self->commandQueue);
    
    error = clEnqueueReadBuffer(self->commandQueue,
                                self->resultMemory,
                                CL_TRUE,
                                0,
                                sizeof(Float32) * self->globalWorkSize,
                                result,
                                0,
                                NULL,
                                NULL);
    assert(error == CL_SUCCESS);
    clFinish(self->commandQueue);
}


char *OpenCLDTW_loadProgramSource(const char *fileName)
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


int OpenCLDTW_deviceStats(cl_device_id device_id)
{
    int err;
	size_t returned_size;
	
        // Report the device vendor and device name
        //
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
	
	for(int i=0;i<6;i++){
		err|= clGetDeviceInfo(device_id, vector_types[i], sizeof(clock_frequency), &vector_width, &returned_size);
		printf("Vector type width for: %s = %i\n",vector_type_names[i],vector_width);
	}
	
	printf("\nMax Work Group Size: %lu\n",max_work_group_size);
        //printf("Max Work Item Dims: %lu\n",max_work_item_dims);
        //for(size_t i=0;i<max_work_item_dims;i++)
        //	printf("Max Work Items in Dim %lu: %lu\n",(long unsigned)(i+1),(long unsigned)max_work_item_sizes[i]);
	
	printf("Max Compute Units: %i\n",max_compute_units);
	printf("\n");
	
	return CL_SUCCESS;
    
}

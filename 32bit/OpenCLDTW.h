//
//  OpenCLDTW.h
//  Audio-Mosaicing
//
//  Created by Edward Costello on 05/04/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//


#import <MacTypes.h>
#import <stdio.h>
#import <stdlib.h>
#import <OpenCL/OpenCL.h>
#import "Matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef enum OpenCLDTW_Device {
        
        OpenCLDTW_useCPU,
        OpenCLDTW_useGPU
        
    } OpenCLDTW_Device;
    
    typedef struct OpenCLDTW
    {
        size_t maximumRowCount;
        size_t maximumColumnCount;
        size_t maximumElementCount;
        size_t paletteRowCount;
        size_t tempMemoryCount;
        size_t globalWorkSize;
        cl_program program;
        cl_kernel kernel;
        Float32 *paletteData;
        Float32 *paletteBeatIndexes;
        Float32 *paletteBeatCounts;
        size_t beatsCount;
        Boolean useBeats;
        cl_command_queue commandQueue;
        cl_context context;
        
        cl_mem analysisMemory;
        cl_mem paletteMemory;
        cl_mem resultMemory;
        cl_mem paletteRowCountsMemory;
        cl_mem paletteRowIndexesMemory;
        cl_device_id device;
        
    } OpenCLDTW;
    
    OpenCLDTW *OpenCLDTW_new(OpenCLDTW_Device device,
                             size_t maximumRowCount,
                             size_t maximumColumnCount,
                             Float32 *paletteData,
                             size_t paletteRowCount,
                             Matrix32 *beats,
                             Boolean useBeats);
    
    void OpenCLDTW_delete(OpenCLDTW *self);
    void OpenCLDTW_configureNoBeats(OpenCLDTW *self);
    void OpenCLDTW_configureBeats(OpenCLDTW *self);

    cl_mem OpenCLDTW_allocateFloatBuffer(OpenCLDTW *self, size_t size, cl_mem_flags flag);
    void OpenCLDTW_writeFloatBuffer(OpenCLDTW *self, cl_mem clBuffer, Float32 *data, size_t size);
    void OpenCLDTW_process(OpenCLDTW *self, Float32 *analysisData, Float32 *result);

    int OpenCLDTW_deviceStats(cl_device_id device_id);
    char *OpenCLDTW_loadProgramSource(const char *fileName);

#ifdef __cplusplus
}
#endif
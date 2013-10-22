DSPTools
========

Audio DSP tools for doing audio programming on Mac OS X. All this code is written in C and makes extensive use of Apple's Accelerate Framework.

Note: 32bit code is legacy, and is being slowly ported the 64bit as I need it.

##Dependencies##

[HDF5](http://www.hdfgroup.org/HDF5/) - Install through [Homebrew](https://github.com/mxcl/homebrew/) (brew install hdf5)


###Notes###
This code used LLVM preprocessor directives, namely __attribute__((unused)), __attribute__((cleanup())) and __attribute__((overloadable)) for convenience.
So it is recommended to compile this code using LLVM.

This makes writing some C code a bit less painful. For instance this allows scoped variables on the heap, you don't have to call a destructor. Many pseudoclasses allow using an underscore before the class type to indicate an automatic call to the classes destructor is made when the variable goes out of scope.

```
void myFunction()
{
	_Matrix *myMatrix = Matrix_new(4,4);
} //Destructor called here...
```
###Audio IO###

-   AudioInput
    * This allows audio input from the systems microphone. The callback from this class is used with an AudioStream for audio output.
-   AudioObject
    * This class opens audio files and copies the contents into Float64 buffers. The callback from this class is used with an AudioStream object to play an audio file.

-   AudioStream

###DSP###

-   FFT
-   PhaseVocoder
-   TriangleFilterBank
-   BandPassFilter
-   DTWAccelerate

###Data Structures###

-   Matrix
-   AudioMatrix
-   MatrixRingBuffer

###OpenCL###

####Classes####

-   OpenCLObject

####Data Structures####

-   MatrixCL
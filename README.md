DSPTools
========

Audio DSP tools for doing audio programming on Mac OS X. All this code is written in C and makes extensive use of Apple's Accelerate Framework.

Note: 32bit code is legacy, and is being slowly ported the 64bit as I need it. These classes will be documented when converted over to 64bit.

##Dependencies##

[HDF5](http://www.hdfgroup.org/HDF5/) - Install through [Homebrew](https://github.com/mxcl/homebrew/) (brew install hdf5)


##Notes##
This code used LLVM preprocessor directives, namely __attribute__((unused)), __attribute__((cleanup())) and __attribute__((overloadable)) for convenience.
So it is recommended to compile this code using LLVM.

This makes writing some C code a bit less painful. For instance this allows scoped variables on the heap, you don't have to call a destructor. Many pseudoclasses allow using an underscore before the class type to indicate an automatic call to the classes destructor is made when the variable goes out of scope.

```
void myFunction()
{
	_Matrix *myMatrix = Matrix_new(4,4);
} //Destructor called here...
```
There is a number of test files contained in this code that show how to use various classes. Also included is an Xcode project that should build on modern Mac OS systems with the HDF5 library installed.

All classes are commented using standard headerdoc style comments. These should show up automatically in Xcode 5.

##Classes##
###Audio IO###

-   __AudioInput__

    This allows audio input from the systems microphone. The callback from this class is used with an AudioStream for audio output.

-   __AudioObject__

    This class opens audio files and copies the contents into Float64 buffers. The callback from this class is used with an AudioStream object to play an audio file.

-   __AudioStream__
    
	This class is used to output or process samples from other classes.

###DSP###

-   __FFT__
    
	A class for peforming FFT analysis of audio samples. Complex or polar values can be calculated and the result of the analysis should be similar to the values found using MATLAB. 

-   __PhaseVocoder__

    A class for performing phase vocoder analysis and synthesis of audio. This class makes use of the __PhaseVocoderData__ class to store frequency and magnitude data. This class is useful for independent time dilation and pitch shifting.

-   __TriangleFilterBank__

    A class for filtering spectral magnitudes using a triangle filter bank calculated in a similar fashion to the filter bank used for MFCC calculations. Filters are spaced logarithmically and the number of filters can be specified using the constructor. 

-   __BandPassFilter__

    A butterworth band pass filter class.

-   __DTWAccelerate__

	A class for performing realtime dynamic time warping. This class is thus named as it makes heavy use of the Accelerate framework, there was an OpenCL version of this class also but has had to be rewritten.

###Data Structures###

-   __Matrix__

    A simple general purpose matrix class for performing linear algebra. This class was created to easily port MATLAB code to C.

-   __AudioMatrix__

    A class which inherets from __Matrix__ but adds functionality for playing back audio samples using the __AudioStream__ class.

-   __MatrixRingBuffer__

    A ringbuffer class for rotating buffers containing frames of samples or analysis data.

###OpenCL###

####Classes####

-   __OpenCLObject__

    A wrapper around Apples implementation of the OpenCL framework. Simplifies many aspects of getting OpenCL up and running using C.



####Data Structures####

-   __MatrixCL__

    The first of many OpenCL data structure classes that quickly and easily allows object based programming using OpenCL C.


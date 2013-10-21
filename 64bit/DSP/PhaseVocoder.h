//
//  PhaseVocoder.h
//  AudioMosaicing
//
//  Created by Edward Costello on 25/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//
//  This Phase Vocoder is based on the implementation in: The Audio Programming Book, Edited by Richard Boulanger and Victor Lazzarini http://mitpress.mit.edu/books/audio-programming-book

#import "AudioMatrix.h"
#import "FFT.h"
#import "AudioObject.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /**
     * @class PhaseVocoder
     *
     * @abstract A class for performing Phase Vocoder analysis and synthesis
     *
     * @param FFTFrameSize The FFT analysis frame size
     * @param hopSize The FFT hop size
     * @param samplerate The samplerate
     * @param fft An FFT object
     * @param window A window for framing input samples, hanning
     * @param tempFrame Temporary frame buffer
     * @param tempSampleShiftFrame Temporary FFT shift buffer
     * @param tempMagnitudes Temporary magnitudes buffer
     * @param tempFrequencies Temporary frequencies buffer
     * @param currentPhases The current input frames phase values
     * @param previousPhases The previous input frames phase values
     * @param twoPiArray An array where every element is set to 2 * pi
     * @param forwardFrequencyScale The forward frequency scaling factors for each bin
     * @param inverseFrequencyScale The inverse frequency scaling factors for each bin
     * @param forwardFactor Forward scaling factor
     * @param inverseFactor Inverse scaling factor
     * @param phasePosition The current phase position
     */
    typedef struct _PhaseVocoder
    {
        
        size_t FFTFrameSize;
        size_t hopSize;
        size_t samplerate;
        FFT *fft;
        Float64 *window;
        Float64 *tempFrame;
        Float64 *tempSampleShiftFrame;
        Float64 *tempMagnitudes;
        Float64 *tempFrequencies;
        
        Float64 *currentPhases;
        Float64 *previousPhases;
        Float64 *twoPiArray;
        Float64 *forwardFrequencyScale;
        Float64 *inverseFrequencyScale;

        Float64 forwardFactor;
        Float64 inverseFactor;
        size_t phasePosition;
        
        
    } PhaseVocoder;
    
    
    /**
     * @class PhaseVocoderData
     *
     * @abstract A structure that contains Phase Vocoder magnitudes and phases
     *
     * @param frequencies Frequency values matrix
     * @param magnitudes Magnitudes values matrix
     */
    typedef struct _PhaseVocoderData
    {
        Matrix *frequencies;
        Matrix *magnitudes;
        
    } PhaseVocoderData;
    
    /**
     *  Contruct a PhaseVocoder object
     *
     *  @param FFTFrameSize The specified FFT analysis frame size
     *  @param hopSize      The specified FFT frame hop size
     *  @param samplerate   The specified sample rate
     *
     *  @return A PhaseVocoder object instance
     */
    PhaseVocoder *PhaseVocoder_new(size_t FFTFrameSize,
                                   size_t hopSize,
                                   size_t samplerate);
    
    /**
     *  Destroy a PhaseVocoder object
     *
     *  @param self A PhaseVocoder object instance
     */
    void PhaseVocoder_delete(PhaseVocoder *const self);
    
    /**
     *  Scoped destruction of a PhaseVocoder object
     *
     *  @param self A PhaseVocoder object instance
     */
    void PhaseVocoder_scopedDelete(PhaseVocoder **self);

    /**
     *  Declare a scoped PhaseVocoder object
     */
#define _PhaseVocoder __attribute__((unused)) __attribute__((cleanup(PhaseVocoder_scopedDelete))) PhaseVocoder

    /**
     *  Analyse an AudioMatrix instance for phase vocoder data
     *
     *  @param self        An instance of a PhaseVocoder object
     *  @param audioMatrix An instance of an AudioMatrix object
     *
     *  @return A PhaseVocoderData object
     */
    PhaseVocoderData *PhaseVocoder_analyseAudioMatrix(PhaseVocoder *const self,
                                                      const AudioMatrix *const audioMatrix);
    
    /**
     *  Analyse an AudioObject instance for phase vocoder data
     *
     *  @param self        An instance of a PhaseVocoder object
     *  @param audioObject An instance of an AudioObject object
     *
     *  @return A PhaseVocoderData object
     */
    PhaseVocoderData *PhaseVocoder_analyseAudioObject(PhaseVocoder *const self,
                                                      const AudioObject *const audioObject);
    
    /**
     *  Analyse an audio file for phase vocoder data
     *
     *  @param self An instance of a PhaseVocoder object
     *  @param filePath The specified file path
     *
     *  @return A PhaseVocoderData object
     */
    OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(PhaseVocoder *const self,
                                                               const char *filePath);
    
    /**
     *  Analyse an audio file for phase vocoder data
     *
     *  @param filePath The specified file path
     *  @param FFTFrameSize The specified FFT analysis frame size
     *  @param hopSize      The specified FFT frame hop size
     *  @return A PhaseVocoderData object
     */
    OVERLOADED PhaseVocoderData *PhaseVocoder_analyseAudioFile(const char *filePath,
                                                               size_t FFTFrameSize,
                                                               size_t hopSize);
    
    /**
     *  Synthesise a frame of audio samples from phase vocoder magnitudes and phases
     *
     *  @param self An instance of a PhaseVocoder object
     *  @param magnitudes  Input magnitudes frame
     *  @param frequencies Input frequencies frame
     *  @param output      Output samples frame
     */
    void PhaseVocoder_synthesise(PhaseVocoder *self,
                                 Float64 *magnitudes,
                                 Float64 *frequencies,
                                 Float64 *output);

    /**
     *  Synthesise a frame of audio samples from a PhaseVocoderData frame
     *
     *  @param self An instance of a PhaseVocoder object
     *  @param phaseVocoderData An instance of a PhaseVocoderData object
     *  @param frameNumber      A specified frame number
     *  @param output      Output samples frame
     */
    void PhaseVocoder_synthesiseFrame(PhaseVocoder *const self,
                                      PhaseVocoderData *phaseVocoderData,
                                      Float64 frameNumber,
                                      Float64 *output);
    
    /**
     *  Get pointers to magnitudes and frequencies frames for particular frame in the PhaseVocoderData object. This method works with fractional frame numbers.
     *
     *  @param self An instance of a PhaseVocoder object
     *  @param phaseVocoderData An instance of a PhaseVocoderData object
     *  @param frameNumber      A specified frame number. Fractional numbers are accepted, linear interpolation is used.
     *  @param magnitudesOut    Output magnitude frame
     *  @param frequenciesOut   Output frequency frame
     */
    void PhaseVocoder_getPhaseVocoderDataForFrame(PhaseVocoder *const self,
                                                  PhaseVocoderData *phaseVocoderData,
                                                  Float64 frameNumber,
                                                  Float64 **magnitudesOut,
                                                  Float64 **frequenciesOut);
    
    /**
     *  Perform phase vocoder synthesis of phase vocoder data
     *
     *  @param self An instance of a PhaseVocoder object
     *  @param phaseVocoderData Input instance of a PhaseVocoderData object
     *  @param audioMatrixOut     Output AudioMatrix object
     *  @param increment          frame number increment for time dilation purposes
     */
    void PhaseVocoder_inverse(PhaseVocoder *const self,
                              PhaseVocoderData *phaseVocoderDataIn,
                              AudioMatrix *audioMatrixOut,
                              Float64 increment);

    /**
     *  Construct a PhaseVocoderData object
     *
     *  @param frameCount   The amount of frames in the PhaseVocoderData object
     *  @param FFTFrameSize The FFT analysis frame size for the phase vocoder data
     *
     *  @return A PhaseVocoderData instance
     */
    PhaseVocoderData *PhaseVocoderData_new(const size_t frameCount,
                                           const size_t FFTFrameSize);
    
    /**
     *  Destroy a PhaseVocoderData object
     *
     *  @param A PhaseVocoderData instance
     */
    void PhaseVocoderData_delete(PhaseVocoderData *const self);
    
    /**
     *  Scoped destructor for PhaseVocoderData object
     *
     *  @param A PhaseVocoderData instance
     */
    void PhaseVocoderData_scopedDelete(PhaseVocoderData **self);

    /**
     *  Declare a scoped PhaseVocoderData object
     */
#define _PhaseVocoderData __attribute__((unused)) __attribute__((cleanup(PhaseVocoderData_scopedDelete))) PhaseVocoderData

    
#ifdef __cplusplus
}
#endif
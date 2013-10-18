//
//  PhaseVocoderTests.h
//  AudioMosaicing
//
//  Created by Edward Costello on 25/08/2013.
//  Copyright (c) 2013 Edward Costello. All rights reserved.
//

#import "PhaseVocoder.h"
#import "AudioStream.h"
#import "Matrix.h"

void PhaseVocoderForward(Matrix *audioMatrixIn,
                         PhaseVocoderData *phaseVocoderDataOut,
                         size_t FFTFrameSize,
                         size_t hopSize)
{
    Float64 *input = audioMatrixIn->rowCount == 1 ? Matrix_getRow(audioMatrixIn, 0) : Matrix_getRow(audioMatrixIn, 2);
    
    Float64 signalFrame[FFTFrameSize];
    Float64 window[FFTFrameSize];
    Float64 real[FFTFrameSize / 2];
    Float64 imag[FFTFrameSize / 2];
    Float64 lastPhi[FFTFrameSize / 2];
    vDSP_vclrD(lastPhi, 1, FFTFrameSize / 2);
    vDSP_hann_windowD(window, FFTFrameSize, vDSP_HANN_DENORM);
    size_t positionIn, phaseVocoderFrame = 0;
    size_t modulus;
    FFT *fft = FFT_new(FFTFrameSize);
    Float64 pi = M_PI;
    Float64 twoPi = M_PI * 2;
    Float64 fac =(44100. / (hopSize * twoPi));
    Float64 scal = (twoPi * hopSize / FFTFrameSize);
    
    for (positionIn = 0; phaseVocoderFrame < phaseVocoderDataOut->frequencies->rowCount; phaseVocoderFrame++, positionIn += hopSize) {
        
        modulus = positionIn % FFTFrameSize;
        
        for (size_t i = 0; i < FFTFrameSize; ++i) {
            
            signalFrame[(i + modulus) % FFTFrameSize] = input[positionIn + i] * window[i];
        }
                
        Float64 *magnitudes = Matrix_getRow(phaseVocoderDataOut->magnitudes, phaseVocoderFrame);
        Float64 *phases = Matrix_getRow(phaseVocoderDataOut->frequencies, phaseVocoderFrame);
        FFT_forwardComplex(fft, signalFrame, real, imag);


        for (size_t i = 0; i < FFTFrameSize / 2; ++i) {
            
            magnitudes[i] = sqrt(real[i] * real[i] + imag[i] * imag[i]);
            phases[i] = atan2(imag[i], real[i]);

            Float64 delta = phases[i] - lastPhi[i];
            lastPhi[i] = phases[i];
            
            while(delta > pi) delta -= twoPi;
            while(delta < -pi) delta += twoPi;
            
            phases[i] = delta;
            phases[i] = (delta + ((i + 1) * scal)) * fac;
        }
    }
    
    
    FFT_delete(fft);
    
}

void PhaseVocoderInverse(PhaseVocoderData *phaseVocoderDataIn,
                         Matrix *audioMatrixOut,
                         size_t FFTFrameSize,
                         size_t hopSize)
{
    Float64 *output = audioMatrixOut->data;
    
    Float64 signalFrame[FFTFrameSize];
    Float64 window[FFTFrameSize];
    Float64 real[FFTFrameSize / 2];
    Float64 imag[FFTFrameSize / 2];
    Float64 tempMagnitudes[FFTFrameSize / 2];
    Float64 tempFrequencies[FFTFrameSize / 2];

    Float64 lastPhi[FFTFrameSize / 2];
    vDSP_vclrD(lastPhi, 1, FFTFrameSize / 2);
    vDSP_hann_windowD(window, FFTFrameSize, vDSP_HANN_DENORM);
    size_t positionOut, phaseVocoderFrame = 0;
    size_t modulus;
    FFT *fft = FFT_new(FFTFrameSize);
    Float64 twoPi = M_PI * 2;
    Float64 fac =((hopSize * twoPi) / 44100.);
    Float64 scal = (44100. / FFTFrameSize);
    
    for (positionOut = 0; phaseVocoderFrame < phaseVocoderDataIn->frequencies->rowCount; phaseVocoderFrame++, positionOut += hopSize) {
        
        Float64 *magnitudes = Matrix_getRow(phaseVocoderDataIn->magnitudes, phaseVocoderFrame);
        Float64 *frequencies = Matrix_getRow(phaseVocoderDataIn->frequencies, phaseVocoderFrame);

        cblas_dcopy((UInt32)FFTFrameSize/2, magnitudes, 1, tempMagnitudes, 1);
        cblas_dcopy((UInt32)FFTFrameSize/2, frequencies, 1, tempFrequencies, 1);

        for (size_t i = 0; i < FFTFrameSize / 2; ++i) {
            
            Float64 delta = (tempFrequencies[i] - ((i + 1) * scal)) * fac;

            tempFrequencies[i] = lastPhi[i] + delta;

            lastPhi[i] = tempFrequencies[i];
            
            real[i] = tempMagnitudes[i] * cos(tempFrequencies[i]);
            imag[i] = tempMagnitudes[i] * sin(tempFrequencies[i]);
        }
        
        FFT_inverseComplex(fft, real, imag, signalFrame);
        
        modulus = positionOut % FFTFrameSize;
        
        for (size_t i = 0; i < FFTFrameSize; ++i) {
            
            output[positionOut + i] +=  signalFrame[(i + modulus) % FFTFrameSize] * window[i] * 0.5;
        }
    }
    
    
    FFT_delete(fft);
}

void PhaseVocoderAudioProgrammingBook()
{
    Matrix *audioMatrix = Matrix_new("AudioMosaicing/input.wav");
    Matrix *reconstructedAudio = Matrix_new(1, audioMatrix->columnCount * 2);
    
    size_t FFTFrameSize = 1024;
    size_t hopSize = 256;
    size_t FFTFramesCount  = (audioMatrix->columnCount - FFTFrameSize) / hopSize;
    
    PhaseVocoderData *phaseVocoderData = PhaseVocoderData_new(FFTFramesCount, FFTFrameSize);
    PhaseVocoderForward(audioMatrix, phaseVocoderData, FFTFrameSize, hopSize);
    PhaseVocoderData *newData = PhaseVocoderData_scale(phaseVocoderData, 2);

    PhaseVocoderInverse(phaseVocoderData, reconstructedAudio, FFTFrameSize, hopSize);
    PhaseVocoderInverse(newData, reconstructedAudio, FFTFrameSize, hopSize);

    AudioStream *audioStream = AudioStream_new(reconstructedAudio, Matrix_readCallback, NULL, NULL);
    
    AudioStream_process(audioStream, 40);
    
    AudioStream_delete(audioStream);
    Matrix_printRange(reconstructedAudio, 0, 1, 0, 100);
    
    Matrix_saveAsHDF(audioMatrix, "/Users/eddyc/Desktop/audio1.hdf", "audio1");
    Matrix_saveAsHDF(reconstructedAudio, "/Users/eddyc/Desktop/audio2.hdf", "audio2");

    Matrix_delete(audioMatrix);
    Matrix_delete(reconstructedAudio);
    
    PhaseVocoderData_delete(phaseVocoderData);
    
}

void PhaseVocoderTests()
{
    PhaseVocoder *phaseVocoder = PhaseVocoder_new();
    Matrix *audioMatrix = Matrix_new("AudioMosaicing/things.mp3");
    Matrix *EDS_newAudio = Matrix_new(1, audioMatrix->columnCount);

    PhaseVocoderData *EDS_phaseVocoderData = PhaseVocoder_analyseAudioMatrix(phaseVocoder, audioMatrix);
    PhaseVocoder_inverse(phaseVocoder, EDS_phaseVocoderData, EDS_newAudio, .66);

    AudioStream *audioStream = AudioStream_new(EDS_newAudio, Matrix_readCallback, NULL, NULL);

    AudioStream_process(audioStream, 120);

    AudioStream_delete(audioStream);

    Matrix_delete(audioMatrix);
    PhaseVocoder_delete(phaseVocoder);    
}


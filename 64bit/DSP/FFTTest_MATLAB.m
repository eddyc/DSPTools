real = hdf5read('/Users/eddyc/Desktop/real.hdf','/real');
imaginary = hdf5read('/Users/eddyc/Desktop/imaginary.hdf','/imaginary');
input = hdf5read('/Users/eddyc/Desktop/input.hdf','/input');
output = hdf5read('/Users/eddyc/Desktop/output.hdf','/output');

magnitude = hdf5read('/Users/eddyc/Desktop/magnitude.hdf','/magnitude');
phase = hdf5read('/Users/eddyc/Desktop/phase.hdf','/phase');

complexData = real + i * imaginary

matlabComplex = fft(input);
matlabComplex = matlabComplex(1:512);

matlabMags = abs(matlabComplex);
matlabPhases = angle(matlabComplex);

difference = matlabComplex - complexData
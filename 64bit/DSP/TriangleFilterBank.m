
function [TriangleFilterWeights] = TriangleFilterBank(filterCount, FFTFrameSize, samplerate)

numberOfFilters = filterCount;
magnitudeBins = FFTFrameSize / 2;
series = (1:(numberOfFilters + 2)) / (numberOfFilters + 2);
y = 10.^series;
y = y - y(1);
y = y ./ y(numberOfFilters + 2);
logs = y .* (samplerate/2);
 
 
lower = logs(1:numberOfFilters);
center = logs(2:numberOfFilters + 1);
upper = logs(3:numberOfFilters + 2);
 
filterWeights = zeros(numberOfFilters,magnitudeBins);
 
triangleHeight = ones(1,numberOfFilters);
fftFreqs = ((0:magnitudeBins-1)/(magnitudeBins - 1) )*(samplerate/2);
 
for chan=1:numberOfFilters
    filterWeights(chan,:) = ...
        (fftFreqs > lower(chan) & fftFreqs <= center(chan)).* ...
        triangleHeight(chan).*(fftFreqs-lower(chan))/(center(chan)-lower(chan)) + ...
        (fftFreqs > center(chan) & fftFreqs < upper(chan)).* ...
        triangleHeight(chan).*(upper(chan)-fftFreqs)/(upper(chan)-center(chan));
end
 
TriangleFilterWeights = filterWeights;

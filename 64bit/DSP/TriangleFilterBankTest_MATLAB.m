
frequencies = h5read('/Users/eddyc/Desktop/triangle.hdf','/triangle');
mags = h5read('/Users/eddyc/Desktop/mags.hdf','/mags');

subplot(2,1,1);

imagesc(log(mags));

subplot(2,1,2);

imagesc(log(frequencies));
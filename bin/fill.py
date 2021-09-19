#!/usr/bin/env python
import numpy
import sys

filename = sys.argv[1]

in_shape = 64, 256, 256
out_shape = 256, 256, 256

diff = [x // y for x, y in zip(out_shape, in_shape)]

in_data = numpy.fromfile(filename, numpy.uint8)
in_data = in_data.reshape(in_shape)
out_data = numpy.zeros(out_shape, numpy.uint8)

for z in xrange(out_shape[2]):
    for y in xrange(out_shape[1]):
        for x in xrange(out_shape[0]):
            out_data[z][y][x] = in_data[z // diff[0]][y // diff[1]][x // diff[2]]

out_data.reshape(reduce(lambda x, y: x * y, out_shape)).tofile(sys.argv[2])


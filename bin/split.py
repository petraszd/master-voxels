#!/usr/bin/env python
import numpy
import sys
import zipfile
import math
import os

filename = sys.argv[1]
CUBES = int(sys.argv[2])

ZOOM = 2.0
T = 0.0
MAX_ITER = 20

voxels = numpy.fromfile(filename, numpy.uint32)
if len(sys.argv) == 7:
    XDIM = int(sys.argv[4])
    YDIM = int(sys.argv[5])
    ZDIM = int(sys.argv[6])
else:
    XDIM = YDIM = ZDIM = int(math.ceil(math.pow(len(voxels), 1.0 / 3.0)))

voxels = voxels.reshape((ZDIM, YDIM, XDIM))

hx = XDIM / CUBES
hy = YDIM / CUBES
hz = ZDIM / CUBES
ziped = zipfile.ZipFile(sys.argv[3], 'w')

info = open('info.txt', 'w')
info.write('{0}\n'.format(CUBES))

i = 0
for z in xrange(CUBES):
    for y in xrange(CUBES):
        for x in xrange(CUBES):
            part = voxels[z * hz:(z + 1) * hz, y * hy:(y + 1) * hy, x * hx:(x + 1) * hx]
            filename = '{0}.a'.format(i)
            part.reshape(hz * hy * hx).tofile(filename)
            ziped.write(filename)
            os.unlink(filename)
            info.write('{0} {1} {2} {3}\n'.format(hx, hy, hz, filename))
            i += 1
info.close()

ziped.write('info.txt')
os.unlink('info.txt')

ziped.close()


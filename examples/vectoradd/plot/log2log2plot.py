#!/usr/bin/env python
import numpy
import matplotlib.pyplot
import sys

if __name__ == "__main__":
    data1 = numpy.loadtxt(sys.argv[1])
    matplotlib.pyplot.loglog(data1[:,0], data1[:,1], "-o", basex = 2, basey = 2)
    matplotlib.pyplot.savefig("output.pdf")
    matplotlib.pyplot.show()

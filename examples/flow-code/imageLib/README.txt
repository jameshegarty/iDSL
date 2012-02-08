imageLib is a small C++ library for 2D multi-band images.  See Image.h
for more detail.  Reading and writing of images is supported in png
and pgm/ppm formats, as well as a subset of Targa.  See ImageIO.h for
more detail.

This code is derived from a subset of the StereoMatcher code by Rick
Szeliski and Daniel Scharstein, which is available at
http://research.microsoft.com/downloads/.

This code is distributed under the same Source Code License Agreement
as the original version.  Please see the file Copyright.h


===============
addendum by PS (19 Jan 2012):

If you've installed libpng but make isn't finding it, use the following
two commands (and use the appropriate path in the first command):

CPLUS_INCLUDE_PATH=/opt/local/include/libpng14/
export CPLUS_INCLUDE_PATH

...and in order to make the flowcode in the outer directory:

LIBRARY_PATH=/opt/local/lib
export LIBRARY_PATH

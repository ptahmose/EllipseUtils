# EllipseUtils

This project contains some utilities in order to work with ellipses.

You will find code for a least-square-fit of an ellipse to a list of points (adapted from this paper:  http://autotrace.sourceforge.net/WSCG98.pdf)

The library is header-only and should work with any decent C++-compiler.

In order to use the library, simply include the file EllipseUtils/ellipseUtils.h.

Included are two programs for testing and demonstrating the library:
* 'Test_EllipseUtils.exe' will execute a couple of unit-tests, and print out success/failure.
* 'EllipseFitToSvg.exe' will perfom a fit to a set of points (which are read from a text-file), and can write the fit as an SVG-graphic.

## building

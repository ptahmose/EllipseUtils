# EllipseUtils

This project contains some utilities in order to work with ellipses.

You will find code for a least-square-fit of an ellipse to a list of points (adapted from this paper:  http://autotrace.sourceforge.net/WSCG98.pdf)

The library is header-only and should work with any decent C++-compiler. It depends on the [Eigen-library](http://eigen.tuxfamily.org/index.php?title=Main_Page).

In order to use the library, simply include the file EllipseUtils/ellipseUtils.h.

Included are two programs for testing and demonstrating the library:
* 'Test_EllipseUtils.exe' will execute a couple of unit-tests, and print out success/failure.
* 'EllipseFitToSvg.exe' will perfom a fit to a set of points (which are read from a text-file), and can write the fit as an SVG-graphic.

## building

For building with VisualStudio there is a solution-file provided (Test_EllipseUtils.sln).

For building on Linux, we support CMake. The following command should create the two executables:

```
git clone https://github.com/ptahmose/EllipseUtils.git
mkdir BuildEllipseUtils
cd BuildEllipseUtils
cmake -D EIGEN3_INCLUDE_DIR=/home/pi/Dev/eigen-eigen-da9b4e14c255 ../EllipseUtils/
make
```

The argument for the cmake invocation gives the path to the Eigen3-header files, and needs to be changed to the actual location on your system.

In the Test-folder there is a little script `runtests.sh` which can be used to test-drive the programs (and it cretes some SVGs in the Test/Results folder).


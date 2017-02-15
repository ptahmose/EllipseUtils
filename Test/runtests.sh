#!/bin/bash

cd "${BASH_SOURCE%/*}" || exit

EXECUTABLE="../Build/EllipseFitToSvg/EllipseFitToSvg"

mkdir -p ./Results

$EXECUTABLE --command fit --input ./Data/coordinates_1.txt --svgoutput ./Results/coordinates_1.svg

$EXECUTABLE --command generate --ellipseparams 100,100,40,50,30 -e 2 -r 42 | $EXECUTABLE --command fit --input - --svgoutput ./Results/coordinates_2.svg  


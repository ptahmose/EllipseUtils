#!/bin/bash

cd "${BASH_SOURCE%/*}" || exit

EXECUTABLE="../Build/EllipseFitToSvg/EllipseFitToSvg"

mkdir -p ./Results

$EXECUTABLE --command fit --input ./Data/coordinates_1.txt --svgoutput ./Results/coordinates_1.svg


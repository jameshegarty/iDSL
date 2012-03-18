#!/bin/bash

#to print the results use grep -H . *

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./temp/*
rm ./results/*

${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.1 -save ./temp/frame2.ppm

for window in 4 6 8 10
do
  for iterations in 1 2 3 4
  do
    for levels in 1 2 3
    do
      outputname="w`printf "%02d" $window`_i`printf "%02d" $iterations`_l`printf "%02d" $levels`"
      output="./temp/vectors_${outputname}.flo"

      ${EXAMPLES}lucaskanade/lucaskanade ./m1.bmp ./temp/frame2.ppm $output $window $iterations $levels && ${EXAMPLES}trim/trim $output 16 16 16 16 ${output}.trim.flo && ${EXAMPLES}diff/diff --L2 ${output}.trim.flo ref.flo >> ./results/${outputname}.txt &
    done
  done
done
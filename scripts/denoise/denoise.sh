#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*

for (( i=0; i<1; i+=1 ))
do
    output="./temp/vectors_`printf "%03d" $i`.bmp"
    ${EXAMPLES}lucaskanade/lucaskanade ./frame`printf "%03d" $i`.bmp ./frame000.bmp $output 6 3 2 && ${EXAMPLES}backAdvect/backAdvect ./frame`printf "%03d" $i`.bmp $output ${output}.back.bmp &
done
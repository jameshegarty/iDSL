#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*

for (( i=1; i<14; i+=1 ))
do
    output="./temp/vectors_`printf "%03d" $i`.bmp"
    ${EXAMPLES}lucaskanade/lucaskanade ./frame000.bmp ./frame`printf "%03d" $i`.bmp  $output 6 3 4 && ${EXAMPLES}backAdvect/backAdvect ./frame`printf "%03d" $i`.bmp $output ${output}.back.bmp && convert ${output}.back.bmp ${output}.back.png &
done

echo -ne "ImageStack -load ./temp/vectors_001.bmp.back.png " >> command.txt

for (( i=2; i<14; i+=1 ))
do
  echo -ne "-load ./temp/vectors_`printf "%03d" $i`.bmp.back.png -add " >> command.txt
done
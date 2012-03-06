#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*
rm command.txt

if [ $# -ne 3 ]; then echo "Usage: denoise.sh animfilenameroot frames pyramidLevels"; exit; fi
anim=$1
frames=$2

for (( i=1; i<$frames; i+=1 ))
do
    output="./temp/vectors_`printf "%03d" $i`.flo"
    ${EXAMPLES}lucaskanade/lucaskanade ./${anim}000.bmp ./${anim}`printf "%03d" $i`.bmp $output 6 3 $3 && ${EXAMPLES}backAdvect/backAdvect ./${anim}`printf "%03d" $i`.bmp $output ${output}.back.bmp && convert ${output}.back.bmp ${output}.back.png &
done

echo -ne "ImageStack -load ./temp/vectors_001.flo.back.png " >> command.txt

for (( i=2; i<$frames; i+=1 ))
do
  echo -ne "-load ./temp/vectors_`printf "%03d" $i`.flo.back.png -add " >> command.txt
done
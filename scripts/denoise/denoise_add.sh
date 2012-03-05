#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*
rm command.txt

if [ $# -ne 1 ]; then echo "Usage: denoise_add.sh animfilenameroot"; exit; fi
anim=$1

for (( i=1; i<14; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.flo"
    ${EXAMPLES}lucaskanade/lucaskanade ./frame`printf "%03d" $j`.bmp ./frame`printf "%03d" $i`.bmp  $output 6 3 4 &
done

wait
echo "awesome"

for (( i=1; i<14; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.flo"
    outputj="./temp/vectors_`printf "%03d" $j`.flo"
    ImageStack -load ${outputj}.flo -load ${output}.flo -add -save  ${output}.add.flo
done

for (( i=1; i<14; i+=1 ))
do
  output="./temp/vectors_`printf "%03d" $i`.flo"
  ${EXAMPLES}backAdvect/backAdvect ./frame`printf "%03d" $i`.bmp ${output}.add.flo ${output}.back.bmp && convert ${output}.back.bmp ${output}.back.png &
done

echo -ne "ImageStack -load ./temp/vectors_001.bmp.back.png " >> command.txt

for (( i=2; i<14; i+=1 ))
do
  echo -ne "-load ./temp/vectors_`printf "%03d" $i`.bmp.back.png -add " >> command.txt
done
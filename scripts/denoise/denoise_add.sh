#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*
rm command.txt

for (( i=1; i<14; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.bmp"
    ${EXAMPLES}lucaskanade/lucaskanade ./frame`printf "%03d" $j`.bmp ./frame`printf "%03d" $i`.bmp  $output 6 3 4 && convert $output ${output}.png &
done

wait
echo "awesome"

for (( i=1; i<14; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.bmp"
    outputj="./temp/vectors_`printf "%03d" $j`.bmp"
    ImageStack -load ${outputj}.png -offset -0.5 -load ${output}.png -offset -0.5 -add -offset 0.5 -save  ${output}.png && convert ${output}.png ${output}.add.bmp
done

for (( i=1; i<14; i+=1 ))
do
  output="./temp/vectors_`printf "%03d" $i`.bmp"
  ${EXAMPLES}backAdvect/backAdvect ./frame`printf "%03d" $i`.bmp ${output}.add.bmp ${output}.back.bmp && convert ${output}.back.bmp ${output}.back.png &
done

echo -ne "ImageStack -load ./temp/vectors_001.bmp.back.png " >> command.txt

for (( i=2; i<14; i+=1 ))
do
  echo -ne "-load ./temp/vectors_`printf "%03d" $i`.bmp.back.png -add " >> command.txt
done
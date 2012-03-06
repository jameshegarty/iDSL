#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*
rm command.txt

if [ $# -ne 2 ]; then echo "Usage: denoise.sh animfilenameroot frames"; exit; fi
anim=$1
frames=$2

for (( i=1; i<$frames; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.flo"
    ${EXAMPLES}lucaskanade/lucaskanade ./${anim}`printf "%03d" $j`.bmp ./${anim}`printf "%03d" $i`.bmp  $output 6 3 4 &
done

wait

cp ./temp/vectors_001.flo ./temp/vectors_001.flo.add.flo

for (( i=2; i<$frames; i+=1 ))
do
    j=`expr $i - 1`
    output="./temp/vectors_`printf "%03d" $i`.flo"
    outputj="./temp/vectors_`printf "%03d" $j`.flo"
    ImageStack -load ${outputj} -load ${output} -add -save  ${output}.add.flo
done

for (( i=1; i<$frames; i+=1 ))
do
  output="./temp/vectors_`printf "%03d" $i`.flo"
  ${EXAMPLES}backAdvect/backAdvect ./${anim}`printf "%03d" $i`.bmp ${output}.add.flo ${output}.back.bmp && convert ${output}.back.bmp ${output}.back.png &
done

echo -ne "ImageStack -load ./temp/vectors_001.flo.back.png " >> command.txt

for (( i=2; i<$frames; i+=1 ))
do
  echo -ne "-load ./temp/vectors_`printf "%03d" $i`.flo.back.png -add " >> command.txt
done
#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/res_me.txt
rm ./results/res_lk3.txt

for (( i=0; i<100; i+=5 ))
do
    ${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.0`printf "%02d" $i` -save ./temp/frame2_`printf "%02d" $i`.ppm &
done

for (( i=0; i<100; i+=5 ))
do
    ${EXAMPLES}motionestimation/motionestimation ./m1.bmp ./temp/frame2_`printf "%02d" $i`.ppm ./temp/vectors_me_`printf "%02d" $i`.bmp 3 3 && ${EXAMPLES}diff/diff --L2 ./temp/vectors_me_`printf "%02d" $i`.bmp ref_w3.bmp >> ./results/res_me.txt &
done

for (( i=0; i<100; i+=5 ))
do
    ${EXAMPLES}lucaskanade/lucaskanade ./m1.bmp ./temp/frame2_`printf "%02d" $i`.ppm ./temp/vectors_lk_`printf "%02d" $i`.bmp 6 3 && ${EXAMPLES}diff/diff --L2 ./temp/vectors_lk_`printf "%02d" $i`.bmp ref_w3.bmp >> ./results/res_lk3.txt &
done
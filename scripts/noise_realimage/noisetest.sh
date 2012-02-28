#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm res_me.txt
rm res_lk3.txt

for (( i=0; i<100; i+=5 ))
do
    ${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.0`printf "%02d" $i` -save frame2.ppm
    
    ${EXAMPLES}motionestimation/motionestimation ./m1.bmp ./frame2.ppm 3 3
    ${EXAMPLES}diff/diff --L2 vectors.bmp ref_w3.bmp >> res_me.txt

    ${EXAMPLES}lucaskanade/lucaskanade ./m1.bmp ./frame2.ppm 9 3
    ${EXAMPLES}diff/diff --L2 vectors.bmp ref_w3.bmp >> res_lk3.txt
done
#!/bin/bash

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/*
rm ./temp/*

for (( i=0; i<100; i+=5 ))
do
    ${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.0`printf "%02d" $i` -save ./temp/frame2_`printf "%02d" $i`.ppm &
done

wait

for (( i=0; i<100; i+=5 ))
do
    output="./temp/vectors_me_`printf "%02d" $i`.flo"
    ${EXAMPLES}motionestimation/motionestimation ./m1.bmp ./temp/frame2_`printf "%02d" $i`.ppm $output 3 3 && ${EXAMPLES}trim/trim $output 16 16 16 16 ${output}.trim.flo && ${EXAMPLES}diff/diff --L2 ${output}.trim.flo ref.flo >> ./results/res_me.txt &
done

for (( i=0; i<100; i+=5 ))
do
    output="./temp/vectors_lk_`printf "%02d" $i`.flo"
    ${EXAMPLES}lucaskanade/lucaskanade ./m1.bmp ./temp/frame2_`printf "%02d" $i`.ppm $output 10 2 1 && ${EXAMPLES}trim/trim $output 16 16 16 16 ${output}.trim.flo && ${EXAMPLES}diff/diff --L2 ${output}.trim.flo ref.flo >> ./results/res_lk3.txt &
done
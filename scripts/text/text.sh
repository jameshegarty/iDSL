#!/bin/bash

#to print the results use grep -H . *

EXAMPLES="../../examples/"


#${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.1 -save ./temp/frame2.ppm

for gaussPyramidThresh in 2 3
do
  for finalMixThresh in 2 3
  do
    for k in 20 40 100 200 400
    do

      for windowRadius in 20 50 100 300
      do
        for inv in 0 1
        do
          for image in 1 2 3 4 5 6 7 8 9 10
          do
	    outputname="`printf "%02d" $image`_inv`printf "%02d" $inv`_wr`printf "%02d" $windowRadius`_k`printf "%02d" $k`_fmt`printf "%02d" $finalMixThresh`_gpt`printf "%02d" $gaussPyramidThresh`"
#	    if [ ! -d "./results/${outputname}" ]; then
	    if [ ! -f "./results/${outputname}/res.bmp" ]; then
	      mkdir ./results/${outputname}
	      ${EXAMPLES}text/text /scratch/jhegarty/ee368project/images/${image}.bmp ./results/${outputname}/ $inv $k $windowRadius $gaussPyramidThresh $finalMixThresh 0 &
            fi

	  done
	done
      done

      wait

    done
  done
done
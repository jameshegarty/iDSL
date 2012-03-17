#!/bin/bash

#to print the results use grep -H . *

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/res.txt

for (( noise=0; noise<100; noise+=5 ))
do

  outputname="n`printf "%02d" $noise`"
  output="./temp/vectors_${outputname}.flo"

  ${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.0`printf "%02d" $noise` -save ./flow_code/data/other-data/RubberWhale/frame11.png &&  matlab -r "cd flow_code;estimate_flow_demo;exit" && mv ./flow_code/result/middle-other/estimated_flow_004.flo ${output} && ${EXAMPLES}trim/trim $output 16 16 16 16 ${output}.trim.flo && ${EXAMPLES}diff/diff --L2 ${output}.trim.flo ref.flo >> ./results/res.txt &
  
done
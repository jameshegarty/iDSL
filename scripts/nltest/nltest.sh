#!/bin/bash

#to print the results use grep -H . *

IMAGESTACK="ImageStack"
EXAMPLES="../../examples/"

rm ./results/res.txt


for (( block=0; block<4; block+=1 ))
do

  for (( i=0; i<5; i+=1 )) #100
  do

    noise=$(echo "(${block}*5+${i})*5" | bc)
    outputname="n`printf "%02d" $noise`"
    output="./temp/vectors_${outputname}.flo"

    ${IMAGESTACK} -load m1.png -translate 2 2 -noise 0 0.0`printf "%02d" $noise` -save ./temp/frame2_`printf "%02d" $noise`.png &&  matlab -r "cd flow_code;nltest('classic+nl-fast','../m1.png','../temp/frame2_`printf "%02d" $noise`.png','.${output}');exit" && ${EXAMPLES}trim/trim $output 16 16 16 16 ${output}.trim.flo && ${EXAMPLES}diff/diff --L2 ${output}.trim.flo ref.flo >> ./results/res.txt &
  
  done

  wait
done
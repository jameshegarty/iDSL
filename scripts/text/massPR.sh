#!/bin/bash


for (( i=27; i<40; i+=1))
do
  start=$(($i*1000))
  end=$(($i*1000+1000))
  echo "matlab -nosplash -nodesktop -r 'massPR(`printf "%02d" $start`,`printf "%02d" $end`)'" | sh &
done

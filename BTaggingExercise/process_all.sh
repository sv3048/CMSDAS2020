#!/bin/bash -f

## run on Data samples
mkdir -p Output
mkdir -p logs

for sample in {-1,1,2,3,4,5,6,7,8,9,10};do
		root -b -q 'Selection.C+('$sample')' > logs/log_$sample.txt &
		echo "Submitted sample index" $sample
		sleep 3
done

wait 

echo "Finished producing all histograms."

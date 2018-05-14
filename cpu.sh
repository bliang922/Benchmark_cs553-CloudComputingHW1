#!/bin/bash

######################
######################

for bmtype in QP HP SP DP
do
		for thread in 1 2 4
		do	
		echo "CPU_Benchmark: Operation type: "$bmtype", thread number: "$thread
		cpu/MyCPUBench $bmtype $thread>>cpu/output/cpu_SP_1thread.out.dat
		echo>>cpu/output/cpu_SP_1thread.out.dat
		done
done

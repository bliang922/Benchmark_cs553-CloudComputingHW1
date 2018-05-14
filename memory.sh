#!/bin/bash

######################
######################

for bmtype in RWS RWR
do
	for bsize in 1KB 1MB 10MB
	do
		for thread in 1 2 4
		do	
		echo "Memory_Benchmark: Operation type: "$bmtype", block size: "$bsize", thread number: "$thread
		memory/MyRAMBench $bmtype $bsize $thread>>memory/output/memory-RWR-1-1thread.out.dat
		echo >>memory/output/memory-RWR-1-1thread.out.dat
	done
	done
done
for bmtype in LTC_RWR LTC_RWS
do
	for bsize in 1KB
	do
		for thread in 1 2 4
		do	
		echo "Memory_Benchmark: Operation type: "$bmtype", block size: "$bsize", thread number: "$thread
		memory/MyRAMBench $bmtype $bsize $thread>>memory/output/memory-RWR-1-1thread.out.dat
		echo >>memory/output/memory-RWR-1-1thread.out.dat


	done
	done
done
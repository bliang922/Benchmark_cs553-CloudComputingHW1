#!/bin/bash

######################
######################
cd disk
for bmtype in WS RS RR WR
do
	for bsize in 1KB 1000KB 10000KB 100000KB
	do
		for thread in 1 2 4
		do	
		echo "Disk_Benchmark: Operation type: "$bmtype", block size: "$bsize", thread number: "$thread
		./MyDiskBench $bmtype $bsize $thread >> output/disk-RR-1-1thread.out.dat
		echo >>output/disk-RR-1-1thread.out.dat

		done
	done
done

for bmtype in LTC_RR LTC_WR
do
	for bsize in 1KB
	do
		for thread in 1 2 4 8 16 32 64 128
		do	
		echo "Disk_Benchmark: Operation type: "$bmtype", block size: "$bsize", thread number: "$thread
		./MyDiskBench $bmtype $bsize $thread >> output/disk-RR-1-1thread.out.dat
		echo >>output/disk-RR-1-1thread.out.dat

		done
	done
done
echo " "


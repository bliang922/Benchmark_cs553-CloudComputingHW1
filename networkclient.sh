#!/bin/bash

    echo "I am the client."
echo $SLURM_JOB_NODELIST
	for bmtype in TCP UDP
	do
		for bsize in 1KB 32KB
		do
			for thread in 1 2 4 8
			do
		sleep 10	
		network/MyNETBench-client $bmtype $bsize $thread 192.168.9.99>>network/network-TCP-1-1thread.out.dat
			echo>>network/network-TCP-1-1thread.out.dat


			done
		done
	done

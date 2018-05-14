#!/bin/bash

    echo "I am the server $(hostname) "
	cat /etc/hosts

	for bmtype in TCP UDP
	do
		for bsize in 1KB 32KB
		do
			for thread in 1 2 4 8
			do	
		network/MyNETBench-server $bmtype $bsize $thread
			done
		done
	done
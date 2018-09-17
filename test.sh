#!/bin/bash

if [ $# != 1 ]; then
	echo "Invalid parameters!"
	exit 1
fi
cislo=${#1}
pocet=$[($cislo*2)-2]
mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp
mpirun --prefix /usr/local/share/OpenMPI -np $pocet pro $1        

rm pro
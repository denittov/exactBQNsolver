#!/bin/bash

# check if number of arguments is exactly 2
if [ ${#} -ne 2 ] ; then
	echo "[qnsolver_graph.sh] Usage: sh qnsolver_graph.sh <n_job> <net_path>"
	exit 1
fi

file1=net.txt

# clean directory TEMP_JOBS and create it if doesn't exist
if [ -d TEMP_JOBS ]; then 
	rm -f TEMP_JOBS/*
else
	mkdir TEMP_JOBS
fi

# check if configuration file exists
if [ ! -f ${2} ]; then 
	echo "[qnsolver.sh] The specified net configuration file does not exist, execution stopped.\n"
	exit 1
fi

cp ${2} TEMP_JOBS/${file1}

# generate indices for each total number of job in the net

#i=80
#while [ "$i" -le 101 ]; do
#    amixer cset numid=1 "$i%"
#    sleep 60
#    i=$(( i + 1 ))
#done 

#i=80
#while [ "$i" -le 101 ]; do
#    amixer cset numid=1 "$i%"
#    sleep 60
#    i=$(( i + 1 ))
#done 

X=1
while [ "$X" -le "${1}" ]; do
    echo Indices with $X job
    
    sh qnsolver.sh $X ${2} >>  TEMP_JOBS/result_verbose.txt
    if [ $? -ne 0 ] ; then 		# check the return value of the last executed program
	    echo "[qnsolver_graph.sh] Error value returned by qnsolver, execution stopped."
	    exit 1
    fi
    
    cp TEMP/station_num.txt TEMP_JOBS/station_num.txt
    
    cd TEMP_JOBS

    ../bin/append_indices
    
    cd ..
    X=$(( X + 1 ))
done

echo "Done...Results in TEMP_JOBS directory"


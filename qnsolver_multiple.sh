#!/bin/bash

# clean directory MULTIPLE_NET_OUTPUT_EXACT and create it if doesn't exist

if [ -d MULTIPLE_NET_OUTPUT_EXACT ]; then 
	rm -rf MULTIPLE_NET_OUTPUT_EXACT/*
else
	mkdir MULTIPLE_NET_OUTPUT_EXACT
fi

# generate indices for each input file in MULTIPLE_NET_INPUT

for input_filename in MULTIPLE_NET_INPUT/*
do
	name=$(basename "$input_filename")
	name="${name%.*}" # name is file input name without extension
	num_jobs="${name##*_}" # num jobs is the input number fo jobs after '_' char
	
	echo "NETWORK in "$input_filename
	sh qnsolver_graph.sh $num_jobs $input_filename
	cp -ar TEMP_JOBS/ MULTIPLE_NET_OUTPUT_EXACT/$name
	echo

done

echo
echo "DONE!! Results in MULTIPLE_NET_OUTPUT_EXACT/ directory"
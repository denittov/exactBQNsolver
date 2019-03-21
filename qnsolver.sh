#!/bin/bash

# check if number of arguments is exactly 2
if [ ${#} -ne 2 ] ; then
	echo "[qnsolver.sh] Usage: sh qnsolver.sh n_job net_path"
	exit 1
fi

file1=net.txt

file2=states.txt
file3=transitions.txt
file4=state_prob.txt

if [ -d TEMP ]; then 
	rm -f TEMP/*
else
	mkdir TEMP
fi

if [ ! -f ${2} ]; then 
	echo "[qnsolver.sh] The specified net configuration file does not exist, execution stopped.\n"
	exit 1
fi
cp ${2} TEMP/${file1}

cd TEMP

echo "EXACT SOLUTION..."

../bin/generator ${1} ${file1} ${file2} ${file3}	# num_job net_file states_file transitions_file
if [ $? -ne 0 ] ; then 		# check the return value of the last executed program
	echo "[qnsolver.sh] Error value returned by generator, execution stopped."
	exit 1
fi
../bin/matrix_solver ${file3} ${file4}				# transitions_file state_prob_file
if [ $? -ne 0 ] ; then 		# check the return value of the last executed program
	echo "[qnsolver.sh] Error value returned by matrix_solver, execution stopped."
	exit 1
fi
../bin/indices ${1} ${file1} ${file2} ${file4}		# num_job net_file states_file state_prob_file
if [ $? -ne 0 ] ; then 		# check the return value of the last executed program
	echo "[qnsolver.sh] Error value returned by indices, execution stopped."
	exit 1
fi

echo "done."

#back to previous directory
cd ..

exit 0

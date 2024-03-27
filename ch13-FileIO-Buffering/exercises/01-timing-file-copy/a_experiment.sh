#!/bin/bash

# Test with different buffer sizes
MAX_BUF_SIZE=65536
FILE_SIZE_UNITS="MiB"
MIN_FILE_SIZE=1 # in units above
MAX_FILE_SIZE=1024
TEST_RUN_COUNT=5
COPY_EXEC_NAME="./copy"
TEST_FILE_PREFIX="test_file_"

# Create test files
for (( N = MIN_FILE_SIZE ; N <= MAX_FILE_SIZE ; N *= 2 )) do
	# Generate test file.
	FILE_SIZE="${N}${FILE_SIZE_UNITS}"
	TEST_FILE_NAME="${TEST_FILE_PREFIX}${FILE_SIZE}"
	fallocate -l "${FILE_SIZE}" $TEST_FILE_NAME
	if [[ $? -ne 0 ]]
	  then
		echo "Failed to generate test file for file size ${FILE_SIZE}"
	fi

	# Write header for results file.
	RESULTS_FILE="results_${FILE_SIZE}"
	echo "BUF_SIZE    real   user   sys" >> ${RESULTS_FILE}

	# Write results for all buffer sizes.
	COPY_FILE_PREFIX="test_copy_"
	for (( BUF_SIZE=1 ; BUF_SIZE <= MAX_BUF_SIZE ; BUF_SIZE *= 2 )) do
		# Delete executable and build with new buffer size.
		if [[ -e $COPY_EXEC_NAME ]]
			then
				rm copy
		fi
		make CFLAGS="-DBUF_SIZE=${BUF_SIZE}" &> /dev/null
		if [[ $? -ne 0 ]]
			then
				echo "Make failed"
				exit 1
		fi

		# Run TEST_RUN_COUNT times and accumulate times.
		TIMES=(0 0 0)
		for (( RUN=0 ; RUN < TEST_RUN_COUNT ; RUN++ )) do
			mapfile -t < <( (time -p ./copy $TEST_FILE_NAME "${COPY_FILE_PREFIX}${RUN}") 2> >(cut -d" " -f 2-) )
			# Accumulate times
			for i in 0 1 2; do
				TIMES[$i]=$(bc <<< "scale=10;${TIMES[$i]} + ${MAPFILE[$i]}")
			done
		done
		# Divide by TEST_RUN_COUNT to compute average.
		for i in 0 1 2; do
			TIMES[$i]=$(bc <<< "scale=10;${TIMES[$i]}/${TEST_RUN_COUNT}")
		done
		# Write to results file and remove files.
		echo "${BUF_SIZE}  ${TIMES[@]}" >> "${RESULTS_FILE}"
		rm ${COPY_FILE_PREFIX}*
	done
done

rm ${TEST_FILE_PREFIX}*

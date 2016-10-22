#!/usr/bin/env bash

WRITER="writer"
CHECKER="checker"
CREATOR="./create_device.sh"
FILE="device-file"
LOCATION="./bin/"
WORKERS=16
TIME=60
DEBUG=false
OUTPUT=false

USAGE="To run: ./run_experiments.sh -l <file locations> -f <device file>
-r <writer> -c <checker> -t <seconds to write> -w <# threads to use>
-o (output validation details to file)\n\
If no arguments are supplied: file location = ./bin/, device file = device-file,
 writer = writer, checker = checker, seconds = 60, threads = 16"

while getopts "l:f:r:c:t:w:do?" opt; do
        case $opt in
                l)
                        LOCATION="./"${OPTARG}
                        ;;
                f)
                        FILE=${OPTARG}
                        ;;
                r)
                        WRITER=${OPTARG}
                        ;;
                c)
                        CHECKER=${OPTARG}
                        ;;
                t)
			TIME=${OPTARG}
			;;
                w)
                        WORKERS=${OPTARG}
                        ;;
                d)
                        DEBUG=true
                        ;;
                o)
                        OUTPUT=true
                        ;;
                \?)
                        echo -e ${USAGE}
                        exit
                        ;;
        esac
done

# clean out old files and compile the programs
if [ ${DEBUG} = false ]; then
        make clean > /dev/null
        make > /dev/null
else
        make clean
        make
fi

# create the device file
if [ ${DEBUG} = false ]; then
        ${CREATOR} -f ${FILE} -l ${LOCATION}
else
        ${CREATOR} -d -f ${FILE} -l ${LOCATION}
fi

# run the writer for t seconds
if [ ${DEBUG} = false ]; then
        ${LOCATION}${WRITER} -w ${WORKERS} -f ${LOCATION}${FILE} &
        sleep ${TIME}
        kill -9 $!
else
        ${LOCATION}${WRITER} -d -w ${WORKERS} -f ${LOCATION}${FILE} \
        > "writer_debug.txt"
fi

# run the checker
if [ ${DEBUG} = false ]; then
        if [ ${OUTPUT} = false ]; then
                ${LOCATION}${CHECKER} -f ${LOCATION}${FILE}
        else
                ${LOCATION}${CHECKER} -f ${LOCATION}${FILE} -o
        fi
else
        ${LOCATION}${CHECKER} -d -f ${LOCATION}${FILE} > "checker_debug.txt"
fi

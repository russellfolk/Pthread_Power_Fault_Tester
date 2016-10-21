#!/usr/bin/env bash
<<'DOCUMENTATION'
This program will create a device that is used to store records and simulate a
power loss using the associated writer and checker programs (located in bin/).
DOCUMENTATION

# This will allow the correct extension to be set based on operating system in
# use for this program. Thus far, only macOS 12 and Linux have been tested.
if [[ "$OSTYPE" == "linux-gnu" ]]; then
        # Linux
        BS_EXT="M"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        # Mac OSX
        BS_EXT="m"
elif [[ "$OSTYPE" == "cygwin" ]]; then
        # POSIX compatibility layer and Linux environment emulation for Windows
        BS_EXT="M" # needs testing...
elif [[ "$OSTYPE" == "msys" ]]; then
        # Lightweight shell and GNU utilities compiled for Windows (MinGW)
        BS_EXT="M" # needs testing...
elif [[ "$OSTYPE" == "win32" ]]; then
        # I'm not sure this can happen.
        BS_EXT="M" # needs testing...
elif [[ "$OSTYPE" == "freebsd"* ]]; then
        # ...
        BS_EXT="M" # needs testing...
else
        # Unknown, use linux default
        BS_EXT="M" # needs testing...
fi

# default arguments provided by assignment
BS="1"${BS_EXT}   # input/output block size in bytes
COUNT=128         # number of blocks to copy

USAGE="To run: ./create_device -f <device filename> -l <save location>\n\
If no arguments are supplied, filename = device-file, location = bin/"
DEBUG=false

# collect the input,
while getopts "f:l:?d" opt; do
        case $opt in
                f)
                        FILE=${OPTARG}
                        ;;
                l)
                        LOCATION=${OPTARG}
                        ;;
                \?)
                        echo -e ${USAGE}
                        exit
                        ;;
                d)
                        DEBUG=true
                        ;;
        esac
done

# if no arguments are supplied, use defaults...
if [ $# -eq 0 ]; then
	FILE="device-file" # name of file to create
        LOCATION="./bin/"  # location to store the file
fi

# create a file based on the input / defaults
if [ ${DEBUG} = false ]; then
        dd if=/dev/zero of=${LOCATION}${FILE} bs=${BS} count=${COUNT} \
        > /dev/null 2>&1
else
        dd if=/dev/zero of=${LOCATION}${FILE} bs=${BS} count=${COUNT}
fi
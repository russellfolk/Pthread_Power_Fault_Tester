#!/usr/bin/env bash

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
        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
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

BS="1"${BS_EXT}
COUNT=128
LOCATION="./"

if [ $# -eq 0 ]; then
	FILE="device-file"
else
	FILE="${1}"
fi

# create a file based on the input
dd if=/dev/zero of=${LOCATION}${FILE} bs=${BS} count=${COUNT}
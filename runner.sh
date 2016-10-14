#!/usr/bin/env bash

WORKERS=2

while getopts "p:t:w:" opt; do
        case $opt in
                p)
                        PROGRAM=${OPTARG}
                        ;;
                t)
			TIME=${OPTARG}
			;;
                w)
                        WORKERS=${OPTARG}
                        ;;
                \?)
                        echo "Invalid option: -$OPTARG" >&2
                        ;;
        esac
done

./${PROGRAM} -w ${WORKERS} > tmp & sleep ${TIME}; kill -INT %+
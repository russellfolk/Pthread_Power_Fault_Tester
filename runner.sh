#!/usr/bin/env bash

while getopts "p:t:" opt; do
        case $opt in
                p)
                        PROGRAM=${OPTARG}
                        ;;
                t)
						TIME=${OPTARG}
						;;
                \?)
                        echo "Invalid option: -$OPTARG" >&2
                        ;;
        esac
done

./${PROGRAM} > tmp & sleep ${TIME}; kill -INT %+
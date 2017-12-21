#!/bin/bash

while IFS='' read -r line || [[ -n "$line" ]]; do
    echo "Text read from file: $line"
    root -l -b run.C"(\"$line\")"
done < "$1"

## root -l -b 'run.C("lruns.txt")'

#!/bin/bash


SCRIPT_DIR=="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -z "$1" ]; then
    echo no parameters
else
    if [ -z "$2" ]; then
        echo parameter 1 = $1
        echo parameter 2 empty
     else
        echo parameter 1 = $1
        echo parameter 2 = $2
    fi  
fi

echo "scriptdir =" $SCRIPT_DIR




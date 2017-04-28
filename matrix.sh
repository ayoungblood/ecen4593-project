#!/usr/bin/env bash

# Simple run matrix script
# Takes an assembly source file and runs it with
# several different cache configurations
# Usage:
#   ./matrix.sh <file>

# I/D cache size = {{128,256}, {64,1024}}
for ISIZE in 128 64
do
    # I/D cache block size = {4, 1}
    for BLOCK in 16 4 1
    do
        # D write policy = {WT, WB}
        for DWRITE in 0 1
        do
            [[ $ISIZE -eq 128 ]] && DSIZE="256" || DSIZE="1024"
            [[ $DWRITE -eq 0 ]] && WP="thru" || WP="back"
            ./sim -ya -C s -J $ISIZE -E $DSIZE -F $BLOCK -K $BLOCK -H $WP $1 \
                2>/dev/null | grep "\$\#" | grep -v "\$\# Isize"
        done
    done
done

exit 0

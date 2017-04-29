#!/usr/bin/env bash

# Run matrix of cache/no cache configurations and check that the memory values
# are correct (match the values given in the assignment)

# For Program 1, expected memory values:
# 0x00000018: 0x00000070 (0d112)
# 0x0000001c: 0x000072ab (0d29355)
# 0x00000020: 0x000037e1 (0d14305)
# 0x00000024: 0x00000000 (0d0)

###############################################################################
# Program 1
function check1()
{
    DATA06=$(cat $FILE.output | grep 0x00000018 | awk '{print $3}' | sed 's/0d//' | tr -d '()')
    DATA07=$(cat $FILE.output | grep 0x0000001c | awk '{print $3}' | sed 's/0d//' | tr -d '()')
    DATA08=$(cat $FILE.output | grep 0x00000020 | awk '{print $3}' | sed 's/0d//' | tr -d '()')
    DATA09=$(cat $FILE.output | grep 0x00000024 | awk '{print $3}' | sed 's/0d//' | tr -d '()') #'
    if [[ "$DATA06" == "112" && "$DATA07" == "29355" && "$DATA08" == "14305" && "$DATA09" == "0" ]]; then
        echo "Memory values match" > /dev/null
    else
        echo "ERROR: Memory values do not match!"
        if [[ "$DATA06" != "112" ]]; then
            echo "0x00000018: Expected 112, got $DATA06"
        fi
        if [[ "$DATA07" != "29355" ]]; then
            echo "0x0000001c: Expected 29355, got $DATA07"
        fi
        if [[ "$DATA08" != "14305" ]]; then
            echo "0x00000020: Expected 14305, got $DATA08"
        fi
        if [[ "$DATA09" != "0" ]]; then
            echo "0x00000024: Expected 0, got $DATA09"
        fi
    fi
}

# Program 1 with no cache
FILE="../asm/program1file.txt"
echo "#### Program 1 ($FILE)"
../sim -ya $FILE 2>/dev/null | tee $FILE.output | grep "\$\#"
check1

for ISIZE in 128 64; do
    # I/D cache block size = {16, 4, 1}
    for BLOCK in 16 4 1; do
        # D write policy = {WT, WB}
        for DWRITE in 0 1; do
            [[ $ISIZE -eq 128 ]] && DSIZE="256" || DSIZE="1024"
            [[ $DWRITE -eq 0 ]] && WP="thru" || WP="back"
            ../sim -ya -C s -J $ISIZE -E $DSIZE -F $BLOCK -K $BLOCK -H $WP $FILE \
                2>/dev/null | tee $FILE.output | grep "\$\#" | grep -v "\$\# Isize"
            check1
        done
    done
done

rm "$FILE.output"

###############################################################################
# Program 2
function check2()
{
    DATA06=$(cat $FILE.output | grep 0x00000018 | awk '{print $2}')
    DATA07=$(cat $FILE.output | grep 0x0000001c | awk '{print $2}')
    DATA08=$(cat $FILE.output | grep 0x00000020 | awk '{print $2}')
    DATA09=$(cat $FILE.output | grep 0x00000024 | awk '{print $2}') #'
    if [[ "$DATA06" == "0x00000001" && "$DATA07" == "0x20696e71" && "$DATA08" == "0x206e7376" && "$DATA09" == "0x20696e71" ]]; then
        echo "Memory values match" > /dev/null
    else
        echo "ERROR: Memory values do not match!"
        if [[ "$DATA06" != "0x00000001" ]]; then
            echo "0x00000018: Expected 0x00000001, got $DATA06"
        fi
        if [[ "$DATA07" != "0x20696e71" ]]; then
            echo "0x0000001c: Expected 0x20696e71, got $DATA07"
        fi
        if [[ "$DATA08" != "0x206e7376" ]]; then
            echo "0x00000020: Expected 0x206e7376, got $DATA08"
        fi
        if [[ "$DATA09" != "0x20696e71" ]]; then
            echo "0x00000024: Expected 0x20696e71, got $DATA09"
        fi
    fi
}

# Program 2 with no cache
FILE="../asm/program2file.txt"
echo "#### Program 2 ($FILE)"
../sim -ya $FILE 2>/dev/null | tee $FILE.output | grep "\$\#"

for ISIZE in 64 128 256; do
    # I/D cache block size = {16, 4, 1}
    for BLOCK in 16 4 1; do
        # D write policy = {WT, WB}
        for DWRITE in 0 1; do
            [[ $ISIZE -eq 128 ]] && DSIZE="256" || DSIZE="512"
            [[ $ISIZE -eq 256 ]] && DSIZE="128"
            [[ $DWRITE -eq 0 ]] && WP="thru" || WP="back"
            ../sim -ya -C s -J $ISIZE -E $DSIZE -F $BLOCK -K $BLOCK -H $WP $FILE \
                2>/dev/null | tee $FILE.output | grep "\$\#" | grep -v "\$\# Isize"
            check2
        done
    done
done

rm "$FILE.output"

exit 0

#!/bin/bash
# Simple script to look at all the colors supported in the terminal

printf "       "
for i in `seq 0 9`; do
    printf " %3d" $i
done
echo
for i in `seq 0 10 255`; do
    printf "%3d -->" $i
    for j in `seq $i $((i + 9))`; do
        echo -n -e " \033[48;5;${j}m  " $(tput sgr0)
    done
    echo
done

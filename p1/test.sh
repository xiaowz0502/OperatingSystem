#!/bin/bash

for i in {0..39}
do
    touch "disk.in$i"
    for j in {0..499}
    do
        echo $((RANDOM % 1000)) >> "disk.in$i"
    done
done
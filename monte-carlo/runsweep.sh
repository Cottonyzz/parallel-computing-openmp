#!/bin/bash
# CS 475 Project 1 - performance sweep script
# Run from the directory containing proj1.cpp
# Output goes to results.csv

OUTPUT="results.csv"
echo "threads,trials,megatrials_per_sec" > $OUTPUT

THREADS="1 2 4 6 8"
TRIALS="1 10 100 1000 10000 50000 100000"

for t in $THREADS; do
    for n in $TRIALS; do
        g++ -O3 -fopenmp -DNUMT=$t -DNUMTRIALS=$n -DNUMTRIES=30 -DCSV proj1.cpp -o proj1 -lm
        result=$(./proj1 2>&1)
        echo "$result" >> $OUTPUT
        echo "Done: $t threads, $n trials -> $result"
    done
done

echo "All done. Results in $OUTPUT"

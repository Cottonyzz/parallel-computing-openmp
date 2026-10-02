# Parallel Computing with OpenMP

Two C++/OpenMP projects from CS 475 (Introduction to Parallel Programming) at Oregon State University, exploring multi-threaded performance on the OSU Flip HPC cluster.

## kmeans/ — K-Means Clustering on US Cities

Clusters 331 US cities into N "capitals" using iterative K-means, parallelizing the per-city nearest-capital assignment with `#pragma omp parallel for`.

- `proj03.cpp` — main implementation
- `UsCities.data` — dataset of 331 US cities (name, longitude, latitude)
- `Makefile` — basic build
- `runsweep.sh` — sweeps thread count (1, 2, 4, 6, 8) and number of capitals (2–50), used to measure scaling behavior
- `results.txt` — raw sweep output, including the closest real city found to each converged capital centroid (extra credit)

Build and run:
```bash
g++ proj03.cpp -o proj03 -lm -fopenmp
./proj03
```

Or run the full performance sweep:
```bash
chmod +x runsweep.sh
./runsweep.sh
```

## monte-carlo/ — Monte Carlo Cannon Simulation

Estimates the probability that a randomly-fired projectile clears a cliff and lands within tolerance of a target, using a Monte Carlo simulation parallelized with OpenMP (`reduction(+:numHits)` across trials).

- `proj1.cpp` — main implementation
- `runsweep.sh` — sweeps thread count and trial count, writing results to `results.csv`

Build and run:
```bash
g++ -O3 -fopenmp proj1.cpp -o proj1 -lm
./proj1
```

Or run the full performance sweep:
```bash
chmod +x runsweep.sh
./runsweep.sh
```

## Environment

Both projects were developed and benchmarked on OSU's Flip cluster (`flip.engr.oregonstate.edu`), compiled with g++ and OpenMP 4.5 (`_OPENMP 201511`).

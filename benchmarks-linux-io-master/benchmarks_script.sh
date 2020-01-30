#!/bin/bash


./io-benchmark "test" 32 512 0 > resultats.csv
./io-benchmark "test" 128 512 0 >> resultats.csv
./io-benchmark "test" 512 512 0 >> resultats.csv
./io-benchmark "test" 1024 512 0 >> resultats.csv
./io-benchmark "test" 4096 512 0 >> resultats.csv
./io-benchmark "test" 16384 512 0 >> resultats.csv

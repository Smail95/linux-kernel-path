#!/bin/bash


function separate() {
    echo $(sudo perf stat -e "block:block_rq_insert" ./separate $1 $2 $3 $4 2>&1 | sed -E -e 's/(.*(seconds|stats).*)|(block:.*)//g' | sed -E -e 's/(.*, [0-9]+)$/\1,/g' | tr -d ' \n') >> $5
}

SIZE=128 # 64MB
IN=$1
OUT=$2

MODE_RALL_SEC=17 # Strategie: Lire tout le fichier et écritures consécutives

HOLE=0
MAXHOLE=64
MAXITER=2


echo "size,writesize,spacing,strategy,duration_ms,inputs,outputs,req" > $OUT

for ((HOLE=0; HOLE < MAXHOLE; HOLE+=2))
do
    echo "Lancement tests pour HOLE=${HOLE}"
    for ((i=0; i < MAXITER; i++))
    do
	#./separate $IN $SIZE $HOLE 0 >> $OUT
	separate $IN 128 $HOLE 0 $OUT
	echo "Iter ${i}"
    done
done
echo "Fin"

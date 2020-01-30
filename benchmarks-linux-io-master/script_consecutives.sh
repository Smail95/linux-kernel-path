#!/bin/bash

function bench() {
    # Parse le resultat de separate (stdin) et le resultat de perf (sderr) de sorte
    # que les donnees interessantes (sortie du programme + nombre requetes disque)
    # soient ecrites dans un fichier
    # sudo est ici requis pour perf
    echo $(sudo perf stat -e "block:block_rq_insert" ./separate $1 $2 $3 $4 2>&1 | sed -E -e 's/(.*(seconds|stats).*)|(block:.*)//g' | sed -E -e 's/(.*, [0-9]+)$/\1,/g' | tr -d ' \n') >> $5
}


SIZE=67108864 # 64MB
IN=$1
OUT=$2

MODE_RALL_SEC=17 # Strategie: Lire tout le fichier et écritures consécutives

MAXCONSEC=64
CONSEC=1
MAXITER=3

echo "size,writesize,spacing,strategy,duration_ms,inputs,outputs,req" > $OUT

for ((CONSEC=1; CONSEC<MAXCONSEC;CONSEC+=1))
do
    echo "Lancement tests pour CONSEC=${CONSEC}"
    for ((i=0; i < MAXITER; i++))
    do
	#./io-benchmark $IN $SIZE $PAGESIZE $CONSEC $MODE_RALL_SEC >> $OUT
	#bench $IN $SIZE $PAGESIZE $CONSEC $MODE_RALL_SEC $OUT
	bench $IN 128 $CONSEC 1 $OUT
	echo "Iter ${i}"
    done
done
echo "Fin"


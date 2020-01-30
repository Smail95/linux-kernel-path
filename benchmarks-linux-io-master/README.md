# Benchmarks

Programmes permettant de realiser des mesures de temps d'execution et d'entree/sorties pour un programme utilisateur sous un systeme Linux.

## Installation

```
git clone https://gitlab.com/natmaurice/benchmarks-linux-io
```

## separate

Programme permettant de tester les ecritures consecutives ou avec trou pour un fichier donne

### Compilation

`make separate`

### Utilisation

`./separate <fichier entree> <nombre ecritures> <espacement> <strategie>`

* strageie:
** 0: trous variables (= espacement * pages), ecritures fixes
** 1: ecritures variables (= espacement * pages), trous fixes 

## script_consecutives.sh

Lance une batterie de tests sur un fichier specifie en faisant varier la taille des ecritures et avec un trou entre chaque ecriture.

### Utilisation

```
./script_consecutives.sh <fichier entree> <fichier sortie avec resultats>
```

### Dependances

* sed
* tr
* perf

## script_separate.sh

Lance une batterie de tests sur un fichier specifie en faisant varier la taille les trous et en effectuant des ecritures d'exactement une page.

### Dependances

* sed
* tr
* perf

## plot.py
Visualise les donnees d'une batterie de test au format CSV

### Utilisation

```
./plot.py <fichier CSV>
```

Notes: Modifier le script pour selectionner les bonnes colonnes

### Dependances

* python3
* matplotlib (`pip install matplotlib`)

## plot2.py:

Visualise les donnees d'une batterie de test au format CSV

### Dependances

* python3
* seaborn (`pip install seaborn`)
* matplotlib (`pip install matplotlib`)
* panda

### Utilisation

```
./plot2.py <fichier CSV>
```

Notes: Modifier le script pour selectionner les colonnes voulues.

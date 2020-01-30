# Detection

Module de detection de trous dans le page cache pour le noyau linux.

## Installation

```
git clone https://gitlab.com/natmaurice/detection
make
```


### Dependances

Noyau Linux >= 4.20 pour detection-xarray.c

## Detection

detection-xarray: detection des trous a l'aide de l'API xarray

## Utilisation

```
$ insmod detection-xarray.ko pathname=<fichier cible> hole=<taille trou max>
$ <Ecritures dans le fichier >
$ cat /sys/kernel/detection
> dirty <dirty>, clean <clean>, holes <holes>
```

## Fichier de test

### Compilation

```
make test
```

### Utilisation

```
insmod detection-xarray.ko pathname=<fichier cible> hole=<taille trou max>
./test <fichier cible>
cat /sys/kernel/detection
```


#define _GNU_SOURCE

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>



/**
 * Define the maximum amount of writes that can be performed with a file
 * This is useful when debugging the benchmark as it could perform several 
 * thousands of write operation which could damage the disk of the host
 * -1: no limit
 */
#ifndef WRITE_LIMIT
#define WRITE_LIMIT (-1)
#endif

#define BUF_SIZE 512

#define LOAD_ALL (1)
#define WRITE_DIRECT (1 << 1)
#define WRITE_SYNC (1 << 2)
#define WRITE_RANDOM (1 << 3)
#define WRITE_CONSEQ (1 << 4)

#define PAGE_SIZE 4096

void benchmark(int fd, size_t bytecount, size_t writesize, size_t spacing, int method);


/**
 * Arguments: <file> <byte count> <write size> <spacing> <method>
 * @method: Methode a utiliser:
 * > 0 : rien
 * > 1 : lire le fichier en entier
 * > 2 : ecriture avec O_DIRECT, ecriture directement sur le disque
 * > 4 : ecriture avec O_SYNC
 * > 8 : ecriture au hasard
 * Les methodes peuvent etre combines par un OU-binaire: 
 * > ex: 7 : lire fichier en entier +  O_DIRECT + O_SYNC
 */
int main(int argc, char* argv[])
{
	if (argc < 6)
	{
		printf("Not enough arguments\n");
		printf("Usage: <file> <byte count> <write size> <spacing> <method>\n");
		return EXIT_FAILURE;
	}

       
	
	char* path = argv[1];
	
	char* pEnd;
	long bytecount = strtol(argv[2], &pEnd, 10);
	long writesize = strtol(argv[3], &pEnd, 10);
	long spacing = strtol(argv[4], &pEnd, 10);
	long method = strtol(argv[5], &pEnd, 10);;
	

	long flags = O_CREAT | O_RDWR;
	if ((method & WRITE_DIRECT) == WRITE_DIRECT) {
		//printf("Direct write\n");
		flags |= O_DIRECT;
	}
	if ((method & WRITE_SYNC) == WRITE_SYNC) {
		//printf("Sync write\n");
		flags |= O_SYNC;
	}
	
	if ((method & WRITE_RANDOM) == WRITE_RANDOM) {
		struct timeval now;
		//printf("Random write\n");
		gettimeofday(&now, NULL);
		srand(now.tv_usec);
	}

	
	/* Ouvre le fichier specifié
	 * mesure les temps d'accès au cache et non au disque
	 */
	int fd = open(path, flags, 0700);
	if (fd == -1)
	{
		printf("Couldn't open file '%s'\n", path);
		perror("Error: ");
		return EXIT_FAILURE;
	}

	//benchmark(fd, bytecount, writesize, spacing);
	benchmark(fd, bytecount, writesize, spacing, method);
	
	close(fd);

	return EXIT_SUCCESS;
}


void read_all(int fd)
{
	char buf[BUF_SIZE];
	while (read(fd, buf, BUF_SIZE) > 0);
}

void write_fragments(int fd, size_t bytecount, size_t writesize, size_t spacing)
{
        size_t datawritten = 0;
	char* buf = (char*)malloc(sizeof(writesize));
	for (datawritten = 0; datawritten < bytecount; datawritten += writesize) {
		write(fd, buf, writesize);
		off_t offset = lseek(fd, 0, SEEK_CUR);
		lseek(fd, spacing, SEEK_CUR);
		offset = lseek(fd, 0, SEEK_CUR);
	}
	
	free(buf);
}



void write_separate(int fd, size_t pagecount, size_t holesize)
{
	size_t pagesread = 0;
	char buf;
	off_t pos;
	//printf("%ld, %ld\n", pagecount, holesize);
	const size_t offset = holesize * PAGE_SIZE + PAGE_SIZE - 1;
	for (pagesread = 0; pagesread < pagecount; pagesread++) {
		write(fd, (void*)&buf, 1);
	        pos =  lseek(fd, offset, SEEK_CUR);
		assert(pos % PAGE_SIZE == 0);
	}
}

void write_separate_limit(int fd, size_t pagecount, size_t holesize, int flushrate)
{
	size_t count = 0;
	for (count = 0; count + flushrate < pagecount; count += flushrate) {
		write_separate(fd, flushrate, holesize);
		fsync(fd);
	}

	size_t remaining = pagecount % flushrate;
	if (remaining > 0)
		write_separate(fd, flushrate, holesize);
}

void write_random(int fd, size_t bytecount, size_t writesize)
{
	size_t datawritten = 0;
	char* buf = (char*)malloc(sizeof(writesize));
	const long fileend = lseek(fd, 0, SEEK_END) / PAGE_SIZE;
	
	for (datawritten = 0; datawritten < bytecount; datawritten += writesize) {
		long offset = (rand()%fileend) * PAGE_SIZE; /* valeur alignee sur une page */
		lseek(fd, offset, SEEK_SET);
		write(fd, (void*)buf, writesize);
	}
}


void write_consecutive(int fd, size_t bytecount, size_t writesize, size_t consecutive)
{
	size_t datawritten = 0;	
	char* buf = (char*)malloc(sizeof(writesize));
	while (datawritten < bytecount) {
		size_t consec = 0;
		off_t offset = lseek(fd, 0, SEEK_CUR);
		for (consec = 0; consec < consecutive && datawritten < bytecount; consec += writesize) {
			write(fd, (void*)buf, writesize);
			datawritten += writesize;
		}
		lseek(fd, writesize, SEEK_CUR);
		offset = lseek(fd, 0, SEEK_CUR);
		off_t data = offset+2;
	}
}

void benchmark(int fd, size_t bytecount, size_t writesize, size_t spacing, int method)
{
	struct timeval start, end;

	/* rusage = statistiques sur les resources utilisées (e.g: E/S, memoire, ...) 
	 * Utilise ici pour mesure le nombre d'entrees/sorties
	 */
	struct rusage rustart, ruend;
	if (getrusage(RUSAGE_SELF, &rustart) < 0) {
	    perror("getrusage: ");
	    return;
	}

	
	/* On lis tout pour tout mettre dans le cache */
	if ((method & LOAD_ALL) == LOAD_ALL) {
		//printf("Reading all\n");
		read_all(fd);
	}

	/* Replace le curseur au début du fichier */
	lseek(fd, 0, SEEK_SET);
	/* Lancement du benchmark */
	gettimeofday(&start, NULL);

	if ((method & WRITE_RANDOM) == WRITE_RANDOM)
		write_random(fd, bytecount, writesize);
	else if ((method & WRITE_CONSEQ) == WRITE_CONSEQ)
		write_consecutive(fd, bytecount, writesize, spacing);
	else 
	    write_separate_limit(fd, bytecount, spacing, 64);
	
	fsync(fd); /* Force l'écriture sur le disque */
	
	gettimeofday(&end, NULL);

	/* Calcul et affichage des mesures */

	long elapsedms = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;
	getrusage(RUSAGE_SELF, &ruend);

	long inblock = ruend.ru_inblock - rustart.ru_oublock;
	long outblock = ruend.ru_oublock - rustart.ru_oublock;
	
	printf("%lu, %lu, %lu, %d, %ld, %ld, %ld\n", bytecount, writesize, spacing, method, elapsedms, inblock, outblock);

}

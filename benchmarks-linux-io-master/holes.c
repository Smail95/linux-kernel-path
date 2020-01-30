#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PAGE_SIZE 4096

size_t write_separate(int fd, size_t writecount, size_t spacing)
{
	size_t written = 0;
	char byte;
	off_t offset = 0; //PAGE_SIZE * 128;
	lseek(fd, offset, SEEK_SET);
	for (written = 0; written < writecount; written++) {
		write(fd, (void*)&byte, 1);
		lseek(fd, PAGE_SIZE - 1 + PAGE_SIZE * spacing, SEEK_CUR);
	}
	fsync(fd);
	assert(written == writecount);
	return written;
}

size_t write_cont(int fd, size_t writecount, size_t spacing)
{
	size_t written = 0;
	char byte;
	lseek(fd, 0, SEEK_SET);
	for (written = 0; written < writecount; written++) {
		write(fd, (void*)&byte, 1);
		lseek(fd, PAGE_SIZE - 1, SEEK_CUR);
		if (spacing > 0 && written % spacing == 0) {
			/* Skip a page */
			lseek(fd, PAGE_SIZE, SEEK_CUR);
		}
	}
	fsync(fd);
	assert(written == writecount);
	return written;

}

void write_page(int fd) {
	char buf;
	write(fd, (void*)&buf, 1);
	lseek(fd, PAGE_SIZE-1, SEEK_CUR);
}

void write_succ(int fd, int conseq, int ntimes)
{
	int i, cons;
	for (i = 0; i < ntimes; i++) {
		for (cons = 0; cons < 2 + conseq; cons++) {
			write_page(fd);
		}
		fsync(fd);
		lseek(fd, 0, SEEK_SET);
	}
}

size_t write_sep(int fd, int holesize, int ntimes)
{
	int i;
	off_t offset = 0;
	for (i = 0; i < ntimes; i++) {
		write_page(fd);
		if (holesize > 0)
			offset = lseek(fd, holesize * PAGE_SIZE, SEEK_CUR);
		write_page(fd);

		fsync(fd);
		lseek(fd, 0, SEEK_SET);
	}
	assert(i == ntimes);
	return i;
}

void write_two_sep(int fd, int ntimes)
{
	int i;
	char buf;
	for (i = 0; i < ntimes; i++) {
		write(fd, (void*)&buf, 1);
		lseek(fd, PAGE_SIZE + PAGE_SIZE - 1, SEEK_CUR);
		write(fd, (void*)&buf, 1);
		lseek(fd, PAGE_SIZE - 1, SEEK_CUR);		
	}
}


void read_all(int fd)
{
	char buf[PAGE_SIZE];
	while (read(fd, (void*)&buf, PAGE_SIZE) > 0);
}



void benchmark_lseek(int fd, size_t writecount, size_t spacing)
{
	size_t written = 0;
	char byte = 0;
	lseek(fd, 0, SEEK_SET);
	for (written = 0; written < writecount; written++) {
		//write(fd, (void*)&byte, 1);
		lseek(fd, PAGE_SIZE + PAGE_SIZE * spacing, SEEK_CUR);
		//size_t offset = written * (spacing * (PAGE_SIZE + 1));
		//pwrite(fd, (void*)&byte, 1, offset);
	}
}

void benchmark(const char* filename, size_t writecount, size_t spacing, long strategy)
{
	struct timeval start, end;
	int fd = open(filename, O_RDWR);
	
	if (fd < 0) {
		perror("Couldn't open file: ");
		exit(-1);
	}

	read_all(fd);
	lseek(fd, SEEK_SET, 0);
	
	gettimeofday(&start, NULL);
	//write_separate(fd, writecount, spacing);
        //benchmark_lseek(fd, writecount, spacing);
	//fsync(fd);
	if (strategy == 0)
		writecount = write_separate(fd, writecount, spacing);
	else
		write_cont(fd, writecount, spacing);
	
	gettimeofday(&end, NULL);


	long elapsedms = (end.tv_sec - start.tv_sec)*1000
		+ (end.tv_usec - start.tv_usec)/1000;

	/* Output line as CSV:
	 * <page written>, <size of consecutive reads>, <hole size>, <strategy used>, <time elapsed ms>, <blocks read>, <blocks written> 
	 */
	printf("%lu, 1, %lu, 1, %ld, 0, 0\n",
	       writecount, spacing, elapsedms);

	close(fd);
}

int main(int argc, char* argv[])
{
	size_t writecount, spacing;
	long strategy;
	char* pEnd;

	if (argc < 3) {
		printf("Argc < 3\n");
		return -1;
	}		
	
	writecount = strtoul(argv[2], &pEnd, 10);
	spacing = strtoul(argv[3], &pEnd, 10);
	strategy = strtol(argv[4], &pEnd, 10);
	
	benchmark(argv[1], writecount, spacing, strategy);
	
	return 0;
}

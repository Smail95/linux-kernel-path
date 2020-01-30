#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define PAGE_SIZE 4096

int main(int argc, char* argv[])
{
	char* path = NULL;
	int fd;
	char *buf;

	srand(time(NULL));
	
	if (argc < 1) {
		printf("Argc < 1\n");
		return -1;
	}
	path = argv[1];
	
	fd = open(path, O_RDWR);
	if (fd < 0) {
		perror("Couldn't open file: ");
		return -1;
	}

	buf = (char*)malloc(sizeof(char) * PAGE_SIZE);

	/* Read file twice to ensure that it is inside the page cache */
	while (read(fd, buf, PAGE_SIZE) > 0);
	while (read(fd, buf, PAGE_SIZE) > 0);

	/* Write a byte at the beginning of the file */
	lseek(fd, 0, SEEK_SET);	
	buf[0] = (char)rand()%256;
	write(fd, buf, 1);

	lseek(fd, 2 * PAGE_SIZE, SEEK_SET);
	write(fd, buf, 1);

	lseek(fd, 5 * PAGE_SIZE, SEEK_SET);
	write(fd, buf, 1);

	lseek(fd, 9 * PAGE_SIZE, SEEK_SET);
	write(fd, buf, 1);
	
	lseek(fd, 0, SEEK_END);
	write(fd, buf, 4096);
	write(fd, buf, 4096);
        

	free(buf);
	close(fd);

	return 0;
}

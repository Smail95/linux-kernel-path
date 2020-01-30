/* @author : AIDER Smail
 * Created on Sat Feb 23 11:49:46 2019
 */
#define _GNU_SOURCE
#include "util.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NORMAL 		0
#define SYNCHRONOUS 1
#define DIRECT  	2

int main(int argc, char *argv[])
{
	int fd, ret, n;	
	int io_sync;		/* Synchronous I/O  */
	int io_page_offset;	/* Offset to take(expressed on number of page) between two write */
	int	io_cache_full; 	/* Enable/Disable the buffer cache */	
	char *path;			/* Path of input file */
	
	if(argc > 5){	
		for(n = 1; n < argc; n += 2){
			if( (strcmp(argv[n], "-SYNC") == 0) && (n+1 < argc) ){
				io_sync = atoi(argv[n+1]);
			}else if( (strcmp(argv[n], "-OFFSET") == 0) && (n+1 < argc) ){
				io_page_offset = atoi(argv[n+1]);			
			}else if ( (strcmp(argv[n],"-PATH") == 0) && (n+1 < argc) ){
				if( (path = (char *)malloc(strlen(argv[n+1])+1)) != NULL){
					strcpy(path, argv[n+1]);		
				}else{
					perror("malloc");
					exit(-1);				
				}	
			}else if ( (strcmp(argv[n],"-CACHE_FULL") == 0) && (n+1 < argc) ){
				io_cache_full = atoi(argv[n+1]);
			}
		}
	}else{
		printf("Arguments on the command line are (key, value) couples.\n");
		printf("The order is not important.\n");
		printf("Accepted arguments are: \n");
		printf("-SYNC type of I/O, \n 0: Not Synchronous,\n 1: Synchronous,\n 2: Make effort to be Synchronous\n");
		printf("-OFFSET offset to take between two write(OUTPUT),\n 0: No Offset,\n 1: Random Offset( >1 pages)\n");
		printf("-CACHE_FULL work with Full/Empty Buffer Cache, \n 0: Empty,\n 1: Full\n");		
		printf("-PATH input file path to read (used to fill buffer cache)\n"); 														 
		exit(-1);
	}	
	
	
	/* Open Input file */
	if((fd = open(path, O_RDWR, 0666)) == -1){
		perror("open_main");		
		goto ERROR;	
	}
	
	switch(io_sync){
		case NORMAL: 
			ret = write_normal(fd, io_page_offset, io_cache_full);
			break;
		case SYNCHRONOUS: 
			ret = write_sync(io_page_offset);
			break;
	}/* switch */	
	
	if(ret == -1){
		printf("Error: <- main function -> \n");	
		goto ERROR;
	}
	
	return 0;
ERROR:
	free(path);
	exit(-1);
}/*main*/

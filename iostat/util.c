/* @author : AIDER Smail
 * Created on Sat Feb 23 11:49:46 2019
 */
#define _GNU_SOURCE
#define UTIL_C_
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* Synchronous I/O */
int write_sync(int page_offset)
{
	int ret, fd;
	char path[100];
	char strpid[10];
	
	/* path = "OUTPUT_PATH/PID.txt" : unique */ 
	memset(path,'\0' , 100);
	sprintf(strpid, "%d", getpid());
	sprintf(path,"%s/%s%s",OUTPUT_FILE, strpid, ".txt");
	
	/* Open Output file -> O_SYNC */ 
	if((fd = open(path,  O_CREAT|O_RDWR|O_SYNC)) == -1){
		return -1;	
	}
	ret = io_write(fd, page_offset);
	
	close(fd);
	return ret;
}


/* Normal I/O, With BUFFER CACHE */
int write_normal(int fd, int io_page_offset, int io_cache_full)
{
	int ret, fd2;
	
	char path[100];
	char strpid[10];
	
	/* path = "OUTPUT_PATH/PID.txt" : unique */ 
	memset(path,'\0' , strlen(path));
	sprintf(strpid, "%d", getpid());
	sprintf(path,"%s/%s%s",OUTPUT_FILE, strpid, ".txt");
		
	/* Fill the Buffer Cache */
	if(io_cache_full)
		ret = fill_buffer_cache(fd);	
	/* Empty the Buffer Cache */	
	else
		ret = empty_buffer_cache();	
	if(ret)
		return -1;	
	
	sleep(2);
	/* Open Output file */ 
	if((fd2 = open(path, O_RDWR|O_CREAT, 0666)) == -1){
		perror("open_write_normal");
		exit(-1);
	}
	ret = io_write(fd2, io_page_offset);

	close(fd2);
	return ret;
}


/* Write 'OUTPUT_SIZE' bytes on the hard disk  */
int io_write(int fd, int page_offset)
{
	struct thread_info * info_th1;
	int pid_stat;
	int pid_io;

	info_th1 = (struct thread_info*)malloc(sizeof(struct thread_info));	
	if(info_th1 == NULL){
		perror("malloc thread_info");
		exit(-1);	
	}	
	/* create process to start I/O on hard disk */
	info_th1->fd = fd;
	info_th1->offset = page_offset;

	if((pid_stat = fork()) == 0){
		execl(IOSTAT_PATH,"iostat","1","-y","-p",DISK_DEVICE,"-k","-x", "-o","JSON",NULL);
		perror("execl");
		exit(-1);			
	}	
	sleep(3);
	if( (pid_io = fork()) == 0){
		io_start(info_th1);
		exit(-1);
	}
	
	waitpid(pid_io, NULL, 0);
	sleep(3);
	kill(pid_stat, SIGINT);	
	return 0;
}


/* Start I/O */
void io_start(struct thread_info *tinfo)
{
	char *buffer;
	long double count;
	int fd = tinfo->fd;	
	int page_offset = tinfo->offset;
	int offset, towrite;
	
	towrite = PAGE_SIZE * TO_WRITE;
	if((buffer = (char*)malloc(towrite)) == NULL){
		perror("malloc");	
		exit(-1);
	}	
	/* Start I/O */
	count = OUTPUT_SIZE;
	while(count > 0){		
		*buffer = 'x';
		if( write(fd, buffer, towrite) == -1){
			perror("write");		
			goto END;	
		}
		if(page_offset){
			offset = OFFSET * PAGE_SIZE;
			if( lseek(fd, offset, SEEK_CUR) == -1){
				perror("lseek");
				goto END;			
			}	
		}
		count -= towrite;	
	}	
	fsync(fd);

	free(buffer);
	return;

END: 
	free(buffer);
	exit(-1);
}

/* Fill Buffer Cache */
int fill_buffer_cache(int fd)
{
	int ret;
	char *buffer;
	
	if((buffer = (char*)malloc(PAGE_SIZE)) == NULL){
		perror("malloc");
		exit(-1);
	}
	while((ret = read(fd, buffer, PAGE_SIZE)) > 0){
		*buffer = 'x';	
	}
	if(ret == -1){
		perror("read");	
		goto END;
	}
	free(buffer);
	return 0;
END:
	free(buffer);
	exit(-1);	
}

/* Empty Buffer Cache */
int empty_buffer_cache()
{
	sync();
	return 0;	
}

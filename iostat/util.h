#ifndef	UTIL_H_
#define UTIL_H_

#define PAGE_SIZE 		4096					 /* PAGE Size */
#define OUTPUT_SIZE		2147483648				 /* Amount bytes to write - MUST BE MULTIPLE OF PAGE_SIZE !!! */
#define OUTPUT_FILE 	"/home/aider/Desktop"  				 /* Output file path */
#define OFFSET 			1						 /* offset to take at a time   */
#define TO_WRITE		1					 /* number of PAGE to write at a time */
#define LENGTH			(OUTPUT_SIZE/PAGE_SIZE)  /* OUTPUT_SIZE / PAGE_SIZE */
#define IOSTAT_PATH  	"/usr/local/bin/iostat"	 /* iostat bin path, can be found by: $ which iostat */
#define DISK_DEVICE 	"sda"					 /* device type to record */


struct statistic {
	int length;						/* size of times[] & sizes[]  */
	int index;						/* index */
	long double  *times;	/* duration (seconds)*/
	unsigned int *data;				
};

struct thread_info {
	int fd;
	int offset;
};

int write_sync(int page_offset);
int io_write(int fd, int page_offset);
int write_normal(int fd, int io_page_offset, int io_cache_full);
int fill_buffer_cache(int fd);
int empty_buffer_cache();
void io_start(struct thread_info *tinfo);
void io_stat(int fd, struct statistic *stats);


#endif

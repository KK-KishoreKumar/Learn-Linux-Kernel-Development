/*
 * ch12/miscdrv/rdwr_test.c
 ***************************************************************
 * This program is part of the source code released for the book
 *  "Learn Linux Kernel Development"
 *  (c) Author: Kaiwan N Billimoria
 *  Publisher:  Packt
 *  GitHub repository:
 *  https://github.com/PacktPublishing/Learn-Linux-Kernel-Development
 *
 * From: Ch 12 : Writing a Simple Misc Device Driver
 ****************************************************************
 * Brief Description:
 * USER SPACE app : a generic read-write test bed for demo drivers.
 * THis simple user mode app allows you to issue a read or a write
 * to a specified device file.
 *
 * For details, please refer the book, Ch 12.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define READ_OPT	0
#define WRITE_OPT	1

static int stay_alive = 0;

static inline void usage(char *prg)
{
	fprintf(stderr,"Usage: %s opt=read/write device_file num_bytes_to_read_or_write\n"
			" opt = '0' => we shall issue the read(2)\n"
			" opt = '1' => we shall issue the write(2)\n",
			prg);
}

int main(int argc, char **argv)
{
	int fd, opt = READ_OPT, flags = O_RDONLY;
	ssize_t n;
	char *buf = NULL;
	size_t num = 0;
	
	if( argc != 4 ) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	opt = atoi(argv[1]);
	if (opt != 0 && opt != 1) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (opt == WRITE_OPT)
		flags = O_WRONLY;

	if( (fd=open(argv[2], flags, 0)) == -1)
		perror("open"),exit(1);
	printf("Device file \"%s\" opened (in %s mode): fd=%d\n",
		       argv[2], (flags == O_RDONLY ? "read-only" : "write-only"), fd);

	num = atoi(argv[3]);
	if ((num < 0) || (num > INT_MAX)) {
		fprintf(stderr,"%s: number of bytes '%ld' invalid.\n", argv[0], num);
		close(fd);
		exit(EXIT_FAILURE);
	}

	buf = malloc(num);
	if (!buf) {
		fprintf(stderr,"%s: out of memory!\n", argv[0]);
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (opt == READ_OPT) {			// test reading..
		n = read(fd, buf, num);
		if( n < 0 ) {
			perror("read failed");
			fprintf(stderr, "Tip: see kernel log\n");
			free(buf); close(fd);
			exit(EXIT_FAILURE);
		}
		printf("%s: read %ld bytes from %s\n", argv[0], n, argv[2]);
		printf(" Data read:\n\"%.*s\"\n", (int)n, buf);
	} else {						// test writing ..
		n = write(fd, buf, num);
		if( n < 0 ) {
			perror("write failed");
			fprintf(stderr, "Tip: see kernel log\n");
			free(buf); close(fd);
			exit(EXIT_FAILURE);
		}
		printf("%s: wrote %ld bytes to %s\n", argv[0], n, argv[2]);
	}

	if (1 == stay_alive) {
		printf("%s:%d: stayin' alive (in pause()) ... \n", argv[0], getpid());
		pause();	/* block until a signal is received */
	}

	free(buf);
	close(fd);
	exit(EXIT_SUCCESS);       
}

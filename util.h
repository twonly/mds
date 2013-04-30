/*
 * =====================================================================================
 *
 *       Filename:  util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2013 10:24:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 5  /*指定套接字可以接受的最大未接受客户机请求的数目*/
#define RIO_BUFSIZE 1024
#define MAXLEN 1024
typedef struct sockaddr SA;
typedef struct {
    int rio_fd;
    int rio_cnt;
    char* rio_bufptr;
    char rio_buf[RIO_BUFSIZE];
} rio_t;


int open_clientfd( char *hostname, int port );
int open_listenfd( int port );

//robust read and write
ssize_t rio_readn( int fd, void *usrbuf, size_t n );
ssize_t rio_writen( int fd, void *usrbuf, size_t n );
void rio_readinitb(rio_t *rp, int fd);
//static ssize_t rio_read(rio_t *rp, char* usrbuf, size_t n);
//size_t rio_readlineb( rio_t *rp, void* usrbuf, size_t maxlen );
//ssize_t rio_readnb( rio_t* rp, void* usrbuf, size_t n );

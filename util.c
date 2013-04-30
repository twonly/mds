/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/28/2013 10:25:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "util.h"

int open_clientfd( char *hostname, int port ) {
    int clientfd;
    struct hostent * hp;
    struct sockaddr_in servaddr;
    if( (clientfd=socket(AF_INET, SOCK_STREAM, 0))<0 )
        return -1;
    if ((hp=gethostbyname(hostname))==NULL) 
        return -2;
    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port = htons(port);
    bcopy((char*)hp->h_addr_list[0],
            (char*)&servaddr.sin_addr.s_addr, hp->h_length);
    if( connect(clientfd, (SA*)&servaddr, sizeof(servaddr))<0 )
        return -1;
    return clientfd;
}

int open_listenfd( int port ) {
    int listenfd, optval = 1;
    struct sockaddr_in servaddr;
    if((listenfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        return -1;
    if( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval,
                sizeof(int))<0 )
        return -1;
    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((unsigned short)port);
    if( bind(listenfd, (SA*)&servaddr, sizeof(servaddr)) < 0 )
        return -1;
    if( listen(listenfd, BACKLOG) <0 )
        return -1;
    return listenfd;
}

ssize_t rio_readn( int fd, void *usrbuf, size_t n ) {
    size_t nleft = n;
    size_t nread;
    char *buf = usrbuf;
    while( nleft>0 ) {
        if( (nread=read(fd, buf, nleft)) <0 ) {
            if(errno==EINTR)
                nread=0;
            else
                return -1; //errno set by read()
        }
        else if( nread==0 ) //eof
            break;
        nleft -= nread;
        buf += nread;
    }
    return n-nleft;
}

ssize_t rio_writen( int fd, void *usrbuf, size_t n ) {
    size_t nleft = n;
    ssize_t nwritten;
    char* buf = usrbuf;
    while( nleft>0 ) {
        if( (nwritten=write(fd, buf, nleft)) <0 ){
            if( errno==EINTR )
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return n;
}

void rio_readinitb( rio_t *rp, int fd ) {
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read( rio_t *rp, char* usrbuf, size_t n ) {
    int cnt;
    while( rp->rio_cnt<=0 ) {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if( rp->rio_cnt<0 ) {
            if( errno!= EINTR )
                return -1;
        }
        else if (rp->rio_cnt==0)
            return 0;
        else
            rp->rio_bufptr = rp->rio_buf;
    }
    cnt = n;
    if( rp->rio_cnt<n )
        cnt = rp->rio_cnt;
    memcpy( usrbuf, rp->rio_bufptr, cnt );
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

size_t rio_readlineb( rio_t* rp, void* usrbuf, size_t maxlen ) {
    int n, rc;
    char c, *bufp = usrbuf;
    for( n=1; n<maxlen; ++n ) {
        if( (rc=rio_read(rp, &c, 1)) == 1 ) {
            *bufp++ = c;
            if( c=='\n' )
                break;
        } else if( rc==0 ) {
            if( n==1 )
                return 0; //no data read, EOF
            else 
                break; //EOF
        } else
            return -1; //error
    }
    *bufp = 0; //NULL
    return n;
}

size_t rio_readnb( rio_t* rp, void* usrbuf, size_t n ) {
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    while( nleft>0 ) {
        if( (nread=rio_read(rp, bufp, nleft)) < 0 ) {
            if(errno==EINTR)
                nread = 0;
            else
                return -1;
        } else if( nread==0 ) 
            break;
        nleft -= nread;
        bufp += nread;
    }
    return n-nleft;
}

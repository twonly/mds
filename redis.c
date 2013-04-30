/*
 * =====================================================================================
 *
 *       Filename:  redis.c
 *
 *    Description:  test hiredis api
 *
 *        Version:  1.0
 *        Created:  04/23/2013 11:12:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <hiredis/hiredis.h>

int main(void) {
    redisContext *conn = redisConnect("127.0.0.1", 6379);
    if(conn->err) {
        printf("Connection error %s\n", conn->errstr);
    }
    redisReply * reply = (redisReply*)redisCommand(conn, "set key value");
    freeReplyObject(reply);
    redisFree(conn);

    return 0;
}


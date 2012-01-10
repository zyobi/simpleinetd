/*
 * =====================================================================================
 *
 *       Filename:  cfg.c
 *
 *    Description:  配置文件解析实现
 *
 *        Version:  1.0
 *        Created:  2012年01月06日 00时17分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robin (), zouyongbin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include    <sys/types.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "cfg.h"

//static Config configs[] = {
//    { "cat", "1990", SOCK_STREAM, -1 }, 
//    { "date", "1991", SOCK_STREAM, -1 }, 
//    { "nl", "1992", SOCK_STREAM, -1 }, 
//    { NULL, NULL, 0, -1 }
//};

const char* get_cmd(Config *c, int fd)
{
    for(; c->cmd != NULL; c++)
    {
        if(c->listenfd == fd)
            return c->cmd;
    }
    return NULL;
}

void print_config(Config *c)
{
    for(; c->cmd != NULL; c++)
    {
        printf("-----------------\n");
        printf("cmd = %s\n", c->cmd);
        printf("port = %s\n", c->port);
        printf("socktype = %d\n", c->socktype);
        printf("-----------------\n\n");
    }
}



/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  简单超级服务器的配置文件
 *
 *        Version:  1.0
 *        Created:  2011年12月28日 21时44分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robin (), zouyongbin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef  CONFIG_INC
#define  CONFIG_INC

typedef struct
{
    char *cmd;
    char *port;
    int socktype; // 和socket的type参数一致
    int listenfd;
} Config;

extern Config* parse_config(const char *path);

const char* get_cmd(Config *c, int fd);


void print_config(Config *c);
#endif   /* ----- #ifndef CONFIG_INC  ----- */

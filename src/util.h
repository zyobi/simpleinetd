/*
 * =====================================================================================
 *
 *       Filename:  unp.h
 *
 *    Description:  工具函数头文件
 *
 *        Version:  1.0
 *        Created:  2011年11月20日 21时38分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef UTIL_H
#define UTIL_H

#include    <sys/socket.h>
#include    <sys/select.h>
#include    <sys/resource.h>

#define BUFF_SIZE 1024
#define SERV_PORT 9999
#define MAX_HOST 40
#define MAX_PORT 20
#define MAX_EVENTS 10

#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif

typedef struct sockaddr SA;
typedef void (*sighandler_t)(int);

void err_msg(const char *format, ...);
int Socket(int family, int type, int protocol);
int Bind(int sockfd, const SA *myaddr, socklen_t addrlen);
int Connect(int sockfd, const SA *servaddr, socklen_t addrlen);
int Listen(int sockfd);
int Accept(int sockfd, SA *cliaddr, socklen_t *addrlen);
int Close(int sockfd);
ssize_t Write(int sockfd, const void *buff, size_t count);
ssize_t Read(int sockfd, void *buff, size_t count);
int Getpeername(int sockfd, SA *addr, socklen_t *addrlen);
char * sock_ntop(const SA *sa, socklen_t salen);
sighandler_t Signal(int signum, sighandler_t handler);
int Select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int Shutdown(int sockfd, int howto);
int Getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int Setsockopt(int sockfd, int level, int optname, const void*optval, socklen_t optlen);
ssize_t Recvfrom(int sockfd, void *buff, size_t nbytes, int flags, SA *from, socklen_t *addrlen);
ssize_t Sendto(int sockfd, void *buff, size_t nbytes, int flags, const SA *to, socklen_t addrlen);
struct addrinfo* host_serv(const char *hostname, const char *service, int family, int sock_type);
int tcp_connect(const char *hostname, const char *service);
pid_t Setsid(void);
pid_t Fork(void);
int Open(const char *pathname, int flags);
int Getrlimit(int resource, struct rlimit *rlim);
void daemonize(const char *appname);
int check_pidfile(char *pidfile);
int create_pidfile(char *pidfile);

#endif


/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  工具函数实现
 *
 *        Version:  1.0
 *        Created:  2012年01月06日 00时19分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robin (), zouyongbin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include    "util.h"
#include    <unistd.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <signal.h>
#include    <sys/wait.h>
#include    <limits.h>
#include    <netdb.h>
#include    <fcntl.h>
#include    <syslog.h>
#include    <sys/stat.h>
#include    <stdarg.h>
#include    <errno.h>



extern int errno;
int daemon_proc = 0;

int Socket(int family, int type, int protocol)
{
    int fd;
    if((fd = socket(family, type, protocol)) == -1)
    {
        err_msg("error opening socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return fd;
}

int Bind(int sockfd, const SA *myaddr, socklen_t addrlen)
{
    int ret;
    if((ret = bind(sockfd, myaddr, addrlen)) == -1)
    {
        err_msg("error binding socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Connect(int sockfd, const SA *servaddr, socklen_t addrlen)
{
    int ret;
    if((ret = connect(sockfd, servaddr, addrlen)) == -1)
    {
        err_msg("error connecting socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Listen(int sockfd)
{
    int ret;
    if((ret = listen(sockfd, 5)) == -1)
    {
        err_msg("error listening socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Accept(int sockfd, SA *cliaddr, socklen_t *addrlen)
{
    int clifd;
    if((clifd = accept(sockfd, cliaddr, addrlen)) == -1)
    {
        err_msg("error accepting client: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return clifd;
}

int Close(int sockfd)
{
    int ret;
    if((ret = close(sockfd)) == -1)
    {
        err_msg("error closing socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

ssize_t Write(int sockfd, const void *buff, size_t count)
{
    ssize_t n;
    if((n = write(sockfd, buff, count)) == -1)
    {
        err_msg("error writing socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return n;
}

ssize_t Read(int sockfd, void *buff, size_t count)
{
    ssize_t n;
    if((n = read(sockfd, buff, count)) == -1)
    {
        err_msg("error reading from socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return n;
}

int Getpeername(int sockfd, SA *addr, socklen_t *addrlen)
{
    int ret;
    if((ret = getpeername(sockfd, addr, addrlen)) != 0)
    {
        err_msg("error getpeername: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
} 

char * sock_ntop(const SA *sa, socklen_t salen)
{
    char str_port[8];
    static char str[128];
    switch(sa->sa_family)
    {
        case AF_INET: 
            {
                struct sockaddr_in *sin = (struct sockaddr_in *)sa;
                if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                {
                    err_msg("error ntop: %s", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                snprintf(str_port, sizeof(str), ":%d", sin->sin_port);
                strcat(str, str_port);
                break;
            }
    }

    return str;
}


sighandler_t Signal(int signum, sighandler_t handler)
{
    struct sigaction action, old;
    int ret;
    bzero(&action, sizeof(action));
    action.sa_handler = handler;
    //action.sa_flags |= SA_RESTART;

    ret = sigaction(signum, &action, &old);
    if(ret == -1)
    {
        err_msg("error while settign signal handler: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return old.sa_handler;
}

int Select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, 
        struct timeval *timeout)
{
    int ret;
    ret = select(maxfdp1, readset, writeset, exceptset, timeout);
    if(ret == -1)
    {
        err_msg("error while selecting: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}

int Shutdown(int sockfd, int howto)
{
    int ret;
    ret = shutdown(sockfd, howto);
    if(ret == -1)
    {
        err_msg("error while shutdown: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}

int Getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    int ret;
    ret = getsockopt(sockfd, level, optname, optval, optlen);
    if(ret == -1)
    {
        err_msg("error getsockopt: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}

int Setsockopt(int sockfd, int level, int optname, const void*optval, socklen_t optlen)
{
    int ret;
    ret = setsockopt(sockfd, level, optname, optval, optlen);
    if(ret == -1)
    {
        err_msg("error setsockopt: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}


ssize_t Recvfrom(int sockfd, void *buff, size_t nbytes, int flags, SA *from, socklen_t *addrlen)
{
    int ret;
    ret = recvfrom(sockfd, buff, nbytes, flags, from, addrlen);
    if(ret == -1)
    {
        err_msg("error recvfrom: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}


ssize_t Sendto(int sockfd, void *buff, size_t nbytes, int flags, const SA *to, socklen_t addrlen)
{
    int ret;
    ret = sendto(sockfd, buff, nbytes, flags, to, addrlen);
    if(ret == -1)
    {
        err_msg("error sendto: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
        return ret;
}

struct addrinfo* host_serv(const char *hostname, const char *service, int family, int sock_type)
{
    struct addrinfo hints, *result;
    int error;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype = sock_type;
    if((error = getaddrinfo(hostname, service, &hints, &result)) != 0)
    {
        err_msg("error getaddrinfo: %s", strerror(errno));
        return NULL;
    }

    return result;
}

int tcp_connect(const char *hostname, const char *service)
{
    int sockfd;
    struct addrinfo *all_addrs, *addr;
    if((all_addrs = host_serv(hostname, service, 0, SOCK_STREAM)) == NULL)
        exit(EXIT_FAILURE);

    for(addr = all_addrs; addr != NULL; addr = addr->ai_next)
    {
        if((sockfd = socket(addr->ai_family, addr->ai_socktype, 0)) <0)
            continue;
        if(connect(sockfd, addr->ai_addr, addr->ai_addrlen) == 0)
            break;
        Close(sockfd);
    }
    freeaddrinfo(all_addrs);
    if(addr == NULL)
    {
        err_msg("error connecting: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

pid_t Setsid(void)
{
    pid_t ret;
    if((ret = setsid()) == -1)
    {
        err_msg("error setsid: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

pid_t Fork(void)
{
    pid_t ret;
    if((ret = fork()) == -1)
    {
        err_msg("error fork: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Open(const char *pathname, int flags)
{
    int ret;
    if((ret = open(pathname, flags)) == -1)
    {
        err_msg("error open path: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Getrlimit(int resource, struct rlimit *rlim)
{
    int ret;
    if((ret = getrlimit(resource, rlim)) == -1)
    {
        err_msg("error getrlimit: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

void err_msg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if(daemon_proc)
        vsyslog(LOG_INFO, format, args);
    else
        vprintf(format, args);
    va_end(args);
}

void daemonize(const char *appname)
{
    daemon_proc = 1;

    // 打开syslog
    openlog(appname, LOG_PID, 0);

    // 第一次fork
    if(Fork() > 0)
        exit(EXIT_SUCCESS);
    Setsid();

    // 忽略SIGHUP信号，会话首进程退出时子进程会收到
    Signal(SIGHUP, SIG_IGN);
    umask(0);

    // 第二次fork确保无法关联终端
    if(Fork() > 0)
        exit(EXIT_SUCCESS);

    // 必须切换至根目录
    chdir("/");

    // 关闭fd
    //struct rlimit rlim;
    //Getrlimit(RLIMIT_NOFILE, &rlim);
    //unsigned int i;
    //for(i = 0; i < rlim.rlim_cur; i++)
    //    close(i);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //// 打开3个知名fd
    int fd = Open("/dev/null", O_RDONLY);
    dup(fd);
    dup(fd);
}

/*
 * 检查pid文件是否存在，若存在则判断是否被锁，被锁表示已有服务进程示例正在运行
 * 返回1表示pid文件无锁，返回0表示被锁，返回-1表示发生错误
 */
int check_pidfile(char *pidfile)
{
    int fd;
    fd = open(pidfile, O_WRONLY);
    if(fd == -1)
    {
        if(errno == ENOENT)
            return 1;
        return -1;
    }
    int ret;
    ret = lockf(fd, F_TLOCK, 0);
    close(fd);
    if(ret == 0)
        return 1;
    if(errno == EACCES || errno == EAGAIN)
        return 0;
    return -1;
}

/*
 * 创建pid锁文件，成功返回文件描述符，失败则返回-1
 */
int create_pidfile(char *pidfile)
{
    FILE *f = fopen(pidfile, "w");
    if(f == NULL)
        return -1;
    fprintf(f, "%d", getpid());
    fclose(f); // 保证pid被写入文件，先close再打开。。。
    int fd;
    fd = open(pidfile, O_WRONLY);
    if(fd == -1 || lockf(fd, F_LOCK, 0) == -1)
        return -1;
    return fd;
}

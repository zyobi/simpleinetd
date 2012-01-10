/*
 * =====================================================================================
 *
 *       Filename:  simpleinetd.c
 *
 *    Description:  简单的超级服务器
 *
 *        Version:  1.0
 *        Created:  2011年12月26日 21时39分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robin (), zouyongbin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include    "util.h"
#include    "cfg.h"
#include    <ev.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <errno.h>
#include    <string.h>
#include    <netdb.h>

#define PID_FILE "/var/tmp/simpleinetd.pid"
#define CONFIG_FILE "simpleinetd.cfg"

Config *global_cfg;

void hup_handler(EV_P_ ev_signal *w, int revents)
{
    err_msg("SIGHUP received, reloading...\n");
}

void int_handler(EV_P_ ev_signal *w, int revents)
{
    err_msg("SIGINT caught, exiting...\n");
    ev_unloop(EV_A_ 2);
}

void chld_handler(EV_P_ ev_child *w, int revents)
{
    while(waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

void accept_handler(EV_P_ ev_io *w, int revents)
{
    if(EV_ERROR & revents)
    {
        err_msg("got an error\n");
        return;
    }

    // 处理accept事件
    int clientfd;
    clientfd = Accept(w->fd, NULL, NULL);
    if(clientfd == -1)
    {
        err_msg("accepting failed\n");
        return;
    }

    if(Fork() > 0)
    {
        Close(clientfd);
        return;
    }

    ev_unloop(EV_A_ 2);

    const char *cmd;
    cmd = get_cmd(global_cfg, w->fd);
    if(!cmd)
    {
        err_msg("cannot find cmd\n");
        exit(EXIT_FAILURE);
    }

    // 关闭所有监听套接字
    Config *tmp_cfg;
    for(tmp_cfg = global_cfg; tmp_cfg->cmd != NULL; tmp_cfg++)
        close(tmp_cfg->listenfd);

    dup2(clientfd, STDIN_FILENO);
    dup2(clientfd, STDOUT_FILENO);
    dup2(clientfd, STDERR_FILENO);
    Close(clientfd);

    if(execlp(cmd, cmd, NULL) == -1)
    {
        err_msg("error exec %s\n", cmd);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    char host[MAX_HOST];
    int c;
    while((c = getopt(argc, argv, "h:")) != -1)
    {
        switch(c)
        {
            case 'h': 
                strcpy(host, optarg);
                break;
            case ':': 
                err_msg("%c missing argument\n", optopt);
                break;
            case '?': 
                err_msg("%c unknown argument\n", optopt);
                break;
        }
    }

    // 检查进程是否单实例运行
    // 子进程不继承文件锁，因此需要先检查，daemonize后再设置锁
    int check_result;
    check_result = check_pidfile(PID_FILE);
    if(check_result == 0)
    {
        err_msg("pidfile is locked\n");
        return EXIT_FAILURE;
    }
    else if(check_result == -1)
    {
        err_msg("error while checking pidfile: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct addrinfo *addrs, *srvaddr;
    int listenfd;
    int reuseaddr = 1;

    // TODO 使用lex/yacc实现配置文件解析
    global_cfg = parse_config(CONFIG_FILE);
    Config *tmp_cfg;
    for(tmp_cfg = global_cfg;tmp_cfg->cmd != NULL; tmp_cfg++)
    {
        addrs = host_serv(host, tmp_cfg->port, AF_INET, tmp_cfg->socktype);
        if(addrs == NULL)
        {
            err_msg("wrong host or port\n");
            exit(EXIT_FAILURE);
        }

        for(srvaddr = addrs; srvaddr != NULL; srvaddr = srvaddr->ai_next)
        {
            listenfd = socket(srvaddr->ai_family, srvaddr->ai_socktype, srvaddr->ai_protocol);
            if(listenfd == -1)
                continue;
            if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) ||
                    bind(listenfd, srvaddr->ai_addr, srvaddr->ai_addrlen) == -1 || 
                    listen(listenfd, 128) == -1)
            {
                close(listenfd);
                continue;
            }
            else
                break;
        }
        if(srvaddr == NULL)
        {
            err_msg("cannot listen socket\n");
            exit(EXIT_FAILURE);
        }

        ev_io *io = (ev_io *)malloc(sizeof(ev_io));
        ev_io_init(io, accept_handler, listenfd, EV_READ);
        ev_io_start(EV_DEFAULT_ io);
        tmp_cfg->listenfd = listenfd;
    }

    //daemonize("simpleinetd");
    //daemon(0, 0); // Linux自带的daemonize方法，不过自己造的轮子用着不错~

    if(create_pidfile(PID_FILE) == -1)
    {
        err_msg("error while creating pidfile: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    ev_signal hup_watcher;
    ev_signal_init(&hup_watcher, hup_handler, SIGHUP);
    ev_signal_start(EV_DEFAULT_ &hup_watcher);

    ev_signal int_watcher;
    ev_signal_init(&int_watcher, int_handler, SIGINT);
    ev_signal_start(EV_DEFAULT_ &int_watcher);

    ev_child chld_watcher;
    ev_child_init(&chld_watcher, chld_handler, 0, 0);
    ev_child_start(EV_DEFAULT_ &chld_watcher);

    ev_loop(EV_DEFAULT_ 0);

    return EXIT_SUCCESS;
}


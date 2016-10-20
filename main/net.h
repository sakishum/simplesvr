#ifndef __NET_H__
#define __NET_H__

int listen_socket = -1;
extern Log *log_main;

int make_server_socket()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CONF.port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        log_main->Error("make_server_socket() socket");
        return -1;
    }

    int r = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if(r != 0) {
        close(fd);
        log_main->Error("make_server_socket() bind");
        return -1;
    }

    return listen_socket = fd;
}

#endif

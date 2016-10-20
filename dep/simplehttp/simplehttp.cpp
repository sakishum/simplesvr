#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <netdb.h>   
#include <netinet/tcp.h>

#include "simplehttp.h"

int make_client_socket(int mtimeout)
{
    int sock;
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        cerr<<"make_client_socket() socket error: "<<strerror(errno)<<endl;
        return -1;
    }
    struct timeval tv;
    tv.tv_sec = mtimeout/1000;
    tv.tv_usec = mtimeout%1000 * 1000;
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        cerr<<"make_client_socket() setsockopt(sndtimeo) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) {
        cerr<<"make_client_socket() setsockopt(rcvtimeo) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }

    int n=1024*1024;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n))) {
        cerr<<"make_client_socket() setsockopt(rcvbuf) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }

    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval))) {
        cerr<<"make_client_socket() setsockopt(keepalive) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }

    optval = 2;
    if (setsockopt(sock, SOL_TCP, TCP_KEEPCNT, &optval, sizeof(optval))) {
        cerr<<"make_client_socket() setsockopt(keepcnt) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }

    optval = 15;
    if (setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(optval))) {
        cerr<<"make_client_socket() setsockopt(keepidle) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }

    optval = 15;
    if (setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(optval))) {
        cerr<<"make_client_socket() setsockopt(keepintvl) error: "<<strerror(errno)<<endl;
        close(sock);
        return -1;
    }   

    return sock;
}

int connect_tmo(int sockfd, const char *ip, int port, int mtimeout)
{
    if (ip == NULL) {
        return -1;
    }

    struct addrinfo *airoot, hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    char strPort[8];
    snprintf(strPort, sizeof(strPort), "%d", port);
    int r = getaddrinfo(ip, strPort, &hints, &airoot);
    if (r == -1) {
        return -1;
    }

    int error=-1, len=sizeof(int);
    timeval tm;
    fd_set set;
    unsigned long ul = 1;
    ioctl(sockfd, FIONBIO, &ul);
    bool ret = true;
    if (connect(sockfd, airoot->ai_addr, airoot->ai_addrlen) == -1) {
        tm.tv_sec = mtimeout/1000;
        tm.tv_usec = mtimeout%1000 * 1000;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if (select(sockfd+1, NULL, &set, NULL, &tm) > 0) {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if (error != 0) {
                ret = false;
            }
        } else {
            ret = false;
        }
    }
    ul = 0;
    ioctl(sockfd, FIONBIO, &ul);

    freeaddrinfo(airoot);

    if(!ret) {
        return -1;
    }
    return 0;
}


int send_recv(int nSocket, const char *pReq, int nReqLen, string &resp, string &errmsg)
{
    int iSendLen = send(nSocket, pReq, nReqLen, 0);
    if (iSendLen != nReqLen) {
        errmsg = "send_recv() sendto failed: " + string(strerror(errno));
        return -1;
    }
    ssize_t count = 0;
    string _resp;
    char pRsp[1024] = {0};
    while (true) {
        count = recv(nSocket, pRsp, sizeof(pRsp), 0);
        if (count == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                errmsg = "send_recv() recv failed: " + string(strerror(errno));
            } else {
                errmsg = "send_recv() recv timeout";
            }
            return -1;
        } else if (count == 0) {
            break;
        } else {
            _resp += string(pRsp, count);
        }
    }

    const char *ok = "200 OK";
    const char *end = "\r\n\r\n";
    const char *okPos;
    if ((okPos = strcasestr(_resp.c_str(), ok)) != NULL) {
        size_t bodyPos = _resp.find(end);
        if (bodyPos == string::npos) {
            errmsg = "send_recv() no body end found";
            return -1;
        }

        resp = _resp.substr(bodyPos+strlen(end));
        return 0;
    } else {
        errmsg = "send_recv() no ok found: " + _resp.substr(0, 256);
        return -1;
    }
}

int simple_get(const char *cgi, const char *host, const char *ip, int port, int mtimeout, string &resp, string &errmsg)
{
    const char *http_head = "GET %s HTTP/1.0\r\n"
        "Connection: close\r\n"
        "Host: %s\r\n\r\n";

    char *tmp_buf=NULL;
    if (asprintf(&tmp_buf, http_head, cgi, host) < 0) {
        return -1;
    }

    int sock = make_client_socket(mtimeout);
    if (sock < 0) {
        free(tmp_buf);
        return -2;
    }

    if (connect_tmo(sock, ip, port, mtimeout) < 0) {
        free(tmp_buf);
        close(sock);
        return -3;
    }

    if (send_recv(sock, tmp_buf, strlen(tmp_buf), resp, errmsg) < 0) {
        free(tmp_buf);
        close(sock);
        return -4;
    }

    free(tmp_buf);
    close(sock);
    return 0;
}

int simple_post(const char *cgi, const char *body, const char *host, const char *ip, int port, int mtimeout, string &resp, string &errmsg)
{
    const char *http_head = "POST %s HTTP/1.0\r\n"
        "Connection: close\r\n"
        "Host: %s\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s";

    char *tmp_buf=NULL;
    if (asprintf(&tmp_buf, http_head, cgi, host, strlen(body), body) < 0) {
        return -1;
    }

    int sock = make_client_socket(mtimeout);
    if (sock < 0) {
        free(tmp_buf);
        return -2;
    }

    if (connect_tmo(sock, ip, port, mtimeout) < 0) {
        free(tmp_buf);
        close(sock);
        return -3;
    }

    if (send_recv(sock, tmp_buf, strlen(tmp_buf), resp, errmsg) < 0) {
        free(tmp_buf);
        close(sock);
        return -4;
    }

    free(tmp_buf);
    close(sock);
    return 0;
}

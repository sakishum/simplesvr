#ifndef __PROC_H__
#define __PROC_H__

Clog *log_proc;

void* timer_thread(void *p)
{
    (void)(p); //ignore

    fd_set read_fds;
    int suicide = new_timer(500);
    if (suicide == -1) {
        log_proc->Error("timer_thread() new_timer");
        return NULL;
    }

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(suicide, &read_fds);
        int num_fd = select(suicide+1, &read_fds, NULL, NULL, NULL);

        if (num_fd < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                log_proc->Error("timer_thread() select %d", errno);
            }
        }

        if (FD_ISSET(suicide, &read_fds)) {
            uint64_t exp; 
            read(suicide, &exp, sizeof(uint64_t));
            if (getppid() == 1) {
                exit(0);
            }
        }
    }

    return NULL;
}

//
//decode data: cmd\x00???\x00???\x00
//return: 
int decode(const char *buf, const ssize_t len, vector<string> &datas)
{
    int ret = 0;

    datas.clear();
    for (const char *bpos=buf, *epos; bpos < buf+len; bpos = epos+1) {
        epos = strchr(bpos, '\0');
        if (!epos) break;
        datas.push_back(string(bpos, epos-bpos));
    }

    if (datas.size() == 0) {
        ret = -1;
    }
    return ret;
}


int encode(char *buf, int *len, vector<string> &datas)
{
    int ret = 0;

    buf[0] = -1;
    *len = 1;

    string cmd = datas[0];

    static map<string, uint32_t> stats;
    if ((stats[cmd] = ++stats[cmd]) % 100 == 0) {
        log_proc->Info("encode() cmd: %s, start_num(just see see): %u", cmd.c_str(), stats[cmd]);
    }

    if (cmd == "Demo") {
        if (datas.size() != 2) return -1;
        string retstr;
        string args = datas[1];
        int iRet = demo(retstr, args);
        if (iRet == 0) {
            buf[0] = 0;
            memcpy(buf+1, retstr.data(), retstr.size());
            *len = (int)retstr.size() + 1;
        } else {
            buf[0] = (char)iRet;
            *len = 1;
        }
    }

    return ret;
}

void proc_child(int num)
{
    char sub_cate[128];
    sprintf(sub_cate, "%d", num);
    log_proc = new Clog(g_module, sub_cate);

    set_affinity(num);

    pthread_t tid;
    pthread_create(&tid, NULL, timer_thread, NULL);

    int ret;
    char recv_buf[65536];
    struct sockaddr addr_from;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    ssize_t recv_len;

    vector<string> datas;

    char retstr[65536];
    int retlen = 0;

    while (true) {
        recv_len = recvfrom(listen_socket,
                recv_buf,
                sizeof(recv_buf)-1, 0,
                (struct sockaddr*)&addr_from, &addr_len);

        if (recv_len <= 0) {
            continue;
        }

        //code trick
        //最后一个字符必是\x00
        recv_buf[recv_len] = 0;

#if DEBUG
        printf("recv_data:\n");
        print_bin(recv_buf, int(recv_len));
#endif

        ret = decode(recv_buf, recv_len+1, datas);
        if (ret < 0) {
            log_proc->Error("proc_child() decode error: %d", ret);
            continue;
        }

        ret = encode(retstr, &retlen, datas);
        if (ret < 0) {
            log_proc->Error("proc_child() encode error: %d", ret);
            continue;
        }

        sendto(listen_socket, retstr, retlen, 0, &addr_from, addr_len);
    }

    exit(0);
}

#endif

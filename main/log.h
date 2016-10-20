#ifndef __LOG_H__
#define __LOG_H__

const char *g_module = "demo";
const int g_agent_port = 28701;

#define VAR_DATA(buf, format)                                   \
    do {                                                        \
            va_list arg;                                        \
            va_start(arg, format);                              \
            (vasprintf(&buf, format, arg) < 0) && (buf = NULL); \
            va_end(arg);                                        \
    } while (false)

string datetime()
{
    time_t now;
    time(&now);
    struct tm* t=localtime(&now);
    char timestr[80];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);
    return timestr;
}

class Log {
    public:
        Log(string module, string subcate) {
            string local_ip = get_localip();
            dbgcate = module + "," + "logdbg" + "," + local_ip + "," + subcate;
            warcate = module + "," + "logwar" + "," + local_ip + "," + subcate;
            errcate = module + "," + "logerr" + "," + local_ip + "," + subcate;
            infocate = module + "," + "loginfo" + "," + local_ip + "," + subcate;
            m_sockfd = -1;
        }

        void Do(const string& cate, const char *content) {
            if (m_sockfd < 0) {
                m_sockfd=socket(AF_INET, SOCK_DGRAM, 0);
                if (m_sockfd < 0) {
                    cerr<<"Log:Do() socket "<<strerror(errno)<<endl;
                    return;
                }
                struct timeval tv;
                tv.tv_sec = 80/1000;
                tv.tv_usec = 0;
                if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
                    cerr<<"Log:Do() setsockopt(sndtimeo) error "<<strerror(errno)<<endl;
                }
                m_servaddr.sin_family = AF_INET;
                m_servaddr.sin_port = htons(g_agent_port);
                m_servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
                connect(m_sockfd, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));
            }
            string buf = cate + "," + content;
            send(m_sockfd, buf.c_str(), buf.size(), 0);
        }

        void Debug(const char *format, ...) {
            if ((CONF.log.level & 1) != 0) {
                char *tmp_buf;
                VAR_DATA(tmp_buf, format);
                if (tmp_buf != NULL) {
                    if ((CONF.log.mode & 1) != 0) {
                        cout<<tmp_buf<<'['<<datetime()<<']'<<endl;
                    }
                    if ((CONF.log.mode & 2) != 0) {
                        Do(dbgcate, tmp_buf);
                    }
                    free(tmp_buf);
                }
            }
        }

        void Warn(const char *format, ...) {
            if ((CONF.log.level & 2) != 0) {
                char *tmp_buf;
                VAR_DATA(tmp_buf, format);
                if (tmp_buf != NULL) {
                    if ((CONF.log.mode & 1) != 0) {
                        cout<<tmp_buf<<'['<<datetime()<<']'<<endl;
                    }
                    if ((CONF.log.mode & 2) != 0) {
                        Do(warcate, tmp_buf);
                    }
                    free(tmp_buf);
                }
            }
        }

        void Error(const char *format, ...) {
            if ((CONF.log.level & 4) != 0) {
                char *tmp_buf;
                VAR_DATA(tmp_buf, format);
                if (tmp_buf != NULL) {
                    if ((CONF.log.mode & 1) != 0) {
                        cerr<<tmp_buf<<'['<<datetime()<<']'<<endl;
                    }
                    if ((CONF.log.mode & 2) != 0) {
                        Do(errcate, tmp_buf);
                    }
                    free(tmp_buf);
                }
            }
        }

        void Info(const char *format, ...) {
            if ((CONF.log.level & 8) != 0) {
                char *tmp_buf;
                VAR_DATA(tmp_buf, format);
                if (tmp_buf != NULL) {
                    if ((CONF.log.mode & 1) != 0) {
                        cout<<tmp_buf<<'['<<datetime()<<']'<<endl;
                    }
                    if ((CONF.log.mode & 2) != 0) {
                        Do(infocate, tmp_buf);
                    }
                    free(tmp_buf);
                }
            }
        }

    private:
        int m_sockfd;
        struct sockaddr_in m_servaddr;
        string dbgcate;
        string warcate;
        string errcate;
        string infocate;
};


#endif

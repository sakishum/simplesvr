#ifndef __LOG_H__
#define __LOG_H__

const string g_module = "demo";
const string g_server_ip = "127.0.0.1";
const int g_server_port = 28702;

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
    char timestr[32];
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
        }

        void Do(const string& cate, const char *content) {
            int sockfd=socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                cerr<<"Log:Do() socket "<<strerror(errno)<<endl;
                return;
            }
            struct sockaddr_in m_servaddr;
            m_servaddr.sin_family = AF_INET;
            m_servaddr.sin_port = htons(g_server_port);
            m_servaddr.sin_addr.s_addr = inet_addr(g_server_ip.c_str());
            string buf = cate + "," + content;
            sendto(sockfd, buf.c_str(), buf.size(), 0, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));
            close(sockfd);
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
        string dbgcate;
        string warcate;
        string errcate;
        string infocate;
};

#endif

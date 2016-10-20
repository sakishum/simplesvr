#ifndef __UTIL_H__
#define __UTIL_H__

int get_ncpus()
{
    long result = sysconf(_SC_NPROCESSORS_ONLN);
    if (result<= 0) {
        return 1;
    } else {
        return int(result);
    }
}

void set_affinity(int num)
{
    int ncpu = get_ncpus();
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(num%ncpu, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
        fprintf(stderr, "set cpu affinity failed!");
    }
}

void str_split(const char *str, const int len, const char delim, vector<string> &dst)
{
    if (str == NULL) return;

    const char *ep = str+len;
    while (true){
        const char *sp = str;
        while(str != ep && *str != delim){
            str++;
        }
        dst.push_back(string(sp,str-sp));
        if(str == ep) break;
        str++;
    }
}

void print_bin(char *buffer, int length)
{
    int i;
    char tmp_buffer[4096];
    char str_temp[32];
    if( length <= 0 || length > 1024 || buffer == NULL )
    {
        return;
    }

    tmp_buffer[0] = '\0';
    for( i = 0; i < length; i++ )
    {
        if(!( i%16 ) )
        {
            sprintf(str_temp, "\n%04d>    ", i/16+1);
            strcat(tmp_buffer, str_temp);
        }
        sprintf(str_temp, "%02X ", (unsigned char )buffer[i]);
        strcat(tmp_buffer, str_temp);
    }
    strcat(tmp_buffer, "\n");
    printf("Print Hex:%s", tmp_buffer);
    return;
}


void daemonize(void) {
    int fd;

    if (fork() != 0) exit(0);
    setsid();

    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }
}

/* get the stringt hash value */
#define STR_HASH(res, kbuf, ksiz)                                 \
    do {                                                          \
        const unsigned char *_p = (const unsigned char *)(kbuf);  \
        int _ksiz = (ksiz);                                       \
        for ((res) = 5381; _ksiz--;) {                            \
            (res) = ((res) << 5) + (res) + *(_p)++;               \
        }                                                         \
    } while (false)


#define TIMER(f)                                                    \
    do {                                                            \
        struct timeval begin, end;                                  \
        gettimeofday(&begin, NULL);                                 \
        (f);                                                        \
        gettimeofday(&end, NULL);                                   \
        long bu=(((long long)begin.tv_sec)*1000000)+begin.tv_usec;  \
        long eu=(((long long)end.tv_sec)*1000000)+end.tv_usec;      \
        printf("elapse time: %ld(us)\n", eu-bu);                    \
    } while (false)


int new_timer(int minterval)
{
    struct itimerspec new_value;
    int fd;
    struct timespec now;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
        fprintf(stderr, "clock_gettime");
        return -1;
    }

    new_value.it_value.tv_sec = now.tv_sec + minterval/1000;
    new_value.it_value.tv_nsec = now.tv_nsec + minterval%1000 * 1000;
    new_value.it_interval.tv_sec = minterval/1000;
    new_value.it_interval.tv_nsec = minterval%1000 * 1000;

    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
        fprintf(stderr, "timerfd_create");
        return -1;
    }

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1) {
        fprintf(stderr, "timerfd_settime");
    }

    return fd;
}


bool is_innerip(string ip)
{
    unsigned long ipa_beg = inet_network("10.0.0.0");
    unsigned long ipa_end = inet_network("10.255.255.255");

    unsigned long ipb_beg = inet_network("172.16.0.0");
    unsigned long ipb_end = inet_network("172.31.255.255");

    unsigned long ipc_beg = inet_network("192.168.0.0");
    unsigned long ipc_end = inet_network("192.168.255.255");

    unsigned long src_ip = inet_network(ip.c_str());

    if ((src_ip >= ipa_beg && src_ip <= ipa_end) || (src_ip >= ipb_beg && src_ip <= ipb_end) ||(src_ip >= ipc_beg && src_ip <= ipc_end)) {

        return true;
    }

    return false;
}

string get_localip()
{
    static string ip;
    if (ip.size() > 0) {
        return ip;
    }

    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (0 != (ifa->ifa_flags & IFF_LOOPBACK)) { 
            continue;
        }
        if (0 == (ifa->ifa_flags & IFF_UP)) { 
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (is_innerip(addressBuffer)) {
                ip = addressBuffer;
                break;
            }
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return ip;
}

int remove_comment(string& str)
{
    string regex = "('([^\\\\']|\\\\.)*'|\"([^\\\\\"]|\\\\.)*\")|"
        "(//[^\n]*|/\\*(.|\n)*?\\*/)";

    size_t start = 0; 
    size_t ppos, pend;
    int options = REG_EXTENDED | REG_ICASE;
    regex_t rbuf;
    regmatch_t match[10];
    int ret;
    if ((ret = regcomp(&rbuf, regex.c_str(), options)) != 0) {
        char buffer[512] = {0};
        regerror (ret, &rbuf, buffer, sizeof(buffer)-1);
        cerr<<"regcomp error: "<<buffer<<endl;
        return -1;
    }
    while (regexec(&rbuf, str.substr(start).c_str(), sizeof(match)/sizeof(regmatch_t), match, 0) == 0) {
        ppos =  match[0].rm_so;
        pend = match[0].rm_eo;

        if(match[4].rm_so != -1) {
            str.erase(ppos+start, pend-ppos);
            start += ppos;
        } else {
            start += pend;
        }
    }
    return 0;
}

string url_encode(string &str)
{
    string retstr;
    const char *ptr = str.data();
    size_t size = str.size();
    for(size_t i = 0; i < size; i++){
        int c = ((unsigned char *)ptr)[i];
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') || (c != '\0' && strchr("_-.!~*'()", c))){
            retstr.push_back((char)c);
        } else {
            retstr.push_back('%');
            char buf[3];
            snprintf(buf, sizeof(buf), "%02X", c);
            retstr.append(buf);

        }
    }
    return retstr;
}

#endif

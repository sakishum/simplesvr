// simplesvr (simple udp server)
// author: simplejia
// date: 2015/10/16
#include <stdarg.h>
#include <climits>
#include <errno.h>
#include <regex.h>
#include <sys/wait.h>
#include <sys/timerfd.h>
#include <sys/un.h>
#include <fcntl.h>

#include <net/if.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <fstream>

using namespace std;

#include "simplehttp/simplehttp.h"
#include "simplecache/simplecache.h"
#include "jsoncpp/dist/json/json.h"

#include "util.h"
#include "conf.h"
#include "log.h"
#include "net.h"

// busi dependencies file here
#include "busi_demo.h"

#include "proc.h"

Log *log_main;

int main(int /*argc*/, char** /*argv*/) 
{
    log_main = new Log(g_module, "main");
    log_main->Info("main()");

    if (parse_conf() < 0) {
        log_main->Error("parse_conf()");
        return -1;
    }

    if (make_server_socket() < 0) {
        log_main->Error("make_server_socket()");
        return -1;
    }

    for (int i=0; i<get_ncpus(); ++i) {
        if (fork() == 0) {
            // child process
            proc_child(i);
        }
    }

    // parent process
    while (true) {
        int status;
        int pid = wait(&status);
        log_main->Error("main() child process: %d exit, retcode: %d", pid, WEXITSTATUS(status));
        break;
    }

    return 0;
}


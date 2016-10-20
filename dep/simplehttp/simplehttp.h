#ifndef _SIMPLEHTTP_H_
#define _SIMPLEHTTP_H_
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

int simple_get(const char *cgi, const char *host, const char *ip, int port, int mtimeout, string &resp, string &errmsg);

int simple_post(const char *cgi, const char *body, const char *host, const char *ip, int port, int mtimeout, string &resp, string &errmsg);


#endif

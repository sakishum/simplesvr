#ifndef _SIMPLECACHE_H_
#define _SIMPLECACHE_H_
#include <iostream>
#include <string>
#include <map>
#include <stdio.h>

using namespace std;

// @return 0: success 1: no data or expire data -1: error
int simple_cache_get(string &key, string &data);
int simple_cache_set(string &key, string &data, int expire);


#endif

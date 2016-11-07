#include "simplecache.h"
#include "list.h"

struct cache_data
{
    struct list_head list;
    string key;
    string value;
    int expire; // 单位：秒

    cache_data& operator=(const cache_data& right) {
        if (this != &right) {
            key = right.key;
            value = right.value;
            expire = right.expire;
        }
        return *this;
    }
};

class cache_data_list 
{
    public:
        cache_data_list() {}
        int init(int capacity);
        int put(const cache_data *pd);
        int get(const string &key, cache_data **ppd);

    private:
        struct list_head head;
        map<string, cache_data*> table;
};


int cache_data_list::init(int capacity)
{
    if(capacity <= 0) {
        return -1;
    }

    INIT_LIST_HEAD(&head);
    for(int i = 0; i < capacity; i++) {
        cache_data *pd = new cache_data();
        if(pd) {
            list_add(&pd->list, &head);
        } else {
            return -1;
        }
    }

    return 0;
}

int cache_data_list::put(const cache_data *pd)
{
    if(pd == NULL) {
        return -1;
    }

    if (table.find(pd->key) != table.end()) {
        *table[pd->key] = *pd;
        return 0;
    }

    cache_data *pd_tmp;
    pd_tmp = list_entry(head.next, cache_data, list);

    table.erase(pd_tmp->key);

    *pd_tmp = *pd;
    table[pd_tmp->key] = pd_tmp;
    list_move_tail(&pd_tmp->list, &head);

    return 0;
}

int cache_data_list::get(const string &key, cache_data **ppd)
{        
    if (table.find(key) != table.end()) {
        *ppd = table[key];
    } else {
        *ppd = NULL;
    }

    if(*ppd != NULL) {
        list_move_tail(&((*ppd)->list), &head);
    }

    return 0;
}

cache_data_list g_cache;

int simple_cache_init(int capacity)
{
    return g_cache.init(capacity);
}

// @return 0: success; 1: no value or expire value
int simple_cache_get(string &key, string &value)
{
    cache_data *pd = NULL;
    g_cache.get(key, &pd);
    if (pd != NULL) {
        value = pd->value;
    }
    if ((pd == NULL) || (pd->expire < time(0))) {
        return 1;
    }
    return 0;
}

int simple_cache_set(string &key, string &value, int expire)
{
    cache_data d;
    d.key = key;
    d.value = value;
    d.expire = time(0) + expire;
    g_cache.put(&d);
    return 0;
}


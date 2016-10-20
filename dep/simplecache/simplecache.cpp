#include "simplecache.h"
#include "list.h"

struct cache_data
{
    struct list_head lru;
    string key;
    string data;
    int expire; // 单位：秒

    cache_data& operator=(const cache_data& right) {
        if (this != &right) {
            key = right.key;
            data = right.data;
            expire = right.expire;
        }
        return *this;
    }
};

class cache_data_list 
{
    public:
        cache_data_list() {
            init(1000);
        }
        int init(int iMaxNrOfBlkc);
        int put(const cache_data *pd);
        int get(const string &key, cache_data **pd);
        void print();

    private:
        struct list_head d_lru_;
        map<string, cache_data*> m_mapBlock;
};


int cache_data_list::init(int iMaxNrOfBlkc)
{
    if(iMaxNrOfBlkc < 0) {
        return -1;
    }

    INIT_LIST_HEAD(&d_lru_);
    for(int i = 0; i < iMaxNrOfBlkc; i++) {
        cache_data *d = new cache_data();
        if(d) {
            list_add(&d->lru, &d_lru_);
        } else {
            break;
        }
    }

    return 0;
}

int cache_data_list::put(const cache_data *pd)
{
    if(pd == NULL) {
        return -1;
    }

    if (m_mapBlock.find(pd->key) != m_mapBlock.end()) {
        *m_mapBlock[pd->key] = *pd;
        return 0;
    }

    cache_data *p;
    p = list_entry(d_lru_.next, cache_data, lru);

    if(!p->key.empty()) {
        m_mapBlock.erase(p->key);
    }

    *p = *pd;
    m_mapBlock[p->key] = p;
    list_move_tail(&p->lru, &d_lru_);

    return 0;
}

int cache_data_list::get(const string &key, cache_data **pd)
{        
    if (m_mapBlock.find(key) != m_mapBlock.end()) {
        *pd = m_mapBlock[key];
    } else {
        *pd = NULL;
    }

    if(*pd != NULL) {
        list_move_tail(&((*pd)->lru), &d_lru_);
    }

    return 0;
}

void cache_data_list::print() 
{
    struct list_head *list;
    cache_data *d;
    list_for_each_entry_safe_l(d, list, &d_lru_, lru) {
        printf("cache_data:----------\n"
               "key                 = %s\n"
               ,
               d->key.c_str());
    }
}

cache_data_list g_cache;

// @return 0: success 1: no data or expire data -1: error
int simple_cache_get(string &key, string &data)
{
    cache_data *pd = NULL;
    g_cache.get(key, &pd);
    if ((pd == NULL) || (pd->expire < time(0))) {
        if (pd != NULL) {
            data = pd->data;
        }
        return 1;
    }

    data = pd->data;
    return 0;
}

int simple_cache_set(string &key, string &data, int expire)
{
    cache_data d;
    d.key = key;
    d.data = data;
    d.expire = time(0) + expire;
    g_cache.put(&d);
    return 0;
}


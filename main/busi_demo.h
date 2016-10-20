#ifndef __BUSI_DEMO_H__
#define __BUSI_DEMO_H__

extern Log *log_proc;

int demo(string &retstr, string &data)
{
    int ret = 0;
    retstr = "get data from demo";
    log_proc->Debug("demo() %s", data.c_str());
    return ret;
}


#endif

#ifndef __CONF_H__
#define __CONF_H__

struct Conf 
{
    int port;
    struct Clog {
        int mode;
        int level;
    } clog;
} CONF;

string ENV = "prod";

int parse_conf(int argc, char *argv[])
{
    if (argc >= 2) {
        ENV = argv[1];
    }

    ifstream is("conf.json");
    if (!is.is_open()) {
        cerr<<"parse_conf() conf file is not exist"<<endl;
        return -1;
    }
    string fcontent((std::istreambuf_iterator<char>(is)),
            std::istreambuf_iterator<char>());
    is.close();

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(fcontent, root)) {
        cerr<<"parse_conf() parse"<<endl;
        return -1;
    }

    Json::Value json_conf = root[ENV];
    if (json_conf.empty()) {
        cerr<<"parse_conf() env not right"<<endl;
        return -1;
    }

    CONF.port = json_conf["port"].asInt();
    CONF.clog.mode = json_conf["clog"]["mode"].asInt();
    CONF.clog.level = json_conf["clog"]["level"].asInt();

    return 0;
}

#endif

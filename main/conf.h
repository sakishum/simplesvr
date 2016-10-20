#ifndef __CONF_H__
#define __CONF_H__

struct Conf 
{
    int port;
    struct Log {
        int mode;
        int level;
        Log(): mode(INT_MAX), level(INT_MAX) {}
    } log;
} CONF;

struct Confs
{
    string env;
    map<string, Conf> envs;
} CONFS;

int parse_conf()
{
    ifstream is("conf.json");
    if (!is.is_open()) {
        cerr<<"parse_conf() conf file is not exist"<<endl;
        return -1;
    }
    string fcontent((std::istreambuf_iterator<char>(is)),
            std::istreambuf_iterator<char>());
    is.close();

    if (remove_comment(fcontent) != 0) {
        cerr<<"parse_conf() remove_comment"<<endl;
        return -1;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(fcontent, root)) {
        cerr<<"parse_conf() parse"<<endl;
        return -1;
    }

    CONFS.env = root["env"].asString();
    Json::Value::Members envs = root["envs"].getMemberNames();
    for (unsigned i=0; i<envs.size(); i++) {
        if (envs[i].empty()) continue;
        Conf conf;
        Json::Value json_conf = root["envs"][envs[i]];
        conf.port = json_conf["port"].asInt();
        conf.log.mode = json_conf["log"]["mode"].asInt();
        conf.log.level = json_conf["log"]["level"].asInt();

        CONFS.envs[envs[i]] = conf;
    }

    if (CONFS.env.empty()) {
        cerr<<"parse_conf() env not right"<<endl;
        return -1;
    }
    
    if (CONFS.envs.find(CONFS.env) == CONFS.envs.end()) {
        cerr<<"parse_conf() env conf not right: "<<CONFS.env<<endl;
        return -1;
    }

    CONF = CONFS.envs[CONFS.env];
    return 0;
}



#endif

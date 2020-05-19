#ifndef TS_URL_H
#define TS_URL_H
#include<vector>
#include <QString>


struct ts_info{
    bool search_flag;
    std::string url;
    std::string title;
    std::string up;
    std::string upload_time;
    std::string duration;
    std::string ts_url_prefix;
    std::vector<std::string> ts_url;
public:
    ts_info() : search_flag(false), url(""), title(""), up(""), upload_time(""), duration(""),ts_url_prefix(""),ts_url({}){}
};


#endif // TS_URL_H

#include "ts_info.h"

ts_info::ts_info():search_flag(false),url(""),title(""),up(""),upload_time(""),duration(""),ts_url_prefix(""),ts_url({""}){}
ts_info ts_info::copy_from(ts_info info){
     set_search_flag(info.get_search_flag());
    if(info.get_up().size())
        set_up(info.get_up());
    if(info.get_url().size())
        set_url(info.get_url());
    if(info.get_title().size())
        set_title(info.get_title());
    if(info.get_ts_url().size())
        set_ts_url(info.get_ts_url());
    if(info.get_duration().size())
        set_duration(info.get_duration());
    if(info.get_upload_time().size())
        set_upload_time(info.get_upload_time());
    if(info.get_ts_url_prefix().size())
        set_ts_url_prefix(info.get_ts_url_prefix());
    return  *this;
}

std::string ts_info::get_up(){
    return up;
}
bool ts_info::get_search_flag(){
    return search_flag;
}
std::string ts_info::get_title(){
    return title;
}
std::string ts_info::get_url(){
    return url;
}
std::string ts_info::get_ts_url_prefix(){
    return ts_url_prefix;
}
std::string ts_info::get_duration(){
    return duration;
}
std::string ts_info::get_upload_time(){
    return upload_time;
}
std::vector<std::string> ts_info::get_ts_url(){
    return  ts_url;
}


void ts_info::set_url(std::string s){
    url = s;
}

void ts_info::set_search_flag(bool flag){
    search_flag = flag;
}

void ts_info::set_title(std::string s){
    title = s;
}

void ts_info::set_up(std::string s){
    up = s;
}

void ts_info::set_duration(std::string s){
    duration = s;
}

void ts_info::set_upload_time(std::string s){
    upload_time = s;
}

void ts_info::set_ts_url_prefix(std::string s){
    ts_url_prefix = s;
}

void ts_info::set_ts_url(std::vector<std::string>s){
    ts_url = s;
}

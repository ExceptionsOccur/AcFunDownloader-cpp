#ifndef HTTP_H
#define HTTP_H
#include <string>
#include <regex>
#include <QString>
#include "ts_info.h"
#include <curl.h>

QString limit_path_len(QString path);

QString str_to_qstr(const std::string str);

std::string qstr_to_str(const QString qstr);

void modifies_illegal_title(std::string& s);

bool file_exists(QString, std::string);

bool re_search(std::string s, std::regex pattern, std::string& result);

void get_ts_url(std::string s, std::vector<std::string>& result);

// check in line_edit widget
std::string get_ac(std::string s);

CURLcode get(std::string url, std::string& response);

// get title, up, upload time, duration, ts_url, pre_ts_url
//bool video_search(ts_info& video_info);

ts_info video_search(std::string url);




#endif // HTTP_H

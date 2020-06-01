#include <string>
#include <unordered_map>
#include <curl.h>
#include <regex>
#include "json.hpp"
#include <chrono>
#include <QString>
#include <stdio.h>
#include <ts_info.h>
#include <fstream>


QString limit_path_len(QString path){
   int len = path.length();
   if(len > 20){
       int i = len - 22;
       QString temp;
       temp = path.mid(0, 3);
       temp[3] = '.';
       temp[4] = '.';
       temp[5] = '/';
       temp = temp + path.mid(i);
       return temp;
   }
   return path;
}

QString str_to_qstr(const std::string str)
{
    return QString::fromLocal8Bit(str.data());
}

std::string qstr_to_str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return std::string(cdata);
}

void modifies_illegal_title(std::string& s){
    std::regex pattern("[\/\\\:\*\?\"\<\>\|\s\n]");
    s = std::regex_replace(s, pattern, "-");
}

bool file_exists(QString path, std::string title){
    if(path.back() != '/')
        path = path + "/";
    std::string file_name = qstr_to_str(path) + title + ".mp4";
    std::fstream in(file_name.c_str(), std::fstream::in);
    if(in.is_open()){
        in.close();
        return true;
    }
    else
        return false;
}

bool re_search(std::string s, std::regex pattern, std::string& result){
    std::smatch smatch;
    std::string::const_iterator it_begin(s.begin()), it_end(s.end());
    while (std::regex_search(it_begin, it_end, smatch, pattern)) {
        result = smatch[0];
        return true;
    }
    return false;
}

void get_ts_url(std::string s, std::vector<std::string>& result) {
    result.clear();
    std::regex re(",\n(.*?)\n");
    std::smatch re_result;
    size_t result_size = 0;
    std::string sub_str("");
    std::string::const_iterator it_begin(s.begin()), it_end(s.end());
    while (std::regex_search(it_begin, it_end, re_result, re)){
        sub_str = re_result[0];
        result_size = sub_str.size();
        result.push_back(sub_str.substr(2, result_size - 3));
        it_begin = re_result[0].second;
    }
}

// check in line_edit widget
std::string get_ac(std::string s) {
    std::string::const_iterator it_begin(s.begin()), it_end(s.end());
    std::smatch result;
    std::regex num("[0-9]{8}");
    while (std::regex_search(it_begin, it_end, result, num)) {
        return result[0];
        break;
    }
    return "url error";
}


size_t get_call_back(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::string* str = reinterpret_cast<std::string*>(stream);
    (*str).append(reinterpret_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}


// QString 有坑，返回的东西缺斤少两，保存到文件还会乱码！
CURLcode get(std::string url, std::string& response) {
    response.clear();
    curl_global_init(CURL_GLOBAL_ALL);
    CURLcode res_code = CURLE_OK;
    CURL* curl = nullptr;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  // const char* 类型很重要！！！string有问题！
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:75.0) Gecko/20100101 Firefox/75.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_call_back);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&response));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    res_code = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return res_code;
}


// get title, up, upload time, duration, ts_url, pre_ts_url
bool video_search(ts_info& video_info){
    std::string response("");
    std::string json_string = "";
    nlohmann::json json;
    if(CURLE_OK == get(video_info.url, response)){
        size_t index_start = response.find("window.videoInfo = ");
        size_t index_end = response.find("window.videoResource");
        if (index_start != std::string::npos && index_end != std::string::npos){
            index_start += 19;
            index_end -= 11;
            if(index_end > index_start){
                json_string = response.substr(index_start, index_end - index_start + 1);
            }
            else
                return false;
            json = nlohmann::json::parse(json_string);
            video_info.title = json["title"];
            modifies_illegal_title(video_info.title);
            video_info.up = json["user"]["name"];
            long long time_(json["videoList"][0]["uploadTime"]);
            std::time_t time(time_ / 1000);
            char upload_time[25];
            std::strftime(upload_time, 25, "%Y-%m-%d %H:%M:%S", localtime(&time));
            video_info. upload_time = upload_time;
            int duration_millis = json["videoList"][0]["durationMillis"];
            int min = duration_millis / 60000, sec = duration_millis / 1000 % 60;
            char duration[10];
            sprintf(duration, "%.2d:%.2d", min, sec);
            video_info.duration = duration;


            std::string ksplay_info = json["currentVideoInfo"]["ksPlayJson"].get<std::string>();
            nlohmann::json ksplay_json = nlohmann::json::parse(ksplay_info);
            std::string m3u8_file_url = ksplay_json["adaptationSet"]["representation"][0]["url"];
            std::regex r_pre_url("https(.*?)hls/");
            re_search(m3u8_file_url, r_pre_url, video_info.ts_url_prefix);
            std::string ts_file("");
            if(CURLE_OK == get(m3u8_file_url, ts_file)){
                get_ts_url(ts_file, video_info.ts_url);
            }
            return true;
        }
    }
    return false;
}


ts_info video_search(std::string url){
    ts_info video_info;
    std::string response("");
    std::string json_string = "";
    nlohmann::json json;
    if(CURLE_OK == get(url, response)){
        size_t index_start = response.find("window.videoInfo = ");
        size_t index_end = response.find("window.videoResource");
        if (index_start != std::string::npos && index_end != std::string::npos){
            index_start += 19;
            index_end -= 11;
            if(index_end > index_start){
                json_string = response.substr(index_start, index_end - index_start + 1);
            }
            else
                return video_info;
            json = nlohmann::json::parse(json_string);
            video_info.title = json["title"];
            modifies_illegal_title(video_info.title);
            video_info.up = json["user"]["name"];
            long long time_(json["videoList"][0]["uploadTime"]);
            std::time_t time(time_ / 1000);
            char upload_time[25];
            std::strftime(upload_time, 25, "%Y-%m-%d %H:%M:%S", localtime(&time));
            video_info. upload_time = upload_time;
            int duration_millis = json["videoList"][0]["durationMillis"];
            int min = duration_millis / 60000, sec = duration_millis / 1000 % 60;
            char duration[10];
            sprintf(duration, "%.2d:%.2d", min, sec);
            video_info.duration = duration;


            std::string ksplay_info = json["currentVideoInfo"]["ksPlayJson"].get<std::string>();
            nlohmann::json ksplay_json = nlohmann::json::parse(ksplay_info);
            std::string m3u8_file_url = ksplay_json["adaptationSet"]["representation"][0]["url"];
            std::regex r_pre_url("https(.*?)hls/");
            re_search(m3u8_file_url, r_pre_url, video_info.ts_url_prefix);
            std::string ts_file("");
            if(CURLE_OK == get(m3u8_file_url, ts_file)){
                get_ts_url(ts_file, video_info.ts_url);
            }
        }
    }
    return video_info;
}


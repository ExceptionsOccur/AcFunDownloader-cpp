#include "search_thread.h"
#include "http.h"


void search_thread::run(){
    ts_info video_ts_info;
    if(url.size() == 0){
        emit search_error("链接无效");
        return;
    }
//    if(video_search(video_ts_info)){
//        emit return_search_result(video_ts_info);
//        if(file_exists(path, video_ts_info.title))
//            emit file_exists_flag(true);
//        else
//            emit file_exists_flag(false);
//        return;
//    }
    video_ts_info = video_search(url);
    if(video_ts_info.ts_url.size()){
        search_flag = true;
        emit return_search_result(video_ts_info);
//        if(file_exists(path, video_ts_info.title))
//            emit file_exists_flag(true);
//        else
//            emit file_exists_flag(false);
        return;
    }
    search_flag = false;
    emit search_error("获取视频信息失败");
}

void search_thread::set_path(QString p){
    path = p;
}

void search_thread::set_search_flag(bool flag){
    search_flag = flag;
}

bool search_thread::get_search_flag(){
    return search_flag;
}

void search_thread::set_url(std::string s){
    url = s;
}

std::string search_thread::get_url(){
    return url;
}

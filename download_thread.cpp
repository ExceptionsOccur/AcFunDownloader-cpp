#include "download_thread.h"
#include <fstream>


void download_thread::run(){
    if(video_ts_info.search_flag == false){
        if(video_search(video_ts_info)){
            video_ts_info.search_flag = true;
            emit update_data(video_ts_info);
        }

    }
//    if(file_exists(path, video_ts_info.title)){
//        emit file_exists_flag(true);
//        return;
//    }
    std::string file_name = video_ts_info.title + ".mp4";
    modifies_illegal_title(file_name);
    if(path.back() == "/")
        file_name = path.toStdString() + file_name;
    else
        file_name = path.toStdString() + "/" + file_name;

    if(video_ts_info.ts_url.size() != 0){
    // fstream 使用文件名参数值就是打不开！显式值可以(乱码问题!!!我日！)
        std::string name = qstr_to_str(QString::fromStdString(file_name));

        if(stop_flag && downloading_flag){
            pause_point = 0;
            downloading_flag = false;
            pause_flag = false;
            stop_flag = false;
            std::remove(name.c_str());
            emit progress_bar(100);
            return;
        }

        if(file_exists(path, video_ts_info.title)){
            emit file_exists_flag(true);
            return;
        }

        std::fstream out(name.c_str(), std::fstream::binary | std::fstream::app | std::fstream::out);

        if(out.is_open()){
            std::string content("");
            size_t video_p = video_ts_info.ts_url.size();
            float j = static_cast<float>(video_p);
            int k = 0;
            for(int i = pause_point; i < j;){
                content.clear();
                k = static_cast<int>(i / j * 100);
                if(get(video_ts_info.ts_url_prefix + video_ts_info.ts_url[i], content) == CURLE_OK){
                    out.write(content.c_str(), content.size());
                    downloading_flag = true;
                    emit progress_bar(static_cast<int>(k));
                    if(pause_flag){
                        pause_point = i + 1;
                        out.close();
                        return;
                    }
                    if(stop_flag){
                        pause_point = 0;
                        downloading_flag = false;
                        stop_flag = false;
                        out.close();
                        std::remove(name.c_str());
                        emit progress_bar(100);
                        return;
                    }
                    i++;
                }
                else
                    continue;
            }
            emit progress_bar(100);
            downloading_flag = false;
            out.close();
        }
    }
}


void download_thread::set_pause_flag(bool flag){
    pause_flag = flag;
}

void download_thread::set_stop_flag(bool flag){
    stop_flag = flag;
}

void download_thread::set_search_flag(bool flag){
    video_ts_info.search_flag = flag;
}

bool download_thread::get_stop_flag(){
    return  stop_flag;
}

bool download_thread::get_pause_flag(){
    return  pause_flag;
}

bool download_thread::get_downloading_flag(){
    return downloading_flag;
}

std::string download_thread::get_title(){
    return video_ts_info.title;
}

void download_thread::set_data(ts_info info){
//    memcpy(&video_ts_info, &info, sizeof(info));
    video_ts_info.search_flag = info.search_flag;
    if(info.up.size())
        video_ts_info.up.assign(info.up);

    if(info.title.size())
        video_ts_info.title.assign(info.title);

    if(info.url.size())
        video_ts_info.url.assign(info.url);

    if(info.ts_url_prefix.size())
        video_ts_info.ts_url_prefix.assign(info.ts_url_prefix);

    if(info.duration.size())
        video_ts_info.duration.assign(info.duration);

    if(info.upload_time.size())
        video_ts_info.upload_time.assign(info.upload_time);

    if(info.ts_url.size())
        video_ts_info.ts_url.assign(info.ts_url.begin(), info.ts_url.end());

}

void download_thread::set_path(QString p){
    path = p;
}

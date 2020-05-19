#ifndef HTTP_THREAD_H
#define HTTP_THREAD_H
#include <QThread>
#include "ts_info.h"
#include "http.h"


class download_thread : public QThread{
    Q_OBJECT
public:
    download_thread(QObject* parent = nullptr) : QThread(parent){
        memset(&video_ts_info, 0, sizeof (video_ts_info));
        pause_flag = false;
        stop_flag = 0;
        pause_point = 0;
        downloading_flag = false;
    }
    void set_data(ts_info);
    void set_path(QString);
    void set_pause_flag(bool);
    void set_stop_flag(bool);
    bool get_pause_flag();
    bool get_stop_flag();
    bool get_downloading_flag();
    void set_search_flag(bool);
    std::string get_title();
protected:
    void run();
signals:
    void update_data(ts_info);
    void progress_bar(int);
private:
    ts_info video_ts_info;
    QString path;

    bool pause_flag;
    bool stop_flag;
    // 下载状态标志
    bool downloading_flag;
    // 暂停时保存数据
    int pause_point;
};

#endif // HTTP_THREAD_H

#ifndef SEARCH_THREAD_H
#define SEARCH_THREAD_H
#include <QThread>
#include <ts_info.h>

class search_thread : public QThread{
    Q_OBJECT
public:
    search_thread(QObject* parent = nullptr) : QThread(parent){
        search_flag = false;
    }
    void set_url(std::string);
    std::string get_url();
    void set_path(QString);
    bool get_search_flag();
    void set_search_flag(bool);
protected:
    void run();
signals:
    void search_error(std::string);
    void return_search_result(ts_info);
    void file_exists_flag(bool);
private:
    QString path;
    std::string url;
    bool search_flag;
};
#endif // SEARCH_THREAD_H

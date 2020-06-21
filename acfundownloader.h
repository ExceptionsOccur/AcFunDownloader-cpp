
#ifndef ACFUNDOWNLOADER_H
#define ACFUNDOWNLOADER_H

#include <QMainWindow>
#include <QPoint>
#include <regex>
#include "ts_info.h"
#include <download_thread.h>
#include "search_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AcFunDownloader; }
QT_END_NAMESPACE

class AcFunDownloader : public QMainWindow
{
    Q_OBJECT

public:
    AcFunDownloader(QWidget *parent = nullptr);
    ~AcFunDownloader();
    void pop_information(std::string);
    void set_propertis();
    void update_ui();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:

    void on_fold_btn_clicked();

    void on_input_box_textChanged();

    void on_search_btn_clicked();

    void on_download_btn_clicked();

    void on_pause_btn_clicked();

    void on_cancel_btn_clicked();

    void set_data(ts_info);

    void process_s_thread_error(std::string);

    void update_progress_bar(int);

    void d_file_is_exists(bool);

    void s_file_is_exists(bool);


private:
    Ui::AcFunDownloader *ui;
    QString path;
    ts_info video_ts_info;
    search_thread s_thread;
    download_thread d_thread;
    std::string input_text;
    std::string last_video;
    bool pause_flag;
    bool file_exists_flag;
    bool mouse_flag;
    QPoint win_pos;
};
#endif // ACFUNDOWNLOADER_H

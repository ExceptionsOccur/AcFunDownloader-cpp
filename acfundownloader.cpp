#include "acfundownloader.h"
#include "ui_acfundownloader.h"
#include <QHeaderView>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <io.h>
#include "ts_info.h"
#include "http.h"
#include <QMetaType>
#include <QTextCodec>

AcFunDownloader::AcFunDownloader(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AcFunDownloader)
    ,s_thread(new search_thread)
    ,d_thread(new download_thread)
    ,pause_flag(false)
    ,file_exists_flag(false)
{
    qRegisterMetaType<ts_info>("ts_info");
    qRegisterMetaType<std::string>("std::string");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    ui->setupUi(this);
    path = QDir::currentPath();

    connect(&s_thread, SIGNAL(search_error(std::string)), this, SLOT(process_s_thread_error(std::string)), Qt::QueuedConnection);
    connect(&s_thread, SIGNAL(return_search_result(ts_info)), this, SLOT(set_data(ts_info)), Qt::QueuedConnection);
    connect(&s_thread, SIGNAL(file_exists_flag(bool)), this, SLOT(s_file_is_exists(bool)), Qt::QueuedConnection);

    connect(&d_thread, SIGNAL(update_data(ts_info)), this, SLOT(set_data(ts_info)), Qt::QueuedConnection);
    connect(&d_thread, SIGNAL(progress_bar(int)), this, SLOT(update_progress_bar(int)), Qt::QueuedConnection);
    connect(&d_thread, SIGNAL(file_exists_flag(bool)), this, SLOT(d_file_is_exists(bool)), Qt::QueuedConnection);
}

AcFunDownloader::~AcFunDownloader()
{
    delete ui;
//    delete &s_thread;  // 删掉第二次程序就执行不起来，注册表会被改变
}

void AcFunDownloader::pop_information(std::string s){
    QMessageBox::information(this, QString::fromLocal8Bit("好像哪里不对"), str_to_qstr(s));
}

void AcFunDownloader::d_file_is_exists(bool flag){
    if(flag == true)
        pop_information("文件已存在");
}

void AcFunDownloader::s_file_is_exists(bool flag){
    if(flag == true)
        file_exists_flag = true;
    else
        file_exists_flag = false;
}

void AcFunDownloader::update_ui(){
    ui->table_widget->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(video_ts_info.title)));
    ui->table_widget->setItem(1, 1, new QTableWidgetItem(QString::fromStdString(video_ts_info.up)));
    ui->table_widget->setItem(2, 1, new QTableWidgetItem(QString::fromStdString(video_ts_info.upload_time)));
    ui->table_widget->setItem(3, 1, new QTableWidgetItem(QString::fromStdString(video_ts_info.duration)));
}

void AcFunDownloader::set_propertis(){
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowOpacity(0.9);
    ui->table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_widget->rowHeight(30);
    ui->table_widget->setColumnWidth(0, 60);
    ui->table_widget->setColumnWidth(1, 338);
    ui->table_widget->setItem(0, 0, new QTableWidgetItem(QString::fromLocal8Bit("标题")));
    ui->table_widget->setItem(1, 0, new QTableWidgetItem(QString::fromLocal8Bit("up主")));
    ui->table_widget->setItem(2, 0, new QTableWidgetItem(QString::fromLocal8Bit("上传时间")));
    ui->table_widget->setItem(3, 0, new QTableWidgetItem(QString::fromLocal8Bit("时长")));
    for (int row = 0; row < 4; row ++) {
        ui->table_widget->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    }

    connect(ui->close_btn, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->mini_btn, SIGNAL(clicked()), this, SLOT(showMinimized()));
//    connect(ui->search_btn, SIGNAL(clicked()), this, SLOT(on_search_btn_clicked()));
    ui->fold_btn->setText(limit_path_len(path));
}


void AcFunDownloader::set_data(ts_info info){
    memcpy(&video_ts_info, &info, sizeof(info));
    d_thread.set_data(info);
    video_ts_info.search_flag = true;
    last_video = video_ts_info.url;
    update_ui();
}

void AcFunDownloader::process_s_thread_error(std::string s){
    pop_information(s);
}

void AcFunDownloader::update_progress_bar(int i){
    ui->progress_bar->setValue(i);
}

void AcFunDownloader::on_fold_btn_clicked()
{
    QString get_dir = QFileDialog::getExistingDirectory(this, "选择保存文件夹", path);
    path = get_dir;
    if(get_dir.length()){
        ui->fold_btn->setText(limit_path_len(get_dir));
    }
}

void AcFunDownloader::on_cancel_btn_clicked(){
    if(d_thread.get_downloading_flag() == false){
        pop_information("当前没有下载任务");
        return;
    }
    ui->pause_btn->setText(str_to_qstr("暂停"));
    d_thread.set_stop_flag(true);
    if(d_thread.get_pause_flag()){
        d_thread.start();
    }
}

void AcFunDownloader::on_pause_btn_clicked(){
    if(d_thread.get_downloading_flag() == false){
        pop_information("当前没有下载任务");
        return;
    }
    if(d_thread.get_pause_flag()){ // 继续
        ui->pause_btn->setText(str_to_qstr("暂停"));
        pause_flag = false;
        d_thread.set_pause_flag(false);
        d_thread.start();
    }
    else{   // 暂停
        ui->pause_btn->setText(str_to_qstr("继续"));
        pause_flag = true;
        d_thread.set_pause_flag(true);
    }
}

void AcFunDownloader::on_input_box_textChanged(){
    input_text = get_ac(qstr_to_str(ui->input_box->text()));
    if(input_text != "url error" && last_video != input_text){
        s_thread.set_search_flag(false);
        last_video = input_text;
    }
}

void AcFunDownloader::on_search_btn_clicked(){
//    std::string ac_num = get_ac(qstr_to_str(ui->input_box->text()));  
    bool flag(s_thread.get_search_flag());
    std::string ac_num = get_ac(input_text);
    if(d_thread.get_downloading_flag()){
        pop_information("当前正在下载");
        return;
    }
    if(ac_num == "url error"){
        pop_information("输入不合法");
        return;
    }
    if(flag){
        pop_information("与上一次输入相同");
        return;
    }
    memset(&video_ts_info, 0, sizeof (video_ts_info));     // 再次进入video_ts_info会变成未初始化状态，赋值会造成段错误(鬼知道怎么回事!)
    video_ts_info.url = "https://www.acfun.cn/v/ac" + ac_num;
    s_thread.set_url(video_ts_info.url);
    s_thread.set_search_flag(flag);
    s_thread.start();
}


void AcFunDownloader::on_download_btn_clicked(){
    std::string ac_num = get_ac(input_text);

    if(d_thread.get_downloading_flag()){
        pop_information("当前正在下载");
        return;
    }
    if(ac_num == "url error"){
        pop_information("输入不合法");
        return;
    }
    if(file_exists(path, d_thread.get_title())){
        pop_information("文件已存在");
        return;
    }
    memset(&video_ts_info, 0, sizeof (video_ts_info));  // 再次进入video_ts_info会变成未初始化状态，赋值会造成段错误(鬼知道怎么回事!)
    video_ts_info.url.assign("https://www.acfun.cn/v/ac" + ac_num);
    d_thread.set_data(video_ts_info);
    d_thread.set_search_flag(s_thread.get_search_flag());
    d_thread.set_path(path);
    d_thread.start();
}

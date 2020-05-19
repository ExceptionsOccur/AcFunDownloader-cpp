#include "acfundownloader.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AcFunDownloader w;
    w.set_propertis();
    w.show();
    return a.exec();
}

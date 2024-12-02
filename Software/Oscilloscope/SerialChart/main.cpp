#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle("Fusion");


    QFile file(":/qss/flatwhite.css");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QString::fromUtf8(file.readAll());
        a.setStyleSheet(styleSheet);
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SerialChart_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // 设置应用程序的显示名称
    a.setApplicationName("TENG&Cap");
    a.setApplicationDisplayName("TENG&Cap");
    MainWindow w;
    w.show();
    return a.exec();
}

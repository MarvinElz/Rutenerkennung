#include "mainwindow.h"
#include <QApplication>
#include <iostream>

#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <string>

#include <QtCore>
#include <QFile>
#include <QDebug>
#include <QtXml/QtXml>

#include "config_manager.h"

QStringList options;

struct SBD_Config SBD_config = {false, false, false, false ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

int main(int argc, char *argv[])
{
    QFile *file = new QFile("/home/pi/Rutenerkennung/Config.xml");
    QDomDocument xml_doc;

    if( !file->open(QIODevice::ReadOnly | QIODevice::Text) ){
        qDebug() << "Datei konnte nicht geoeffnet werden";
    }else{
        xml_doc.setContent(file);
        // TODO: SBD_config füllen mit Daten aus .xml Datei.
    }

    QApplication a(argc, argv);
    MainWindow w( &SBD_config );

    w.show();

    return a.exec();
}

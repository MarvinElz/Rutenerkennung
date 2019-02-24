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

#include "Beobachter.h"
#include "Kommunikation.h"
#include "Rutenerkennung.h"
#include "Videoquelle.h"

#include "config_manager.h"

// QStringList options;

struct SBD_Config SBD_config = {false, false, false, false ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

QDomDocument xml_doc;


int main(int argc, char *argv[])
{
    QFile *file = new QFile("/home/pi/Rutenerkennung/Rutenerkennung/Config.xml");

    if( !file->open(QIODevice::ReadOnly | QIODevice::Text) ){
        qDebug() << "Datei konnte nicht geoeffnet werden";
    }else{
        xml_doc.setContent(file);
        // TODO: SBD_config füllen mit Daten aus .xml Datei.
    }

    Videoquelle *v = new Videoquelle( &xml_doc );

    Rutenerkennung *r = new Rutenerkennung( &xml_doc );

    Beobachter *b = new Beobachter( &xml_doc );

    Kommunikation *k = new Kommunikation(&xml_doc);

    QObject::connect( b, SIGNAL(FahreAnPositionUndWirfAus(Vec2i)), k, SLOT(FahreAnPositionUndWirfAus(Vec2i) ));
    QObject::connect( k, SIGNAL(BefehlBearbeitet(void)), b, SLOT(BefehlBearbeitet(void) ));
    QObject::connect( v, SIGNAL(NeuesBild( Mat* )), r, SLOT(NeuesBild( Mat* )) );
    QObject::connect( r, SIGNAL(HoleNeuesBild()), v, SLOT(HoleNeuesBild()));
    // ErkannteStecklinge

    QMetaObject::invokeMethod( v, "HoleNeuesBild", Qt::QueuedConnection );

    QApplication a(argc, argv);
    MainWindow w( &SBD_config, &xml_doc );

    w.show();

    return a.exec();
}

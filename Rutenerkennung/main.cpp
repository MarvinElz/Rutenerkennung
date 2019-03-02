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

#include <thread>

//#include "config_manager.h"

// QStringList options;

//struct SBD_Config SBD_config = {false, false, false, false ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

QDomDocument xml_doc;

QApplication *a;

void init(){
    a->exec();
}

int main(int argc, char *argv[])
{
    QFile *file = new QFile("/home/pi/Rutenerkennung/Rutenerkennung/Config.xml");

    if( !file->open(QIODevice::ReadOnly | QIODevice::Text) ){
        qDebug() << "Datei konnte nicht geoeffnet werden";
    }else{
        xml_doc.setContent(file);        
    }

    Videoquelle *v = new Videoquelle( &xml_doc );

    Rutenerkennung *r = new Rutenerkennung( &xml_doc );

    Beobachter *b = new Beobachter( &xml_doc );

    Kommunikation *k = new Kommunikation(&xml_doc);

     QApplication a(argc, argv);

    QThread* v_thread = new QThread;
    QThread* k_thread = new QThread;
    QThread* r_thread = new QThread;
    QThread* b_thread = new QThread;

    v->moveToThread(v_thread);
    k->moveToThread(k_thread);
    r->moveToThread(r_thread);
    b->moveToThread(b_thread);

    QObject::connect( r_thread, SIGNAL( started() ), r, SLOT(run()) );
    QObject::connect( r, SIGNAL(finished()), r_thread, SLOT(quit()) );

    // Kommunikation in Thread auslagern
    QObject::connect(k_thread, SIGNAL( started() ), k, SLOT(run()) );
    QObject::connect(k, SIGNAL (finished()), k_thread, SLOT (quit()));

    // Videoquelle in Thread auslagern
    QObject::connect(v_thread, SIGNAL (started()), v, SLOT (ReadImage()));
    QObject::connect(v, SIGNAL (finished()), v_thread, SLOT (quit()));

    // Restlichen Signale verbinden
    // Rutenerkennung löst Ausgabe des letzten Bildes aus
    QObject::connect( r, SIGNAL(HoleNeuesBild()),                   v, SLOT(HoleNeuesBild()), Qt::DirectConnection);
    // Videoquelle gibt aktuelles Bild zurück
    QObject::connect( v, SIGNAL(NeuesBild( Mat* )),                 r, SLOT(NeuesBild( Mat* )), Qt::DirectConnection);
    // Rutenerkennung gibt die im letzten Bild erkannten Ruten an den Beobachter
    QObject::connect( r, SIGNAL(ErkannteStecklinge( vector<Vec2i>)),b, SLOT(ErkannteStecklinge( vector<Vec2i>) ), Qt::DirectConnection);
    // Beobachter gibt die Position der geeignetsten Rute aus (in Bildkoordinaten)
    QObject::connect( b, SIGNAL(FahreAnPositionUndWirfAus(Vec2i*)),  k, SLOT(FahreAnPositionUndWirfAus(Vec2i*) ), Qt::DirectConnection);
    // Kommunikation versendet den G-Code-Befehl und wartet, bis dieser abgearbeitet wurde und löst im Anschluss daran
    // das Signal BefehlBearbeitet aus
    QObject::connect( k, SIGNAL(BefehlBearbeitet()),                b, SLOT(BefehlBearbeitet() ));

    v_thread->start();
    k_thread->start();
    r_thread->start();
    b_thread->start();

    //std::thread t(init);

    MainWindow w( v, r, b, &xml_doc );

    w.show();

    QObject::connect( r, SIGNAL(Ergebnis_BW(cv::Mat *)), &w, SLOT(ShowImage(cv::Mat *)));
    QObject::connect( b, SIGNAL(Valide_Stecklinge( vector<Steckling*>* )), &w, SLOT(Valide_Stecklinge( vector<Steckling*>* )));

    //usleep(3000*1000);

    // Warum bringt dieser Teil das Programm durcheinander?
    // Bearbeitung starten
    //if( !QMetaObject::invokeMethod( v, "HoleNeuesBild"   , Qt::DirectConnection ) ){        //DirectConnection
    //    cout << "Invoked HoleNeuesBild fehlgeschlagen" << endl;
    //    exit(0);
    //}
    //if( !QMetaObject::invokeMethod( b, "BefehlBearbeitet", Qt::QueuedConnection ) ){
    //    cout << "Invoked BefehlBearbeitet fehlgeschlagen" << endl;
    //    exit(0);
    //}

    a.exec();

    //std::thread t1(task1, "Hello");

    //t.join();
    /*
    while(1){
        if( t.joinable() )
            cout << "Thread zu Ende" << endl;
        usleep(10000);
    }
    */

}

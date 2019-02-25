#ifndef BEOBACHTER_H_
#define BEOBACHTER_H_

#include "Steckling.h"
#include <iostream>
#include <QObject>
#include <QtXml/QtXml>
#include <QMetaObject>

#include <unistd.h>   // usleep

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Beobachter: public QObject{

    Q_OBJECT

public:
    //explicit Beobachter(QDomDocument *xml_doc);// : QObject(parent) {};
    Beobachter( QDomDocument *xml_doc );
    virtual ~Beobachter(){}

    private:
        vector<Steckling*> m_stecklinge;

        // Anzahl der aufeinanderfolgenden Bilder, in der der Steckling
        // erkannt werden muss
        int m_Min_Wiederkennungen = 10;

        // Zulässige Abweichung der Position eines Stecklings,
        // da die Positionserkennung leichtes Rauschen ausweisen wird
        int m_Max_Abweichung = 10;

        Vec2i m_pos_Aktuator = Vec2i(0, 0);



    public slots:
        void ErkannteStecklinge( vector<Vec2i> points );
        void BefehlBearbeitet();

    signals:
        void FahreAnPositionUndWirfAus(Vec2i *p);
};

#endif

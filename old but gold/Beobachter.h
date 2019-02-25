#ifndef BEOBACHTER_H_
#define BEOBACHTER_H_

#include "Steckling.h"
#include <iostream>
#include <QObject>
#include <QtXml/QtXml>

using namespace std;
using namespace cv;

class Beobachter : public QObject{

	Q_OBJECT

	private:
		vector<*Steckling> m_stecklinge;

		// Anzahl der aufeinanderfolgenden Bilder, in der der Steckling
		// erkannt werden muss
		int m_Min_Wiederkennungen = 10;

		// Zulässige Abweichung der Position eines Stecklings,
		// da die Positionserkennung leichtes Rauschen ausweisen wird
		int m_Max_Abweichung = 10;

		Vec2i m_pos_Aktuator(0, 0);

	public:
		Beobachter( QDomDocument *xml_doc );

	slots:
		ErkannteStecklinge( vector<Vec2i> points );	
		BefehlBearbeitet();
	
	signals:
		FahreAnPositionUndWirfAus(Vec2i p);
}

#endif
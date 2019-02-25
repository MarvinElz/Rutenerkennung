#ifndef KOMMUNIKATION_H_
#define KOMMUNIKATION_H_

#include <iostream>
#include <string>

#include <QObject>
#include <QtXml/QtXml>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

using namespace std;

class Kommunikation : public QObject{

	Kommunikation( QDomDocument *xml_doc );
	
public:
	run();
	stop();

	signal:
		BefehlBearbeitet();

	slot:
		FahreAnPositionUndWirfAus(Vec2i p);

	private:
		// Rotationsmatrix von B nach R
		// Enthält den Maßstab
		Mat m_R_B_R = Mat::zeros(2, 2, CV_32F);

		// Translationsvektor von Ursprung (Org = Origin) K-System nach B-System dargestellt im
		// K-System
		Mat m_RpB_Org = Mat::zeros(2,1,CV_32F);

		bool m_running = true;

		uint m_baudrate = 115200;
		const string m_serialport = "/dev/ttyACM0";

		boost::asio::io_service io;
		boost::asio::serial_port m_serial(io);


}

#endif
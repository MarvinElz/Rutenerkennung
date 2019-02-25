#include "Kommunikation.h"

Kommunikation::Kommunikation( QDomDocument *xml_doc ){
	// Parameter der seriellen Schnittstelle einlesen
	QDomNodeList nodeList = docElem.elementsByTagName("Kommunikation"); 
	if( !nodeList.isEmpty() ){
		QDomElement e = nodeList.at(0).toElement();
		//////
		nodeList = e.elementsByTagName("Baudrate");
		if( !nodeList.isEmpty() ){
			m_baudrate = nodeList.at(0).toElement().text().toInt();
			cout << "Verwende: Baudrate: " << m_baudrate << endl;
		}else{
			cout << "Nicht vorhandenes Feld: 'Baudrate' in xml" << endl;
			cout << "Verwende default-Wert: " << m_baudrate << endl;
		}
		nodeList = e.elementsByTagName("Serial");
		if( !nodeList.isEmpty() ){
			m_serialport = nodeList.at(0).toElement().text().constData();;
			cout << "Verwende: Serial: " << m_serialport << endl;
		}else{
			cout << "Nicht vorhandenes Feld: 'Serial' in xml" << endl;
			cout << "Verwende default-Wert: " << m_serialport << endl;
		}
	}

	m_serial.open( m_serialport );
	if( !m_serial.is_open() ){
		cout << m_serialport << " konnte nicht geoffnet werden." << endl;		
	}else{
		boost::asio::serial_port_base::baud_rate baud_option(m_baudrate);
		m_serial.set_option(baud_option); // set the baud rate after the port has been opened
   }

   // Rotationsmatrix und Translationsvektor aus XML einlesen
   // Parsen von R_B_R
	nodeList = cam_element.elementsByTagName("Rotation");
	if( !nodeList.isEmpty() ){
		QDomElement rotation_element = nodeList.at(0).toElement();
		qInfo() << "\t\t" << rotation_element.tagName();
		nodeList = rotation_element.elementsByTagName("r");
		for(int ii = 0;ii < nodeList.count(); ii++){
			QDomElement mat = nodeList.at(ii).toElement();
			char *mat_entry = mat.attribute("id").toLocal8Bit().data();
			qInfo() << "\t\t\tRead Mat_Entry " << mat.attribute("id");
			m_R_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1') = mat.text().toFloat();
		}
	}
	cout << "R_B_R" << m_R_B_R << endl;

	// Parsen von R_B_Org in R_B_T
	nodeList = cam_element.elementsByTagName("Translation");
	if( !nodeList.isEmpty() ){
		QDomElement trans_element = nodeList.at(0).toElement();
		qInfo() << "\t" << trans_element.tagName();
		if( !nodeList.isEmpty() ){
			QDomElement calib_element = nodeList.at(0).toElement();
			qInfo() << "\t\t" << calib_element.tagName();
			nodeList = calib_element.elementsByTagName("Point");
			QDomElement point = nodeList.at(0).toElement();
			m_RpB_Org.at<float>(0,0) = point.elementsByTagName("x").at(0).toElement().text().toFloat();
			m_RpB_Org.at<float>(1,0) = point.elementsByTagName("y").at(0).toElement().text().toFloat();
		}
	}
	cout << "RpB_Org" << m_RpB_Org << endl;
}



Kommunikation::FahreAnPositionUndWirfAus(Vec2i Bp){
	// Umrechnung von B-System in K-System
	Vec2f Kp = m_R_B_R * Bp + m_RpB_Org;	

	// Erstellen des G-Code-Strings
	string G_Code = "G1 X" + std::to_string(Kp[0]) + " Y" + std::to_string(Kp[1]);   
	cout << "GCode:" << G_Code; 	

	if( m_serial.is_open() ){
		boost::asio::write(m_serial, boost::asio::buffer(G_Code, G_Code.length()) );
	}
}

void Kommunikation::run(){
	m_running = true;
	boost::asio::streambuf b;
	uint gelesene_Bytes = 0;
	while(m_running){
		if( m_serial.is_open() ){
			boost::asio::async_read_until(serialport, b, "DONE\n"); // anpassen an Ausgabe vom Atmega
			cout << b << endl;
			emit BefehlBearbeitet;
		}else{
			usleep(1000);
		}
	}
	///cout << "read " << byte_count << " bytes" << endl;
  	//memcpy( buffer, boost::asio::buffer_cast<const void*>(b.data()), b.size() );
}

void Kommunikation::stop(){
	m_running = false;
}
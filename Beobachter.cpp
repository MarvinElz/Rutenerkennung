#include "Beobachter.h"

Beobachter::Beobachter( QDomDocument *xml_doc ){

	// Einlesen von Membervariablen
	QDomElement docElem = m_xml_doc->documentElement(); 	// Rutenerkennung

	QDomNodeList nodeList = docElem.elementsByTagName("Erkennung"); 
	if( !nodeList.isEmpty() ){
		QDomElement e = nodeList.at(0).toElement();
		//////
		nodeList = e.elementsByTagName("Min_Wiedererkennungen");
		if( !nodeList.isEmpty() ){
			m_Min_Wiederkennungen = nodeList.at(0).toElement().text().toInt();
			cout << "Verwende: Min_Wiederkennungen: " << m_Min_Wiederkennungen << endl;
		}else{
			cout << "Nicht vorhandenes Feld: 'Min_Wiedererkennungen' in xml" << endl;
		}
		//////
		nodeList = e.elementsByTagName("Max_Abweichung");
		if( !nodeList.isEmpty() ){
			m_Max_Abweichung = nodeList.at(0).toElement().text().toInt();
			cout << "Verwende: Max_Abweichung: " << m_Max_Abweichung << endl;
		}else{
			cout << "Nicht vorhandenes Feld: 'm_Max_Abweichung' in xml" << endl;
		}
	}else{
		cout << "Nicht vorhandenes Feld: 'Erkennung' in xml" << endl;
	}
}

void Beobachter::ErkannteStecklinge( vector<Vec2i> points ){	
	// Vergleich der erhaltenen Koordinaten der erkannten Stecklinge
	// mit den in m_stecklinge gespeichertern

	vector<*Steckling> copy_stecklinge;
	vector<Vec2i>::iterator    i = points.begin();
	//vector<Stecking>::iterator j = m_stecklinge.begin();

	// Achtung: Iterator über vector, während Elemente gelöscht werden
	while( !points.empty() && i != points.end() ){
		vector<Steckling>::iterator j = m_stecklinge.begin();
		while( !m_stecklinge.empty() && j != m_stecklinge.end() ){
			if( cv::norm( *i - *j->pos ) < (float) m_Max_Abweichung ){
				copy_stecklinge.push_back( m_stecklinge.erase( j ) );
				*j->m_plausibility++;
				i = points.erase( i );
				break;
			}
			j++;
		}
		i++;		
	}

	// In points sind nun die Positionen erhalten, die zum ersten mal detektiert wurden.
	// Diese werden nun zu copy_steckline hinzufügen
	for (vector<Vec2i>::iterator i = points.begin() ; i != points.end(); ++i){
		// neuen Steckling erzeugen und der Liste hinzufügen
		copy_stecklinge.push_back( new Steckling( *i ) );
	}

	// In m_stecklinge sind nun die Stecklinge gespeichert, die im aktuellen Bild nicht erkannt wurden
	// Alle löschen
	for (vector<Steckling>::iterator j = m_stecklinge.begin() ; j != m_stecklinge.end(); ++j){
		delete j;
	}

	// m_stecklinge wieder beschreiben
	m_stecklinge = copy_stecklinge;	
}

// Wird aufgerufen, wenn der Kommunikator (Kommunikation.cpp) die Nachricht von der CNC-Steuerung erhält,
// das der letzte Befehl ausgeführt wurde. Daraufhin wird der günstigste Steckling gesucht, der als nächstes
// ausgeworfen werden soll
void Beobachter::BefehlBearbeitet(){
	// m_stecklinge nach dem optimalen Steckling durchsuchen, der die Anforderungen erfüllt und 
	// die geringste Distanz zum Aktuator hat

	Steckling *best = NULL;
	float best_distance = 1000000;

	if( m_stecklinge.empty() ){
		cout << "Keine Stecklinge gefunden:" << __FUNCTION__ << "m_stecklinge.empty()" << endl;
		goto emit_same_signal;
	}

	for (vector<*Steckling>::iterator j = m_stecklinge.begin() ; j != m_stecklinge.end(); ++j){
		if( *j->m_plausibility < m_Min_Wiederkennungen )
			continue;
		float temp;
		if( (temp = norm( m_pos_Aktuator - *j->pos )) < best_distance ){
			best_distance = temp;
			best = *j;
		}		
	}

	// Keiner der Stecklinge erfüllt die Anforderung
	if( best == NULL ){
		cout << "Kein Optimum gefunden" << endl;
		goto emit_same_signal;
	}
	
	m_pos_Aktuator = *j->pos;
	emit FahreAnPositionUndWirfAus( m_pos_Aktuator );
	return;

emit_same_signal:
	// was passiert, wenn CNC-Befehl bearbeitet wurde, aber kein Steckling valid ist?
	// kurz warten und BefehlBearbeitet erneut aufrufen
	usleep(100000);
	QMetaObject::invokeMethod( this, "BefehlBearbeitet", Qt::QueuedConnection );
	return;
}


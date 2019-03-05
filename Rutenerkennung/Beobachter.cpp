#include "Beobachter.h"
//  : QObject(parent)
Beobachter::Beobachter( QDomDocument *xml_doc ){

    // Einlesen von Membervariablen
    QDomElement docElem = xml_doc->documentElement(); 	// Rutenerkennung

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
    //cout << "ErkannteStecklinge" << endl;
    //mutex.lock();
    // Vergleich der erhaltenen Koordinaten der erkannten Stecklinge
    // mit den in m_stecklinge gespeichertern

    vector<Steckling*> copy_stecklinge;
    vector<Vec2i>::iterator    i = points.begin();
    //vector<Stecking>::iterator j = m_stecklinge.begin();

    mutex.lock();
    // Achtung: Iterator über vector, während Elemente gelöscht werden
    while( !points.empty() && i != points.end() ){
        vector<Steckling*>::iterator j = m_stecklinge.begin();
        while( !m_stecklinge.empty() && j != m_stecklinge.end() ){
            assert( *j != NULL );
            if( cv::norm( *i, (*j)->pos ) < (double) m_Max_Abweichung ){
                // cout << "Wiedererkannt" << endl;
                (*j)->m_plausibility++;
                copy_stecklinge.push_back( *j );
                m_stecklinge.erase( j );
                i = points.erase( i );
                //copy vector of shared_ptr
                //i++;    // Test
                goto end;
            }
            j++;
        }
        i++;
  end:
        ;
    }
    // In points sind nun die Positionen erhalten, die zum ersten mal detektiert wurden.
    // Diese werden nun zu copy_steckline hinzufügen
    if( !points.empty() ){
        for (vector<Vec2i>::iterator i = points.begin() ; i != points.end(); ++i){
            // neuen Steckling erzeugen und der Liste hinzufügen
            //cout << "Neuen Steckling erkannt" << endl;
            copy_stecklinge.push_back( new Steckling( *i ) );
        }
    }
    //mutex.unlock();
    // In m_stecklinge sind nun die Stecklinge gespeichert, die im aktuellen Bild nicht erkannt wurden
    // Alle löschen
    for (vector<Steckling*>::iterator j = m_stecklinge.begin() ; j != m_stecklinge.end(); ++j){
        //cout << "Nicht wiedererkannte Stecklinge loeschen" << endl;
        if ( *j != NULL ){
            delete *j;
            *j = NULL;
        }else{
            assert(false);
        }

    }

    // m_stecklinge wieder beschreiben

    m_stecklinge = copy_stecklinge;
    mutex.unlock();
    //cout << "Kopiere m_stecklinge" << endl;
    //cout << "Valide Stecklinge: " << m_stecklinge.size() << endl;
    emit Valide_Stecklinge( &m_stecklinge );
}

// Wird aufgerufen, wenn der Kommunikator (Kommunikation.cpp) die Nachricht von der CNC-Steuerung erhält,
// das der letzte Befehl ausgeführt wurde. Daraufhin wird der "günstigste" Steckling gesucht, der als nächstes
// ausgeworfen werden soll
void Beobachter::BefehlBearbeitet(){
    //cout << "BefehlBearbeitet" << endl;
    // m_stecklinge nach dem optimalen Steckling durchsuchen, der die Anforderungen erfüllt und
    // die geringste Distanz zum Aktuator hat

    //Steckling *best = NULL;
    vector<Steckling*>::iterator best;
    //best = NULL;
    float best_distance = 1000000;
    mutex.lock();
    if( m_stecklinge.empty() ){
        //cout << "Keine Stecklinge gefunden: " << __FUNCTION__ << " m_stecklinge.empty()" << endl;
        goto release_mutex;
    }

    for (vector<Steckling*>::iterator j = m_stecklinge.begin() ; j != m_stecklinge.end(); ++j){
        if( (*j)->m_plausibility < m_Min_Wiederkennungen )
            continue;
        float temp;
        if( (temp = norm( m_pos_Aktuator - (*j)->pos )) < best_distance ){
            best_distance = temp;
            best = j;  //best = *j;
        }
    }

    // Keiner der Stecklinge erfüllt die Anforderung
    if( best_distance == 1000000 ){
        //cout << "Kein Steckling erfuellt die Anforderung" << endl;
        goto release_mutex;
    }

    m_pos_Aktuator = (*best)->pos;
    cout << "Wirf aus: (" << m_pos_Aktuator[0] << ", " << m_pos_Aktuator[1] << ")" << endl;
    emit FahreAnPositionUndWirfAus( &m_pos_Aktuator );

    delete *best;
    m_stecklinge.erase(best);


release_mutex:
    // was passiert, wenn CNC-Befehl bearbeitet wurde, aber kein Steckling valide ist?
    // kurz warten und BefehlBearbeitet erneut aufrufen
    //usleep(100000);
    mutex.unlock();
    //QMetaObject::invokeMethod( this, "BefehlBearbeitet", Qt::QueuedConnection );
    return;
}


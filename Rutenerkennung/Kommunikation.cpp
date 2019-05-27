#include "Kommunikation.h"

// Fahre an ermittelte Position und wirf "besten" Steckling aus
// Zustandsdaten können optional Informationen enthalten, müssen jedoch beim
// erstellen des Zustands mit übergeben werden.
void WirfAus( void *Zustandsdaten ){
    cout << "Ausführen von: " << __FUNCTION__ << endl;
    Kommunikation *kom = (Kommunikation*)Zustandsdaten;
   // sende_ueber_serielle_schnittstelle muss dann in Kommunikation unter public: stehen
   kom->sende_ueber_serielle_schnittstelle("Test");
   // oder direkt kom->m_serial verwenden.
}


//  Definieren von Funktionen, die Zustandsverhalten beschreiben
void WarteAufTrigger(void *Zustandsdaten){
    cout << "Ausführen von: " << __FUNCTION__ << endl;
    // nichts tun....
}

// Definieren von Funktionen, die Bedingungen zum Schalten der Transition beschreiben
bool TriggerSignalAusgelöst(void *Zustandsdaten){
    // Trigger wurde ausgelöst, wenn TRIGGER_PIN auf Ground/0V gezogen wurde.
    return !digitalRead(TRIGGER_PIN); 
}

Kommunikation::Kommunikation( QDomDocument *xml_doc ){

    // Einlesen von Membervariablen
    QDomElement docElem = xml_doc->documentElement(); 	// Rutenerkennung
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
            m_serialport = nodeList.at(0).toElement().text().toStdString();
            cout << "Verwende: Serial: " << m_serialport << endl;
        }else{
            cout << "Nicht vorhandenes Feld: 'Serial' in xml" << endl;
            cout << "Verwende default-Wert: " << m_serialport << endl;
        }
    }

    m_serial = open( m_serialport.c_str(), O_RDWR| O_NOCTTY );
    
    if( m_serial < 0 ){
        cout << m_serialport << " konnte nicht geoffnet werden." << endl;
        //exit(0);
    }else{
        struct termios term;
        tcgetattr(m_serial, &term);
        speed_t baudrate = B115200;
        switch( m_baudrate ){
            case 9600  : baudrate = B9600; break;
            case 19200 : baudrate = B19200; break;
            case 38400 : baudrate = B38400; break;
            case 57600 : baudrate = B57600; break;
            case 115200: baudrate = B115200; break;
            case 230400: baudrate = B230400; break;
            case 460800: baudrate = B460800; break;
            default: cout << "Baudrate " << m_baudrate << " nicht unterstuetzt, nutze 115200" << endl; 
        }
        cfsetospeed(&term,baudrate);
        cfsetispeed(&term,baudrate);
        tcsetattr(m_serial, TCSANOW, &term);
   }

   // TODO: Homing der Kinematik (und was sonst noch ansteht) ausführen
   // "G28 X Y"

   // Rotationsmatrix und Translationsvektor aus XML einlesen
   // Parsen von K_B_R
    nodeList = docElem.elementsByTagName("Camera");
    if( !nodeList.isEmpty() ){
        QDomElement cam_element = nodeList.at(0).toElement();
        nodeList = cam_element.elementsByTagName("Rotation");
        if( !nodeList.isEmpty() ){
            QDomElement rotation_element = nodeList.at(0).toElement();
            qInfo() << "\t\t" << rotation_element.tagName();
            nodeList = rotation_element.elementsByTagName("r");
            for(int ii = 0;ii < nodeList.count(); ii++){
                QDomElement mat = nodeList.at(ii).toElement();
                char *mat_entry = mat.attribute("id").toLocal8Bit().data();
                qInfo() << "\t\t\tRead Mat_Entry " << mat.attribute("id");
                m_K_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1') = mat.text().toFloat();
            }
        }

        cout << "K_B_R" << endl;
        cout << m_K_B_R << endl;

        m_B_K_R  = m_K_B_R.inv();
        cout << "m_B_K_B" << endl;
        cout << m_B_K_R << endl;

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
                m_KpB_Org.at<float>(0,0) = point.elementsByTagName("x").at(0).toElement().text().toFloat();
                m_KpB_Org.at<float>(1,0) = point.elementsByTagName("y").at(0).toElement().text().toFloat();
            }
        }
        cout << "KpB_Org" << endl;
        cout << m_KpB_Org << endl;

        m_BpK_Org = m_B_K_R * (- m_KpB_Org);
        cout << "BpK_Org" << endl;
        cout << m_BpK_Org << endl;
    }

    // WiringPi initialieren
    wiringPiSetup();

    pinMode(TRIGGER_PIN, INPUT);
    pullUpDnControl (TRIGGER_PIN, PUD_UP) ;
    // Pin auf Eingang, Pullup
    
}

Vec2i Kommunikation::K_to_B(const Vec2f &Kp){
    Mat Kp_mat = Mat(2,1,CV_32F);
    Kp_mat.at<float>(0,0) = Kp[0];
    Kp_mat.at<float>(0,1) = Kp[1];
    Mat Bp_mat = m_B_K_R * Kp_mat + m_BpK_Org;
    return Vec2i( Bp_mat.at<float>(0,0), Bp_mat.at<float>(0,1) );
}

Vec2f Kommunikation::B_to_K(const Vec2i &Bp){
    Mat Bp_mat = Mat(2,1,CV_32F);
    Bp_mat.at<float>(0,0) = Bp[0];
    Bp_mat.at<float>(0,1) = Bp[1];
    Mat Kp_mat = m_K_B_R * Bp_mat + m_KpB_Org;
    return Vec2f( Kp_mat.at<float>(0,0), Kp_mat.at<float>(0,1) );
}

Vec2i *Bp_BesterSteckling;

void Kommunikation::FahreAnPositionUndWirfAus(Vec2i *Bp){
    //cout << "FahreAnPositionUndWirfAus" << endl;
    // Umrechnung von B-System in K-System
Bp_BesterSteckling = Bp;
neuerSteckling = true;
/*

    Mat Bp_mat = Mat(2,1,CV_32F);
    Bp_mat.at<float>(0,0) = (*Bp)[0];
    Bp_mat.at<float>(0,1) = (*Bp)[1];
    Mat Kp_mat = m_K_B_R * Bp_mat + m_KpB_Org;
    Vec2f Kp = Vec2f( Kp_mat.at<float>(0,0), Kp_mat.at<float>(0,1) );

    // Erstellen des G-Code-Strings
    string G_Code = "G1 X" + std::to_string(Kp[0]) + " Y" + std::to_string(Kp[1]);
    std::replace( G_Code.begin(), G_Code.end(), ',', '.' );
    cout << "GCode:" << G_Code << "\n";

    // TODO: Befehl anfügen, der den Auswurfmechanismus ansteuert.
    m_mutex.lock();
    if( m_serial >= 0 ){
        write(m_serial, G_Code.c_str(), (size_t)G_Code.length());
    }else{
        cout << "Konnte Sollposition nicht an CNC-Steuerung uebermitteln, m_serial nicht (mehr) geoeffnet" << endl;
    }    
    m_mutex.unlock();
    */
}


void Kommunikation::run(){

    // Erstellen des Zustandsautomaten
    // Erklärung:
    // Parameter:   On_Enter, 
    //              On_Stay, 
    //              On_Exit, 
    //              On_Enter beim ersten Ausführen des Zustandes ausführen?, 
    //              Zeiger auf optionale Zustandsdaten
    Zustand S1 = {NULL, WarteAufTrigger, NULL , true };
    Zustand S2 = {NULL, WirfAus        , NULL , true, this };
    //Zustand S3 = {NULL, TuteIrgendWasAnderes, NULL, true, &optinaleZustandsdaten};

    const Transition transitions[] = {
      { &S1, &TriggerSignalAusgelöst, &S2 }//,
      //{ &S2, &bedingung_uart_erfolgreich, &S3 }     
    };

    // Anfangszustand  
    Zustand *aktueller_Zustand = &S1;

    m_running = true;
    //uint gelesene_Bytes = 0;
    char buffer [100];
    int n;
    int rv;
    struct timeval tv;
    fd_set set;
    m_running = true;
    while(m_running){        
        if( m_serial >= 0 ){

            // hier Zustandsautomat regelmäßig ausführen
            // besser wäre hier eine Entflechtung von Kommunikation und Steuerung (Zustandsautomaten)
            // evtl. könnte man das Handling der seriellen Schnittstelle den Zuständen und Transitionen überlassen.
            // Der Zustand, der auf das erfolgreiche Ausführen des CNC-Codes wartet, kann in seiner Transition den
            // Zugriff auf die serielle Schnittstelle bekommen und auf "DONE" warten. Dazu müsste man den Funktionen
            // über die Zustandsdaten den m_serial übergeben. (evtl. muss dafür m_serial public gemacht werden).
            aktueller_Zustand = run_Automat( aktueller_Zustand, transitions, sizeof(transitions)/sizeof(Transition) );

            // Timeout für read
            tv.tv_sec = 0;
            tv.tv_usec = 50 * 1000;
            FD_ZERO(&set); /* clear the set */

            // m_serial blockieren
            m_mutex.lock();
            FD_SET(m_serial, &set); /* add our file descriptor to the set */
            rv = select(m_serial + 1, &set, NULL, NULL, &tv);

            n = 0;
            if( rv > 0 )
                n = read (m_serial, buffer, sizeof(buffer) );

            m_mutex.unlock();
            if( n == 0 ) continue;
            // m_serial wieder freigeben
            //usleep(1000000);
            //cout << "Gelesene Bytes: " << n << endl;
            //cout << "Gelesen:" << buffer << endl;



            // nach dem richtigen Identifier suchen
            if (strcmp(buffer, "Done\n") == 0){ // evtl. \n anfügen?
                //cout << "emit BefehlBearbeitet" << endl;
                emit BefehlBearbeitet();
            }
            memset(buffer, 0, n);
        }else{
            cout << "m_serial nicht (mehr) geoeffnet" << endl;
            usleep(1000000);
        }
    }
    emit finished();
}

void Kommunikation::stop(){
    m_running = false;
}

#include "Videoquelle.h"

/*
void * Videoquelle::static_fct( void*  p){
    static_cast<Videoquelle*>(p)->ReadImage();
    pthread_exit(NULL);
}
*/

void Videoquelle::HoleNeuesBild(void){
    cout << "Emit NeuesBild" << endl;
    //last_Frame_Call = system_clock::now();
    emit NeuesBild( &frame );
}


void Videoquelle::ReadImage(  ) {
    while(1){
        //usleep(200000);
        if( !cap.isOpened() )
            break;
        //m_mutex.lock();
        cap >> frame;
        //m_mutex.unlock();
        cout << "Bild gelesen" << endl;
        //duration<double> diff = system_clock::now()-last_Frame_Call;
        //cout << diff.count() << endl;
        //if( diff.count() > 0.3){
        //    cout << "3 Sekunden kein Bild angefragt" << endl;
        //    last_Frame_Call = system_clock::now();
        //    emit NeuesBild( &frame );
            //QMetaObject::invokeMethod( this, "HoleNeuesBild" , Qt::QueuedConnection );    // QueuedConnection
        //}
        //emit NeuesBild( &frame );
    }
    emit finished();
}


Videoquelle::Videoquelle(QDomDocument *xml_doc)
{
    // Einlesen von Membervariablen
    QDomElement docElem = xml_doc->documentElement();   // Rutenerkennung
    // Parameter der seriellen Schnittstelle einlesen
    QDomNodeList nodeList = docElem.elementsByTagName("Video");
    if( !nodeList.isEmpty() ){
        QDomElement e = nodeList.at(0).toElement();
        //////
        nodeList = e.elementsByTagName("Quelle");
        if( !nodeList.isEmpty() ){
            bool ok = false;
            int cam = e.toElement().text().toInt(&ok, 10);
            if( ok ){
                cout << "Nutze Kamera: " << cam << endl;
                cap.open(cam);
            }else{
                string quelle = e.toElement().text().toStdString();
                cout << "Nutze VideoQuelle: " << quelle << endl;
                cap.open(quelle);
            }
        }
    }

    if( !cap.isOpened() ){
        std::cout << "VideoCapture konnte nicht geoeffnet werden." << std::endl;
        exit(0);
    }
}



#include "Videoquelle.h"

/*
void * Videoquelle::static_fct( void*  p){
    static_cast<Videoquelle*>(p)->ReadImage();
    pthread_exit(NULL);
}
*/

void Videoquelle::HoleNeuesBild(void){
    cout << "Emit NeuesBild" << endl;
    emit NeuesBild( &frame );
}


void Videoquelle::ReadImage(  ) {
    while(1){
        //usleep(1000);
        if( !cap.isOpened() )
            break;
        cap >> frame;
        cout << "Bild gelesen" << endl;
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



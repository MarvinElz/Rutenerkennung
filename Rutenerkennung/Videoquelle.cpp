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
    // TODO: XML einlesen
    cap.open(0);
    if( !cap.isOpened() ){
        std::cout << "VideoCapture konnte nicht geoeffnet werden." << std::endl;
        return;
    }
}



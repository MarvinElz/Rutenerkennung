#include "Videoquelle.h"


void * Videoquelle::static_fct( void*  p){
    static_cast<Videoquelle*>(p)->ReadImage();
    pthread_exit(NULL);
}


void Videoquelle::HoleNeuesBild(void){
    cout << "Emit neues Bild" << endl;
    emit NeuesBild( &frame );
}


void Videoquelle::ReadImage(  ) {
    while(1){
        /*
        if( (double)(clock() - ticks)/CLOCKS_PER_SEC < 0.2){
            usleep(1000);
            continue;
        }
        ticks = clock();
        */
        usleep(1000);
        cap >> frame;
        cout << "Bild gelesen" << endl;
    }
}


Videoquelle::Videoquelle(QDomDocument *xml_doc)
{
    // TODO: XML einlesen
    cap.open(0);
    if( !cap.isOpened() ){
        std::cout << "VideoCapture konnte nicht geoeffnet werden." << std::endl;
        return;
    }

    // TODO: Thread starten, der Bilder einließt
    int rc = pthread_create(&thread, NULL, &Videoquelle::static_fct, this); //, void);
}



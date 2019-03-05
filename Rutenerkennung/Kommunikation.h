#ifndef KOMMUNIKATION_H_
#define KOMMUNIKATION_H_

#include <iostream>
#include <string>

#include <QObject>
#include <QtXml/QtXml>

#include <opencv2/opencv.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <algorithm>

//#include <boost/asio.hpp>
//#include <boost/asio/serial_port.hpp>

//using namespace boost;
using namespace std;
using namespace cv;

class Kommunikation : public QObject{

    Q_OBJECT


    public:
        Kommunikation( QDomDocument *xml_doc );        
        void stop();
        Vec2i K_to_B(const Vec2f &);
        Vec2f B_to_K(const Vec2i &);

    signals:
        void BefehlBearbeitet();        
        void finished();

    public slots:
        void run();
        void FahreAnPositionUndWirfAus(Vec2i *p);

    private:
        QMutex m_mutex;

        // Rotationsmatrix von B nach R
        // Enthält den Maßstab
        Mat m_K_B_R = Mat::zeros(2, 2, CV_32F);
        Mat m_B_K_R;

        // Translationsvektor von Ursprung (Org = Origin) K-System nach B-System dargestellt im
        // K-System
        Mat m_KpB_Org = Mat::zeros(2,1,CV_32F);
        Mat m_BpK_Org;

        bool m_running = false;

        uint m_baudrate = 115200;
        string m_serialport = "/dev/ttyACM0";

        int m_serial;
};

#endif

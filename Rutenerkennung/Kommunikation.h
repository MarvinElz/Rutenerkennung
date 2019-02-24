#ifndef KOMMUNIKATION_H_
#define KOMMUNIKATION_H_

#include <iostream>
#include <string>

#include <QObject>
#include <QtXml/QtXml>

#include <opencv2/opencv.hpp>

//#include <boost/asio.hpp>
//#include <boost/asio/serial_port.hpp>

//using namespace boost;
using namespace std;
using namespace cv;

class Kommunikation : public QObject{

    Q_OBJECT


    public:
        Kommunikation( QDomDocument *xml_doc );
        void run();
        void stop();

    signals:
        void BefehlBearbeitet();

    public slots:
        void FahreAnPositionUndWirfAus(Vec2i p);

    private:
        // Rotationsmatrix von B nach R
        // Enthält den Maßstab
        Mat m_R_B_R = Mat::zeros(2, 2, CV_32F);

        // Translationsvektor von Ursprung (Org = Origin) K-System nach B-System dargestellt im
        // K-System
        Mat m_RpB_Org = Mat::zeros(2,1,CV_32F);

        bool m_running = true;

        uint m_baudrate = 115200;
        string m_serialport = "/dev/ttyACM0";

        //boost::asio::io_service io;
        //boost::asio::serial_port m_serial(io);


};

#endif

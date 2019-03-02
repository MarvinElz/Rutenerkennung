#ifndef VIDEOQUELLE_H
#define VIDEOQUELLE_H

#include <QtXml/QtXml>
#include <time.h>
#include <pthread.h>
#include <unistd.h>   // usleep
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
using namespace std::chrono;

class Videoquelle : public QObject
{

    Q_OBJECT
public:
    Videoquelle( QDomDocument *xml_doc );

public slots:
    void HoleNeuesBild(void);
    void ReadImage( void );

signals:
    void finished( void );
    void NeuesBild( Mat *frame );
    void New_Raw_Image( cv::Mat *frame );

private:
    QMutex m_mutex;
    system_clock::time_point last_Frame_Call = system_clock::now();
    Mat frame;
    clock_t ticks;
    cv::VideoCapture cap;    
};

#endif // VIDEOQUELLE_H

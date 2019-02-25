#ifndef VIDEOQUELLE_H
#define VIDEOQUELLE_H

#include <QtXml/QtXml>
#include <time.h>
#include <pthread.h>
#include <unistd.h>   // usleep
#include <iostream>

#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

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

private:
    Mat frame;
    clock_t ticks;
    cv::VideoCapture cap;
    //pthread_t thread;
    //static void * static_fct( void* );
    
};

#endif // VIDEOQUELLE_H

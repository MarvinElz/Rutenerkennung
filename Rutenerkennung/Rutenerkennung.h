#ifndef RUTENERKENNUNG_H
#define RUTENERKENNUNG_H

#include <QObject>
#include <QtXml/QtXml>
#include <time.h>

#include <iostream>

#include <unistd.h>   // usleep
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Rutenerkennung : public QObject{

Q_OBJECT
public:
    Rutenerkennung( QDomDocument *xml_doc );

public slots:
    void NeuesBild(Mat *frame);
    void run();

signals:
    void finished();
    void HoleNeuesBild();
    void ErkannteStecklinge( vector<Vec2i> points );
    void Ergebnis_BW(cv::Mat *);
    void Ergebnis_Binary(cv::Mat *);
    void Ergebnis_Binary_Opened(cv::Mat *);
    void Ergebnis_Dist(cv::Mat *);
    void Ergebnis_Dist_Thres(cv::Mat *);
    void Ergebnis( cv::Mat * );

 private:
    cv::Mat bw;
    cv::Mat binary;
    cv::Mat binary_opened;
    cv::Mat dist_transformed;
    cv::Mat dist_transformat_thres;
    cv::Mat bw_with_pos;

 	float m_Min = 8.0;	// mm
 	float m_Max = 60.0;	// mm
 	int m_Threashold_SW = 80;	// Schwarz-Weis-Schwellwert

    QMutex m_mutex;
    bool m_busy = false;

 	float m_masstab = 0.1;	// mm/px

    clock_t ticks;
    vector<Vec2i> m_pos;
};

#endif // RUTENERKENNUNG_H

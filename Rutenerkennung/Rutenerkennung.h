#ifndef RUTENERKENNUNG_H
#define RUTENERKENNUNG_H

#include <QObject>
#include <QtXml/QtXml>
#include <time.h>

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

signals:
    void HoleNeuesBild();
    void ErkannteStecklinge( vector<Vec2i> points );

 private:
    clock_t ticks;
    vector<Vec2i> m_pos;
};

#endif // RUTENERKENNUNG_H

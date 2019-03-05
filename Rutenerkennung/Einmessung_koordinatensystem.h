#ifndef EINMESSUNG_KOORDINATENSYSTEM_H
#define EINMESSUNG_KOORDINATENSYSTEM_H

#include <QWidget>
#include <iostream>
#include <QInputEvent>
#include "Rutenerkennung.h"
#include <opencv2/opencv.hpp>
#include <QMessageBox>

using namespace std;

namespace Ui {
class Einmessung_Koordinatensystem;
}

class Einmessung_Koordinatensystem : public QWidget
{
    Q_OBJECT

public:
    explicit Einmessung_Koordinatensystem(Rutenerkennung *r, QDomDocument *xml_doc, QWidget *parent = 0);
    ~Einmessung_Koordinatensystem();
    void clear();

public slots:
    void Bild( const cv::Mat *img_ );

private slots:
    void on_Image_Label_Clicked(QMouseEvent *);

    void on_Berechne_Koord_clicked();

private:
    Ui::Einmessung_Koordinatensystem *ui;
    QDomDocument *m_xml_doc;
    vector<Vec2i> B_Points;
    vector<Vec2f> K_Points;
};

#endif // EINMESSUNG_KOORDINATENSYSTEM_H

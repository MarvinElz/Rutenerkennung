#ifndef EINMESSUNG_MASKE_H
#define EINMESSUNG_MASKE_H

#include <QWidget>
#include <QInputEvent>
#include "Rutenerkennung.h"
#include <opencv2/opencv.hpp>
#include <QMessageBox>

namespace Ui {
class Einmessung_Maske;
}

class Einmessung_Maske : public QWidget
{
    Q_OBJECT

public:
    explicit Einmessung_Maske(Rutenerkennung *r, QDomDocument *xml_doc, QWidget *parent = 0);
    ~Einmessung_Maske();
    void clear();

public slots:
    void Bild( const cv::Mat *img_ );

private slots:

    void on_Erstelle_Maske_clicked();

    void on_Image_Label_Clicked(QMouseEvent *);

private:
    Ui::Einmessung_Maske *ui;
    QDomDocument *m_xml_doc;
    vector<Vec2i> M_Points;
};

#endif // EINMESSUNG_MASKE_H

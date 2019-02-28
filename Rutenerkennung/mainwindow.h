#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Rutenerkennung.h"
#include "Beobachter.h"

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <iostream>
#include <QtXml/QtXml>

using namespace cv;
using namespace std;

enum Bilder{bw, binary, binary_opened, distance_, distance_threshold};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent = 0);
    ~MainWindow();

    public slots:
        void ShowImage( cv::Mat * );

private slots:
        void on_Bildauswahl_activated(int index);

private:
    Ui::MainWindow *ui;
    Rutenerkennung *m_r;
    Beobachter *m_b;
    QImage::Format m_format = QImage::Format_Grayscale8;
};

#endif // MAINWINDOW_H

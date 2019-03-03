#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Rutenerkennung.h"
#include "Beobachter.h"
#include "Videoquelle.h"
#include "Steckling.h"

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <iostream>
#include <QtXml/QtXml>
#include <vector>
#include <QMessageBox>
#include <QFileDialog>

using namespace cv;
using namespace std;

enum Images{ bw, mask, masked, binary, binary_opened, distance, distance_threshold, bw_with_pos};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( Videoquelle *v, Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent = 0);
    ~MainWindow();

    public slots:
        void ShowImage( const cv::Mat * );
        void GetImageForCoordinateSystem( const cv::Mat * );
        void GetImageForMask( const cv::Mat * );
        void Valide_Stecklinge( vector<Steckling*>* );

private slots:
        void on_Bildauswahl_activated(int index);

        void on_Einmessen_clicked();

        void on_Maske_clicked();

private:
    QMutex mutex;
    vector<Vec2i> B_Points;
    vector<Vec2f> K_Points;
    vector<Steckling*> Stecklinge;
    vector<Vec2i> M_Points;

    QDomDocument *m_xml_doc;
    Ui::MainWindow *ui;
    Rutenerkennung *m_r;
    Beobachter *m_b;
    Videoquelle *m_v;
    QImage::Format m_format = QImage::Format_Grayscale8;
};

#endif // MAINWINDOW_H

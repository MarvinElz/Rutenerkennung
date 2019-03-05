#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Rutenerkennung.h"
#include "Beobachter.h"
#include "Videoquelle.h"
#include "Steckling.h"
#include "Kommunikation.h"

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QObject>
#include <iostream>
#include <QtXml/QtXml>
#include <vector>
#include <QMessageBox>
#include <QFileDialog>


#include "Einmessung_koordinatensystem.h"
#include "Einmessung_maske.h"

using namespace cv;
using namespace std;

enum Images{ bw, masked, binary, binary_opened, distance, distance_threshold};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( Kommunikation *k, Videoquelle *v, Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent = 0);
    ~MainWindow();

    public slots:
        void ShowImage( const cv::Mat * );
        //void GetImageForCoordinateSystem( const cv::Mat * );
        //void GetImageForMask( const cv::Mat * );
        void Valide_Stecklinge( vector<Steckling*>* );

private slots:
        void on_Bildauswahl_activated(int index);

        //void on_Einmessen_clicked();

        //void on_Maske_clicked();

        void on_Einmessen_clicked();

        void on_Save_Configuration_clicked();

        void on_Maske_clicked();

private:
    QMutex mutex;
    vector<Steckling*> Stecklinge;
    vector<Vec2i> M_Points;

    Ui::MainWindow *ui;
    Kommunikation *m_k;
    Videoquelle *m_v;
    Rutenerkennung *m_r;
    Beobachter *m_b;
    QDomDocument *m_xml_doc;

    Einmessung_Koordinatensystem ko;
    Einmessung_Maske ma;

    QImage::Format m_format = QImage::Format_Grayscale8;
};

#endif // MAINWINDOW_H

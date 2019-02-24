#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <QtXml/QtXml>
#include "config_manager.h"

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(struct SBD_Config *SBD_config, QDomDocument *xml_doc, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Parameter_Updated_clicked();

    void on_dSB_minDistance_valueChanged(double arg1);


    void on_dSB_maxThreshold_valueChanged(double arg1);

    void on_dSB_minThreshold_valueChanged(double arg1);

    void on_dSB_thresholdStep_valueChanged(double arg1);

    void on_dSB_minRepeatability_valueChanged(double arg1);

    void on_cB_filterByArea_clicked(bool checked);

    void on_dSB_minArea_valueChanged(double arg1);

    void on_dSB_maxArea_valueChanged(double arg1);

    void on_cB_filterByInertia_clicked(bool checked);

    void on_dSB_minInertia_valueChanged(double arg1);

    void on_dSB_maxInertia_valueChanged(double arg1);

    void on_cB_filterByConvexity_clicked(bool checked);

    void on_dSB_minConvexity_valueChanged(double arg1);

    void on_dSB_maxConvexity_valueChanged(double arg1);

    void on_cB_filterByCircularity_clicked(bool checked);

    void on_dSB_maxCircularity_valueChanged(double arg1);

    void on_dSB_minCircularity_valueChanged(double arg1);

    void on_Einmessung_Koordinatensystem_clicked();

private:
    Ui::MainWindow *ui;
    struct SBD_Config *SBD_config;
    QDomDocument *m_xml_doc;
    void update_Parameters(cv::SimpleBlobDetector::Params*);
    vector<Vec2i> m_pos;
};

#endif // MAINWINDOW_H

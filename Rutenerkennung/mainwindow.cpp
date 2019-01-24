#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>


#include <vector>


clock_t ticks;

using namespace cv;

MainWindow::MainWindow(struct SBD_Config *SBD_config, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), SBD_config(SBD_config)
{    
    ui->setupUi(this);
    //ui->Input_Delta         ->setValue( MSER_config->delta          );
    SBD_config->filterByArea = ui->cB_filterByArea->isChecked();
    SBD_config->filterByCircularity = ui->cB_filterByCircularity->isChecked();
    SBD_config->filterByConvexity = ui->cB_filterByConvexity->isChecked();
    SBD_config->filterByInertia = ui->cB_filterByInertia->isChecked();

    SBD_config->maxArea = ui->dSB_maxArea->value();
    SBD_config->maxCircularity = ui->dSB_maxCircularity->value();
    SBD_config->maxConvexity = ui->dSB_maxConvexity->value();
    SBD_config->maxInertiaRatio = ui->dSB_maxInertia->value();
    SBD_config->maxThreshold = ui->dSB_maxThreshold->value();

    SBD_config->minArea = ui->dSB_minArea->value();
    SBD_config->minCircularity = ui->dSB_minCircularity->value();
    SBD_config->minConvexity = ui->dSB_minConvexity->value();
    SBD_config->minDistBetweenBlobs = ui->dSB_minDistance->value();
    SBD_config->minInertiaRatio = ui->dSB_minInertia->value();
    SBD_config->minRepeatability = ui->dSB_minRepeatability->value();
    SBD_config->minThreshold = ui->dSB_minThreshold->value();

    SBD_config->thresholdStep = ui->dSB_thresholdStep->value();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_Parameters(cv::SimpleBlobDetector::Params* params){
    params->filterByArea = SBD_config->filterByArea;
    params->filterByCircularity = SBD_config->filterByCircularity;
    params->filterByConvexity = SBD_config->filterByConvexity;
    params->filterByInertia = SBD_config->filterByInertia;

    params->maxArea = SBD_config->maxArea;
    params->maxCircularity = SBD_config->maxCircularity;
    params->maxConvexity = SBD_config->maxConvexity;
    params->maxInertiaRatio = SBD_config->maxInertiaRatio;
    params->maxThreshold = SBD_config->maxThreshold;

    params->minArea = SBD_config->minArea;
    params->minCircularity = SBD_config->minCircularity;
    params->minConvexity = SBD_config->minConvexity;
    params->minDistBetweenBlobs = SBD_config->minDistBetweenBlobs;
    params->minInertiaRatio = SBD_config->minInertiaRatio;
    params->minRepeatability = SBD_config->minRepeatability;
    params->minThreshold = SBD_config->minThreshold;

    params->thresholdStep = SBD_config->thresholdStep;

}

void MainWindow::on_Parameter_Updated_clicked()
{

    // hier kann alternativ auch ein Bild geladen werden:
    //Mat frame;
    //frame = imread("/home/pi/Desktop/Rute.JPG", CV_LOAD_IMAGE_COLOR);

    cv::VideoCapture cap(0);
    if( !cap.isOpened() ){
        std::cout << "VideoCapture konnte nicht geoeffnet werden." << std::endl;
        return;
    }
    Mat bw;

    cv::SimpleBlobDetector::Params params;
    update_Parameters(&params);

    cv::SimpleBlobDetector *sbd = new cv::SimpleBlobDetector(params);

    std::vector<KeyPoint> keypoints;

    Size size(400, 300);

    cv::Mat frame;

    while(1){
        cap >> frame;
        //cv::resize(frame, frame, size);
        keypoints.clear();
        cvtColor(frame, bw, CV_BGR2GRAY);

        std::cout << "Begin Detection" << std::endl;
        //ms( bw, regions ); //, mask );
        Mat bw_invert;

        ticks = clock();
        cv::subtract( cv::Scalar::all(255), bw, bw_invert );
        sbd->detect( bw_invert, keypoints );
        std::cout << "Keypoints:" << keypoints.size() << std::endl;
        Mat im_with_keypoints;

        std::cout << "Calculated in " << (double)(clock() - ticks)/CLOCKS_PER_SEC << " seconds" << std::endl;

        drawKeypoints( bw_invert, keypoints, im_with_keypoints, Scalar(0, 0, 255),  DrawMatchesFlags::DEFAULT);
        cv::cvtColor(im_with_keypoints, im_with_keypoints, CV_BGR2RGB);

        //ui->label_15->setPixmap(QPixmap::fromImage(QImage(im_with_keypoints.data, im_with_keypoints.cols, im_with_keypoints.rows, im_with_keypoints.step, QImage::Format_RGB888)));
        //ui->label_15->show();
        imshow( "simpleBlobDetector", im_with_keypoints );
        if( cv::waitKey(30) >= 0 ) break;
    }

    delete sbd;
    cap.release();
}


void MainWindow::on_dSB_minDistance_valueChanged(double arg1)
{
    SBD_config->minDistBetweenBlobs = arg1;
}


void MainWindow::on_dSB_maxThreshold_valueChanged(double arg1)
{
    SBD_config->maxThreshold = arg1;
}

void MainWindow::on_dSB_minThreshold_valueChanged(double arg1)
{
    SBD_config->minThreshold = arg1;
}

void MainWindow::on_dSB_thresholdStep_valueChanged(double arg1)
{
    SBD_config->thresholdStep = arg1;
}

void MainWindow::on_dSB_minRepeatability_valueChanged(double arg1)
{
    SBD_config->minRepeatability = arg1;
}

void MainWindow::on_cB_filterByArea_clicked(bool checked)
{
    SBD_config->filterByArea = checked;
}

void MainWindow::on_dSB_minArea_valueChanged(double arg1)
{
    SBD_config->minArea = arg1;
}

void MainWindow::on_dSB_maxArea_valueChanged(double arg1)
{
    SBD_config->maxArea = arg1;
}

void MainWindow::on_cB_filterByInertia_clicked(bool checked)
{
    SBD_config->filterByInertia = checked;
}

void MainWindow::on_dSB_minInertia_valueChanged(double arg1)
{
    SBD_config->minInertiaRatio = arg1;
}

void MainWindow::on_dSB_maxInertia_valueChanged(double arg1)
{
    SBD_config->maxInertiaRatio = arg1;
}

void MainWindow::on_cB_filterByConvexity_clicked(bool checked)
{
    SBD_config->filterByConvexity = checked;
}

void MainWindow::on_dSB_minConvexity_valueChanged(double arg1)
{
    SBD_config->minConvexity = arg1;
}

void MainWindow::on_dSB_maxConvexity_valueChanged(double arg1)
{
    SBD_config->maxConvexity = arg1;
}

void MainWindow::on_cB_filterByCircularity_clicked(bool checked)
{
    SBD_config->filterByCircularity = checked;
}

void MainWindow::on_dSB_maxCircularity_valueChanged(double arg1)
{
    SBD_config->maxCircularity = arg1;
}

void MainWindow::on_dSB_minCircularity_valueChanged(double arg1)
{
    SBD_config->minCircularity = arg1;
}

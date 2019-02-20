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
      
      ticks = clock();
      cvtColor(frame, bw, CV_BGR2GRAY);
      

      cv::Mat binary;
      // TODO: Threshold anpassen
      threshold( bw, binary, 125, 255, THRESH_BINARY );
      imshow( "binary", binary );

      // noise removal
      cv::Mat kernel = Mat::ones( 3, 3, bw.type() );
      cv::Mat binary_opened;
		morphologyEx( binary, binary_opened, MORPH_OPEN, kernel );
		imshow( "binary_opened", binary_opened );

		// TODO: Alternative checken: watershed(src, markers);
		// Finding sure foreground area
		cv::Mat dist_transformed;
		distanceTransform(binary_opened, dist_transformed, CV_DIST_L2, 3);
		imshow( "dist_transformed", dist_transformed );

		cv::Mat dist_transformat_thres;
		// TODO: über Maßstab des Bildes den Threshold bestimmen
		threshold( dist_transformed, dist_transformat_thres, 0.7*dist_transform.max(), 255, CV_THRESH_BINARY );
		imshow( "dist_transformat_thres", dist_transformat_thres );

		
		// Detektion über Kontur
    	vector<vector<Point> > contours;
    	findContours(dist_transformat_thres, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    	// Mittelpunkt der Kontur bestimmen
    	vector<Point> pos;
    	for ( int c = 0; c < contours.size(); c++ ){
    		Point sum = Point(0,0);
    		const vector<Point> contour = contours.at(c);
    		for( int i = 0; i < contour.size(); i++ ){
    			sum += contour.at(i);
    		}
    		sum /= contour.size();
    		circle(bw, sum, 5, CV_RGB(255,0,0), -1);
    		pos.push_back(sum);
    	}
    	imshow( "bw", bw );
		/*
    	// Marker für Watershed vorbereiten
    	Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    	// Draw the foreground markers
    	for (size_t i = 0; i < contours.size(); i++)
      	drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
     	// Draw the background marker
    	circle(markers, Point(5,5), 3, CV_RGB(255,255,255), -1);
		imshow("Markers", markers*10000);

		
		// Watershed zum Auffüllen
    	watershed(frame, markers);
    	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
    	markers.convertTo(mark, CV_8UC1);
    	bitwise_not(mark, mark);
		//    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
                                  // image looks like at that point
    	// Generate random colors
    	vector<Vec3b> colors;
    	for (size_t i = 0; i < contours.size(); i++)
    	{
      	int b = theRNG().uniform(0, 255);
        	int g = theRNG().uniform(0, 255);
        	int r = theRNG().uniform(0, 255);
        	colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    	}
    	// Create the result image
    	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    	// Fill labeled objects with random colors
    	for (int i = 0; i < markers.rows; i++)
    	{
        	for (int j = 0; j < markers.cols; j++)
        	{
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
                dst.at<Vec3b>(i,j) = colors[index-1];
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        	}
    	}
    	// Visualize the final image
    	imshow("Final Result", dst);
		*/

		

		/*
		Detektion über SimpleBlobDetection
      std::cout << "Begin Detection" << std::endl;
      Mat bw_invert;
		cv::subtract( cv::Scalar::all(255), dist_transformat_thres, bw_invert );
      
      sbd->detect( bw_invert, keypoints );
      std::cout << "Keypoints:" << keypoints.size() << std::endl;
      Mat im_with_keypoints;


      std::cout << "Calculated in " << (double)(clock() - ticks)/CLOCKS_PER_SEC << " seconds" << std::endl;

      drawKeypoints( bw_invert, keypoints, im_with_keypoints, Scalar(255, 0, 0),  DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      cv::cvtColor(im_with_keypoints, im_with_keypoints, CV_BGR2RGB);
		*/
      //ui->label_15->setPixmap(QPixmap::fromImage(QImage(im_with_keypoints.data, im_with_keypoints.cols, im_with_keypoints.rows, im_with_keypoints.step, QImage::Format_RGB888)));
      //ui->label_15->show();
      
      //imshow( "bw_contrast", bw_contrast );
      //imshow( "bw_contrast_opened", bw_contrast_opened );
      //imshow( "simpleBlobDetector", im_with_keypoints );
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

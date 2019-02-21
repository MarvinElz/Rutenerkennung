#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>


#include <vector>


clock_t ticks;

/*
	┌─────→ x
	│ {B}
	↓y
						↑ x
				y		│
				←─────┘{K}
*/



// Liste von Punkten, jeweils im realen (Kinematik-) und virtuellen (Kamera/Bild-)Koordinatensystem
vector<Vec2i> B_Points;
vector<Vec2f> K_Points;

// Rotationsmatrix von B nach R
Mat R_B_R = Mat::zeros(2, 2, CV_32F);

// Translationsvektor von Ursprung (Org = Origin) K-System nach B-System dargestellt im
// K-System
Mat RpB_Org = Mat::zeros(2,1,CV_32F);



using namespace cv;

MainWindow::MainWindow(struct SBD_Config *SBD_config, QDomDocument *xml_doc, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), SBD_config(SBD_config), m_xml_doc(xml_doc)
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

    // K_Points aus XML einlesen
	QDomElement docElem = m_xml_doc.documentElement(); 	// Rutenerkennung
	QString rootTag = docElem.tagName();
	qInfo() << rootTag;

	QDomNodeList nodeList = docElem.elementsByTagName("Camera"); 
	if( !nodeList.isempty() ){
		QDomElement cam_element = nodeList.at(0).toElement();
		qInfo() << "\t" << cam_element.tagName();
		nodeList = cam_element.elementsByTagName("Calibration"); 
		if( !nodeList.isempty() ){
			QDomElement calib_element = nodeList.at(0).toElement();
			qInfo() << "\t\t" << calib_element.tagName();
			nodeList = calib_element.elementsByTagName("Point");
			for(int ii = 0;ii < nodeList.count(); ii++){
				Vec2f vec;
				QDomElement point = nodeList.at(ii).toElement();
				qInfo() << "\t\t\tRead Point " << point.attribute("id").toInt();

				vec[0] = point.elementsByTagName("x").at(0).toElement().text().toFloat();
				vec[1] = point.elementsByTagName("y").at(0).toElement().text().toFloat();

				qInfo() << "\t\t\tX:" << vec[0];
				qInfo() << "\t\t\tY:" << vec[1];

				K_Points.push_back(vec);
			}
		}

		// Parsen von R_B_R
		nodeList = cam_element.elementsByTagName("Rotation");
		if( !nodeList.isempty() ){
			QDomElement rotation_element = nodeList.at(0).toElement();
			qInfo() << "\t\t" << rotation_element.tagName();
			nodeList = rotation_element.elementsByTagName("r");
			for(int ii = 0;ii < nodeList.count(); ii++){
				QDomElement mat = nodeList.at(ii).toElement();
				char mat_entry[3] = mat.attribute("id").toLocal8Bit().data();
				qInfo() << "\t\t\tRead Mat_Entry " << mat.attribute("id");
				R_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1') = mat.text().toFloat();
			}
		}
		cout << "R_B_R" << R_B_R << endl;

		// Parsen von R_B_Org in R_B_T
		nodeList = cam_element.elementsByTagName("Translation");
		if( !nodeList.isempty() ){
			QDomElement trans_element = nodeList.at(0).toElement();
			qInfo() << "\t" << trans_element.tagName();
			if( !nodeList.isempty() ){
				QDomElement calib_element = nodeList.at(0).toElement();
				qInfo() << "\t\t" << calib_element.tagName();
				nodeList = calib_element.elementsByTagName("Point");
				QDomElement point = nodeList.at(ii).toElement();
				RpB_Org.at<float>(0,0) = point.elementsByTagName("x").at(0).toElement().text().toFloat();
				RpB_Org.at<float>(1,0) = point.elementsByTagName("y").at(0).toElement().text().toFloat();
			}
		}
	}
	cout << "RpB_Org" << RpB_Org << endl;
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

/*
evtl. können wir das Einlesen der Koordinatensysteme auch "manuell" machen. Dazu
erzeugen wir uns ein Bild aus der Kamera (wenn sie an ihrer finalen Position angebracht wurde)
und lesen es am PC ein. Nachteil ist, dass wir dann vor Ort keine Änderungen übernehmen
können 
*/

static void onMouse( int event, int x, int y, int flag , void* param ){
	Vec2i pos = Vec2i(x,y);
	switch (event){
	case EVENT_LBUTTONUP:
		//Mat* frame = (Mat*) param;
		B_Points.push_back( pos );
		break;
	case EVENT_RBUTTONUP:
		if( !B_Points.empty() ){
			for (std::vector<Vec2i>::iterator it = B_Points.begin() ; it != B_Points.end(); ++it){
				if( norm( pos - *it ) < 10 ){
					// Wenn rechte Maustaste gedrückt, Punkt löschen
					it = B_Points.erase(it);
					return;			
				}
			}
		}
		break;	
	}
}

// Das Einmessen der Koordinatensysteme (K-Kinematik, B-Bild) wird
// durch eine Vorlage realisiert, die an die entsprechende Stelle im Blick-
// feld der Kamera angebracht wrid. Über erkennbare Symbole, die im 
// K-Koordinatensystem definiert wurden, kann die Transformation B->K
// berechnet werden.
void MainWindow::on_Einmessung_Koordinatensystem(){
	namedWindow("Einmessung Koordinatensystem", WINDOW_NORMAL);
	// evtl. über Bild laden?
	cv::VideoCapture cap(0);
   if( !cap.isOpened() ){
      std::cout << "VideoCapture konnte nicht geoeffnet werden." << std::endl;
      return;
   }

	Mat frame;

	// Mouse-Callback setzen, um die Symbole auf dem Bild auszuwählen
	setMouseCallback("Einmessung Koordinatensystem", onMouse, &frame );

   
   while( true ){
   	cap >> frame;

		if( !B_Points.empty() ){
			for (std::vector<Vec2i>::iterator it = B_Points.begin() ; it != B_Points.end(); ++it){
				// Zeichne einen Kreis, damit wird sicher sind.
				circle(frame, Point( *it[0],*it[1] ), 10, 255, 1, 8, 0);
			}
		}
   	imshow( "Einmessung Koordinatensystem", frame );

   	//usleep( 1 );
   	if( cv::waitKey(1) >= 0 ) break;
   }

	// Berechne Transformation
	if( B_Points.size() < 3 || B_Points.size() > K_Points.size() ){
		qInfo() << "Achtung: Diskrepanz bei Anzahl der Punkte waehrend der Einmessung des Koordinatensystems";
		goto quit;
	}

	uint size_points = B_Points.size();

	// D: Data, R: real (K-Koordinaten), B: Bild (B-Koordinaten)
	Mat R_D = Mat::ones( 2, size_points, CV_32F );
	Mat B_D = Mat::ones( 3, size_points, CV_32F );

	// Fill Mats
	for (uint i = 0; i < size_points; i++){
		B_D.at<float>(0,i) = (float)B_Points.at(i)[0];
		B_D.at<float>(1,i) = (float)B_Points.at(i)[1];

		R_D.at<float>(0,i) = R_Points.at(i)[0];
		R_D.at<float>(1,i) = R_Points.at(i)[1];
	}

	cout << "R_D:" << R_D << endl;
	cout << "B_D:" << B_D << endl;

	Mat temp = B_D * B_D.t();
	Mat R_B_T = R_D * B_D.t() * temp.inv();

	cout << "R_B_T:" << R_B_T << endl;

	// TODO: R_B_T aufteilen in Mat R_B_R und Mat RpB_Org
	R_B_R   = R_B_T(cv::Rect(0,0,2,2)).clone();
	cout << "R_B_R:" << R_B_R << endl;
	RpB_Org = R_B_T(cv::Rect(0,2,1,2)).clone();
	cout << "RpB_Org:" << RpB_Org << endl;

quit:
	destroyWindow("Einmessung Koordinatensystem");	
	cap.release();
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
      if( cv::waitKey(1) >= 0 ) break;
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

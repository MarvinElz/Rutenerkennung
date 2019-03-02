#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Videoquelle *v, Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_v(v),
    m_r(r),
    m_b(b),
    m_xml_doc(xml_doc)
{
    ui->setupUi(this);
    // K_Points einlesen
    QDomElement docElement = m_xml_doc->documentElement();
    QDomNodeList nodeList = docElement.elementsByTagName("Camera");
    if( !nodeList.isEmpty() ){
        QDomElement cam_element = nodeList.at(0).toElement();
        nodeList = cam_element.elementsByTagName("Calibration");
        if( !nodeList.isEmpty() ){
            QDomElement calib_element = nodeList.at(0).toElement();
            nodeList = calib_element.elementsByTagName("Point");
            for( int i = 0; i < nodeList.count(); i++ ){
                Vec2f vec;
                QDomElement point = nodeList.at(i).toElement();
                qInfo() << "Read CalibrationPoint: " << point.attribute("id").toInt();
                vec[0] = point.elementsByTagName("x").at(0).toElement().text().toFloat();
                vec[1] = point.elementsByTagName("y").at(0).toElement().text().toFloat();
                qInfo() << "( " << vec[0] << " , " << vec[1] << " )";
                K_Points.push_back(vec);
             }
        }
    }
}

void MainWindow::Valide_Stecklinge( vector<Steckling*>* valide_stecklinge ){
    cout << "Valide_Stecklinge" << endl;
    Stecklinge = *valide_stecklinge;
}

void MainWindow::ShowImage( cv::Mat *img_ ){
    Mat img = img_->clone();
    if( m_format == QImage::Format_RGB888 )
        cv::cvtColor( img, img, CV_BGR2RGB );
    if( m_format == QImage::Format_Grayscale8 )
        cv::cvtColor( img, img, CV_GRAY2RGB );

    if( !Stecklinge.empty() ){
        for (vector<Steckling*>::iterator j = Stecklinge.begin() ; j != Stecklinge.end(); ++j){
            uint radius = 3;
            circle( img, (*j)->pos, radius, CV_RGB(255, 0, 0), 1, 8, 0 );
            Point p((*j)->pos[0], (*j)->pos[1]);
            putText( img, std::to_string((*j)->m_plausibility), p + Point(radius+2, radius+2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255, 0, 0) );
        }
    }
    ui->Image_Label->setPixmap( QPixmap::fromImage( QImage( img.data, img.cols, img.rows, img.step, QImage::Format_RGB888) ) );
    ui->Image_Label->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Bildauswahl_activated(int index)
{
    QObject::disconnect(m_r, 0, this, SLOT(ShowImage(cv::Mat *)));
    QObject::disconnect(m_b, 0, this, SLOT(ShowImage(cv::Mat *)));
    QObject::disconnect(m_v, 0, this, SLOT(ShowImage(cv::Mat *)));
    switch( index ){
        //case raw:
        //    QObject::connect( m_v, SIGNAL(New_Raw_Image(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
        //    m_format = QImage::Format_RGB888;
        //    break;
        case bw:
            QObject::connect( m_r, SIGNAL(Ergebnis_BW(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case binary:
            QObject::connect( m_r, SIGNAL(Ergebnis_Binary(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case binary_opened:
            QObject::connect( m_r, SIGNAL(Ergebnis_Binary_Opened(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case distance_:
            QObject::connect( m_r, SIGNAL(Ergebnis_Dist(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case distance_threshold:
            QObject::connect( m_r, SIGNAL(Ergebnis_Dist_Thres(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case bw_with_pos:
            QObject::connect( m_r, SIGNAL(Ergebnis(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        default:
            cout << "Bildauswahl nicht unterstuetzt!!!" << endl;
    }
}

static void onMouseMask( int event, int x, int y, int flag, void *param ){
    Vec2i pos = Vec2i(x, y);
    vector<Vec2i> *M_Points = (vector<Vec2i>*)param;
    switch( event ){
        case EVENT_LBUTTONUP:
            M_Points->push_back(pos);
        break;
        case EVENT_RBUTTONUP:
            for( std::vector<Vec2i>::iterator it = M_Points->begin(); it != M_Points->end(); ++it ){
                if( norm(pos - *it) < 10 ){
                    it = M_Points->erase(it);
                    return;
                }
            }
        break;
    }
}

static void onMouseKoord( int event, int x, int y, int flag, void *param ){
    Vec2i pos = Vec2i(x, y);
    vector<Vec2i> *B_Points = (vector<Vec2i>*)param;
    switch( event ){
        case EVENT_LBUTTONUP:
            B_Points->push_back(pos);
        break;
        case EVENT_RBUTTONUP:
            for( std::vector<Vec2i>::iterator it = B_Points->begin(); it != B_Points->end(); ++it ){
                if( norm(pos - *it) < 10 ){
                    it = B_Points->erase(it);
                    return;
                }
            }
        break;
    }
}

void MainWindow::GetImageForCoordinateSystem( cv::Mat *img_ ){
    Mat img;
    cvtColor( *img_, img, cv::COLOR_GRAY2BGR );
    if( !B_Points.empty() ){
        for( uint i = 0; i < B_Points.size(); i++ ){
            Point p = B_Points.at(i);
            uint radius = 3;
            circle( img, p, radius, CV_RGB(255, 0, 0), 1, 8, 0 );
            putText( img, std::to_string(i), p + Point(radius+2, radius+2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255, 0, 0) );
        }
    }
    imshow("Einmessung Koordinatensystem", img );
    if( cv::waitKey(30) >= 0 ){
        //assert(false);
        QObject::disconnect(m_r, SIGNAL(Ergebnis_BW(cv::Mat *)), this, SLOT(GetImageForCoordinateSystem(cv::Mat *)));
        // TODO: Berechnung der Transformation
        Mat K_D;
        Mat B_D;
        Mat temp;
        Mat K_B_T;
        Mat K_B_R;
        Mat KpB_Org;
        uint size_points = B_Points.size();

        if( B_Points.size() < 3 ){
            QMessageBox msgBox;
            msgBox.setText("Zu wenig Punkte gesetzt: Minimum 3");
            msgBox.exec();
            goto quit;
        }
        if( B_Points.size() > K_Points.size() ){
            QMessageBox msgBox;
            msgBox.setText("Mehr Punkte gesetzt als in XML definiert.");
            msgBox.setInformativeText("Berechnung wird dennoch mit " + QString::number(K_Points.size()) + " Punkten ausgeführt");
            msgBox.exec();
            // Anzahl der Punkte auf K_Points begrenzen
            size_points = K_Points.size();
        }
        K_D = Mat::ones( 2, size_points, CV_32F );
        B_D = Mat::ones( 3, size_points, CV_32F );

        // Matrizen füllen
        for( uint i = 0; i < size_points; i++ ){
            B_D.at<float>(0,i) = (float)B_Points.at(i)[0];
            B_D.at<float>(1,i) = (float)B_Points.at(i)[1];
            K_D.at<float>(0,i) = (float)K_Points.at(i)[0];
            K_D.at<float>(1,i) = (float)K_Points.at(i)[1];
        }
        cout << "K_D" << endl;
        cout << K_D << endl;
        cout << "B_D" << endl;
        cout << B_D << endl;

        temp = B_D * B_D.t();
        K_B_T = K_D * B_D.t() * temp.inv();

        cout << "K_B_T" << endl;
        cout << K_B_T << endl;

        K_B_R = K_B_T( cv::Rect( 0, 0, 2, 2 )).clone();
        cout << "K_B_R" << endl;
        cout << K_B_R << endl;

        KpB_Org =K_B_T( cv::Rect(2,0,1,2) ).clone();
        cout << "KpB_Org" << endl;
        cout << KpB_Org << endl;
        QMessageBox msgBox;
        msgBox.setText("Einmessung der Koordinatensysteme erfolgreich.");
        msgBox.setInformativeText("Konfiguration speichern?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel );
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        QDomElement docElem;
        QDomNodeList nodeList;
        QDomElement cam_element;
        QDomElement rotation_element;
        QDomElement mat;
        QString filename;
        QFile *file;
        QTextStream *stream;
        switch( ret ){
            case QMessageBox::Save:
            // TODO: Daten in XML speichern
                cout << "Save" << endl;
                docElem = m_xml_doc->documentElement(); 	// Rutenerkennung
                nodeList = docElem.elementsByTagName("Camera");
                if( !nodeList.isEmpty() ){
                    cam_element = nodeList.at(0).toElement();
                    nodeList = cam_element.elementsByTagName("Rotation");
                    if( !nodeList.isEmpty() ){
                        rotation_element = nodeList.at(0).toElement();
                        nodeList = rotation_element.elementsByTagName("r");
                        assert( nodeList.count() == 4 );
                        for(int ii = 0;ii < nodeList.count(); ii++){
                            mat = nodeList.at(ii).toElement();
                            char *mat_entry = mat.attribute("id").toLocal8Bit().data();
                            mat.firstChild().setNodeValue( QString::number(K_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1')) );
                        }
                    }else{
// TODO: XML-Cleanup
                        assert(false);
                    }
                }else{
                    assert(false);
                }
                filename = QFileDialog::getSaveFileName( this, tr("Test"), tr("Config.xml") );
                file = new QFile(filename);
                file->open( QIODevice::WriteOnly | QIODevice::Text );
                stream = new QTextStream( file );
                *stream << m_xml_doc->toString();
                file->close();
                delete file;
                delete stream;
                break;
            case QMessageBox::Cancel: break;
            default: break;
        }
        goto quit;
    }
    return;
    quit:
        destroyWindow( "Einmessung Koordinatensystem" );
}

void MainWindow::GetImageForMask( cv::Mat *img_ ){
    Mat img;
    cvtColor( *img_, img, cv::COLOR_GRAY2BGR );
    if( !M_Points.empty() ){
        const cv::Point *pts = (const cv::Point*) Mat(M_Points).data;
        int nptr = M_Points.size(); //Mat(M_Points.rows)

        polylines(img, &pts, &nptr, 1, true, CV_RGB(255, 0, 0) );
    }
    imshow("Einmessung Maske", img );
    if( cv::waitKey(30) >= 0 ){
        //assert(false);
        QObject::disconnect(m_r, SIGNAL(Ergebnis_BW(cv::Mat *)), this, SLOT(GetImageForMask(cv::Mat *)));
        destroyWindow( "Einmessung Maske" );

    }
}


void MainWindow::on_Einmessen_clicked()
{
    B_Points.clear();
    namedWindow( "Einmessung Koordinatensystem", WINDOW_NORMAL );
    setMouseCallback( "Einmessung Koordinatensystem", onMouseKoord , &B_Points);
    QObject::connect( m_r, SIGNAL(Ergebnis_BW(cv::Mat *)), this, SLOT(GetImageForCoordinateSystem(cv::Mat *)), Qt::QueuedConnection);
}

void MainWindow::on_Maske_clicked()
{
    namedWindow( "Einmessung Maske", WINDOW_NORMAL );
    setMouseCallback( "Einmessung Maske", onMouseMask , &M_Points);
    QObject::connect( m_r, SIGNAL(Ergebnis_BW(cv::Mat *)), this, SLOT(GetImageForMask(cv::Mat *)), Qt::QueuedConnection);
}

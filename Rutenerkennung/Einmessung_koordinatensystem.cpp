#include "Einmessung_koordinatensystem.h"
#include "ui_einmessung_koordinatensystem.h"

Einmessung_Koordinatensystem::Einmessung_Koordinatensystem(Rutenerkennung *r, QDomDocument *xml_doc, QWidget *parent) :
    m_xml_doc(xml_doc),
    QWidget(parent),
    ui(new Ui::Einmessung_Koordinatensystem)
{
    ui->setupUi(this);

    // K_Points einlesen
    QDomElement docElement = m_xml_doc->documentElement();
    QDomElement cam = docElement.firstChildElement("Camera");
    QDomElement calib = cam.firstChildElement("Calibration");
    QDomNodeList nodeList = calib.elementsByTagName("Point");
    for( int i = 0; i < nodeList.count(); i++ ){
        Vec2f vec;
        QDomElement point = nodeList.at(i).toElement();
        qInfo() << "Read CalibrationPoint: " << point.attribute("id").toInt();
        vec[0] = point.elementsByTagName("x").at(0).toElement().text().toFloat();
        vec[1] = point.elementsByTagName("y").at(0).toElement().text().toFloat();
        qInfo() << "( " << vec[0] << " , " << vec[1] << " )";
        K_Points.push_back(vec);
    }

    QObject::connect( r, SIGNAL(Ergebnis_BW(const cv::Mat *)), this, SLOT(Bild(const cv::Mat *)), Qt::QueuedConnection);
}

Einmessung_Koordinatensystem::~Einmessung_Koordinatensystem()
{
    delete ui;
}

void Einmessung_Koordinatensystem::clear(){
    B_Points.clear();
}

void Einmessung_Koordinatensystem::Bild( const cv::Mat *img_ ){
    Mat img;
    cv::cvtColor( *img_, img, CV_GRAY2RGB );
    //cv::resize(img, img, Size( ui->Image_Label->width(), ui->Image_Label->height() ) );

    uint counter = 1;
    for ( vector<Vec2i>::iterator it = B_Points.begin(); it != B_Points.end(); it++ ){
        Point p = (*it);
        uint radius = 3;
        circle( img, p, radius, CV_RGB(0, 0, 255), 1, 8, 0 );
        putText( img, std::to_string(counter), p + Point(radius+2, radius+2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 0, 255) );
        counter++;
    }

    ui->Image_Label->setFixedSize(QSize( img.cols, img.rows ) );
    ui->Image_Label->setPixmap( QPixmap::fromImage( QImage( img.data, img.cols, img.rows, img.step, QImage::Format_RGB888) ) );
    ui->Image_Label->show();
}

void Einmessung_Koordinatensystem::on_Image_Label_Clicked(QMouseEvent *event)
{
    Vec2i pos = Vec2i(event->pos().x(), event->pos().y());
    switch( event->button() ){
        case Qt::LeftButton:
            B_Points.push_back(pos);
        break;
        case Qt::RightButton:
            for( std::vector<Vec2i>::iterator it = B_Points.begin(); it != B_Points.end(); ++it ){
                if( norm(pos - *it) < 10 ){
                    it = B_Points.erase(it);
                    return;
                }
            }
        break;
    default: break;
    }
}

void Einmessung_Koordinatensystem::on_Berechne_Koord_clicked()
{
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
        return;
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

    KpB_Org = K_B_T( cv::Rect(2,0,1,2) ).clone();
    cout << "KpB_Org" << endl;
    cout << KpB_Org << endl;

    QDomElement docElement = m_xml_doc->documentElement();
    QDomElement cam = docElement.firstChildElement("Camera");
    QDomElement rot = cam.firstChildElement("Rotation");
    QDomNodeList nodeList = rot.elementsByTagName("r");
    QDomElement mat;

    for(int ii = 0;ii < nodeList.count(); ii++){
        mat = nodeList.at(ii).toElement();
        char *mat_entry = mat.attribute("id").toLocal8Bit().data();
        mat.firstChild().setNodeValue( QString::number(K_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1')) );
    }

    QDomElement trans = cam.firstChildElement("Translation");
    QDomElement point = trans.firstChildElement("Point");
    point.firstChildElement("x").firstChild().setNodeValue( QString::number(KpB_Org.at<float>(0,0)) );
    point.firstChildElement("y").firstChild().setNodeValue( QString::number(KpB_Org.at<float>(0,1)) );

    QMessageBox msgBox;
    msgBox.setText("Einmessung der Koordinatensysteme erfolgreich.");
    msgBox.setInformativeText("Konfiguration muss noch gespeichert werden.");
    msgBox.exec();

}

#include "Einmessung_maske.h"
#include "ui_einmessung_maske.h"

Einmessung_Maske::Einmessung_Maske(Rutenerkennung *r, QDomDocument *xml_doc, QWidget *parent) :
    m_xml_doc(xml_doc),
    QWidget(parent),
    ui(new Ui::Einmessung_Maske)
{
    ui->setupUi(this);

    QObject::connect( r, SIGNAL(Ergebnis_BW(const cv::Mat *)), this, SLOT(Bild(const cv::Mat *)), Qt::QueuedConnection);
}

Einmessung_Maske::~Einmessung_Maske()
{
    delete ui;
}

void Einmessung_Maske::clear(){
    M_Points.clear();
}

void Einmessung_Maske::Bild( const cv::Mat *img_ ){
    Mat img;
    cv::cvtColor( *img_, img, CV_GRAY2RGB );
    //cv::resize(img, img, Size( ui->Image_Label->width(), ui->Image_Label->height() ) );

    if( !M_Points.empty() ){
        const cv::Point *pts = (const cv::Point*) Mat(M_Points).data;
        int nptr = M_Points.size(); //Mat(M_Points.rows)
        cout << "Einmessung Maske" << endl;
        polylines(img, &pts, &nptr, 1, true, CV_RGB(0, 0, 255) );
    }

    ui->Image_Label->setFixedSize(QSize( img.cols, img.rows ) );
    ui->Image_Label->setPixmap( QPixmap::fromImage( QImage( img.data, img.cols, img.rows, img.step, QImage::Format_RGB888) ) );
    ui->Image_Label->show();
}


void Einmessung_Maske::on_Erstelle_Maske_clicked()
{
    QDomElement docElem = m_xml_doc->documentElement(); 	// Rutenerkennung
    QDomElement e = docElem.firstChildElement("Erkennung");
    QDomElement m_old = e.firstChildElement("Mask");

    QDomElement m_new = m_xml_doc->createElement("Mask");
    uint counter = 1;
    for( vector<Vec2i>::iterator i = M_Points.begin() ; i != M_Points.end(); ++i ){
        QDomElement p = m_xml_doc->createElement( "Point" );
        p.setAttribute("id", QString::number(counter));

        QDomElement x = m_xml_doc->createElement( "x" );
        QDomText valueX = m_xml_doc->createTextNode( QString::number((*i)[0]) );

        QDomElement y = m_xml_doc->createElement( "y" );
        QDomText valueY = m_xml_doc->createTextNode( QString::number((*i)[1]) );

        p.appendChild(x).appendChild(valueX);
        p.appendChild(y).appendChild(valueY);
        m_new.appendChild(p);
        counter++;
    }
    e.replaceChild(m_new, m_old);

    QMessageBox msgBox;
    msgBox.setText("Erstellung der Maske erfolgreich.");
    msgBox.setInformativeText("Konfiguration muss noch gespeichert werden.");
    msgBox.exec();
}

void Einmessung_Maske::on_Image_Label_Clicked(QMouseEvent *event)
{
    Vec2i pos = Vec2i(event->pos().x(), event->pos().y());
    switch( event->button() ){
        case Qt::LeftButton:
            M_Points.push_back(pos);
        break;
        case Qt::RightButton:
            for( std::vector<Vec2i>::iterator it = M_Points.begin(); it != M_Points.end(); ++it ){
                if( norm(pos - *it) < 10 ){
                    it = M_Points.erase(it);
                    return;
                }
            }
        break;
    default: break;
    }
}

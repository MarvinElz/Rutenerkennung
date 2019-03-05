#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Kommunikation *k, Videoquelle *v, Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_k(k),
    m_v(v),
    m_r(r),
    m_b(b),
    m_xml_doc(xml_doc),
    ko(r, xml_doc),
    ma(r, xml_doc)
{
    ui->setupUi(this);
}

void MainWindow::Valide_Stecklinge( vector<Steckling*>* valide_stecklinge ){
    //cout << "Valide_Stecklinge" << endl;
    Stecklinge = *valide_stecklinge;
}

void MainWindow::ShowImage( const cv::Mat *img_ ){
    mutex.lock();
    Mat img = img_->clone();
    mutex.unlock();

    if( m_format == QImage::Format_RGB888 )
        cv::cvtColor( img, img, CV_BGR2RGB );
    if( m_format == QImage::Format_Grayscale8 )
        cv::cvtColor( img, img, CV_GRAY2RGB );

    if( ui->Show_Grid->isChecked() ){
        for( uint x = 0; x < 100; x+=10 ){
            Vec2i lo = m_k->K_to_B( Vec2f(x, 0) );
            Vec2i hi = m_k->K_to_B( Vec2f(x, 100) );
            cv::line( img, lo, hi, CV_RGB(0, 0, 255) );
        }
        for( uint y = 0; y < 100; y += 10){
            Vec2i lo = m_k->K_to_B( Vec2f(0  , y) );
            Vec2i hi = m_k->K_to_B( Vec2f(100, y) );
            cv::line( img, lo, hi, CV_RGB(0, 0, 255) );
        }
    }

    if( ui->Show_Plausibility->isChecked() ){
        for (vector<Steckling*>::iterator j = Stecklinge.begin() ; j != Stecklinge.end(); ++j){
            uint radius = 3;
            circle( img, (*j)->pos, radius, CV_RGB(0, 255, 0), 1, 8, 0 );
            Point p((*j)->pos[0], (*j)->pos[1]);
            putText( img, std::to_string((*j)->m_plausibility), p + Point(radius+2, 0), FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(0, 255, 0) );
        }
    }
    cv::resize(img, img, Size( ui->Image_Label->width(), ui->Image_Label->height() ) );
    ui->Image_Label->setPixmap( QPixmap::fromImage( QImage( img.data, img.cols, img.rows, img.step, QImage::Format_RGB888) ) );
    ui->Image_Label->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Bildauswahl_activated(int index)
{
    QObject::disconnect(m_r, 0, this, SLOT(ShowImage(const cv::Mat *)));
    QObject::disconnect(m_b, 0, this, SLOT(ShowImage(const cv::Mat *)));
    QObject::disconnect(m_v, 0, this, SLOT(ShowImage(const cv::Mat *)));
    switch( index ){
        //case raw:
        //    QObject::connect( m_v, SIGNAL(New_Raw_Image(cv::Mat *)), this, SLOT(ShowImage(cv::Mat *)), Qt::QueuedConnection);
        //    m_format = QImage::Format_RGB888;
        //    break;
        case Images::bw:
            QObject::connect( m_r, SIGNAL( Ergebnis_BW(const cv::Mat *)), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case Images::masked:
            QObject::connect( m_r, SIGNAL( Ergebnis_Masked( const cv::Mat * )), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case Images::binary:
            QObject::connect( m_r, SIGNAL( Ergebnis_Binary(const cv::Mat *)), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case Images::binary_opened:
            QObject::connect( m_r, SIGNAL( Ergebnis_Binary_Opened(const cv::Mat *)), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case Images::distance:
            QObject::connect( m_r, SIGNAL( Ergebnis_Dist(const cv::Mat *)), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        case Images::distance_threshold:
            QObject::connect( m_r, SIGNAL(Ergebnis_Dist_Thres(const cv::Mat *)), this, SLOT(ShowImage(const cv::Mat *)), Qt::QueuedConnection);
            m_format = QImage::Format_Grayscale8;
            break;
        default:
            cout << "Bildauswahl nicht unterstuetzt!!!" << endl;
    }
}


void MainWindow::on_Maske_clicked()
{
    ma.clear();
    ma.show();
}

void MainWindow::on_Einmessen_clicked()
{
    ko.clear();
    ko.show();
}

void MainWindow::on_Save_Configuration_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Konfiguration speichern?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel );
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch(ret){
        case QMessageBox::Save:
        {
            QString filename = QFileDialog::getSaveFileName( this, tr("Test"), tr("Config.xml") );
            QFile file(filename);
            file.open( QIODevice::WriteOnly | QIODevice::Text );
            QTextStream stream( &file );
            stream << m_xml_doc->toString();
            file.close();
        }
    default:
        break;
    }
}


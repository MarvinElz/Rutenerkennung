#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Rutenerkennung *r, Beobachter *b, QDomDocument *xml_doc, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_r(r),
    m_b(b)
{
    ui->setupUi(this);
}

void MainWindow::ShowImage( cv::Mat *img ){
    //Mat img;
    //cv::cvtColor( *img_, img, CV_BGR2RGB );
    ui->Image_Label->setPixmap( QPixmap::fromImage( QImage( img->data, img->cols, img->rows, img->step, m_format) ) );
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
    switch( index ){
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
        default:
            cout << "Bildauswahl nicht unterstuetzt!!!" << endl;
    }
}

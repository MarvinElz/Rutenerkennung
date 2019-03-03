#include "Rutenerkennung.h"

Rutenerkennung::Rutenerkennung( QDomDocument *xml_doc )
{
    QDomElement docElem = xml_doc->documentElement();   // Rutenerkennung
    // Parameter der seriellen Schnittstelle einlesen
    QDomElement s = docElem.firstChildElement("Stecklinge");
    QDomElement g = s.firstChildElement("Groesse");
    m_Min = g.firstChildElement("Min").text().toFloat();
    cout << "m_Min: " << m_Min << endl;
    m_Max = g.firstChildElement("Max").text().toFloat();
    cout << "m_Max: " << m_Max << endl;

    Mat K_B_R = Mat::zeros(2, 2, CV_32F);
    QDomElement cam = docElem.firstChildElement("Camera");

    QDomElement rot = cam.firstChildElement("Rotation");
    QDomNodeList nodeList = rot.elementsByTagName("r");
    for(int ii = 0;ii < nodeList.count(); ii++){
        QDomElement mat = nodeList.at(ii).toElement();
        char *mat_entry = mat.attribute("id").toLocal8Bit().data();
        qInfo() << "\t\t\tRead Mat_Entry " << mat.attribute("id");
        K_B_R.at<float>(mat_entry[0]-'1',mat_entry[1]-'1') = mat.text().toFloat();
    }

    float masstabx, masstaby;
    masstabx = norm( K_B_R(cv::Rect(0,0,1,2)) , Mat::zeros(2,1, CV_32F), NORM_L2 );
    masstaby = norm( K_B_R(cv::Rect(1,0,1,2)) , Mat::zeros(2,1, CV_32F), NORM_L2 );
    m_masstab = (masstabx + masstaby)/2.0;
    cout << "m_masstab: " << m_masstab << endl;

    QDomElement e = docElem.firstChildElement("Erkennung");
    m_Threashold_SW = e.firstChildElement("Threshold_SW").text().toInt();
    cout << "m_Threashold_SW" << m_Threashold_SW << endl;

    QDomElement m = e.firstChildElement("Mask");
    QDomNodeList mask_elements = m.elementsByTagName("Point");
    for(int ii = 0;ii < mask_elements.count(); ii++){
        QDomElement point = mask_elements.at(ii).toElement();
        int index = point.attribute("id").toInt();
        int x = point.firstChildElement("x").text().toInt();
        int y = point.firstChildElement("y").text().toInt();
        cout << "Masken Punkt: " << index << " (" << x << "," << y << ")" << endl;
        M_Points.push_back( Vec2i( x, y ) );
     }

}

void Rutenerkennung::run(){
    cout << "Rutenerkennungsthread gestartet" << endl;
    while(1){
        if( m_busy ){
            usleep(100);
        }else{
            m_busy = true;
            cout << "Emit HoleNeuesBild" << endl;
            emit HoleNeuesBild();
            //usleep(3*1000*1000);
        }
    }
}


// Parameter: SchwarzWeiss-Threshold
//              float Min = 0.3;	// Durchmesser in mm
//              float Max = 123;

void Rutenerkennung::NeuesBild(Mat *frame){
    m_busy = true;

    cout << "Neues Bild erhalten" << endl;
    //imshow( "frame", *frame );
    if( frame->cols == 0 ){
        m_busy = false;
        return;
    }

    ticks = clock();
    m_mutex.lock();
    cvtColor(*frame, bw, CV_BGR2GRAY);
    m_mutex.unlock();

    cv::resize(bw, bw, Size( bw.cols * 0.7, bw.rows * 0.7 ) );

    emit Ergebnis_BW( &bw );

    if( mask.cols == 0 ){
        mask = cv::Mat::zeros(bw.size(), CV_8UC1);
        //const cv::Point *pts = (const cv::Point*) Mat(M_Points).data;
        const cv::Point *pts = (const cv::Point*) Mat(M_Points).data;
        int nptr = M_Points.size(); //Mat(M_Points.rows)
        fillPoly(mask, &pts, &nptr, 1, 255 );
    }

    cv::bitwise_and( bw, mask, bw_masked );
    emit Ergebnis_Masked( &bw_masked );

    // Binäres Bild berechnen
    threshold( bw_masked, binary, m_Threashold_SW, 255, THRESH_BINARY );
    //imshow( "binary", binary );
    emit Ergebnis_Binary( &binary );

    // noise removal (rechenaufwendig)
    int dilation_size = 3;
    Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );
    //cv::Mat kernel = Mat::ones( 2, 2, bw.type() );

    erode( binary, binary_opened, element );
    //morphologyEx( binary, binary_opened, MORPH_OPEN, kernel );
    //imshow( "binary_opened", binary_opened );
    emit Ergebnis_Binary_Opened( &binary_opened );

    // Finding sure foreground area    
    distanceTransform(binary_opened, dist_transformed, CV_DIST_L2, 3);
    //imshow( "dist_transformed", dist_transformed );

    dist_transformed.convertTo(dist_transformed, CV_8U);


  //  m_mutex.lock();
    // Alles, was unterhalb von dem halben Durchmesser ist, fliegt raus
    threshold( dist_transformed, dist_transformat_thres, static_cast<uint>(m_Min/m_masstab/2.0), 255, THRESH_BINARY );
    //threshold( dist_transformed, dist_transformat_thres, 20, 255, THRESH_BINARY );
    //imshow( "dist_transformat_thres", dist_transformat_thres );
  //  m_mutex.unlock();

    dist_transformed *= 10;
    emit Ergebnis_Dist( &dist_transformed );

    //dist_transformat_thres.convertTo(dist_transformat_thres, CV_8U);
    emit Ergebnis_Dist_Thres( &dist_transformat_thres );

    // Detektion über Kontur
    vector<vector<Point> > contours;            // CV_RETR_EXTERNAL
    findContours(dist_transformat_thres.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);




    bw_with_pos = bw.clone();
    // Mittelpunkt der Kontur (Position der Rute) bestimmen
    m_pos.clear();
    for ( uint c = 0; c < contours.size(); c++ ){
        Point center = Point(0,0);
        const vector<Point> contour = contours.at(c);
        for( uint i = 0; i < contour.size(); i++ ){
            center += contour.at(i);
        }
        center.x /= contour.size();
        center.y /= contour.size();

        // hier Größe berechnen und entsprechend Filtern
        // eine Filterung über Min ist hier leider nicht möglich,
        // weil die Querschnittsflächen der Ruten aufgrund der
        // vorherigen Schritte kleiner sind als in Realität
        double area = contourArea( contour );
        if ( area/M_PI < (m_Max/m_masstab/2)*(m_Max/m_masstab/2) ){
            circle(bw_with_pos, center, 5, CV_RGB(255,0,0), -1);
            m_pos.push_back(Vec2i(center.x, center.y));
        }
    }
    emit Ergebnis( &bw_with_pos );
    //imshow( "bw", bw );
    std::cout << "Calculated in " << (double)(clock() - ticks)/CLOCKS_PER_SEC << " seconds" << std::endl;

    // Auswertung der Rutenpositionen hinsichtlich der Plausibilität
    // Je häufiger ein Steckling in Bildern detektiert wurde,
    // desto eher soll er ausgeworfen werden.
    cout << "emit ErkannteStecklinge mit Size: " << m_pos.size() << endl;
    emit ErkannteStecklinge( m_pos );
    //cout << "emit HoleNeuesBild" << endl;
    //usleep(3 * 1000 * 1000);
    //emit HoleNeuesBild();
    m_busy = false;
}

#include "Rutenerkennung.h"

Rutenerkennung::Rutenerkennung( QDomDocument *xml_doc )
{

}

// Parameter: SchwarzWeiss-Threshold
//              float Min = 0.3;	// Durchmesser in mm
//              float Max = 123;

void Rutenerkennung::NeuesBild(Mat *frame){
    cout << "Neues Bild erhalten" << endl;
    //cv::resize(frame, frame, size);
    Mat bw;

    ticks = clock();
    cvtColor(*frame, bw, CV_BGR2GRAY);

    cv::Mat binary;
    // TODO: Threshold anpassen
    threshold( bw, binary, 80, 255, THRESH_BINARY );
    imshow( "binary", binary );

    // noise removal (rechenaufwendig)
    cv::Mat kernel = Mat::ones( 2, 2, bw.type() );
    cv::Mat binary_opened;
    morphologyEx( binary, binary_opened, MORPH_OPEN, kernel );
    imshow( "binary_opened", binary_opened );

    // TODO: Alternative checken: watershed(src, markers);
    // Finding sure foreground area
    cv::Mat dist_transformed;// = Mat::zeros(frame.size(), CV_8U);
    distanceTransform(binary_opened, dist_transformed, CV_DIST_L2, 3);
    imshow( "dist_transformed", dist_transformed );

    cv::Mat dist_transformat_thres;
    // TODO: Min aus XML einlesen
    float Min = 0.3;	// Durchmesser in mm
    // Alles, was unterhalb von dem halben Durchmesser ist, fliegt raus
    //threshold( dist_transformed, dist_transformat_thres, static_cast<uint>(Min/masstab/2.0), 255, CV_THRESH_BINARY );
    threshold( dist_transformed, dist_transformat_thres, 20, 255, THRESH_BINARY );
    imshow( "dist_transformat_thres", dist_transformat_thres );

    dist_transformat_thres.convertTo(dist_transformat_thres, CV_8U);

    // Detektion über Kontur
    vector<vector<Point> > contours;
    findContours(dist_transformat_thres, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

      // Mittelpunkt der Kontur (Position der Rute) bestimmen
    m_pos.clear();
    for ( int c = 0; c < contours.size(); c++ ){
        Point center = Point(0,0);
        const vector<Point> contour = contours.at(c);
        for( int i = 0; i < contour.size(); i++ ){
            center += contour.at(i);
        }
        center.x /= contour.size();
        center.y /= contour.size();

        // TODO: Max aus XML auslesen
        float Max = 10;	// Durchmesser in mm
        float masstab = 0.1;
        // hier Größe berechnen und entsprechend Filtern
        // eine Filterung über Min ist hier leider nicht möglich,
        // weil die Querschnittsflächen der Ruten aufgrund der
        // vorherigen Schritte kleiner sind als in Realität
        double area = contourArea( contour );
        if ( area/M_PI < (Max/masstab/2)*(Max/masstab/2) ){
            circle(bw, center, 5, CV_RGB(255,0,0), -1);
            m_pos.push_back(Vec2i(center.x, center.y));
        }
    }
    imshow( "bw", bw );
    std::cout << "Calculated in " << (double)(clock() - ticks)/CLOCKS_PER_SEC << " seconds" << std::endl;
    // TODO: im eigenen Thread aufrufen:
    // Auswertung der Rutenpositionen hinsichtlich der Plausibilität
    // Je häufiger ein Steckling in Bildern detektiert wurde,
    // desto eher soll er ausgeworfen werden.
    cout << "emit ErkannteStecklinge" << endl;
    emit ErkannteStecklinge( m_pos );
    cout << "emit HoleNeuesBild" << endl;
    usleep(3000000);
    emit HoleNeuesBild();
}

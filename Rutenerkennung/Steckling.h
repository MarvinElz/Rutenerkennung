#ifndef STECKLING_H
#define STECKLING_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Steckling
{
public:
    Steckling(Vec2i);
    Vec2i pos;
    uint m_plausibility;
};

#endif // STECKLING_H

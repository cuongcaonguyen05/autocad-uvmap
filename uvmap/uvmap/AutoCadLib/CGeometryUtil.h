#pragma once

#include "CvGePolygon2D.h"

class CGeometryUtil
{
public:
    static bool isPointInside(const CvGe::CvGePolygon2D& polygon, const CvGe::CvGePoint2D& point);
    static char* getRandomString32();
};

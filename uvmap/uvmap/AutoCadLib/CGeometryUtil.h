// CGeometryUtil.h : Các hàm TÍNH TOÁN hình học dùng chung cho cả project.
// Hàm thuần (stateless) nên để static, gọi trực tiếp: CGeometryUtil::isPointInside(...).
#pragma once

#include "CvGePolygon2D.h"

class CGeometryUtil
{
public:
    // Kiểm tra điểm có nằm trong đa giác không (thuật toán winding-number).
    // Điểm trùng đỉnh -> coi như nằm trong (true).
    static bool isPointInside(const CvGe::CvGePolygon2D& polygon, const CvGe::CvGePoint2D& point);

    static char* getRandomString32();
};

// AutoCadUVMap.h : Tính năng sinh toạ độ UV cho đối tượng AutoCAD.
#pragma once

#include "pch.h"
#include "IFeature.h"
#include "AcDbUVMap.h"

#include <string>

class AutoCadUVMap : public IFeature
{
public:
    AutoCadUVMap();

    // ----- IFeature -----
    std::string Name() const override;
    std::string Description() const override;
    int Execute() override;

	// ----- Các hàm riêng của bạn -----
    void uvmapFollowPath();
    void modelSurfaceGeometryData(std::list<CAcDbUVMap*>& lstDbUVMap);
};

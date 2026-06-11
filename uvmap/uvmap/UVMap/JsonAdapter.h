#pragma once
#include "C3dFaceJson.h"
#include "GeoJson.h"
#include "ReadWriteJson.h"

class CJsonAdapter
{
    double m_bulkCx, m_bulkCy, m_bulkMaxDist;

public:
    CJsonAdapter();
    ~CJsonAdapter();

    static CJsonAdapter* create();

    void exportEntity(AcDbEntity* pEnt, vizDatabase::CReadWriteJson* prwJson);
    void face3dJson(AcDbEntity* acdbFace, vizDatabase::CReadWriteJson* geoJson);
    void getListUVFaceWithXatlas(AcDbFace*& dbFace, std::list<vizDatabase::CGeoJson*>& listGeoJson);

    bool entityIsOutlier(AcDbEntity* pEnt);
    bool face3dJsonEx(AcDbEntity* acdbFace, std::list<vizDatabase::CGeoJson*>& listGeoJson);
};
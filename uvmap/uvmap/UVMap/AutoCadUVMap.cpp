#include "pch.h"
#include "AutoCadUVMap.h"
#include "AcDbUVMap.h"
#include "AcDbUVGrid.h"
#include "JsonAdapter.h"
#include "ReadWriteJson.h"

AutoCadUVMap::AutoCadUVMap()
{
}

std::string AutoCadUVMap::Name() const
{
    return "autocad_uvmap";
}

std::string AutoCadUVMap::Description() const
{
    return "Sinh toa do UV cho doi tuong AutoCAD";
}

int AutoCadUVMap::Execute()
{
    return 0;
}

void AutoCadUVMap::uvmapFollowPath()
{
	AcGePoint3d pSel;
	AcDbEntity* pEnt = CAcadUtil::GetInstance()->EntSelectEntity(L"\nSelect Alignment Or Polyline: ", pSel);
	if (!pEnt)
		return;

	pEnt->close();

	if (!pEnt->isKindOf(AcDbPolyline::desc()))
		return;

	double height = 1, width = 1, row = 1;

	if (!CAcadUtil::GetInstance()->GetDouble(_T("\r\nHeight : "), 1, height))
	{
		return;
	}

	if (!CAcadUtil::GetInstance()->GetDouble(_T("\r\nWidth : "), 1, width))
	{
		return;
	}

	if (!CAcadUtil::GetInstance()->GetDouble(_T("\r\nNumber Of Rows : "), 1, row))
	{
		return;
	}

	AcDbPolyline* polyline = AcDbPolyline::cast(pEnt);
	if (!polyline)
	{
		return;
	}

	CAcDbUVMap* dbUvMap = new CAcDbUVMap;
	CAcDbUVGrid* uvGrid = new CAcDbUVGrid;
	int nrow = std::ceil(row);
	uvGrid->setMapPointUVByAlignment(polyline, height, width, nrow, 0);
	dbUvMap->getGeUVMap()->setGeUVGrid(uvGrid->getGeUVGrid());
	dbUvMap->setDbUVGrip(uvGrid);
	dbUvMap->m_dbPolyline = polyline;
	dbUvMap->setDrawUVGrid(true);
	uvGrid->setFixed(false);
	CAcadUtil::GetInstance()->Append(dbUvMap);
}

void AutoCadUVMap::modelSurfaceGeometryData(std::list<CAcDbUVMap*>& lstDbUVMap)
{
	CAcadOpenForWrite opw;
	std::list<AcDbEntity*> objectIds;
	AcDbDatabase* database = acDocManager->curDocument()->database();
	if (!database)
		database = acdbHostApplicationServices()->workingDatabase();

	if (!database)
		return;

	if (!CAcadUtil::GetInstance()->scanAllDatabase(objectIds, database))
		return;

	bool bCatch = false;
	CAcDbUVGrid uvGrid;
	std::list<CAcGeUVGrid*> lstGeUvGrip;
	if (lstDbUVMap.size() > 0)
	{
		uvGrid.setAllMapGeQuadrangle(lstDbUVMap, lstGeUvGrip);
	}

	if (lstGeUvGrip.empty())
		return;

	CJsonAdapter* jsonAdapter = CJsonAdapter::create();
	vizDatabase::CReadWriteJson* geoJson = vizDatabase::CReadWriteJson::create();

	std::list<AcDbEntity*>::iterator ite = objectIds.begin();
	for (; ite != objectIds.end(); ite++)
	{
		AcDbEntity* pEnt = *ite;
		if (!pEnt)
			continue;

		if (!pEnt->isKindOf(AcDbFace::desc()))
			continue;

		jsonAdapter->exportEntity(pEnt, geoJson);
		bCatch = true;
	}

	if (bCatch) {
		double dWidth = 1, dHeight = 1; // tỉ lệ uv 1-1
		geoJson->surfaceUVMap(lstGeUvGrip, dWidth, dHeight);
	}

	delete jsonAdapter;
	delete geoJson;
}

void AutoCadUVMap::modelSolidGeometryData(std::list<CAcDbUVMap*>& lstDbUVMap) {
	CAcadOpenForWrite opw;
	std::list<AcDbEntity*> objectIds;
	AcDbDatabase* database = acDocManager->curDocument()->database();
	if (!database)
		database = acdbHostApplicationServices()->workingDatabase();

	if (!database)
		return;

	if (!CAcadUtil::GetInstance()->scanAllDatabase(objectIds, database))
		return;

	CJsonAdapter* jsonAdapter = CJsonAdapter::create();
	vizDatabase::CReadWriteJson* geoJson = vizDatabase::CReadWriteJson::create();
	bool bCatch = false;

	std::list<AcDbEntity*>::iterator ite = objectIds.begin();
	for (; ite != objectIds.end(); ite++)
	{
		AcDbEntity* pEnt = *ite;
		if (!pEnt)
			continue;

		if (!pEnt->isKindOf(AcDb3dSolid::desc()))
			continue;

		jsonAdapter->exportEntity(pEnt, geoJson);
		bCatch = true;
	}

	if (bCatch) {
		double dWidth = 1, dHeight = 1; // tỉ lệ uv 1-1
		geoJson->meshUVMap(dWidth, dHeight);
	}

	delete jsonAdapter;
	delete geoJson;
	return;
}

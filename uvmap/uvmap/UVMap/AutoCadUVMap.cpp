// AutoCadUVMap.cpp : Hiện thực tính năng AutoCadUVMap.
// Các header ObjectARX (AcGePoint3d, AcDbEntity, AcDbPolyline, _T...) đã nằm trong pch.h.
#include "pch.h"
#include "AutoCadUVMap.h"
#include "AcDbUVMap.h"
#include "AcDbUVGrid.h"
#include "JsonAdapter.h"
#include "ReadWriteJson.h"
// --- Header các class/hàm riêng của bạn: bổ sung đúng đường dẫn ---
// #include "CAcDbAlignmentObject.h"  // CAcDbAlignmentObject
// #include "CAcDbUVMap.h"            // CAcDbUVMap
// #include "CAcDbUVGrid.h"           // CAcDbUVGrid
// và khai báo hàm getDouble(...)

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
	// đọc toàn bộ data geometry của các đối tượng trong AutoCAD, lưu vào objectIds - và sử dụng CAcDbUVMap đã được tạo ở hàm modelSurfaceGeometryData
	// hàm này dành cho suface 3 đỉnh(tam giác) để mapping uv - data > 4 đỉnh cần convert sang tam giác trước khi mapping uv

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

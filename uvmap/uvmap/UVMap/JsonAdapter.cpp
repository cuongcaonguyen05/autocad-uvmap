#include "pch.h"
#include "JsonAdapter.h"

bool isClockwise(const AcGePoint3d& pt1, const AcGePoint3d& pt2, const AcGePoint3d& pt3) {
	AcGeVector3d ve1 = pt2 - pt1;
	AcGeVector3d ve2 = pt3 - pt2;
	AcGeVector3d vCross = ve1.crossProduct(ve2);
	return vCross.z > 0;
}

bool entityCentroidXY(AcDbEntity* pEnt, double& cx, double& cy)
{
	AcDbExtents ext;
	if (!pEnt || pEnt->getGeomExtents(ext) != Acad::eOk)
		return false;
	cx = (ext.minPoint().x + ext.maxPoint().x) * 0.5;
	cy = (ext.minPoint().y + ext.maxPoint().y) * 0.5;
	return true;
}

CJsonAdapter::CJsonAdapter()
{
	m_bulkCx = 0; m_bulkCy = 0; m_bulkMaxDist = 0;
}

CJsonAdapter::~CJsonAdapter()
{
}

CJsonAdapter* CJsonAdapter::create() {
    return new CJsonAdapter;
}

void CJsonAdapter::exportEntity(AcDbEntity* pEnt, vizDatabase::CReadWriteJson* prwJson)
{
	if (!pEnt) {
		return;
	}
	if (pEnt->isKindOf(AcDbFace::desc())) {
		this->face3dJson(pEnt, prwJson);
	}
	//else if (pEnt->isKindOf(AcDb3dSolid::desc())) {
	//	this->solidJson(pEnt, prwJson);
	//}
	return;
}

void CJsonAdapter::face3dJson(AcDbEntity* acdbFace, vizDatabase::CReadWriteJson* geoJson)
{
	if (!geoJson)
		return;

	if (entityIsOutlier(acdbFace))
		return;

	std::list<vizDatabase::CGeoJson*> listGeoJson;
	if (face3dJsonEx(acdbFace, listGeoJson))
	{
		AcDbHandle handle;
		acdbFace->getAcDbHandle(handle);
		unsigned long aligmentPos = CAcadUtil::GetInstance()->whash(acdbFace->layer());
		if (aligmentPos == 0)
			aligmentPos = 212005;

		std::list<vizDatabase::CGeoJson*>::iterator geo = listGeoJson.begin();
		for (; geo != listGeoJson.end(); geo++)
		{
			if ((*geo))
			{
				double dR = 0, dG = 0, dB = 0;
				geoJson->getColor(dR, dG, dB);
				int iR = int(255 * dR),
					iG = int(255 * dG),
					iB = int(255 * dB);

				(*geo)->setRGBColor(iR, iG, iB);

				int maxSize = 0;
				std::wstring wstrLayer = acdbFace->layer();
				int pos = wstrLayer.find_first_of(_T("|"));
				int strSize = wstrLayer.size();
				unsigned layerId = CAcadUtil::GetInstance()->whash(wstrLayer.substr(pos + 1, strSize).c_str());
				(*geo)->setLayer(layerId);
				(*geo)->setWLayer(wstrLayer);
				(*geo)->setAlignmentPos(aligmentPos);

				geoJson->append((*geo));
			}
		}
		return;
	}
	return;
}

bool CJsonAdapter::entityIsOutlier(AcDbEntity* pEnt)
{
	if (!pEnt)
		return false;
	double cx, cy;
	if (!entityCentroidXY(pEnt, cx, cy))
		return false;
	const double dx = cx - m_bulkCx, dy = cy - m_bulkCy;
	const double dist = sqrt(dx * dx + dy * dy);
	if (dist <= m_bulkMaxDist)
		return false;

	static long s_out = 0; ++s_out;
	if (s_out <= 200)
	{
		AcDbHandle h; pEnt->getAcDbHandle(h);
		ACHAR hb[64] = { 0 }; h.getIntoAsciiBuffer(hb);
	}
	return true;
}

bool CJsonAdapter::face3dJsonEx(AcDbEntity* acdbFace, std::list<vizDatabase::CGeoJson*>& listGeoJson) {
	if (!acdbFace)
		return false;

	if (!acdbFace->isKindOf(AcDbFace::desc()))
		return false;

	AcDbFace* dbFace = AcDbFace::cast(acdbFace);
	if (!dbFace)
		return false;

	getListUVFaceWithXatlas(dbFace, listGeoJson);
	return true;
}

void CJsonAdapter::getListUVFaceWithXatlas(AcDbFace*& dbFace, std::list<vizDatabase::CGeoJson*>& listGeoJson)
{
	double __x = 0, __y = 0, __z = 0; //có thể data cách gốc tạo độ 1 đoạn x

	AcGePoint3d pt0, pt1, pt2, pt3;
	dbFace->getVertexAt(0, pt0);
	dbFace->getVertexAt(1, pt1);
	dbFace->getVertexAt(2, pt2);
	dbFace->getVertexAt(3, pt3);

	vizDatabase::C3dFaceJson* geo = vizDatabase::C3dFaceJson::create();
	geo->setGeoInfo(pt0.x - __x, pt0.y - __y, pt0.z - __z);
	geo->setGeoInfo2d(pt0.x - __x, pt0.y - __y);
	geo->setGeoInfo(pt1.x - __x, pt1.y - __y, pt1.z - __z);
	geo->setGeoInfo2d(pt1.x - __x, pt1.y - __y);
	geo->setGeoInfo(pt2.x - __x, pt2.y - __y, pt2.z - __z);
	geo->setGeoInfo2d(pt2.x - __x, pt2.y - __y);

	if (!isClockwise(pt0, pt1, pt2))
	{
		geo = vizDatabase::C3dFaceJson::create();
		geo->setGeoInfo(pt2.x - __x, pt2.y - __y, pt2.z - __z);
		geo->setGeoInfo2d(pt2.x - __x, pt2.y - __y);
		geo->setGeoInfo(pt1.x - __x, pt1.y - __y, pt1.z - __z);
		geo->setGeoInfo2d(pt1.x - __x, pt1.y - __y);
		geo->setGeoInfo(pt0.x - __x, pt0.y - __y, pt0.z - __z);
		geo->setGeoInfo2d(pt0.x - __x, pt0.y - __y);
	}

	listGeoJson.push_back(geo);
	if (pt3.isEqualTo(pt0) || pt3.isEqualTo(pt1) || pt3.isEqualTo(pt2))
	{
		return;
	}

	geo = vizDatabase::C3dFaceJson::create();
	geo->setGeoInfo(pt0.x - __x, pt0.y - __y, pt0.z - __z);
	geo->setGeoInfo2d(pt0.x - __x, pt0.y - __y);
	geo->setGeoInfo(pt2.x - __x, pt2.y - __y, pt2.z - __z);
	geo->setGeoInfo2d(pt2.x - __x, pt2.y - __y);
	geo->setGeoInfo(pt3.x - __x, pt3.y - __y, pt3.z - __z);
	geo->setGeoInfo2d(pt3.x - __x, pt3.y - __y);

	if (!isClockwise(pt0, pt2, pt3))
	{
		geo = vizDatabase::C3dFaceJson::create();
		geo->setGeoInfo(pt3.x - __x, pt3.y - __y, pt3.z - __z);
		geo->setGeoInfo2d(pt3.x - __x, pt3.y - __y);
		geo->setGeoInfo(pt2.x - __x, pt2.y - __y, pt2.z - __z);
		geo->setGeoInfo2d(pt2.x - __x, pt2.y - __y);
		geo->setGeoInfo(pt0.x - __x, pt0.y - __y, pt0.z - __z);
		geo->setGeoInfo2d(pt0.x - __x, pt0.y - __y);
	}

	listGeoJson.push_back(geo);
	return;
}
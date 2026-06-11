#include "pch.h"
#include "JsonAdapter.h"
#include <sstream>   // std::wstringstream (đọc handle từ xdata SEWER_SOURCE)
#include <cstdlib>   // std::getenv, atof (meshNodeSpacing)

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

double meshNodeSpacing(double diag)
{
	if (diag <= 1e-9)
		return 0.0;
	const char* e = std::getenv("VPV_MESH_NS_DIV");
	if (!e || !*e)
		return 0.0;
	const double v = atof(e);
	if (v < 1.0)
		return 0.0;
	return diag / v;
}

// Stub: chưa map node id từ entity nguồn. Trả 0 -> mặt giữ alignmentPos theo handle.
// Thay bằng implementation thật khi cần gán node id theo xdata SEWER_SOURCE.
static unsigned long getIdNode(AcDbEntity* /*pEnt*/)
{
	return 0;
}

static long g_inCall = 0, g_inSewer = 0, g_inMassFail = 0, g_inVertFail = 0,
g_inFrameFail = 0, g_inProto = 0, g_inNoProtoV = 0, g_inVerifyFail = 0, g_inInstance = 0,
g_inLowCount = 0;

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
	else if (pEnt->isKindOf(AcDb3dSolid::desc())) {
		this->solidJson(pEnt, prwJson);
	}
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

void CJsonAdapter::solidJson(AcDbEntity* acdbSolid, vizDatabase::CReadWriteJson* geoJson)
{
	if (!acdbSolid)
		return;

	AcDbHandle handle;
	acdbSolid->getAcDbHandle(handle);
	unsigned long aligmentPos = handle.low() + handle.high();

	if (!acdbSolid->isKindOf(AcDb3dSolid::desc()))
		return;

	AcDb3dSolid* dbSolid = AcDb3dSolid::cast(acdbSolid);
	if (!dbSolid)
		return;

	if (entityIsOutlier(acdbSolid))
		return;

	AcDbExtents dbExt;
	dbSolid->bounds(dbExt);
	AcGePoint3d pMin = dbExt.minPoint(),
		pMax = dbExt.maxPoint(),
		pCen = pMin + (pMax - pMin) * 0.5;
	AcGeVector3d vTrans(pCen.x, pCen.y, pCen.z);

	AcDbEntity* iDbEnt = CAcadUtil::GetInstance()->GetEntytifromObjectID(dbSolid->objectId(), AcDb::kForWrite);
	dbSolid->assertWriteEnabled();
	dbSolid->transformBy(-vTrans);
	dbSolid->close();

	AcDbObjectIdArray objIdList;
	objIdList.setPhysicalLength(1);
	objIdList.append(dbSolid->objectId());
	AcDbFullSubentPath subPath;
	subPath.objectIds() = objIdList;

	AcBrBrep* brep = new AcBrBrep();
	brep->set(subPath);

	AcBrMesh2dControl mc;
	mc.setMaxAspectRatio(0.001);
	mc.setElementShape(AcBr::kAllTriangles);
	{
		AcDbExtents __ext;
		if (dbSolid->getGeomExtents(__ext) == Acad::eOk)
		{
			const double __ns = meshNodeSpacing((__ext.maxPoint() - __ext.minPoint()).length());
			if (__ns > 0.0) mc.setMaxNodeSpacing(__ns);
		}
	}
	AcBrMesh2dFilter mf;
	mf.insert(brep::make_pair((const AcBrEntity*&)brep, (const AcBrMesh2dControl)mc));
	AcBrMesh2d faceMesh;
	AcBr::ErrorStatus es;
	if ((es = faceMesh.generate(mf)) != AcBr::eOk) {
		delete brep;
		AcDbEntity* dbE2 = CAcadUtil::GetInstance()->GetEntytifromObjectID(dbSolid->objectId(), AcDb::kForWrite);
		if (dbE2)
		{
			dbE2->assertWriteEnabled();
			dbE2->transformBy(vTrans);
			dbE2->close();
		}
		return;
	}

	double __x = 0, __y = 0, __z = 0;
	std::wstring wstrLayer = acdbSolid->layer();
	int pos = wstrLayer.find_first_of(_T("|"));
	int strSize = wstrLayer.size();
	std::wstring wzSub = wstrLayer.substr(pos + 1, strSize);
	unsigned layerId = CAcadUtil::GetInstance()->whash(wzSub.c_str());

	delete brep;
	AcBrMesh2dElement2dTraverser meshElemTrav;
	if ((es = meshElemTrav.setMesh(faceMesh)) != AcBr::eOk) {
		acutPrintf(ACRX_T("\n Error in AcBrMesh2dElement2dTraverser::setMesh:"));
		acutPrintf(ACRX_T(" AutoCAD Error Code: %d\n"));
		AcDbEntity* dbE2 = CAcadUtil::GetInstance()->GetEntytifromObjectID(dbSolid->objectId(), AcDb::kForWrite);
		if (dbE2)
		{
			dbE2->assertWriteEnabled();
			dbE2->transformBy(vTrans);
			dbE2->close();
		}
		return;
	}
	
	const double __sdiag = (pMax - pMin).length();
	const double __mg = (__sdiag > 1.0 ? __sdiag : 1.0);
	const double __loX = pMin.x - __mg, __hiX = pMax.x + __mg;
	const double __loY = pMin.y - __mg, __hiY = pMax.y + __mg;
	const double __loZ = pMin.z - __mg, __hiZ = pMax.z + __mg;

	AcBr::ErrorStatus returnValue = AcBr::eOk;
	while (!meshElemTrav.done() && !acedUsrBrk() && returnValue == AcBr::eOk) {
		AcBrElement2dNodeTraverser elemNodeTrav;

		if (elemNodeTrav.setElement(meshElemTrav) != AcBr::eOk) {
			acutPrintf(ACRX_T("\n Error in AcBrElement2dNodeTraverser::setElement:"));
			acutPrintf(ACRX_T(" AutoCAD Error Code: %d\n"));
			break;
		}

		vizDatabase::C3dFaceJson* pobjFace = vizDatabase::C3dFaceJson::create();
		int vIndex = 0;
		bool faceOutlier = false; double badX = 0, badY = 0;
		AcGeVector3d normal;
		elemNodeTrav.getSurfaceNormal(normal);
		pobjFace->setNormal(normal.x, normal.y, normal.z);
		while (!elemNodeTrav.done() && returnValue == AcBr::eOk) {
			AcBrNode node;

			if (elemNodeTrav.getNode(node) != AcBr::eOk) {
				acutPrintf(ACRX_T("\n Error in AcBrElement2dNodeTraverser::getNode:"));
				acutPrintf(ACRX_T(" AutoCAD Error Code: %d\n"));
				continue;
			}
			AcGePoint3d point;
			node.getPoint(point);
			point.transformBy(vTrans);

			if (!_finite(point.x) || !_finite(point.y) || !_finite(point.z) ||
				point.x < __loX || point.x > __hiX ||
				point.y < __loY || point.y > __hiY ||
				point.z < __loZ || point.z > __hiZ)
			{
				faceOutlier = true; badX = point.x; badY = point.y;
			}

			pobjFace->setGeoInfo(point.x - __x, point.y - __y, point.z - __z);

			AcGePoint2d p2;
			elemNodeTrav.getParamPoint(p2);
			pobjFace->setGeoInfo2d(p2.x - __x, p2.y - __y);

			returnValue = elemNodeTrav.next();
		}

		if (faceOutlier) {
			static long s_badv = 0; ++s_badv;
			if (s_badv <= 200) {
				AcDbHandle h; acdbSolid->getAcDbHandle(h);
				ACHAR hb[64] = { 0 }; h.getIntoAsciiBuffer(hb);
			}
			delete pobjFace;
			returnValue = meshElemTrav.next();
			continue;
		}

		pobjFace->setRGBColor(204, 204, 204);
		pobjFace->setLayer(layerId);
		pobjFace->setWLayer(wstrLayer);
		pobjFace->setIdRoadSign(aligmentPos);
		pobjFace->setAlignmentPos(aligmentPos);
		geoJson->append(pobjFace);
		returnValue = meshElemTrav.next();

		struct resbuf* pRb, * pTemp;
		pRb = acdbSolid->xData(_T("SEWER_SOURCE"));
		if (!pRb)
		{
			continue;
		}
		pTemp = pRb->rbnext;
		CString czLow = L"";
		if (pTemp->restype == 1000
			|| pTemp->restype == 1001
			|| pTemp->restype == 1002
			|| pTemp->restype == 1003
			|| pTemp->restype == 1005)
			czLow = pTemp->resval.rstring;
		uint32_t ui32Low = 0;
		std::wstringstream wsLow(czLow.GetString());
		wsLow >> ui32Low;
		int32_t i32Low = pTemp->resval.rlong;

		pTemp = pTemp->rbnext;
		CString czHigh = L"";
		if (pTemp->restype == 1000
			|| pTemp->restype == 1001
			|| pTemp->restype == 1002
			|| pTemp->restype == 1003
			|| pTemp->restype == 1005)
			czHigh = pTemp->resval.rstring;
		uint32_t ui32High = 0;
		std::wstringstream wsHigh(czHigh.GetString());
		wsHigh >> ui32High;
		int32_t i32High = pTemp->resval.rlong;

		AcDbHandle hdlSrc;
		hdlSrc.setLow(ui32Low);
		hdlSrc.setHigh(ui32High);
		AcDbEntity* entSource = CAcadUtil::GetInstance()->GetEntytifromHandle(hdlSrc, AcDb::kForRead);
		if (!entSource)
		{
			AcDbHandle hdlSrcNew;
			hdlSrcNew.setLow(i32Low);
			hdlSrcNew.setHigh(i32High);
			entSource = CAcadUtil::GetInstance()->GetEntytifromHandle(hdlSrcNew, AcDb::kForRead);
			if (!entSource)
				continue;
		}
		entSource->close();

		unsigned long dbNodeId = 0;
		dbNodeId = getIdNode(entSource);
		if (dbNodeId != 0)
		{
			pobjFace->setAlignmentPos(dbNodeId);
		}
	}

	iDbEnt = CAcadUtil::GetInstance()->GetEntytifromObjectID(dbSolid->objectId(), AcDb::kForWrite);
	if (iDbEnt)
	{
		iDbEnt->assertWriteEnabled();
		iDbEnt->transformBy(vTrans);
		iDbEnt->close();
	}
	return;
}
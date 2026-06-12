#include "pch.h"
#include "ReadWriteJson.h"
#include "C3dFaceJson.h"
#include "xatlas.h"

using namespace vizDatabase;

CReadWriteJson::CReadWriteJson(void)
{
	m_numItem				= 0;
	m_currentObjectID		= -1;
	m_red					= 1;
	m_green					= 0;
	m_blue					= 0;
}

CReadWriteJson::~CReadWriteJson(void)
{
}

CReadWriteJson* CReadWriteJson::create()
{
	return new CReadWriteJson;
}

void CReadWriteJson::setColor(const double& r, const double& g, const double& b) {
	m_red = r;
	m_green = g;
	m_blue = b;
}
void CReadWriteJson::getColor(double& r, double& g, double& b)
{
	r = m_red;
	g = m_green;
	b = m_blue;
}

bool CReadWriteJson::append(CGeoJson* geo)
{
	if (!geo)
	{
		return false;
	}
	m_numItem++;
	m_listEntities[geo->type()].push_back(geo);
	return true;
}

bool CReadWriteJson::surfaceUVMap(std::list<CAcGeUVGrid*>& lstGeUVGrip, const double& width, const double& height)
{
	if (lstGeUVGrip.size() == 0) 
		return false;

	std::map<unsigned long, std::list<CGeoJson*>>::iterator it = m_listEntities.find(vizDatabase::C3dFaceJson::desc());
	if (it == m_listEntities.end())
		return false;
	
	if (it->second.empty())
		return false;

	std::list<CGeoJson*> listGeo = it->second;
	std::list<CGeoJson*>::iterator geo = listGeo.begin();

	double dYmax = -1e10,
		dXmin = 1e10;
	for (; geo != listGeo.end(); geo++)
	{
		POINT3D* p0 = NULL, * p1 = NULL, * p2 = NULL, * p3 = NULL;
		(*geo)->getVertex(p0, p1, p2, p3);
		dXmin = min(dXmin, p0->x);
		dXmin = min(dXmin, p1->x);
		dXmin = min(dXmin, p2->x);

		dYmax = max(dYmax, p0->y);
		dYmax = max(dYmax, p1->y);
		dYmax = max(dYmax, p2->y);
	}

	AcGeVector2d vecTran = AcGePoint2d::kOrigin - AcGePoint2d(dXmin, dYmax);
	geoTran(listGeo, vecTran);
	std::list<CAcGeUVGrid*> lstGeUVGripTran;
	mapQuadrangleTran(lstGeUVGrip, lstGeUVGripTran, vecTran);

	std::list<AcGePoint3d> lstPoint3d;
	geo = listGeo.begin();
	for (; geo != listGeo.end(); geo++)
	{
		POINT3D* p1 = NULL, * p2 = NULL, * p3 = NULL, * p4 = NULL;
		(*geo)->getGeoInfo(p1, p2, p3, p4);
		if (p1 && p2 && p3)
		{
			lstPoint3d.push_back(AcGePoint3d(p1->x, p1->y, p1->z));
			lstPoint3d.push_back(AcGePoint3d(p2->x, p2->y, p2->z));
			lstPoint3d.push_back(AcGePoint3d(p3->x, p3->y, p3->z));
		}
	}

	geo = listGeo.begin();
	std::vector<unsigned int> listIndices;
	int index = 0;
	std::vector<float> listTexcoords;
	for (std::list<AcGePoint3d>::const_iterator it = lstPoint3d.begin(); it != lstPoint3d.end(); it++) {
		AcGePoint3d point = *it;
		listTexcoords.push_back(point.x);
		listTexcoords.push_back(point.y);
		listIndices.push_back(index);
		index++;
	}

	std::vector<float> listPosition;
	for (std::list<AcGePoint3d>::const_iterator it = lstPoint3d.begin(); it != lstPoint3d.end(); it++) {
		AcGePoint3d point = *it;
		listPosition.push_back(point.x);
		listPosition.push_back(point.y);
		listPosition.push_back(point.z);
	}

	// Create empty atlas.
	xatlas::Atlas* atlas = xatlas::Create();
	uint32_t totalVertices = 0, totalFaces = 0;
	xatlas::MeshDecl meshDecl;
	meshDecl.vertexCount = (uint32_t)listPosition.size() / 3;
	meshDecl.vertexPositionData = listPosition.data();
	meshDecl.vertexPositionStride = sizeof(float) * 3;

	meshDecl.vertexUvData = listTexcoords.data();
	meshDecl.vertexUvStride = sizeof(float) * 2;

	meshDecl.indexCount = (uint32_t)listIndices.size();
	meshDecl.indexData = listIndices.data();
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

	xatlas::AddMeshError error = xatlas::AddMesh(atlas, meshDecl, 1);
	if (error != xatlas::AddMeshError::Success) {
		xatlas::Destroy(atlas);
		return false;
	}
	totalVertices += meshDecl.vertexCount;
	if (meshDecl.faceCount > 0) {
		totalFaces += meshDecl.faceCount;
	}
	else {
		totalFaces += meshDecl.indexCount / 3;
	}

	// NEXT STEP
	xatlas::AddMeshJoin(atlas);
	xatlas::Generate(atlas);
	totalVertices = 0;
	for (uint32_t i = 0; i < atlas->meshCount; i++) {
		const xatlas::Mesh& mesh = atlas->meshes[i];
		totalVertices += mesh.vertexCount;
		assert(mesh.indexCount == (uint32_t)listIndices.size());
	}

	// Write meshes.
	std::vector<AcGePoint3d> inputPoint3d;
	std::vector<AcGePoint3d> inputPoint2d;
	for (uint32_t i = 0; i < atlas->meshCount; i++) {
		const xatlas::Mesh& mesh = atlas->meshes[i];
		for (uint32_t v = 0; v < mesh.vertexCount; v++) {
			const xatlas::Vertex& vertex = mesh.vertexArray[v];
			const float* pos = &listPosition[vertex.xref * 3];
			inputPoint3d.push_back(AcGePoint3d(pos[0], pos[1], pos[2]));
			inputPoint2d.push_back(AcGePoint3d(100 * vertex.uv[0] / atlas->height, 100 * vertex.uv[1] / atlas->height, 0));
		}
	}

	std::list<uint32_t> listFaceCount;
	if (atlas->width > 0 && atlas->height > 0) {
		for (uint32_t i = 0; i < atlas->meshCount; i++) {
			const xatlas::Mesh& mesh = atlas->meshes[i];
			uint32_t numbVertex = 0;
			for (uint32_t j = 0; j < mesh.chartCount; j++) {
				const xatlas::Chart* chart = &mesh.chartArray[j];
				listFaceCount.push_back(chart->faceCount);
			}
		}
	}

	if (inputPoint2d.size() != inputPoint3d.size()) {
		return false;
	}
	for (int i = 0; i < inputPoint3d.size(); i++) {
		if (i % 3 == 2) {
			getTexCoordsWithGrip(*geo, lstGeUVGripTran, width, height);
			geo++;
			if (geo == listGeo.end())
				break;
		}
	}
	geoTran(listGeo, -vecTran);
	xatlas::Destroy(atlas);

	return true;
}

bool vizDatabase::CReadWriteJson::meshUVMap(const double& width, const double& height)
{
	std::list<AcGePoint3d> lstP3d;
	std::list<AcGePoint2d> lstP2d;

	std::map<unsigned long, std::list<CGeoJson*>>::iterator it = m_listEntities.find(vizDatabase::C3dFaceJson::desc());
	if (it == m_listEntities.end())
		return false;

	if (it->second.empty())
		return false;

	std::list<CGeoJson*>::iterator its = it->second.begin();
	bool bGetCoord = true;
	COLORREF clrRef = RGB(255, 255, 255);
	unsigned long ulLayerID = 0;
	for (; its != it->second.end(); its++) {
		CGeoJson* iJs = *its;
		if (!iJs)
			continue;

		if (its == it->second.begin())
		{
			double iR = 0,
				iG = 0,
				iB = 0;
			iJs->getRGBColor(iR, iG, iB);
			clrRef = RGB(iR, iG, iB);

			ulLayerID = iJs->getLayer();
		}

		std::list<POINT3D*>::iterator itp = iJs->begin();
		for (; itp != iJs->end(); itp++) {
			AcGePoint3d p3((*itp)->x, (*itp)->y, (*itp)->z);
			lstP3d.push_back(p3);
		}

		std::list<POINT2D*>::iterator itp2d = iJs->begin2d();
		for (; itp2d != iJs->end2d(); itp2d++) {
			AcGePoint2d p2((*itp2d)->x, (*itp2d)->y);
			lstP2d.push_back(p2);
		}

		POINT2D* p0 = NULL, * p1 = NULL, * p2 = NULL, * p3 = NULL;

		bool ibGet = iJs->getTexCoords(p0, p1, p2, p3);
		if (!ibGet)
		{
			bGetCoord = false;
		}
	}
	if (bGetCoord)
	{
		meshWithCoord(it->second,
			width,
			height);
	}
	else
	{
		meshNoCoord(it->second,
			width, height,
			lstP3d,
			lstP2d);
		if (lstP3d.size() != lstP2d.size())
		{
			return false;
		}
		int iSize = lstP3d.size();
		if (iSize / 3 == it->second.size())
		{
			std::list<AcGePoint3d>::iterator itP3 = lstP3d.begin();
			std::list<AcGePoint2d>::iterator itP2 = lstP2d.begin();
			for (std::list<CGeoJson*>::iterator itJs = it->second.begin(); itJs != it->second.end(); itJs++)
			{
				CGeoJson* iJs = *itJs;
				if (!iJs)
				{
					continue;
				}

				AcGePoint3d p30, p31, p32;
				AcGePoint2d p20, p21, p22;
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p30 = *(itP3++);
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p31 = *(itP3++);
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p32 = *(itP3++);

				if (itP2 == lstP2d.end())
				{
					break;
				}
				p20 = *(itP2++);
				if (itP2 == lstP2d.end())
				{
					break;
				}
				p21 = *(itP2++);
				if (itP2 == lstP2d.end())
				{
					break;
				}
				p22 = *(itP2++);

				POINT3D pt30 = { p30.x, p30.y, p30.z },
					pt31 = { p31.x, p31.y, p31.z },
					pt32 = { p32.x, p32.y, p32.z };
				iJs->setGeoInfoAt(0, pt30);
				iJs->setGeoInfoAt(1, pt31);
				iJs->setGeoInfoAt(2, pt32);

				POINT2D pt20 = { p20.x, p20.y },
					pt21 = { p21.x, p21.y },
					pt22 = { p22.x, p22.y };
				iJs->setTexCoords(pt20);
				iJs->setTexCoords(pt21);
				iJs->setTexCoords(pt22);
			}
		}
		else
		{
			for (std::list<CGeoJson*>::iterator itJs = it->second.begin(); itJs != it->second.end(); itJs++)
			{
				CGeoJson* iJs = *itJs;
				if (iJs)
				{
					delete iJs;
				}
			}
			it->second.clear();
			std::list<AcGePoint3d>::iterator itP3 = lstP3d.begin();
			std::list<AcGePoint2d>::iterator itP2 = lstP2d.begin();
			for (; itP3 != lstP3d.end() || itP2 != lstP2d.end(); )
			{
				AcGePoint3d p30, p31, p32;
				AcGePoint2d p20, p21, p22;
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p30 = *(itP3++);
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p31 = *(itP3++);
				if (itP3 == lstP3d.end())
				{
					break;
				}
				p32 = *(itP3++);

				if (itP2 == lstP2d.end())
				{
					break;
				}
				p20 = *(itP2++);
				if (itP2 == lstP2d.end())
				{
					break;
				}
				p21 = *(itP2++);
				if (itP2 == lstP2d.end())
				{
					break;
				}
				p22 = *(itP2++);

				POINT3D pt30 = { p30.x, p30.y, p30.z },
					pt31 = { p31.x, p31.y, p31.z },
					pt32 = { p32.x, p32.y, p32.z };

				POINT2D pt20 = { p20.x, p20.y },
					pt21 = { p21.x, p21.y },
					pt22 = { p22.x, p22.y };

				C3dFaceJson* jsFace = C3dFaceJson::create();
				jsFace->setGeoInfo(pt30);
				jsFace->setGeoInfo(pt31);
				jsFace->setGeoInfo(pt32);

				jsFace->setTexCoords(pt20);
				jsFace->setTexCoords(pt21);
				jsFace->setTexCoords(pt22);

				int iR = GetRValue(clrRef),
					iG = GetGValue(clrRef),
					iB = GetBValue(clrRef);
				jsFace->setRGBColor(iR, iG, iB);
				jsFace->setLayer(ulLayerID);
				it->second.push_back(jsFace);
			}
		}
	}

	return true;
}

void CReadWriteJson::meshWithCoord(std::list<CGeoJson*>& lstGeo, const double& dWidth, const double& dHeight)
{
	double dXmin = 1e10,
		dXmax = -1e10;
	double dYmin = 1e10,
		dYmax = -1e10;

	std::list<CGeoJson*>::const_iterator its = lstGeo.begin();
	for (; its != lstGeo.end(); its++)
	{
		POINT2D* p0 = NULL, * p1 = NULL, * p2 = NULL, * p3 = NULL;
		(*its)->getTexCoords(p0, p1, p2, p3);
		dXmin = min(dXmin, p0->x);
		dXmin = min(dXmin, p1->x);
		dXmin = min(dXmin, p2->x);

		dXmax = max(dXmax, p0->x);
		dXmax = max(dXmax, p1->x);
		dXmax = max(dXmax, p2->x);

		dYmin = min(dYmin, p0->y);
		dYmin = min(dYmin, p1->y);
		dYmin = min(dYmin, p2->y);

		dYmax = max(dYmax, p0->y);
		dYmax = max(dYmax, p1->y);
		dYmax = max(dYmax, p2->y);
	}

	dXmin -= 1;
	dXmax += 1;

	dYmin -= 1;
	dYmax += 1;

	AcGePoint3d ipCen(0.5 * (dXmin + dXmax), 0.5 * (dYmin + dYmax), 0);

	int nNumRow = std::ceil((dXmax - dXmin) / dWidth),
		nNumCol = 2 * std::ceil((dYmax - dYmin) / (2 * dHeight));
	double dUVWidth = dWidth * nNumRow,
		dUVHeight = dHeight * nNumCol;

	double dhWidth = 0.5 * dUVWidth;
	double dhHeight = 0.5 * dUVHeight;
	AcGePoint3d pTopLeft = ipCen - AcGeVector3d::kXAxis * dhWidth + AcGeVector3d::kYAxis * dhHeight,
		pBotLeft = ipCen - AcGeVector3d::kXAxis * dhWidth - AcGeVector3d::kYAxis * dhHeight;

	CAcGeUVGrid geGrid;
	geGrid.setNodesSize(nNumRow, nNumCol);
	AcGePoint2d p200(pBotLeft.x, pBotLeft.y);
	geGrid.initGrid(p200, dWidth, dHeight);

	for (its = lstGeo.begin(); its != lstGeo.end(); its++)
	{
		POINT3D* p30 = NULL, * p31 = NULL, * p32 = NULL, * p33 = NULL;
		(*its)->getVertex(p30, p31, p32, p33);
		m_lstGeo.push_back(AcGePoint3d(p30->x, p30->y, p30->z));
		m_lstGeo.push_back(AcGePoint3d(p31->x, p31->y, p31->z));
		m_lstGeo.push_back(AcGePoint3d(p32->x, p32->y, p32->z));

		POINT2D* p0 = NULL, * p1 = NULL, * p2 = NULL, * p3 = NULL;
		(*its)->getTexCoords(p0, p1, p2, p3);
		if (!p0 || !p1 || !p2)
		{
			continue;
		}
		AcGePoint2d pC0(p0->x, p0->y),
			pC1(p1->x, p1->y),
			pC2(p2->x, p2->y);

		AcGePoint2dArray arrVert;
		arrVert.setPhysicalLength(3);
		arrVert.append(pC0);
		arrVert.append(pC1);
		arrVert.append(pC2);

		double dU = 0, dV = 0;

		std::map<int, AcGePoint2d> mapGridUV;
		int iRow = geGrid.getMapRow(),
			iCol = geGrid.getMapColumn();
		for (int i = 0; i < iRow; i++)
		{
			for (int j = 0; j < iCol; j++)
			{
				AcGePoint2d pN0 = geGrid.nodeAt(i, j),
					pN1 = geGrid.nodeAt(i, j + 1),
					pN2 = geGrid.nodeAt(i + 1, j + 1),
					pN3 = geGrid.nodeAt(i + 1, j);

				CAcGeQuadrangle2d geQuad;
				geQuad.setPoint(pN0, pN1, pN2, pN3);

				int iIdx = 0;
				for (AcGePoint2d* p2nter = arrVert.begin(); p2nter != arrVert.end(); p2nter++)
				{
					std::map<int, AcGePoint2d>::iterator itFind = mapGridUV.find(iIdx);
					if (itFind != mapGridUV.end())
					{
						iIdx++;
						continue;
					}
					if (!geQuad.isPointInQuadrangle2d(*p2nter))
					{
						iIdx++;
						continue;
					}
					geQuad.getUVInQuadEx(*p2nter, dU, dV);
					dU += j;
					dV += i;
					mapGridUV[iIdx++] = AcGePoint2d(dV, dU);
				}
				if (mapGridUV.size() == 3)
				{
					break;
				}
			}
			if (mapGridUV.size() == 3)
			{
				break;
			}
		}
		if (mapGridUV.size() < 3)
		{
			continue;
		}

		POINT2D pp0 = { mapGridUV[0].x, mapGridUV[0].y };
		(*its)->setTexCoordAt(0, pp0);
		m_lstTex.push_back(AcGePoint2d(dV, dU));

		POINT2D pp1 = { mapGridUV[1].x, mapGridUV[1].y };
		(*its)->setTexCoordAt(1, pp1);
		m_lstTex.push_back(AcGePoint2d(dV, dU));

		POINT2D pp2 = { mapGridUV[2].x, mapGridUV[2].y };
		(*its)->setTexCoordAt(2, pp2);
		m_lstTex.push_back(AcGePoint2d(dV, dU));
	}

	return;
}

void CReadWriteJson::meshNoCoord(std::list<CGeoJson*>& lstGeo, const double& dWidth, const double& dHeight, std::list<AcGePoint3d>& lstP3d, std::list<AcGePoint2d>& lstP2d)
{
	if (lstGeo.empty())
	{
		return;
	}
	std::list<CGeoJson*>::iterator its = lstGeo.begin();
	CGeoJson* js1st = *its;
	COLORREF clrRef = RGB(255, 255, 255);
	unsigned long ulLayerID = 0;
	if (js1st)
	{
		double iR = 0,
			iG = 0,
			iB = 0;
		(*its)->getRGBColor(iR, iG, iB);
		clrRef = RGB(iR, iG, iB);

		ulLayerID = js1st->getLayer();
	}
	double dXmax = -1000000000,
		dXmin = 1000000000,
		dYmax = -1000000000,
		dYmin = 1000000000,
		dZmax = -1000000000,
		dZmin = 1000000000;
	for (; its != lstGeo.end(); its++)
	{
		CGeoJson* iJs = *its;
		if (!iJs)
		{
			continue;
		}

		POINT3D* p0 = NULL, * p1 = NULL, * p2 = NULL, * p3 = NULL;
		iJs->getVertex(p0, p1, p2, p3);
		std::map<int, POINT3D*> mapVert;
		mapVert[0] = p0;
		mapVert[1] = p1;
		mapVert[2] = p2;
		mapVert[3] = p3;
		for (std::map<int, POINT3D*>::iterator itMap = mapVert.begin(); itMap != mapVert.end(); itMap++)
		{
			if (!itMap->second)
			{
				continue;
			}
			dXmin = std::min<double>(itMap->second->x, dXmin);
			dXmax = std::max<double>(itMap->second->x, dXmax);
			dYmin = std::min<double>(itMap->second->y, dYmin);
			dYmax = std::max<double>(itMap->second->y, dYmax);
			dZmin = std::min<double>(itMap->second->z, dZmin);
			dZmax = std::max<double>(itMap->second->z, dZmax);
		}
	}
	AcGeVector3d vTrans((dXmin + dXmax) * 0.5, (dYmin + dYmax) * 0.5, (dZmin + dZmax) * 0.5);

	std::vector<float> lstTextcoord;
	lstTextcoord.reserve(lstP2d.size() * 2);
	std::vector<float> vecPosition;
	vecPosition.reserve(lstP3d.size() * 3);
	std::list<AcGePoint2d>::const_iterator itP2 = lstP2d.begin();
	std::list<AcGePoint3d>::const_iterator itP3 = lstP3d.begin();
	std::vector<unsigned int> lstIndice;
	int iIdx = 0;
	for (; itP2 != lstP2d.end() || itP3 != lstP3d.end(); itP2++, itP3++)
	{
		if (itP2 == lstP2d.end() || itP3 == lstP3d.end())
		{
			break;
		}
		AcGePoint2d p2d = *itP2;
		AcGePoint3d p3d = *itP3;

		p3d.transformBy(-vTrans);
		vecPosition.push_back(p3d.x);
		vecPosition.push_back(p3d.y);
		vecPosition.push_back(p3d.z);

		lstTextcoord.push_back(p2d.x);
		lstTextcoord.push_back(p2d.y);

		lstIndice.push_back(iIdx++);
	}

	std::list<AcGePoint3d> inputPoint3d, inputPoint2d;
	POINT3D pnt3d;
	meshUnwrap(vecPosition, lstTextcoord, lstIndice,
		inputPoint3d, inputPoint2d);

	if (inputPoint2d.size() != inputPoint3d.size()) {
		return;
	}

	double dScale = 1;
	meshGetScale(inputPoint3d, inputPoint2d, dScale);

	AcGePoint3d pSet = AcGePoint3d(0, 0, 0);
	uint32_t currentIndex = 0;
	AcGeVector3d vMoveUV(pSet.x + 1, pSet.y + 1, pSet.z);
	AcGeMatrix3d matScale;
	matScale.setToScaling(dScale, AcGePoint3d::kOrigin);

	std::list<AcGePoint3d> lst3d;
	lst3d.insert(lst3d.end(),
		inputPoint3d.begin(),
		inputPoint3d.end());
	std::list<AcGePoint2d> lst2d;
	for (std::list<AcGePoint3d>::iterator itVec = inputPoint2d.begin();
		itVec != inputPoint2d.end(); itVec++)
	{
		AcGePoint3d iPnt = *itVec;
		iPnt.transformBy(matScale);
		lst2d.push_back(AcGePoint2d(iPnt.x, iPnt.y));
	}

	//std::list<CGeoJson*> lstGeoTmp;
	meshDataToGeoJson(lst3d, lst2d,
		dWidth, dHeight/*,
		lstGeoTmp*/);

	lstP3d.clear();
	for (itP3 = lst3d.begin(); itP3 != lst3d.end(); itP3++)
	{
		AcGePoint3d iPnt = *itP3;
		iPnt.transformBy(vTrans);
		lstP3d.push_back(iPnt);
	}
	lstP2d.clear();
	lstP2d.insert(lstP2d.end(), lst2d.begin(), lst2d.end());
}

void CReadWriteJson::meshUnwrap(const std::vector<float>& vecPosition, 
	const std::vector<float>& lstTextcoord, 
	const std::vector<unsigned int>& lstIndice,
	std::list<AcGePoint3d>& inputPoint3d,
	std::list<AcGePoint3d>& inputPoint2d)
{
	// Create empty atlas.
	xatlas::Atlas* atlas = xatlas::Create();

	uint32_t totalVertices = 0, totalFaces = 0;
	xatlas::MeshDecl meshDecl;
	meshDecl.vertexCount = (uint32_t)vecPosition.size() / 3;
	meshDecl.vertexPositionData = vecPosition.data();
	meshDecl.vertexPositionStride = sizeof(float) * 3;

	meshDecl.vertexUvData = lstTextcoord.data();
	meshDecl.vertexUvStride = sizeof(float) * 2;

	meshDecl.indexCount = (uint32_t)lstIndice.size();
	meshDecl.indexData = lstIndice.data();
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

	xatlas::AddMeshError error = xatlas::AddMesh(atlas, meshDecl, 1);
	if (error != xatlas::AddMeshError::Success) {
		xatlas::Destroy(atlas);
		return;
	}
	totalVertices += meshDecl.vertexCount;
	if (meshDecl.faceCount > 0) {
		totalFaces += meshDecl.faceCount;
	}
	else {
		totalFaces += meshDecl.indexCount / 3; // Assume triangles if MeshDecl::faceCount not specified.
	}
	xatlas::AddMeshJoin(atlas); // Not necessary. Only called here so geometry totals are printed after the AddMesh progress indicator.
	// Generate atlas.
	xatlas::Generate(atlas);
	totalVertices = 0;
	for (uint32_t i = 0; i < atlas->meshCount; i++) {
		const xatlas::Mesh& mesh = atlas->meshes[i];
		totalVertices += mesh.vertexCount;
		// Input and output index counts always match.
		assert(mesh.indexCount == (uint32_t)lstIndice.size());
	}
	// Write meshes.
	for (uint32_t i = 0; i < atlas->meshCount; i++) {
		const xatlas::Mesh& mesh = atlas->meshes[i];
		for (uint32_t v = 0; v < mesh.vertexCount; v++) {
			const xatlas::Vertex& vertex = mesh.vertexArray[v];
			const float* pos = &vecPosition[vertex.xref * 3];
			inputPoint3d.push_back(AcGePoint3d(pos[0], pos[1], pos[2]));
			inputPoint2d.push_back(AcGePoint3d(vertex.uv[0], vertex.uv[1], 0));
		}
	}
	std::list<uint32_t> listFaceCount;
	if (atlas->width > 0 && atlas->height > 0) {
		for (uint32_t i = 0; i < atlas->meshCount; i++) {
			const xatlas::Mesh& mesh = atlas->meshes[i];
			// Rasterize mesh triangles.
			uint32_t numbVertex = 0;
			for (uint32_t j = 0; j < mesh.chartCount; j++) {
				const xatlas::Chart* chart = &mesh.chartArray[j];
				listFaceCount.push_back(chart->faceCount);
			}
		}
	}
	xatlas::Destroy(atlas);
}

void CReadWriteJson::meshGetScale(const std::list<AcGePoint3d>& inputPoint3d,
	const std::list<AcGePoint3d>& inputPoint2d,
	double& dScale)
{
	dScale = 1;
	AcGeTol tol;
	tol.setEqualPoint(0.0001);
	std::list<AcGePoint3d>::const_iterator it3d = inputPoint3d.begin(),
		it2d = inputPoint2d.begin();
	for (; it3d != inputPoint3d.end() || it2d != inputPoint2d.end();)
	{
		AcGePoint3d p30 = *it3d,
			p20 = *it2d;
		it3d++; it2d++;
		if (it3d == inputPoint3d.end() || it2d == inputPoint2d.end())
		{
			break;
		}
		AcGePoint3d p31 = *it3d,
			p21 = *it2d;
		it3d++; it2d++;
		if (it3d == inputPoint3d.end() || it2d == inputPoint2d.end())
		{
			break;
		}
		AcGePoint3d p32 = *it3d,
			p22 = *it2d;
		it3d++; it2d++;

		if (!p30.isEqualTo(p31, tol) && !p20.isEqualTo(p21, tol))
		{
			dScale = p30.distanceTo(p31) / p20.distanceTo(p21);
			break;
		}
		if (!p31.isEqualTo(p32, tol) && !p21.isEqualTo(p22, tol))
		{
			dScale = p31.distanceTo(p32) / p21.distanceTo(p22);
			break;
		}
		if (!p32.isEqualTo(p30, tol) && !p22.isEqualTo(p20, tol))
		{
			dScale = p32.distanceTo(p30) / p22.distanceTo(p20);
			break;
		}
	}
}

void CReadWriteJson::meshDataToGeoJson(std::list<AcGePoint3d> lstP3d,
	std::list<AcGePoint2d> lstP2d,
	const double dWidth, const double dHeight)
{
	if (lstP2d.empty() || lstP3d.empty())
	{
		return;
	}

	std::list<double> lstX, lstY;
	std::list<AcGePoint2d>::iterator it2d = lstP2d.begin();
	double dXmin = it2d->x,
		dXmax = it2d->x;
	double dYmin = it2d->y,
		dYmax = it2d->y;
	for (; it2d != lstP2d.end(); it2d++)
	{
		dXmin = std::min<double>(dXmin, it2d->x);
		dXmax = std::max<double>(dXmax, it2d->x);
		dYmin = std::min<double>(dYmin, it2d->y);
		dYmax = std::max<double>(dYmax, it2d->y);
	}

	AcGePoint3d ipCen((dXmin + dXmax) * 0.5, (dYmin + dYmax) * 0.5, 0);

	int nNumRow = std::ceil((dXmax - dXmin) / dWidth),
		nNumCol = std::ceil((dYmax - dYmin) / (2 * dHeight)) * 2;
	double dUVWidth = double(nNumRow) * dWidth,
		dUVHeight = double(nNumCol) * dHeight;
	double dHalfCol = double(nNumCol) * 0.5;
	double dhUVWidth = dUVWidth * 0.5,
		dhUVHeight = dUVHeight * 0.5;

	AcGePoint3d pTopLeft = ipCen - AcGeVector3d::kXAxis * dhUVWidth + AcGeVector3d::kYAxis * dhUVHeight,
		pBotLeft = ipCen - AcGeVector3d::kXAxis * dhUVWidth - AcGeVector3d::kYAxis * dhUVHeight;

	std::list<AcGePoint3d>::iterator itP3 = lstP3d.begin();
	std::list<AcGePoint2d>::iterator itP2 = lstP2d.begin();
	std::list<AcGePoint3d> lst3dRet;
	std::list<AcGePoint2d> lst2dRet;
	for (; itP3 != lstP3d.end() || itP2 != lstP2d.end(); )
	{
		AcGePoint3d p30, p31, p32;
		AcGePoint2d p20, p21, p22;
		if (itP3 == lstP3d.end())
		{
			break;
		}
		p30 = *(itP3++);
		if (itP3 == lstP3d.end())
		{
			break;
		}
		p31 = *(itP3++);
		if (itP3 == lstP3d.end())
		{
			break;
		}
		p32 = *(itP3++);

		if (itP2 == lstP2d.end())
		{
			break;
		}
		p20 = *(itP2++);
		if (itP2 == lstP2d.end())
		{
			break;
		}
		p21 = *(itP2++);
		if (itP2 == lstP2d.end())
		{
			break;
		}
		p22 = *(itP2++);

		lst3dRet.push_back(p30);
		lst3dRet.push_back(p31);
		lst3dRet.push_back(p32);

		AcGePoint2dArray arrVert;
		arrVert.setPhysicalLength(3);
		arrVert.append(p20);
		arrVert.append(p21);
		arrVert.append(p22);

		std::map<int, AcGePoint2d> mapGridUV;
		int iIdx = 0;
		for (AcGePoint2d* p2nter = arrVert.begin(); p2nter != arrVert.end(); p2nter++)
		{
			double dU = (p2nter->x - pBotLeft.x) / dWidth,
				dV = (p2nter->y - pBotLeft.y) / dHeight;
			lst2dRet.push_back(AcGePoint2d(dU, dV));
		}
	}
	lstP3d.clear();
	lstP3d.insert(lstP3d.end(), lst3dRet.begin(), lst3dRet.end());
	lstP2d.clear();
	lstP2d.insert(lstP2d.end(), lst2dRet.begin(), lst2dRet.end());
}

bool CReadWriteJson::geoTran(std::list<CGeoJson*>& listGeo, const AcGeVector2d& vecTran)
{
	std::list<CGeoJson*>::iterator geo = listGeo.begin();
	for (; geo != listGeo.end(); geo++)
	{
		POINT3D* p1 = NULL, * p2 = NULL, * p3 = NULL, * p4 = NULL;
		(*geo)->getGeoInfo(p1, p2, p3, p4);
		if (p1 && p2 && p3)
		{
			AcGePoint2d pC1 = AcGePoint2d(p1->x, p1->y),
				pC2 = AcGePoint2d(p2->x, p2->y),
				pC3 = AcGePoint2d(p3->x, p3->y);

			pC1.transformBy(vecTran);
			pC2.transformBy(vecTran);
			pC3.transformBy(vecTran);

			double z1 = p1->z, z2 = p2->z, z3 = p3->z;

			(*geo)->clearGeoInfo();
			(*geo)->setGeoInfo(pC1.x, pC1.y, z1);
			(*geo)->setGeoInfo(pC2.x, pC2.y, z2);
			(*geo)->setGeoInfo(pC3.x, pC3.y, z3);
		}
	}
	return true;
}

bool CReadWriteJson::mapQuadrangleTran(std::list<CAcGeUVGrid*>& lstGeUVGrip, std::list<CAcGeUVGrid*>& lstGeUVGripTran, AcGeVector2d& vecTran)
{
	std::list<CAcGeUVGrid*>::iterator geGrip = lstGeUVGrip.begin();
	for (; geGrip != lstGeUVGrip.end(); geGrip++)
	{
		std::map<int, std::list<CAcGeQuadrangle2d*>> mapGeQuadrangle = (*geGrip)->getMapGeQuadrangle2d();
		std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = mapGeQuadrangle.begin();
		CAcGeUVGrid* geUVGripTrans = new CAcGeUVGrid;
		geUVGripTrans->setSizeOfRow((*geGrip)->getSizeOfRow());
		geUVGripTrans->setSizeOfColumn((*geGrip)->getSizeOfColumn());
		for (; itMap != mapGeQuadrangle.end(); itMap++)
		{
			std::list<CAcGeQuadrangle2d*> lstQua = itMap->second;
			for (std::list<CAcGeQuadrangle2d*>::iterator itQua = lstQua.begin(); itQua != lstQua.end(); itQua++)
			{
				AcGePoint2d p1 = (*itQua)->getFirstPoint();
				AcGePoint2d p2 = (*itQua)->getSecondPoint();
				AcGePoint2d p3 = (*itQua)->getThirdPoint();
				AcGePoint2d p4 = (*itQua)->getFourthPoint();
				p1.transformBy(vecTran);
				p2.transformBy(vecTran);
				p3.transformBy(vecTran);
				p4.transformBy(vecTran);

				CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
				geQuad->setPoint(p1, p2, p3, p4);
				geQuad->dU = (*itQua)->dU;
				geQuad->dV = (*itQua)->dV;
				geQuad->isValid = (*itQua)->isValid;
				geUVGripTrans->addGeQuadrangle2d(geQuad);
			}
		}
		lstGeUVGripTran.push_back(geUVGripTrans);
	}
	return true;
}

bool CReadWriteJson::getTexCoordsWithGrip(CGeoJson*& geo, std::list<CAcGeUVGrid*>& lstGeUVGrip, const double& width, const double& height)
{
	POINT3D* p1 = NULL, * p2 = NULL, * p3 = NULL, * p4 = NULL;
	geo->getGeoInfo(p1, p2, p3, p4);

	AcGePoint2d pC1(p1->x, p1->y),
		pC2(p2->x, p2->y),
		pC3(p3->x, p3->y);

	AcGePoint2dArray arrVert;
	arrVert.setPhysicalLength(3);
	arrVert.append(pC1);
	arrVert.append(pC2);
	arrVert.append(pC3);

	double dU = 0, dV = 0;
	std::list<CAcGeUVGrid*>::iterator geGrip = lstGeUVGrip.begin();
	for (; geGrip != lstGeUVGrip.end(); geGrip++)
	{
		double sizeColumn = (*geGrip)->getSizeOfColumn();
		int difference = 2 * std::ceil(sizeColumn);
		std::map<int, std::list<CAcGeQuadrangle2d*>> mapGeQuadrangle = (*geGrip)->getMapGeQuadrangle2d();
		if (mapGeQuadrangle.size() == 1)
		{
			getTexCoordsWithMap(geo, mapGeQuadrangle, width, height);
			return true;
		}
		int count = 0;
		std::list<AcGePoint2d> lstTexCoords;
		for (AcGePoint2d* p2nter = arrVert.begin(); p2nter != arrVert.end(); p2nter++)
		{
			bool isCheck = false;
			int progress = std::ceil(p2nter->x);
			for (int i = (progress - difference); i <= (progress + difference); i++)
			{
				std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itQua2d = mapGeQuadrangle.find(i);
				if (itQua2d != mapGeQuadrangle.end())
				{
					std::list<CAcGeQuadrangle2d*> lst2d = itQua2d->second;
					for (std::list<CAcGeQuadrangle2d*>::iterator it2d = lst2d.begin(); it2d != lst2d.end(); it2d++)
					{
						if ((*it2d)->isValid)
						{
							continue;
						}
						if (!(*it2d)->isPointInQuadrangle2d(*p2nter))
						{
							continue;
						}
						(*it2d)->getUVInQuadEx(*p2nter, dU, dV);
						dU += (*it2d)->dU;
						dU = dU * width;
						dV += (*it2d)->dV;
						dV = dV * height;
						lstTexCoords.push_back(AcGePoint2d(dU, dV));
						count++;
						isCheck = true;
						break;
					}
				}
				if (isCheck)
				{
					break;
				}
			}
			if (isCheck)
			{
				continue;
			}
		}
		if (count == 3)
		{
			geo->clearTexCoord();
			std::list<AcGePoint2d>::iterator itTex = lstTexCoords.begin();
			for (; itTex != lstTexCoords.end(); itTex++)
			{
				geo->setTexCoords((*itTex).x, (*itTex).y);
			}
			break;
		}
	}
	return true;
}

bool CReadWriteJson::getTexCoordsWithMap(CGeoJson*& geo, std::map<int, std::list<CAcGeQuadrangle2d*>>& mapGeQuadrangle, const double& width, const double& height)
{
	POINT3D* p1 = NULL, * p2 = NULL, * p3 = NULL, * p4 = NULL;
	geo->getGeoInfo(p1, p2, p3, p4);

	AcGePoint2d pC1(p1->x, p1->y),
		pC2(p2->x, p2->y),
		pC3(p3->x, p3->y);

	AcGePoint2dArray arrVert;
	arrVert.setPhysicalLength(3);
	arrVert.append(pC1);
	arrVert.append(pC2);
	arrVert.append(pC3);

	double dU = 0, dV = 0;

	int count = 0;
	std::list<AcGePoint2d> lstTexCoords;
	for (AcGePoint2d* p2nter = arrVert.begin(); p2nter != arrVert.end(); p2nter++)
	{
		bool isCheck = false;

		if (mapGeQuadrangle.size() == 1)
		{
			std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itQua2d = mapGeQuadrangle.begin();
			if (itQua2d != mapGeQuadrangle.end())
			{
				std::list<CAcGeQuadrangle2d*> lst2d = itQua2d->second;
				for (std::list<CAcGeQuadrangle2d*>::iterator it2d = lst2d.begin(); it2d != lst2d.end(); it2d++)
				{
					if ((*it2d)->isValid)
					{
						continue;
					}
					if (!(*it2d)->isPointInQuadrangle2d(*p2nter))
					{
						continue;
					}
					(*it2d)->getUVInQuadEx(*p2nter, dU, dV);
					dU += (*it2d)->dU;
					dU = dU * width;
					dV += (*it2d)->dV;
					dV = dV * height;
					lstTexCoords.push_back(AcGePoint2d(dU, dV));
					count++;
					isCheck = true;
					break;
				}
			}
		}
	}
	if (count == 3)
	{
		geo->clearTexCoord();
		std::list<AcGePoint2d>::iterator itTex = lstTexCoords.begin();
		for (; itTex != lstTexCoords.end(); itTex++)
		{
			geo->setTexCoords((*itTex).x, (*itTex).y);
		}
	}
	return true;
}
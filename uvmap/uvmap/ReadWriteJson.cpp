#include "pch.h"
#include "ReadWriteJson.h"
#include "C3dFaceJson.h"
#include "xatlas.h"

using namespace vizDatabase;

CReadWriteJson::CReadWriteJson(void)
{
	numItem = 0;
	m_currentObjectID = -1;
	m_red = 1;
	m_green = 0;
	m_blue = 0;
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
	numItem++;
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
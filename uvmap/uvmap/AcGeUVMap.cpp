#include "pch.h"
#include "AcGeUVMap.h"

CAcGeUVMap::CAcGeUVMap() : m_szName(L""), m_szGroupName(L""), m_szBridgeName(L"")
{
	m_geUVFace			= new CAcGeUVFace;
	m_geUVGrid			= new CAcGeUVGrid;
	m_ownsFace			= true;
	m_ownsGrid			= true;
}

CAcGeUVMap::~CAcGeUVMap()
{
	if (m_ownsFace)
		SAFE_DELETE(m_geUVFace);
	if (m_ownsGrid)
		SAFE_DELETE(m_geUVGrid);
}

void CAcGeUVMap::setName(const std::wstring& name)
{
	m_szName = name;
}

std::wstring CAcGeUVMap::getName() const
{
	return m_szName;
}

void  CAcGeUVMap::setGroupName(const std::wstring& name)
{
	m_szGroupName = name;
}

std::wstring  CAcGeUVMap::getGroupName() const
{
	return m_szGroupName;
}

void CAcGeUVMap::setBridgeName(const std::wstring& name)
{
	m_szBridgeName = name;
}

std::wstring CAcGeUVMap::getBridgeName() const
{
	return m_szBridgeName;
}
CAcGeUVFace* CAcGeUVMap::getGeUVFace() const
{
	return m_geUVFace;
}

bool CAcGeUVMap::setGeUVFace(const CAcGeUVFace* geUVFace)
{
	if (!geUVFace) {
		return false;
	}
	if (m_ownsFace)
		SAFE_DELETE(m_geUVFace);
	m_geUVFace = const_cast<CAcGeUVFace*>(geUVFace);
	m_ownsFace = false;
	return true;
}

CAcGeUVGrid* CAcGeUVMap::getGeUVGrid() const
{
	return m_geUVGrid;
}

bool CAcGeUVMap::setGeUVGrid( CAcGeUVGrid* geUVGrid)
{
	if (!geUVGrid) {
		return false;
	}
	if (m_ownsGrid)
		SAFE_DELETE(m_geUVGrid);
	m_geUVGrid = const_cast<CAcGeUVGrid*>(geUVGrid);
	m_ownsGrid = false;
	return true;
}

void CAcGeUVMap::copyFrom(const CAcGeUVMap* geUVMap)
{
	m_szName = geUVMap->getName();
	m_szGroupName = geUVMap->getGroupName();
	m_szBridgeName = geUVMap->getBridgeName();
	m_geUVFace->cloneFrom(geUVMap->getGeUVFace());
}

void CAcGeUVMap::getListUV(std::vector<AcGePoint3d>& listPoint3d, std::vector<AcGePoint2d>& listUVPoint)
{
	CAcGeUVFace::iterator it2d = m_geUVFace->begin2d();
	CAcGeUVFace::iterator it3d = m_geUVFace->begin3d();
	for (; it2d != m_geUVFace->end2d() && it3d != m_geUVFace->end3d(); it2d++, it3d++) {
		CAcGeTriangle* geTriangle2d = *it2d;
		CAcGeTriangle* geTriangle3d = *it3d;

		listPoint3d.push_back(geTriangle3d->getFirstPoint());
		listPoint3d.push_back(geTriangle3d->getSecondPoint());
		listPoint3d.push_back(geTriangle3d->getThirdPoint());

		double u, v;
		u = 0; v = 0;
		m_geUVGrid->getUVInGrid(AcGePoint2d(geTriangle2d->getFirstPoint().x, geTriangle2d->getFirstPoint().y), u, v);
		listUVPoint.push_back(AcGePoint2d(u, v));

		u = 0; v = 0;
		m_geUVGrid->getUVInGrid(AcGePoint2d(geTriangle2d->getSecondPoint().x, geTriangle2d->getSecondPoint().y), u, v);
		listUVPoint.push_back(AcGePoint2d(u, v));

		u = 0; v = 0;
		m_geUVGrid->getUVInGrid(AcGePoint2d(geTriangle2d->getThirdPoint().x, geTriangle2d->getThirdPoint().y), u, v);
		listUVPoint.push_back(AcGePoint2d(u, v));
	}
}
CAcGeUVMap& CAcGeUVMap::transformBy(const AcGeMatrix3d& leftSide) {
	return *this;
}
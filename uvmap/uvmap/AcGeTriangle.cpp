#include "pch.h"
#include "AcGeTriangle.h"

CAcGeTriangle::CAcGeTriangle()
{
	for (int i = 0; i < 3; i++) {
		m_vertex[i]		= AcGePoint3d(0, 0, 0);
		m_trcood[i]		= AcGePoint2d(0, 0);
		m_vertexId[i]	= 0;
	}
	m_triangleId		= 0;
}
CAcGeTriangle::CAcGeTriangle(const AcGePoint3d& first, const AcGePoint3d& second, const AcGePoint3d& third)
{
	m_vertex[0] = first;
	m_vertex[1] = second;
	m_vertex[2] = third;
	for (int i = 0; i < 3; i++) { m_trcood[i] = AcGePoint2d(0, 0); m_vertexId[i] = 0; }
	m_triangleId = 0;
}

CAcGeTriangle::CAcGeTriangle(const AcGePoint3d& base, const AcGeVector3d& direct1, const AcGeVector3d& direct2)
{
	m_vertex[0] = base;
	m_vertex[1] = m_vertex[0] + direct1;
	m_vertex[2] = m_vertex[0] + direct2;
	for (int i = 0; i < 3; i++) { m_trcood[i] = AcGePoint2d(0, 0); m_vertexId[i] = 0; }
	m_triangleId = 0;
}

CAcGeTriangle::~CAcGeTriangle()
{
}

void CAcGeTriangle::setFirstPoint(const AcGePoint3d& point)
{
	m_vertex[0] = point;
}

void CAcGeTriangle::setSecondPoint(const AcGePoint3d& point)
{
	m_vertex[1] = point;
}

void CAcGeTriangle::setThirdPoint(const AcGePoint3d& point)
{
	m_vertex[2] = point;
}

void CAcGeTriangle::setFirstTrcood(const AcGePoint2d& point)
{
	m_trcood[0] = point;
}

void CAcGeTriangle::setSecondTrcood(const AcGePoint2d& point)
{
	m_trcood[1] = point;
}

void CAcGeTriangle::setThirdTrood(const AcGePoint2d& point)
{
	m_trcood[2] = point;
}

void CAcGeTriangle::setPoint(const AcGePoint3d& first, const AcGePoint3d& second, const AcGePoint3d& third)
{
	m_vertex[0] = first;
	m_vertex[1] = second;
	m_vertex[2] = third;
}

AcGePoint3d CAcGeTriangle::getFirstPoint() const
{
	return m_vertex[0];
}

AcGePoint3d CAcGeTriangle::getSecondPoint() const
{
	return m_vertex[1];
}

AcGePoint3d CAcGeTriangle::getThirdPoint() const
{
	return m_vertex[2];
}

AcGePoint2d CAcGeTriangle::getFirstTrcood() const
{
	return m_trcood[0];
}

AcGePoint2d CAcGeTriangle::getSecondTrcood() const
{
	return m_trcood[1];
}

AcGePoint2d CAcGeTriangle::getThirdTrcood() const
{
	return m_trcood[2];
}

AcGePoint3d* CAcGeTriangle::getFirstPointPtr()
{
	return &m_vertex[0];
}

AcGePoint3d* CAcGeTriangle::getSecondPointPtr()
{
	return &m_vertex[1];
}

AcGePoint3d* CAcGeTriangle::getThirdPointPtr()
{
	return &m_vertex[2];
}

void CAcGeTriangle::copyFrom(const CAcGeTriangle* geTriangle)
{
	m_vertex[0] = geTriangle->getFirstPoint();
	m_vertex[1] = geTriangle->getSecondPoint();
	m_vertex[2] = geTriangle->getThirdPoint();
}
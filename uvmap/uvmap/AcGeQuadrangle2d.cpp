#include "pch.h"
#include "AcGeQuadrangle2d.h"
#include "geline2d.h"
#include <dbents.h>
#include <aced.h>
#include <cstring>
#include <atlstr.h>
#include "CvGePolygon2D.h"

CAcGeQuadrangle2d::CAcGeQuadrangle2d()
{
	for (int i = 0; i < QUADRANGLE_VERTEX; i++) {
		m_vertex[i] = AcGePoint2d(0, 0);
	}
	dU = 0;
	dV = 0;
	isValid = false;
}

CAcGeQuadrangle2d::CAcGeQuadrangle2d(const AcGePoint2d& first, const AcGePoint2d& second, const AcGePoint2d& third, const AcGePoint2d& fourth)
{
	m_vertex[0] = first;
	m_vertex[1] = second;
	m_vertex[2] = third;
	m_vertex[3] = fourth;
}

CAcGeQuadrangle2d::~CAcGeQuadrangle2d()
{

}

void CAcGeQuadrangle2d::setFirstPoint(const AcGePoint2d& point)
{
	m_vertex[0] = point;
}

void CAcGeQuadrangle2d::setSecondPoint(const AcGePoint2d& point)
{
	m_vertex[1] = point;
}

void CAcGeQuadrangle2d::setThirdPoint(const AcGePoint2d& point)
{
	m_vertex[2] = point;
}

void CAcGeQuadrangle2d::setFourthPoint(const AcGePoint2d& point)
{
	m_vertex[3] = point;
}

void CAcGeQuadrangle2d::setPoint(const AcGePoint2d& first, const AcGePoint2d& second, const AcGePoint2d& third, const AcGePoint2d& fourth)
{
	m_vertex[0] = first;
	m_vertex[1] = second;
	m_vertex[2] = third;
	m_vertex[3] = fourth;
}

AcGePoint2d CAcGeQuadrangle2d::getFirstPoint() const
{
	return m_vertex[0];
}

AcGePoint2d CAcGeQuadrangle2d::getSecondPoint() const
{
	return m_vertex[1];
}

AcGePoint2d CAcGeQuadrangle2d::getThirdPoint() const
{
	return m_vertex[2];
}

AcGePoint2d CAcGeQuadrangle2d::getFourthPoint() const
{
	return m_vertex[3];
}

AcGePoint2d* CAcGeQuadrangle2d::getFirstPointPtr()
{
	return &m_vertex[0];
}

AcGePoint2d* CAcGeQuadrangle2d::getSecondPointPtr()
{
	return &m_vertex[1];
}

AcGePoint2d* CAcGeQuadrangle2d::getThirdPointPtr()
{
	return &m_vertex[2];
}

AcGePoint2d* CAcGeQuadrangle2d::getFourthPointPtr()
{
	return &m_vertex[3];
}

AcGePoint2d CAcGeQuadrangle2d::getPoint(int index) const
{
	return m_vertex[index];
}

void CAcGeQuadrangle2d::copyFrom(const CAcGeQuadrangle2d* geQuadrangle2d)
{
	for (int i = 0; i < QUADRANGLE_VERTEX; i++) {
		m_vertex[i] = geQuadrangle2d->m_vertex[i];
	}
}

bool CAcGeQuadrangle2d::isPointInQuadrangle2d(const AcGePoint2d& point)
{
	return this->isPointInBoudingBox(point);
}

void CAcGeQuadrangle2d::getBoudingBox(AcGePoint2d& minPoint, AcGePoint2d& maxPoint)
{
	minPoint = m_vertex[0];
	maxPoint = m_vertex[0];
	for (int i = 1; i < QUADRANGLE_VERTEX; i++) {
		AcGePoint2d point = m_vertex[i];
		minPoint.x = (minPoint.x < point.x) ? minPoint.x : point.x;
		minPoint.y = (minPoint.y < point.y) ? minPoint.y : point.y;

		maxPoint.x = (maxPoint.x > point.x) ? maxPoint.x : point.x;
		maxPoint.y = (maxPoint.y > point.y) ? maxPoint.y : point.y;
	}
}

bool CAcGeQuadrangle2d::isPointInBoudingBox(const AcGePoint2d& point)
{
	CvGe::CvGePolygon2D pg2d;
	pg2d.addVertex(CvGe::CvGePoint2D(m_vertex[0].x, m_vertex[0].y));
	pg2d.addVertex(CvGe::CvGePoint2D(m_vertex[1].x, m_vertex[1].y));
	pg2d.addVertex(CvGe::CvGePoint2D(m_vertex[2].x, m_vertex[2].y));
	pg2d.addVertex(CvGe::CvGePoint2D(m_vertex[3].x, m_vertex[3].y));

	return CGeometryUtil::isPointInside(pg2d, CvGe::CvGePoint2D(point.x, point.y));
}
bool CAcGeQuadrangle2d::isPointInEdge(const AcGePoint2d& point)
{
	for (int i = 0; i < QUADRANGLE_VERTEX - 1; i++) {
		double distance1 = m_vertex[i].distanceTo(point);
		double distance2 = m_vertex[i + 1].distanceTo(point);
		double distance3 = m_vertex[i + 1].distanceTo(m_vertex[i]);

		if (abs(distance3 - distance2 - distance1) <= 0.0001) {
			return true;
		}
	}
	double distance1 = m_vertex[3].distanceTo(point);
	double distance2 = m_vertex[0].distanceTo(point);
	double distance3 = m_vertex[0].distanceTo(m_vertex[3]);

	if (abs(distance3 - distance2 - distance1) <= 0.0001) {
		return true;
	}
	return false;
}

bool CAcGeQuadrangle2d::getUVInQuadrangle(const AcGePoint2d& point, double& u, double& v)
{
	AcGeVector2d AB = m_vertex[1] - m_vertex[0];
	AcGeVector2d BC = m_vertex[2] - m_vertex[1];

	AcGeVector2d AP = point - m_vertex[0];
	double t = AP.dotProduct(AB) / AB.dotProduct(AB);

	AcGeVector2d BP = point - m_vertex[1];
	double k = BP.dotProduct(BC) / BC.dotProduct(BC);

	AcGePoint2d H = m_vertex[0] + t * AB;
	AcGePoint2d K = m_vertex[1] + k * BC;

	AcGeVector2d PH = H - point;
	AcGeVector2d PK = K - point;

	u = sqrt(abs(PH.x * PH.x + PH.y * PH.y));
	v = sqrt(abs(PK.x * PK.x + PK.y * PK.y));

	return true;
}

bool CAcGeQuadrangle2d::getUVInQuadEx(const AcGePoint2d& pGet, double& u, double& v)
{
	AcGePoint3d p3dGet(pGet.x, pGet.y, 0);

	AcGePoint2d p00 = m_vertex[0],
		p01 = m_vertex[1],
		p02 = m_vertex[2],
		p03 = m_vertex[3];

	int iPrec = 5;
	double dPrecX = 0,
		dPrecY = 0;
	double dUnit = 1,
		dScale = 1;
	bool bOnEdge = false;
	bool bCal = false;

	for (int i = 0; i < iPrec; i++)
	{
		dUnit /= 10;
		dScale *= 10;
		double dTmpX = dPrecX;
		std::map<int, AcGePoint2d> mapVert1, mapVert2;
		for (int ii = 0; ii <= 10; ii++)
		{
			mapVert1[ii] = p00 + (p01 - p00) * double(ii) / 10.0;
			mapVert2[ii] = p03 + (p02 - p03) * double(ii) / 10.0;
		}

		int ii = 0;
		for (; ii < 10; ii++)
		{
			AcGePoint2d ip00 = mapVert1[ii],
				ip01 = mapVert1[ii + 1],
				ip02 = mapVert2[ii + 1],
				ip03 = mapVert2[ii];

			double dOnPre = 0,
				dOnAft = 0;
			if (isPointOnEdge(ip00, ip03, pGet))
			{
				dOnPre = ip00.distanceTo(pGet);
				dPrecX = dTmpX + dUnit * ii;
				double dY = dOnPre / ip00.distanceTo(ip03);
				dPrecY = std::ceil(dY * dScale - 0.5) / dScale;
				bCal = true;
				bOnEdge = true;
				break;
			}
			else if (isPointOnEdge(ip01, ip02, pGet))
			{
				dOnAft = ip01.distanceTo(pGet);
				dPrecX = dTmpX + dUnit * (ii + 1);
				double dY = dOnAft / ip01.distanceTo(ip02);
				dPrecY = std::ceil(dY * dScale - 0.5) / dScale;
				bCal = true;
				bOnEdge = true;
				break;
			}
			else
			{
				CAcGeQuadrangle2d tmpQuad;
				tmpQuad.setPoint(ip00, ip01, ip02, ip03);
				if (tmpQuad.isPointInQuadrangle2d(pGet))
				{
					p00 = ip00;
					p01 = ip01;
					p02 = ip02;
					p03 = ip03;
					bCal = true;
					break;
				}
				else
				{
					dPrecX += dUnit;
				}
			}
		}

		if (bOnEdge)
		{
			break;
		}
	}

	if (bOnEdge)
	{
		u = dPrecX;
		v = dPrecY;
		return true;
	}

	dUnit = 1;
	dScale = 1;
	bCal = false;
	bOnEdge = false;
	for (int i = 0; i < iPrec; i++)
	{
		dUnit /= 10;
		dScale *= 10;
		double dTmpY = dPrecY;
		std::map<int, AcGePoint2d> mapVert1, mapVert2;
		for (int ii = 0; ii <= 10; ii++)
		{
			mapVert1[ii] = p00 + (p03 - p00) * double(ii) / 10.0;
			mapVert2[ii] = p01 + (p02 - p01) * double(ii) / 10.0;
		}

		int ii = 0;
		for (; ii < 10; ii++)
		{
			AcGePoint2d ip00 = mapVert1[ii],
				ip01 = mapVert2[ii],
				ip02 = mapVert2[ii + 1],
				ip03 = mapVert1[ii + 1];

			double dOnPre = 0,
				dOnAft = 0;
			if (isPointOnEdge(ip00, ip01, pGet))
			{
				dOnPre = ip00.distanceTo(pGet);
				double dY = dOnPre / ip00.distanceTo(ip01);
				dPrecY = std::ceil(dY * dScale - 0.5) / dScale;
				bCal = true;
				bOnEdge = true;
				break;
			}
			else if (isPointOnEdge(ip03, ip02, pGet))
			{
				dOnAft = ip03.distanceTo(pGet);
				double dY = dOnAft / ip03.distanceTo(ip02);
				dPrecY = std::ceil(dY * dScale - 0.5) / dScale;
				bCal = true;
				bOnEdge = true;
				break;
			}
			else
			{
				CAcGeQuadrangle2d tmpQuad;
				tmpQuad.setPoint(ip00, ip01, ip02, ip03);
				if (tmpQuad.isPointInQuadrangle2d(pGet))
				{
					p00 = ip00;
					p01 = ip01;
					p02 = ip02;
					p03 = ip03;
					bCal = true;
					break;
				}
				else
				{
					dPrecY += dUnit;
				}
			}
		}

		if (bOnEdge)
		{
			break;
		}
	}
	u = dPrecX;
	v = dPrecY;
	return true;
}

bool CAcGeQuadrangle2d::isPointOnEdge(const AcGePoint2d& edgeStart, const AcGePoint2d& edgeEnd,
	const AcGePoint2d& pCheck)
{
	double dLenEdge = edgeStart.distanceTo(edgeEnd);
	double dChkS = edgeStart.distanceTo(pCheck),
		dChkE = edgeEnd.distanceTo(pCheck);
	if (abs(dLenEdge - (dChkS + dChkE)) < 0.00001)
	{
		return true;
	}
	return false;
}

bool CAcGeQuadrangle2d::setValidQuadrangle(const AcGePoint2d& point)
{
	if (isPointInQuadrangle2d(point))
	{
		isValid = true;
	}
	return true;
}

AcGePoint2d CAcGeQuadrangle2d::getPointQuadrilateralCenter()
{
	AcGePoint2d p;
	double sumX = 0, sumY = 0;
	for (int i = 0; i < QUADRANGLE_VERTEX; i++) {
		sumX += m_vertex[i].x;
		sumY += m_vertex[i].y;
	}
	p.x = sumX / QUADRANGLE_VERTEX;
	p.y = sumY / QUADRANGLE_VERTEX;
	return p;
}
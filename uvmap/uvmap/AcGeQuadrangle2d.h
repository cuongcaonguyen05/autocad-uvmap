#pragma once
#include "pch.h"

class CAcGeQuadrangle2d
{
public:
	CAcGeQuadrangle2d();
	CAcGeQuadrangle2d(const AcGePoint2d& first, const AcGePoint2d& second, const AcGePoint2d& third, const AcGePoint2d& fourth);
	~CAcGeQuadrangle2d();
private:
	AcGePoint2d m_vertex[4];
public:
	void setFirstPoint(const AcGePoint2d& point);
	void setSecondPoint(const AcGePoint2d& point);
	void setThirdPoint(const AcGePoint2d& point);
	void setFourthPoint(const AcGePoint2d& point);
	void setPoint(const AcGePoint2d& first, const AcGePoint2d& second, const AcGePoint2d& third, const AcGePoint2d& fourth);

	AcGePoint2d getFirstPoint() const;
	AcGePoint2d getSecondPoint() const;
	AcGePoint2d getThirdPoint() const;
	AcGePoint2d getFourthPoint() const;

	AcGePoint2d* getFirstPointPtr();
	AcGePoint2d* getSecondPointPtr();
	AcGePoint2d* getThirdPointPtr();
	AcGePoint2d* getFourthPointPtr();

	AcGePoint2d getPoint(int index) const;

	void copyFrom(const CAcGeQuadrangle2d* getQuadrangle2d);
	bool isPointInQuadrangle2d(const AcGePoint2d& point);

	void getBoudingBox(AcGePoint2d& minPoint, AcGePoint2d& maxPoint);
	bool isPointInBoudingBox(const AcGePoint2d& point);
	bool isPointInEdge(const AcGePoint2d& point);
	bool getUVInQuadrangle(const AcGePoint2d& point, double& u, double& v);

	bool getUVInQuadEx(const AcGePoint2d& point, double& u, double& v);

	static bool isPointOnEdge(const AcGePoint2d& edgeStart, const AcGePoint2d& edgeEnd,
		const AcGePoint2d& pCheck);
	int dU, dV;
	bool isValid;
	bool setValidQuadrangle(const AcGePoint2d& point);
	AcGePoint2d getPointQuadrilateralCenter();
};
#pragma once
class CAcGeTriangle
{
public:
	CAcGeTriangle();
	CAcGeTriangle(const AcGePoint3d& first, const AcGePoint3d& second, const AcGePoint3d& third);
	CAcGeTriangle(const AcGePoint3d& base, const AcGeVector3d& direct1, const AcGeVector3d& direct2);
	~CAcGeTriangle();

	void setFirstPoint(const AcGePoint3d& point);
	void setSecondPoint(const AcGePoint3d& point);
	void setThirdPoint(const AcGePoint3d& point);
	void setPoint(const AcGePoint3d& first, const AcGePoint3d& second, const AcGePoint3d& third);

	void setFirstTrcood(const AcGePoint2d& point);
	void setSecondTrcood(const AcGePoint2d& point);
	void setThirdTrood(const AcGePoint2d& point);

	AcGePoint3d getFirstPoint() const;
	AcGePoint3d getSecondPoint() const;
	AcGePoint3d getThirdPoint() const;

	AcGePoint2d getFirstTrcood() const;
	AcGePoint2d getSecondTrcood() const;
	AcGePoint2d getThirdTrcood() const;

	AcGePoint3d* getFirstPointPtr();
	AcGePoint3d* getSecondPointPtr();
	AcGePoint3d* getThirdPointPtr();
	AcGePoint3d m_vertex[3];
	AcGePoint2d m_trcood[3];
	unsigned long m_vertexId[3];
	unsigned long m_triangleId;
	void copyFrom(const CAcGeTriangle* geTriangle);
};
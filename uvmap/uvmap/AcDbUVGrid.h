#pragma once
#include "pch.h"
#include "AcDbUVMap.h"
#include "AcDbQuadrangle2d.h"
#include "AcGeUVGrid.h"
#include "AcGeQuadrangle2d.h"

class CAcDbUVGrid : public AcDbEntity
{
	CAcGeUVGrid*						m_geUVGrid;
	bool								m_ownsGe; 
	AcDbCurve*							m_dbCurve;
	bool								m_fixed;
	std::list<CAcDbQuadrangle2d*>		m_listQuadrangle;
	std::list<CvGe::CvGePolygon2D>		m_listPg2d;
	bool								m_isUVMap;
	bool								m_alignment;

protected:
	static Adesk::UInt32 kCurrentVersionNumber;
	virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const;
public:
	ACRX_DECLARE_MEMBERS(CAcDbUVGrid);

	enum CurveType
	{
		None		= 0,
		Line		= 1,
		Polyline	= 2,
		Spline		= 3,
		Xline		= 4,
		Ray			= 5,
		Arc			= 6
	};

	typedef std::list<CAcDbQuadrangle2d*>::iterator iterator;
	typedef std::list<CAcDbQuadrangle2d*>::const_iterator const_iterator;
	CAcDbUVGrid();
	~CAcDbUVGrid();

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	void clear();
	void pushBack(CAcDbQuadrangle2d* dbQuadrangle);
	bool addGeQuadrangle(const CAcDbQuadrangle2d* dbQuadrangle);

	CAcGeUVGrid* getGeUVGrid();
	bool setGeUVGrid(const CAcGeUVGrid* geUVGrid);
	void updateGeFromDb();
	void updateDbFromGe();

	AcDbCurve* getDbCurvePath();
	bool setDbCurvePath(const AcGeCurve3d& geCurvePath);
	bool setMapUV(double& height, double& width, double& row, AcGePoint3d& ptPoint, AcDbCurve*& dbCurvePath, AcGePoint3d& pointRootStart, AcGePoint3d& pointRootEnd, AcGePoint3d& pSet);
	bool setMapPoint(double& height, double& width, double& row, AcGePoint3d& ptPoint, AcGePoint3d& pointRootStart, AcGePoint3d& pointRootEnd, AcGePoint3d& pSet);
	bool setMapPointByAlignment(AcDbCurve*& dbCurve);
	bool setMapPointUVByAlignment(AcDbPolyline*& dbPolyline, const double& height, const double& width, const int& row, const int& column);
	bool setMapPointUVByAlignmentEx(AcDbPolyline*& dbPolyline, const double& height, const double& width, const int& row, const int& column);
	bool setNumberSegOfU(const int& numb);
	bool setNumberSegOfV(const int& numb);
	void setIsUVMap(bool& isUVMap);
	bool getIsUVMap();

	bool getFixed();
	void setFixed(bool isFixed);
	bool getBasePoint(AcGePoint3d& basePoint);

	unsigned int getNumbVerts() const;
	bool setNumbVerts(const int& input);
	bool getUVInGrid(const AcGePoint2d& point, double& u, double& v);
	std::list<CAcDbQuadrangle2d*> getDbQuadrangle2d();
	bool extendAlignment(AcDbPolyline*& dbPolyline, AcDbPolyline*& dbPolylineExtend);
	bool setAllMapGeQuadrangle(std::list<CAcDbUVMap*>& lstDbUVMap, std::list<CAcGeUVGrid*>& lstGeUvGrip);

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
	virtual Acad::ErrorStatus subGetGeomExtents(AcDbExtents& extents) const;
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);
	virtual Acad::ErrorStatus subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const;

	virtual Acad::ErrorStatus subGetGripPoints(
		AcGePoint3dArray& gripPoints,
		AcDbIntArray& osnapModes,
		AcDbIntArray& geomIds) const;

	virtual Acad::ErrorStatus subMoveGripPointsAt(
		const AcDbIntArray& indices,
		const AcGeVector3d& offset);

	virtual void subList() const;

	virtual Acad::ErrorStatus subIntersectWith(
		const AcDbEntity* ent,
		AcDb::Intersect intType,
		AcGePoint3dArray& points,
		Adesk::GsMarker thisGsMarker = 0,
		Adesk::GsMarker otherGsMarker = 0) const;

	virtual Acad::ErrorStatus subIntersectWith(
		const AcDbEntity* ent,
		AcDb::Intersect intType,
		const AcGePlane& projPlane,
		AcGePoint3dArray& points,
		Adesk::GsMarker thisGsMarker = 0,
		Adesk::GsMarker otherGsMarker = 0) const;

	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds) const;

	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds,
		const AcGeMatrix3d& insertionMat) const;

	virtual Acad::ErrorStatus subGetStretchPoints(
		AcGePoint3dArray& stretchPoints) const;

	virtual Acad::ErrorStatus subMoveStretchPointsAt(
		const AcDbIntArray& indices,
		const AcGeVector3d& offset);

	bool bounds(AcDbExtents& bounds) const override;

	/// override from AcDbObject
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* filer);
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* filer) const;

	virtual Acad::ErrorStatus subHighlight(
		const AcDbFullSubentPath& subId = kNullSubent,
		const Adesk::Boolean highlightAll = false) const;

	virtual Acad::ErrorStatus subUnhighlight(
		const AcDbFullSubentPath& subId = kNullSubent,
		const Adesk::Boolean highlightAll = false) const;

	virtual Acad::ErrorStatus setColorIndex(Adesk::UInt16 color,
		Adesk::Boolean doSubents = true);
};
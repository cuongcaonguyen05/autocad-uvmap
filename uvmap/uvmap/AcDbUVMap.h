#pragma once
#include "pch.h"
#include "AcDbUVGrid.h"
#include "AcDbUVFace.h"
#include "AcGeUVGrid.h"
#include "AcGeUVMap.h"

class CAcDbUVMap : public AcDbEntity
{
private:
	CAcGeUVMap*					m_geUvMap;
	CAcDbUVGrid*				m_dbUvGrip;
	AcDbPolyline*				m_dbPolyline;
	bool						m_drawUVGrid;
	bool						m_bEditTri;

protected:
	static Adesk::UInt32 kCurrentVersionNumber;
	virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const;

public:
	ACRX_DECLARE_MEMBERS(CAcDbUVMap);
	CAcDbUVMap();
	~CAcDbUVMap();
	CAcGeUVGrid* getGridItem();
	CAcGeUVMap* getGeUVMap() const;
	void setDbPolyline(AcDbPolyline* dbPolyline);
	void setGeUVMap(const CAcGeUVMap* geUVMap);
	void setDbUVGrip(const CAcDbUVGrid* dbUvGrip);
	CAcDbUVGrid* getDbUVGrip();
	void setDrawUVGrid(const bool& isDraw);
	bool getDrawUVGrid();

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
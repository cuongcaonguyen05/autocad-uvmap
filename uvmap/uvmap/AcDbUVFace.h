#pragma once
#include "AcGeUVFace.h"
#include "AcDbTriangle.h"

class CAcGeUVGrid;
class CAcGeUVGrid;
class CAcDbUVFace : public AcDbEntity
{
	bool m_draw3d;
	bool m_draw2d;
	CAcGeUVFace* m_geUVFace;
	CAcGeUVGrid* m_geUVGrid;

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	ACRX_DECLARE_MEMBERS(CAcDbUVFace);

	~CAcDbUVFace();
	CAcDbUVFace();

	typedef std::list<CAcDbTriangle*>::iterator iterator;
	typedef std::list<CAcDbTriangle*>::const_iterator const_iterator;

	void setDraw3d(const bool& isDraw);
	bool getDraw3d() const;
	void setDraw2d(const bool& isDraw);
	bool getDraw2d() const;
	CAcGeUVFace* getGeUVFace() const;
	bool setGeUVFace(const CAcGeUVFace* geUVFace);
	double getMaxSize() const;
	void getBoundingBox2d(AcGePoint3d& A, AcGePoint3d& B, AcGePoint3d& C, AcGePoint3d& D) const;

	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* filer);
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* filer) const;
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
	virtual Acad::ErrorStatus subHighlight(
		const AcDbFullSubentPath& subId = kNullSubent,
		const Adesk::Boolean highlightAll = false) const;
	virtual Acad::ErrorStatus subUnhighlight(
		const AcDbFullSubentPath& subId = kNullSubent,
		const Adesk::Boolean highlightAll = false) const;
	virtual Acad::ErrorStatus setColorIndex(Adesk::UInt16 color,
		Adesk::Boolean doSubents = true);
	virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const;
};
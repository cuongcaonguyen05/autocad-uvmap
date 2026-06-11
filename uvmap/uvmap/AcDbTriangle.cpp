#include "pch.h"
#include "AcDbTriangle.h"

Adesk::UInt32 CAcDbTriangle::kCurrentVersionNumber = 1;

ACRX_DXF_DEFINE_MEMBERS(CAcDbTriangle, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kEraseAllowed |
	AcDbProxyEntity::kTransformAllowed |
	AcDbProxyEntity::kLayerChangeAllowed |
	AcDbProxyEntity::kLinetypeChangeAllowed |
	AcDbProxyEntity::kLinetypeScaleChangeAllowed |
	AcDbProxyEntity::kVisibilityChangeAllowed |
	AcDbProxyEntity::kCloningAllowed |
	AcDbProxyEntity::kLineWeightChangeAllowed |
	AcDbProxyEntity::kPlotStyleNameChangeAllowed |
	AcDbProxyEntity::kAllButCloningAllowed |
	AcDbProxyEntity::kAllAllowedBits |
	AcDbProxyEntity::kMaterialChangeAllowed |
	AcDbProxyEntity::kColorChangeAllowed |
	AcDbProxyEntity::kNoOperation,
	ACDBTRIANGLE,
	"AcDbTriangle custom from AcDbEntity");

CAcDbTriangle::CAcDbTriangle()
{
	m_geTriangle = new CAcGeTriangle;
}

CAcDbTriangle::~CAcDbTriangle()
{
	SAFE_DELETE(m_geTriangle);
}

Adesk::Boolean CAcDbTriangle::subWorldDraw(AcGiWorldDraw* mode)
{
	assertReadEnabled();
	if (!mode)
	{
		return Adesk::kFalse;
	}

	if (!mode->isDragging())
	{
		assertReadEnabled();
	}

	if (mode->regenAbort())
	{
		return Adesk::kTrue;
	}

	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.worldDraw(mode);
	return Adesk::kTrue;
}

Acad::ErrorStatus CAcDbTriangle::subGetGeomExtents(AcDbExtents& extents) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbTriangle::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	for (int i = 0; i < 3; i++)
	{
		m_geTriangle->m_vertex[i].transformBy(xform);
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbTriangle::subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbTriangle::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.getGripPoints(gripPoints, osnapModes, geomIds);
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbTriangle::subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	assertWriteEnabled();
	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.moveGripPointsAt(indices, offset);
	return Acad::eOk;
}

void CAcDbTriangle::subList() const
{
	assertReadEnabled();
	AcDbEntity::subList();
	acutPrintf(_T("\n---- CAcDbTriangle ----"));
}

Acad::ErrorStatus CAcDbTriangle::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbTriangle::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, const AcGePlane& projPlane, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbTriangle::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbTriangle::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds, const AcGeMatrix3d& insertionMat) const
{
	assertReadEnabled();
	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat);
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbTriangle::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
{
	assertReadEnabled();
	AcDbIntArray osnapModes, geomIds;
	return subGetGripPoints(stretchPoints, osnapModes, geomIds);
}

Acad::ErrorStatus CAcDbTriangle::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	return subMoveGripPointsAt(indices, offset);
}

Acad::ErrorStatus CAcDbTriangle::dwgInFields(AcDbDwgFiler* filer)
{
	assertWriteEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgInFields(filer);
	if (es != Acad::eOk)
	{
		return es;
	}

	// object version - must always be the first item.
	Adesk::UInt32 version = 0;
	if ((es = filer->readUInt32(&version)) != Acad::eOk)
		return es;

	if (version > CAcDbTriangle::kCurrentVersionNumber)
	{
		return Acad::eMakeMeProxy;
	}

	AcGePoint3d first;
	es = filer->readItem(&first);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geTriangle->setFirstPoint(first);

	AcGePoint3d second;
	es = filer->readItem(&second);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geTriangle->setSecondPoint(second);

	AcGePoint3d third;
	es = filer->readItem(&third);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geTriangle->setThirdPoint(third);

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbTriangle::dwgOutFields(AcDbDwgFiler* filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgOutFields(filer);
	if (es != Acad::eOk)
	{
		return es;
	}

	// object version - must always be the first item.
	if ((es = filer->writeUInt32(CAcDbTriangle::kCurrentVersionNumber)) != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geTriangle->getFirstPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geTriangle->getSecondPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geTriangle->getThirdPoint());
	if (es != Acad::eOk)
	{
		return es;
	}
	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbTriangle::subHighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subHighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbTriangle::subUnhighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subUnhighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbTriangle::subExplode(AcDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	AcGePoint3dArray pointArray;
	for (int i = 0; i < 3; i++)
	{
		pointArray.append(m_geTriangle->m_vertex[i]);
	}
	AcDb3dPolyline polyline(AcDb::Poly3dType::k3dSimplePoly, pointArray);
	polyline.setClosed(true);

	polyline.explode(entitySet);
	return Acad::eOk;
}
Acad::ErrorStatus CAcDbTriangle::setColorIndex(Adesk::UInt16 color, Adesk::Boolean doSubents)
{
	assertWriteEnabled();
	return Acad::eOk;
}

bool CAcDbTriangle::setGeTriangle(const CAcGeTriangle* geTriangle)
{
	if (!geTriangle)
	{
		return false;
	}
	m_geTriangle->copyFrom(geTriangle);
	return true;
}
CAcGeTriangle* CAcDbTriangle::getGeTriangle()
{
	return m_geTriangle;
}
#include "pch.h"
#include "AcDbUVMap.h"

Adesk::UInt32 CAcDbUVMap::kCurrentVersionNumber = 1;

ACRX_DXF_DEFINE_MEMBERS(CAcDbUVMap, AcDbEntity,
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
	ACDBUVMAP,
	"AcDbUVMap custom from AcDbEntity");

CAcDbUVMap::CAcDbUVMap()
{
	m_geUvMap			= new CAcGeUVMap;
	m_drawUVGrid		= true;
	m_bEditTri			= false;
	m_dbUvGrip			= new CAcDbUVGrid;
	m_dbPolyline		= new AcDbPolyline();
}

CAcDbUVMap::~CAcDbUVMap()
{
	SAFE_DELETE(m_geUvMap);
	SAFE_DELETE(m_dbUvGrip);
	SAFE_DELETE(m_dbPolyline);
}

Adesk::Boolean CAcDbUVMap::subWorldDraw(AcGiWorldDraw* mode)
{
	assertReadEnabled();
	if (!mode)
		return Adesk::kFalse;

	if (!mode->isDragging())
		assertReadEnabled();

	if (mode->regenAbort())
		return Adesk::kTrue;

	AcCmEntityColor oldColor = mode->subEntityTraits().trueColor();
	AcCmEntityColor newColor;
	newColor.setColorIndex(1);
	mode->subEntityTraits().setTrueColor(newColor);

	CAcDbUVFace dbUVFace;
	dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace());
	CAcDbUVGrid dbUVGrid;
	dbUVGrid.setGeUVGrid(m_geUvMap->getGeUVGrid());
	dbUVFace.subWorldDraw(mode);
	if (m_drawUVGrid)
	{
		newColor.setColorIndex(7);
		mode->subEntityTraits().setTrueColor(newColor);
		dbUVGrid.subWorldDraw(mode);
	}

	mode->subEntityTraits().setTrueColor(oldColor);

	return Adesk::kTrue;
}

CAcGeUVGrid* CAcDbUVMap::getGridItem()
{
	return m_geUvMap->getGeUVGrid();
}

Acad::ErrorStatus CAcDbUVMap::subGetGeomExtents(AcDbExtents& extents) const
{
	assertReadEnabled();
	if (!m_geUvMap)
		return Acad::eNotApplicable;

	CAcGeUVGrid* geGrid = m_geUvMap->getGeUVGrid();
	if (!geGrid)
		return Acad::eNotApplicable;

	CAcDbUVGrid dbGrid;
	dbGrid.setGeUVGrid(geGrid);
	return dbGrid.bounds(extents) ? Acad::eOk : Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVMap::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	CAcGeUVGrid* geUVGrid = NULL;
	geUVGrid = m_geUvMap->getGeUVGrid();
	if (!geUVGrid)
		return Acad::eNullPtr;

	if (geUVGrid->getFixed())
		return Acad::eNullPtr;

	CAcDbUVGrid dbUVGrid;
	if (!dbUVGrid.setGeUVGrid(geUVGrid))
		return Acad::eNullPtr;

	dbUVGrid.transformBy(xform);
	if (m_dbPolyline)
		m_dbPolyline->transformBy(xform);

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVMap::subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVMap::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
	CAcGeUVGrid* geUVGrid = NULL;
	geUVGrid = m_geUvMap->getGeUVGrid();
	if (!geUVGrid)
		return Acad::eNullPtr;

	if (geUVGrid->getFixed())
		return Acad::eNullPtr;

	CAcDbUVFace dbUVFace;
	CAcDbUVGrid dbUVGrid;
	if (!dbUVGrid.setGeUVGrid(geUVGrid) || !dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace()))
		return Acad::eNullPtr;

	assertReadEnabled();

	if (!m_bEditTri)
	{
		if (m_drawUVGrid)
		{
			dbUVGrid.subGetGripPoints(gripPoints, osnapModes, geomIds);
			dbUVGrid.getGeUVGrid()->editNodePosition();
		}
	}

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVMap::subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	CAcGeUVGrid* geUVGrid = NULL;
	geUVGrid = m_geUvMap->getGeUVGrid();
	if (!geUVGrid)
		return Acad::eNullPtr;

	if (geUVGrid->getFixed())
		return Acad::eNullPtr;

	CAcDbUVFace dbUVFace;
	CAcDbUVGrid dbUVGrid;
	if (!dbUVGrid.setGeUVGrid(geUVGrid) || !dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace()))
		return Acad::eNullPtr;

	assertWriteEnabled();

	if (!m_bEditTri)
	{
		if (m_drawUVGrid)
		{
			dbUVGrid.subMoveGripPointsAt(indices, offset);
		}
	}
	return Acad::eOk;
}

void CAcDbUVMap::subList() const
{
	assertReadEnabled();
	AcDbEntity::subList();
	acutPrintf(_T("\n---- CAcDbUVMap ----"));
}

Acad::ErrorStatus CAcDbUVMap::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVMap::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, const AcGePlane& projPlane, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVMap::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds) const
{
	if (!m_geUvMap)
		return Acad::eNullPtr;

	assertReadEnabled();
	CAcDbUVFace dbUVFace;
	CAcDbUVGrid dbUVGrid;
	if (!dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace()) || !dbUVGrid.setGeUVGrid(m_geUvMap->getGeUVGrid()))
		return Acad::eNullPtr;

	dbUVFace.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
	if (m_drawUVGrid)
		dbUVGrid.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVMap::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds, const AcGeMatrix3d& insertionMat) const
{
	if (!m_geUvMap)
		return Acad::eNullPtr;

	assertReadEnabled();
	CAcDbUVFace dbUVFace;
	CAcDbUVGrid dbUVGrid;
	if (!dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace()) || !dbUVGrid.setGeUVGrid(m_geUvMap->getGeUVGrid()))
		return Acad::eNullPtr;

	dbUVFace.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat);

	if (m_drawUVGrid)
		dbUVGrid.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat);

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVMap::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
{
	assertReadEnabled();
	AcDbIntArray osnapModes, geomIds;
	return subGetGripPoints(stretchPoints, osnapModes, geomIds);
}

Acad::ErrorStatus CAcDbUVMap::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	return subMoveGripPointsAt(indices, offset);
}

bool CAcDbUVMap::bounds(AcDbExtents& bounds) const
{
	return true;
}

Acad::ErrorStatus CAcDbUVMap::dwgInFields(AcDbDwgFiler* filer)
{
	assertWriteEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgInFields(filer);
	if (es != Acad::eOk)
		return es;

	// object version - must always be the first item.
	Adesk::UInt32 version = 0;
	if ((es = filer->readUInt32(&version)) != Acad::eOk)
		return es;

	if (version > CAcDbUVMap::kCurrentVersionNumber)
		return Acad::eMakeMeProxy;

	CAcDbUVGrid dbUVGrid;
	CAcGeUVGrid* geUVGrid = m_geUvMap->getGeUVGrid();
	geUVGrid->clear();
	geUVGrid = m_geUvMap->getGeUVGrid();
	dbUVGrid.setGeUVGrid(geUVGrid);

	es = dbUVGrid.dwgInFields(filer);
	if (es != Acad::eOk)
		return es;

	CAcDbUVFace dbUVFace;
	CAcGeUVFace* geUVFace = m_geUvMap->getGeUVFace();
	geUVFace->clear();
	geUVFace = m_geUvMap->getGeUVFace();
	dbUVFace.setGeUVFace(geUVFace);

	es = dbUVFace.dwgInFields(filer);
	if (es != Acad::eOk)
		return es;

	if (m_dbPolyline)
		delete m_dbPolyline;

	m_dbPolyline = new AcDbPolyline();
	es = m_dbPolyline->dwgInFields(filer);

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbUVMap::dwgOutFields(AcDbDwgFiler* filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgOutFields(filer);
	if (es != Acad::eOk)
		return es;

	// object version - must always be the first item.
	if ((es = filer->writeUInt32(CAcDbUVMap::kCurrentVersionNumber)) != Acad::eOk)
		return es;

	CAcDbUVGrid dbUVGrid;
	dbUVGrid.setGeUVGrid(m_geUvMap->getGeUVGrid());
	es = dbUVGrid.dwgOutFields(filer);
	if (es != Acad::eOk)
		return es;

	CAcDbUVFace dbUVFace;
	dbUVFace.setGeUVFace(m_geUvMap->getGeUVFace());
	es = dbUVFace.dwgOutFields(filer);
	if (es != Acad::eOk)
		return es;

	if (m_dbPolyline)
	{
		es = m_dbPolyline->dwgOutFields(filer);
	}
	else
	{
		AcDbPolyline emptyPline;
		es = emptyPline.dwgOutFields(filer);
	}
	if (es != Acad::eOk)
		return es;

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbUVMap::subHighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subHighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVMap::subUnhighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subUnhighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVMap::subExplode(AcDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	return Acad::eOk;
}
Acad::ErrorStatus CAcDbUVMap::setColorIndex(Adesk::UInt16 color, Adesk::Boolean doSubents)
{
	assertWriteEnabled();
	return Acad::eOk;
}
CAcGeUVMap* CAcDbUVMap::getGeUVMap() const
{
	return m_geUvMap;
}

void CAcDbUVMap::setDbPolyline(AcDbPolyline* dbPolyline)
{
	if (m_dbPolyline == dbPolyline)
		return;

	SAFE_DELETE(m_dbPolyline);
	if (!dbPolyline)
	{
		m_dbPolyline = NULL;
		return;
	}
	m_dbPolyline = AcDbPolyline::cast(dbPolyline->clone());
}

void CAcDbUVMap::setGeUVMap(const CAcGeUVMap* geUVMap)
{
	if (!geUVMap)
		return;

	m_geUvMap->copyFrom(geUVMap);
}

void CAcDbUVMap::setDrawUVGrid(const bool& isDraw)
{
	m_drawUVGrid = isDraw;
}

bool CAcDbUVMap::getDrawUVGrid()
{
	return m_drawUVGrid;
}

void CAcDbUVMap::setDbUVGrip(const CAcDbUVGrid* dbUvGrip)
{
	if (m_dbUvGrip == dbUvGrip)
		return;
	SAFE_DELETE(m_dbUvGrip);
	m_dbUvGrip = const_cast<CAcDbUVGrid*>(dbUvGrip);
}

CAcDbUVGrid* CAcDbUVMap::getDbUVGrip()
{
	return m_dbUvGrip;
}
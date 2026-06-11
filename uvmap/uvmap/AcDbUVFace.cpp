#include "pch.h"
#include "AcDbUVFace.h"
#include "AcGeQuadrangle2d.h"

Adesk::UInt32 CAcDbUVFace::kCurrentVersionNumber = 1;

ACRX_DXF_DEFINE_MEMBERS(CAcDbUVFace, AcDbEntity,
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
	ACDBUVFACE,
	"AcDbUVFace custom from AcDbEntity");

CAcDbUVFace::CAcDbUVFace()
{
	m_draw3d = false;
	m_draw2d = true;
	m_geUVFace = new CAcGeUVFace;
}

CAcDbUVFace::~CAcDbUVFace()
{
}

Adesk::Boolean CAcDbUVFace::subWorldDraw(AcGiWorldDraw* mode)
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

	if (m_draw2d)
	{
		std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
		for (; it2d != m_geUVFace->end2d(); it2d++)
		{
			CAcDbTriangle dbTri2d;
			dbTri2d.m_geTriangle->setFirstPoint((*it2d)->getFirstPoint());
			dbTri2d.m_geTriangle->setSecondPoint((*it2d)->getSecondPoint());
			dbTri2d.m_geTriangle->setThirdPoint((*it2d)->getThirdPoint());
			dbTri2d.subWorldDraw(mode);
		}
	}

	if (m_draw3d)
	{
		std::list<CAcGeTriangle*>::iterator it3d = m_geUVFace->begin3d();
		for (; it3d != m_geUVFace->end3d(); it3d++)
		{
			CAcDbTriangle dbTri3d;
			dbTri3d.m_geTriangle->setFirstPoint((*it3d)->getFirstPoint());
			dbTri3d.m_geTriangle->setSecondPoint((*it3d)->getSecondPoint());
			dbTri3d.m_geTriangle->setThirdPoint((*it3d)->getThirdPoint());
			dbTri3d.subWorldDraw(mode);
		}
	}

	return Adesk::kTrue;
}

Acad::ErrorStatus CAcDbUVFace::subGetGeomExtents(AcDbExtents& extents) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVFace::subTransformBy(const AcGeMatrix3d& xform)
{
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVFace::subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVFace::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	if (m_draw2d)
	{
		std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
		for (; it2d != m_geUVFace->end2d(); it2d++)
		{
			CAcDbTriangle dbTri2d;
			gripPoints.append((*it2d)->getFirstPoint());
			gripPoints.append((*it2d)->getSecondPoint());
			gripPoints.append((*it2d)->getThirdPoint());
		}
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVFace::subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	assertWriteEnabled();
	if (m_draw2d)
	{
		std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
		std::map<int, CAcGeTriangle*> mapIndices;
		int iIdx = 0;
		for (; it2d != m_geUVFace->end2d(); it2d++)
		{
			mapIndices[iIdx++] = *it2d;
			mapIndices[iIdx++] = *it2d;
			mapIndices[iIdx++] = *it2d;
		}

		int nGripPnt = indices.length();
		for (int i = 0; i < nGripPnt; i++)
		{
			std::map<int, CAcGeTriangle*>::iterator itFind = mapIndices.find(indices.at(i));
			if (itFind == mapIndices.end())
			{
				continue;
			}

			CAcGeTriangle* iDbTri = itFind->second;
			if (!iDbTri)
			{
				continue;
			}

			int nVert = itFind->first % 3;
			switch (nVert)
			{
			case 0:
			{
				AcGePoint3d iPnt = iDbTri->getFirstPoint() + AcGeVector3d(offset.x, offset.y, 0);
				iDbTri->setFirstPoint(iPnt);
			}
			break;
			case 1:
			{
				AcGePoint3d iPnt = iDbTri->getSecondPoint() + AcGeVector3d(offset.x, offset.y, 0);
				iDbTri->setSecondPoint(iPnt);
			}
			break;
			case 2:
			{
				AcGePoint3d iPnt = iDbTri->getThirdPoint() + AcGeVector3d(offset.x, offset.y, 0);
				iDbTri->setThirdPoint(iPnt);
			}
			break;
			default:
				break;
			}
		}
	}
	return Acad::eOk;
}

void CAcDbUVFace::subList() const
{
	assertReadEnabled();
	AcDbEntity::subList();
	acutPrintf(_T("\n---- CAcDbUVFace ----"));
}

Acad::ErrorStatus CAcDbUVFace::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVFace::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, const AcGePlane& projPlane, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVFace::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	if (m_draw2d)
	{
		std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
		for (; it2d != m_geUVFace->end2d(); it2d++)
		{
			CAcDbTriangle dbTri2d;
			dbTri2d.m_geTriangle->setFirstPoint((*it2d)->getFirstPoint());
			dbTri2d.m_geTriangle->setSecondPoint((*it2d)->getSecondPoint());
			dbTri2d.m_geTriangle->setThirdPoint((*it2d)->getThirdPoint());
			dbTri2d.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
		}
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVFace::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds, const AcGeMatrix3d& insertionMat) const
{
	assertReadEnabled();
	if (m_draw2d)
	{
		std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
		for (; it2d != m_geUVFace->end2d(); it2d++)
		{
			CAcDbTriangle dbTri2d;
			dbTri2d.m_geTriangle->setFirstPoint((*it2d)->getFirstPoint());
			dbTri2d.m_geTriangle->setSecondPoint((*it2d)->getSecondPoint());
			dbTri2d.m_geTriangle->setThirdPoint((*it2d)->getThirdPoint());
			dbTri2d.subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat);
		}
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVFace::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
{
	assertReadEnabled();
	AcDbIntArray osnapModes, geomIds;
	return subGetGripPoints(stretchPoints, osnapModes, geomIds);
}

Acad::ErrorStatus CAcDbUVFace::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	return subMoveGripPointsAt(indices, offset);
}

Acad::ErrorStatus CAcDbUVFace::dwgInFields(AcDbDwgFiler* filer)
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

	if (version > CAcDbUVFace::kCurrentVersionNumber)
	{
		return Acad::eMakeMeProxy;
	}

	m_geUVFace->clear2d();
	m_geUVFace->clear3d();

	AcDbPolyline* polyline = new AcDbPolyline;

	int numbOfTriangle2d = 0;
	es = filer->readItem(&numbOfTriangle2d);
	if (es != Acad::eOk)
	{
		return es;
	}

	for (int i = 0; i < numbOfTriangle2d; i++)
	{
		CAcDbTriangle* dbTriangle = new CAcDbTriangle;
		es = dbTriangle->dwgInFields(filer);
		if (es != Acad::eOk)
		{
			return es;
		}
		polyline->addVertexAt(i, dbTriangle->m_geTriangle->m_vertex[0].convert2d(AcGePlane()));
		m_geUVFace->addGeTriangle2d(dbTriangle->getGeTriangle());
	}

	int numbOfTriangle3d = 0;
	es = filer->readItem(&numbOfTriangle3d);
	if (es != Acad::eOk)
	{
		return es;
	}

	for (int i = 0; i < numbOfTriangle3d; i++)
	{
		CAcDbTriangle* dbTriangle = new CAcDbTriangle;
		es = dbTriangle->dwgInFields(filer);
		if (es != Acad::eOk)
		{
			return es;
		}
		m_geUVFace->addGeTriangle3d(dbTriangle->getGeTriangle());
	}

	es = filer->readItem(&m_draw3d);
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->readItem(&m_draw2d);
	if (es != Acad::eOk)
	{
		return es;
	}

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbUVFace::dwgOutFields(AcDbDwgFiler* filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgOutFields(filer);
	if (es != Acad::eOk)
	{
		return es;
	}

	// object version - must always be the first item.
	if ((es = filer->writeUInt32(CAcDbUVFace::kCurrentVersionNumber)) != Acad::eOk)
	{
		return es;
	}

	int numbOfTriangle2d = m_geUVFace->size2d();
	es = filer->writeItem(numbOfTriangle2d);
	if (es != Acad::eOk)
	{
		return es;
	}

	std::list<CAcGeTriangle*>::iterator it2d = m_geUVFace->begin2d();
	for (; it2d != m_geUVFace->end2d(); it2d++)
	{
		CAcDbTriangle dbTri2d;
		dbTri2d.m_geTriangle->setFirstPoint((*it2d)->getFirstPoint());
		dbTri2d.m_geTriangle->setSecondPoint((*it2d)->getSecondPoint());
		dbTri2d.m_geTriangle->setThirdPoint((*it2d)->getThirdPoint());
		es = dbTri2d.dwgOutFields(filer);
		if (es != Acad::eOk)
		{
			return es;
		}
	}

	int numbOfTriangle3d = m_geUVFace->size3d();
	es = filer->writeItem(numbOfTriangle3d);
	if (es != Acad::eOk)
	{
		return es;
	}

	std::list<CAcGeTriangle*>::iterator it3d = m_geUVFace->begin3d();
	for (; it3d != m_geUVFace->end3d(); it3d++)
	{
		CAcDbTriangle dbTri3d;
		dbTri3d.m_geTriangle->setFirstPoint((*it3d)->getFirstPoint());
		dbTri3d.m_geTriangle->setSecondPoint((*it3d)->getSecondPoint());
		dbTri3d.m_geTriangle->setThirdPoint((*it3d)->getThirdPoint());
		es = dbTri3d.dwgOutFields(filer);
		if (es != Acad::eOk)
		{
			return es;
		}
	}

	es = filer->writeItem(m_draw3d);
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_draw2d);
	if (es != Acad::eOk)
	{
		return es;
	}

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbUVFace::subHighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subHighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVFace::subUnhighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subUnhighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVFace::subExplode(AcDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVFace::setColorIndex(Adesk::UInt16 color, Adesk::Boolean doSubents)
{
	assertWriteEnabled();
	return Acad::eOk;
}

void CAcDbUVFace::setDraw3d(const bool& isDraw)
{
	m_draw3d = isDraw;
	if (!isDraw)
	{
		m_draw2d = true;
	}
}

bool CAcDbUVFace::getDraw3d() const
{
	return m_draw3d;
}

void CAcDbUVFace::setDraw2d(const bool& isDraw)
{
	m_draw2d = isDraw;
	if (!isDraw)
	{
		m_draw3d = true;
	}
}

bool CAcDbUVFace::getDraw2d() const
{
	return m_draw2d;
}

CAcGeUVFace* CAcDbUVFace::getGeUVFace() const
{
	return m_geUVFace;
}

bool CAcDbUVFace::setGeUVFace(const CAcGeUVFace* geUVFace)
{
	if (!geUVFace)
	{
		return false;
	}
	m_geUVFace = const_cast<CAcGeUVFace*>(geUVFace);
	return true;
}

void CAcDbUVFace::getBoundingBox2d(AcGePoint3d& A, AcGePoint3d& B, AcGePoint3d& C, AcGePoint3d& D) const
{
	m_geUVFace->getBoundingBox2d(A, B, C, D);
}

double CAcDbUVFace::getMaxSize() const
{
	return m_geUVFace->getMaxSize();
}
#include "pch.h"
#include "AcDbUVGrid.h"

Adesk::UInt32 CAcDbUVGrid::kCurrentVersionNumber = 1;

ACRX_DXF_DEFINE_MEMBERS(CAcDbUVGrid, AcDbEntity,
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
	ACDBUVGRID,
	"AcDbUVGrid custom from AcDbEntity");

CAcDbUVGrid::CAcDbUVGrid()
{
	m_geUVGrid				= new CAcGeUVGrid;
	m_ownsGe				= true;
	m_dbCurve				= NULL;
	m_fixed					= false;
	m_alignment				= false;
	m_isUVMap				= false;
}

CAcDbUVGrid::~CAcDbUVGrid()
{
	if (m_ownsGe)
		SAFE_DELETE(m_geUVGrid);
}

CAcDbUVGrid::iterator CAcDbUVGrid::begin()
{
	return m_listQuadrangle.begin();
}

CAcDbUVGrid::iterator CAcDbUVGrid::end()
{
	return m_listQuadrangle.end();
}

CAcDbUVGrid::const_iterator CAcDbUVGrid::begin() const
{
	return m_listQuadrangle.begin();
}

CAcDbUVGrid::const_iterator CAcDbUVGrid::end() const
{
	return m_listQuadrangle.end();
}

void CAcDbUVGrid::clear()
{
	deleteListPointer(m_listQuadrangle);
}

void CAcDbUVGrid::pushBack(CAcDbQuadrangle2d* dbQuadrangle)
{
	m_listQuadrangle.push_back(dbQuadrangle);
}

bool CAcDbUVGrid::addGeQuadrangle(const CAcDbQuadrangle2d* dbQuadrangle)
{
	return true;
}

Adesk::Boolean CAcDbUVGrid::subWorldDraw(AcGiWorldDraw* mode)
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

	int iRow = m_geUVGrid->m_iRow,
		iCol = m_geUVGrid->m_iCol;

	if (!m_geUVGrid->m_arrNode)
	{
		return Adesk::kTrue;
	}

	AcCmEntityColor oldColor = mode->subEntityTraits().trueColor();
	AcCmEntityColor newColor;
	newColor.setColorIndex(5);
	mode->subEntityTraits().setTrueColor(newColor);

	for (int i = 0; i <= iRow; i++)
	{
		for (int j = 0; j <= iCol; j++)
		{
			AcGePoint2d iPnt0 = m_geUVGrid->m_arrNode[i][j];
			if (i < iRow)
			{
				AcGePoint2d iPntVer = m_geUVGrid->m_arrNode[i + 1][j];
				AcDbLine lnVer(AcGePoint3d(iPnt0.x, iPnt0.y, 0), AcGePoint3d(iPntVer.x, iPntVer.y, 0));
				lnVer.worldDraw(mode);
			}
			if (j < iCol)
			{
				AcGePoint2d iPntHor = m_geUVGrid->m_arrNode[i][j + 1];
				AcDbLine lnHor(AcGePoint3d(iPnt0.x, iPnt0.y, 0), AcGePoint3d(iPntHor.x, iPntHor.y, 0));
				lnHor.worldDraw(mode);
			}
		}
	}

	if (m_geUVGrid->m_mapGeQuadrangle.size() > 0)
	{
		std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = m_geUVGrid->m_mapGeQuadrangle.begin();
		for (; itMap != m_geUVGrid->m_mapGeQuadrangle.end(); itMap++)
		{
			std::list<CAcGeQuadrangle2d*>::iterator ge2d = itMap->second.begin();
			for (; ge2d != itMap->second.end(); ge2d++)
			{
				CAcDbQuadrangle2d dbQuadrangle;
				dbQuadrangle.setGeQuadrangle2d(*ge2d);
				dbQuadrangle.subWorldDraw(mode);
			}
		}
	}

	mode->subEntityTraits().setTrueColor(oldColor);

	return Adesk::kTrue;
}

Acad::ErrorStatus CAcDbUVGrid::subGetGeomExtents(AcDbExtents& extents) const
{
	assertReadEnabled();
	return bounds(extents) ? Acad::eOk : Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVGrid::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	if (m_dbCurve)
	{
		m_dbCurve->transformBy(xform);
	}
	if (m_geUVGrid->m_arrNode)
	{
		int iRow = m_geUVGrid->getMapRow(),
			iCol = m_geUVGrid->getMapColumn();
		for (int i = 0; i <= iRow; i++)
		{
			for (int j = 0; j <= iCol; j++)
			{
				AcGePoint2d iPnt = m_geUVGrid->m_arrNode[i][j];
				AcGePoint3d iP3d(iPnt.x, iPnt.y, 0);
				iP3d.transformBy(xform);
				m_geUVGrid->m_arrNode[i][j] = AcGePoint2d(iP3d.x, iP3d.y);
			}
		}
	}

	std::map<int, std::list<CAcGeQuadrangle2d*>> mapGeQuadrangle = m_geUVGrid->m_mapGeQuadrangle;
	std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMapQua = mapGeQuadrangle.begin();
	for (; itMapQua != mapGeQuadrangle.end(); itMapQua++)
	{
		std::list<CAcGeQuadrangle2d*> lstQua = itMapQua->second;
		std::list<CAcGeQuadrangle2d*>::iterator itQua = lstQua.begin();
		for (; itQua != lstQua.end(); itQua++)
		{
			AcGePoint2d p1 = (*itQua)->getFirstPoint();
			AcGePoint2d p2 = (*itQua)->getSecondPoint();
			AcGePoint2d p3 = (*itQua)->getThirdPoint();
			AcGePoint2d p4 = (*itQua)->getFourthPoint();

			AcGePoint3d p3d1(p1.x, p1.y, 0);
			AcGePoint3d p3d2(p2.x, p2.y, 0);
			AcGePoint3d p3d3(p3.x, p3.y, 0);
			AcGePoint3d p3d4(p4.x, p4.y, 0);
			p3d1.transformBy(xform);
			p3d2.transformBy(xform);
			p3d3.transformBy(xform);
			p3d4.transformBy(xform);
			(*itQua)->setFirstPoint(AcGePoint2d(p3d1.x, p3d1.y));
			(*itQua)->setSecondPoint(AcGePoint2d(p3d2.x, p3d2.y));
			(*itQua)->setThirdPoint(AcGePoint2d(p3d3.x, p3d3.y));
			(*itQua)->setFourthPoint(AcGePoint2d(p3d4.x, p3d4.y));
		}
	}

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVGrid::subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVGrid::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	if (m_fixed)
	{
		if (m_dbCurve)
		{
			m_dbCurve->getGripPoints(gripPoints, osnapModes, geomIds);
		}
	}
	else
	{
		if (m_geUVGrid->m_arrNode)
		{
			int iCol = m_geUVGrid->m_iCol,
				iRow = m_geUVGrid->m_iRow;
			for (int i = 0; i <= iRow; i++)
			{
				for (int j = 0; j <= iCol; j++)
				{
					AcGePoint2d iPnt = m_geUVGrid->m_arrNode[i][j];
					gripPoints.append(AcGePoint3d(iPnt.x, iPnt.y, 0));
				}
			}
		}
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVGrid::subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	assertWriteEnabled();
	if (m_fixed)
	{
		if (m_dbCurve)
		{
			m_dbCurve->moveGripPointsAt(indices, offset);
			int numbSegU, numbSegV;
			numbSegU = m_geUVGrid->getNumberSegOfU();
			numbSegV = m_geUVGrid->getNumberSegOfV();
		}
	}
	else
	{
		if (m_geUVGrid->m_arrNode)
		{
			std::map<int, std::pair<int, int>> mapIndices;
			int iCol = m_geUVGrid->m_iCol,
				iRow = m_geUVGrid->m_iRow,
				iIdx = 0;
			for (int i = 0; i <= iRow; i++)
			{
				for (int j = 0; j <= iCol; j++)
				{
					mapIndices[iIdx++] = std::pair<int, int>(i, j);
				}
			}

			for (int i = 0; i < indices.length(); i++)
			{
				std::map<int, std::pair<int, int>>::iterator itFind = mapIndices.find(indices.at(i));
				if (itFind == mapIndices.end())
				{
					continue;
				}
				AcGePoint2d iPnt = m_geUVGrid->m_arrNode[itFind->second.first][itFind->second.second];
				iPnt += AcGeVector2d(offset.x, offset.y);
				m_geUVGrid->setNodePosition(itFind->second.first, itFind->second.second, iPnt);
			}
			m_geUVGrid->m_isEditMap = true;
		}
	}

	return Acad::eOk;
}

void CAcDbUVGrid::subList() const
{
	assertReadEnabled();
	AcDbEntity::subList();
	acutPrintf(_T("\n---- CAcDbUVGrid ----"));
}

Acad::ErrorStatus CAcDbUVGrid::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVGrid::subIntersectWith(const AcDbEntity* ent, AcDb::Intersect intType, const AcGePlane& projPlane, AcGePoint3dArray& points, Adesk::GsMarker thisGsMarker /*= 0*/, Adesk::GsMarker otherGsMarker /*= 0*/) const
{
	return Acad::eNotApplicable;
}

Acad::ErrorStatus CAcDbUVGrid::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds) const
{
	assertReadEnabled();
	if (m_dbCurve)
	{
		m_dbCurve->getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
	}

	if (m_geUVGrid->m_arrNode)
	{
		int iRow = m_geUVGrid->getMapRow(),
			iCol = m_geUVGrid->getMapColumn();
		for (int i = 0; i <= iRow; i++)
		{
			for (int j = 0; j <= iCol; j++)
			{
				AcGePoint2d iPnt = m_geUVGrid->nodeAt(i, j);
				snapPoints.append(AcGePoint3d(iPnt.x, iPnt.y, 0));
			}
		}
	}
	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVGrid::subGetOsnapPoints(AcDb::OsnapMode osnapMode, Adesk::GsMarker gsSelectionMark, const AcGePoint3d& pickPoint, const AcGePoint3d& lastPoint, const AcGeMatrix3d& viewXform, AcGePoint3dArray& snapPoints, AcDbIntArray& geomIds, const AcGeMatrix3d& insertionMat) const
{
	assertReadEnabled();
	if (m_dbCurve)
	{
		m_dbCurve->getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat);
	}

	if (m_geUVGrid->m_arrNode)
	{
		int iRow = m_geUVGrid->getMapRow(),
			iCol = m_geUVGrid->getMapColumn();
		for (int i = 0; i <= iRow; i++)
		{
			for (int j = 0; j <= iCol; j++)
			{
				AcGePoint2d iPnt = m_geUVGrid->nodeAt(i, j);
				snapPoints.append(AcGePoint3d(iPnt.x, iPnt.y, 0));
			}
		}
	}

	return Acad::eOk;
}

Acad::ErrorStatus CAcDbUVGrid::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
{
	assertReadEnabled();
	AcDbIntArray osnapModes, geomIds;
	return subGetGripPoints(stretchPoints, osnapModes, geomIds);
}

Acad::ErrorStatus CAcDbUVGrid::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	return subMoveGripPointsAt(indices, offset);
}

bool CAcDbUVGrid::bounds(AcDbExtents& bounds) const
{
	if (!m_geUVGrid || !m_geUVGrid->m_arrNode)
		return false;

	std::list<double> lstX, lstY;
	int iRow = m_geUVGrid->getMapRow(),
		iCol = m_geUVGrid->getMapColumn();
	for (int i = 0; i <= iRow; i++)
	{
		for (int j = 0; j <= iCol; j++)
		{
			AcGePoint2d iPnt = m_geUVGrid->nodeAt(i, j);
			lstX.push_back(iPnt.x);
			lstY.push_back(iPnt.y);
		}
	}

	double dMinX = 0, dMaxX = 0,
		dMinY = 0, dMaxY = 0;
	if (!lstX.empty())
	{
		lstX.sort();
		dMinX = *lstX.begin();
		dMaxX = *lstX.rbegin();
	}
	if (!lstY.empty())
	{
		lstY.sort();
		dMinY = *lstY.begin();
		dMaxY = *lstY.rbegin();
	}
	bounds.set(AcGePoint3d(dMinX, dMinY, 0),
		AcGePoint3d(dMaxX, dMaxY, 0));
	return true;
}

Acad::ErrorStatus CAcDbUVGrid::dwgInFields(AcDbDwgFiler* filer)
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

	if (version > CAcDbUVGrid::kCurrentVersionNumber)
	{
		return Acad::eMakeMeProxy;
	}

	int numbOfU, numbOfV;
	es = filer->readItem(&numbOfU);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_numbSegU = numbOfU;

	es = filer->readItem(&numbOfV);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_numbSegV = numbOfV;

	double sizeU, sizeV;
	es = filer->readItem(&sizeU);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->setSizeOfU(sizeU);

	es = filer->readItem(&sizeV);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->setSizeOfV(sizeV);

	double baseDistance;
	es = filer->readItem(&baseDistance);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->setBaseDistance(baseDistance);

	es = filer->readItem(&m_fixed);
	if (es != Acad::eOk)
	{
		return es;
	}

	int curveType = CurveType::None;
	es = filer->readItem(&curveType);
	if (es != Acad::eOk)
	{
		return es;
	}

	SAFE_DELETE(m_dbCurve);
	switch (curveType)
	{
	case CurveType::Line:
	{
		m_dbCurve = new AcDbLine;
	}
	break;
	case CurveType::Polyline:
	{
		m_dbCurve = new AcDbPolyline;
	}
	break;
	case CurveType::Spline:
	{
		m_dbCurve = new AcDbSpline;
	}
	break;
	case CurveType::Xline:
	{
		m_dbCurve = new AcDbXline;
	}
	break;
	case CurveType::Ray:
	{
		m_dbCurve = new AcDbRay;
	}
	break;
	case CurveType::Arc:
	{
		m_dbCurve = new AcDbArc;
	}
	break;
	default:
		break;
	}

	if (m_dbCurve)
	{
		es = m_dbCurve->dwgInFields(filer);
		if (es != Acad::eOk)
		{
			return es;
		}

		AcGeCurve3d* geCurvePath = NULL;
		m_dbCurve->getAcGeCurve(geCurvePath);
		if (geCurvePath)
		{
			m_geUVGrid->setGeCurvePath(*geCurvePath);
		}
	}

	Adesk::UInt32 rotateUVMap, rotateFace;
	es = filer->readItem(&rotateUVMap);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_rotateUVMap = rotateUVMap;

	es = filer->readItem(&rotateFace);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_rotateFace = rotateFace;

	int row, column;
	es = filer->readItem(&row);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_iRow = row;

	es = filer->readItem(&column);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_iCol = column;

	double sizeOfRow, sizeOfColumn;
	es = filer->readItem(&sizeOfRow);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_sizeOfRow = sizeOfRow;

	es = filer->readItem(&sizeOfColumn);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_sizeOfColumn = sizeOfColumn;

	AcGePoint2d gePointStartUV;
	es = filer->readItem(&gePointStartUV);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_gePointStartUV = gePointStartUV;

	AcGePoint3d pStartUVMap;
	es = filer->readItem(&pStartUVMap);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_pStartUVMap = pStartUVMap;

	double dbLength;
	es = filer->readItem(&dbLength);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->m_dblength = dbLength;

	m_geUVGrid->clearNodes();
	Adesk::UInt32 uiRow = 0,
		uiCol = 0;
	if ((es = filer->readItem(&uiRow)) != Acad::eOk)
	{
		return es;
	}
	if ((es = filer->readItem(&uiCol)) != Acad::eOk)
	{
		return es;
	}
	bool bIsCreate = true;
	if ((es = filer->readItem(&bIsCreate)) != Acad::eOk)
	{
		return es;
	}
	if (!bIsCreate)
	{
		return es;
	}

	m_geUVGrid->setNodesSize(uiRow, uiCol);
	Adesk::UInt32 uiSize = (uiRow + 1) * (uiCol + 1);
	AcGePoint2dArray arrNode;
	arrNode.setLogicalLength(uiSize);
	if ((es = filer->readItem(&arrNode[0], sizeof(AcGePoint2d) * uiSize)) != Acad::eOk)
	{
		return es;
	}

	int iIdx = 0;
	for (int i = 0; i <= uiRow; i++)
	{
		for (int j = 0; j <= uiCol; j++)
		{
			m_geUVGrid->setNodePosition(i, j, arrNode.at(iIdx++));
		}
	}

	m_geUVGrid->removeAll();
	Adesk::UInt32 mapSize = 0;
	es = filer->readItem(&mapSize);

	for (Adesk::UInt32 i = 0; i < mapSize; ++i)
	{
		int key = 0;
		filer->readItem(&key);

		std::list<CAcGeQuadrangle2d*> quadList;
		Adesk::UInt32 listSize = 0;
		filer->readItem(&listSize);

		for (Adesk::UInt32 j = 0; j < listSize; ++j)
		{
			CAcGeQuadrangle2d* geQuadrangle = new CAcGeQuadrangle2d;
			CAcDbQuadrangle2d dbQuadrangle;
			dbQuadrangle.setGeQuadrangle2d(geQuadrangle);
			dbQuadrangle.dwgInFields(filer);
			quadList.push_back(geQuadrangle);
		}
		m_geUVGrid->m_mapGeQuadrangle[key] = quadList;
	}

	bool isFix;
	es = filer->readItem(&isFix);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geUVGrid->setFixed(isFix);

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbUVGrid::dwgOutFields(AcDbDwgFiler* filer) const
{
	assertReadEnabled();
	Acad::ErrorStatus es;
	es = AcDbEntity::dwgOutFields(filer);
	if (es != Acad::eOk)
	{
		return es;
	}

	// object version - must always be the first item.
	if ((es = filer->writeUInt32(CAcDbUVGrid::kCurrentVersionNumber)) != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geUVGrid->getNumberSegOfU());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geUVGrid->getNumberSegOfV());
	if (es != Acad::eOk)
	{
		return es;
	}


	es = filer->writeItem(m_geUVGrid->getSizeOfU());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geUVGrid->getSizeOfV());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geUVGrid->getBaseDistance());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_fixed);
	if (es != Acad::eOk)
	{
		return es;
	}

	CurveType curveType = CurveType::None;
	if (m_dbCurve)
	{
		if (m_dbCurve->isKindOf(AcDbLine::desc()))
		{
			curveType = CurveType::Line;
		}
		else if (m_dbCurve->isKindOf(AcDbPolyline::desc()))
		{
			curveType = CurveType::Polyline;
		}
		else if (m_dbCurve->isKindOf(AcDbSpline::desc()))
		{
			curveType = CurveType::Spline;
		}
		else if (m_dbCurve->isKindOf(AcDbXline::desc()))
		{
			curveType = CurveType::Xline;
		}
		else if (m_dbCurve->isKindOf(AcDbRay::desc()))
		{
			curveType = CurveType::Ray;
		}
		else if (m_dbCurve->isKindOf(AcDbArc::desc()))
		{
			curveType = CurveType::Arc;
		}

		es = filer->writeItem(curveType);
		if (es != Acad::eOk)
		{
			return es;
		}

		if (curveType != CurveType::None)
		{
			es = m_dbCurve->dwgOutFields(filer);
			if (es != Acad::eOk)
			{
				return es;
			}
		}
	}
	else
	{
		es = filer->writeItem(curveType);
		if (es != Acad::eOk)
		{
			return es;
		}
	}

	Adesk::UInt32 rotateUVMap, rotateFace;
	rotateUVMap = m_geUVGrid->getRotateUVMap();
	es = filer->writeItem(rotateUVMap);
	if (es != Acad::eOk)
	{
		return es;
	}

	rotateFace = m_geUVGrid->getRotateFace();
	es = filer->writeItem(rotateFace);
	if (es != Acad::eOk)
	{
		return es;
	}

	Adesk::UInt32 mapRow, mapColumn;
	mapRow = m_geUVGrid->getMapRow();
	es = filer->writeItem(mapRow);
	if (es != Acad::eOk)
	{
		return es;
	}

	mapColumn = m_geUVGrid->getMapColumn();
	es = filer->writeItem(mapColumn);
	if (es != Acad::eOk)
	{
		return es;
	}

	double sizeOfRow, sizeOfColumn;
	sizeOfRow = m_geUVGrid->getSizeOfRow();
	es = filer->writeItem(sizeOfRow);
	if (es != Acad::eOk)
	{
		return es;
	}

	sizeOfColumn = m_geUVGrid->getSizeOfColumn();
	es = filer->writeItem(sizeOfColumn);
	if (es != Acad::eOk)
	{
		return es;
	}

	AcGePoint2d gePointStartUV;
	gePointStartUV = m_geUVGrid->getPointStartUV();
	es = filer->writeItem(gePointStartUV);
	if (es != Acad::eOk)
	{
		return es;
	}

	AcGePoint3d pStartUVMap;
	pStartUVMap = m_geUVGrid->m_pStartUVMap;
	es = filer->writeItem(pStartUVMap);
	if (es != Acad::eOk)
	{
		return es;
	}

	double dbLength;
	dbLength = m_geUVGrid->m_dblength;
	es = filer->writeItem(dbLength);
	if (es != Acad::eOk)
	{
		return es;
	}

	Adesk::UInt32 uiRow = m_geUVGrid->m_iRow,
		uiCol = m_geUVGrid->m_iCol;
	if ((es = filer->writeItem(uiRow)) != Acad::eOk)
	{
		return es;
	}
	if ((es = filer->writeItem(uiCol)) != Acad::eOk)
	{
		return es;
	}
	bool bIsCreate = false;
	if (m_geUVGrid->m_arrNode)
	{
		bIsCreate = true;
	}
	if ((es = filer->writeItem(bIsCreate)) != Acad::eOk)
	{
		return es;
	}
	if (!bIsCreate)
	{
		return es;
	}

	Adesk::UInt32 uiSize = (uiRow + 1) * (uiCol + 1);
	AcGePoint2dArray arrNode;
	arrNode.setPhysicalLength(uiSize);
	for (int i = 0; i <= uiRow; i++)
	{
		for (int j = 0; j <= uiCol; j++)
		{
			arrNode.append(m_geUVGrid->nodeAt(i, j));
		}
	}
	if ((es = filer->writeItem(&arrNode[0], sizeof(AcGePoint2d) * uiSize)) != Acad::eOk)
	{
		return es;
	}

	Adesk::UInt32 mapSize = m_geUVGrid->m_mapGeQuadrangle.size();
	if ((es = filer->writeItem(mapSize)) != Acad::eOk)
	{
		return es;
	}

	for (const auto& pair : m_geUVGrid->m_mapGeQuadrangle)
	{
		int key = pair.first;
		const std::list<CAcGeQuadrangle2d*>& quadrangles = pair.second;

		filer->writeItem(key);

		Adesk::UInt32 listSize = quadrangles.size();
		filer->writeItem(listSize);

		for (const auto quadrangle : quadrangles)
		{
			CAcDbQuadrangle2d dbQua;
			dbQua.setGeQuadrangle2d(quadrangle);
			es = dbQua.dwgOutFields(filer);
			if (es != Acad::eOk)
			{
				return es;
			}
		}
	}

	es = filer->writeItem(m_geUVGrid->getFixed());
	if (es != Acad::eOk)
	{
		return es;
	}

	return filer->filerStatus();
}


Acad::ErrorStatus CAcDbUVGrid::subHighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subHighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVGrid::subUnhighlight(const AcDbFullSubentPath& subId /*= kNullSubent*/, const Adesk::Boolean highlightAll /*= false*/) const
{
	Acad::ErrorStatus es;
	AcDbObjectId id = objectId();
	acdbQueueForRegen(&id, 1);
	return AcDbEntity::subUnhighlight(subId, highlightAll);
}

Acad::ErrorStatus CAcDbUVGrid::subExplode(AcDbVoidPtrArray& entitySet) const
{
	assertReadEnabled();

	if (m_dbCurve)
	{
		m_dbCurve->explode(entitySet);
	}
	return Acad::eOk;
}
Acad::ErrorStatus CAcDbUVGrid::setColorIndex(Adesk::UInt16 color, Adesk::Boolean doSubents)
{
	assertWriteEnabled();
	return Acad::eOk;
}
CAcGeUVGrid* CAcDbUVGrid::getGeUVGrid()
{
	return m_geUVGrid;
}

bool CAcDbUVGrid::setGeUVGrid(const CAcGeUVGrid* geUVGrid)
{
	if (!geUVGrid)
		return false;

	if (m_ownsGe)
		SAFE_DELETE(m_geUVGrid);
	m_geUVGrid = const_cast<CAcGeUVGrid*>(geUVGrid);
	m_ownsGe = false;
	return true;
}

void CAcDbUVGrid::updateGeFromDb()
{
}

void CAcDbUVGrid::setIsUVMap(bool& isUVMap)
{
	m_isUVMap = isUVMap;
}

bool CAcDbUVGrid::getIsUVMap()
{
	return m_isUVMap;
}

void CAcDbUVGrid::updateDbFromGe()
{
}

AcDbCurve* CAcDbUVGrid::getDbCurvePath()
{
	return m_dbCurve;
}

bool CAcDbUVGrid::setDbCurvePath(const AcGeCurve3d& geCurvePath)
{
	if (AcDbCurve::createFromAcGeCurve(geCurvePath, m_dbCurve) == Acad::ErrorStatus::eOk)
	{
		m_geUVGrid->setGeCurvePath(geCurvePath);
		return true;
	}
	return false;
}

bool CAcDbUVGrid::setMapPoint(double& height, double& width, double& row, AcGePoint3d & ptPoint, AcGePoint3d & pointRootStart, AcGePoint3d & pointRootEnd, AcGePoint3d & pSet)
{
	AcDbCurve* dbCur = NULL;
	dbCur = getDbCurvePath();
	if (!dbCur)
		return false;
	AcGePoint3dArray arrPsplit;
	arrPsplit.append(pointRootStart);
	arrPsplit.append(pointRootEnd);
	AcDbVoidPtrArray arrDbCurve;
	Acad::ErrorStatus es = dbCur->getSplitCurves(arrPsplit, arrDbCurve);
	dbCur->close();
	if (es != Acad::eOk)
	{
		return false;
	}

	AcDbCurve* newDbCurve = NULL;
	int iLen = arrDbCurve.length();
	for (int i = 0; i < iLen; i++)
	{
		AcDbCurve* iDbCur = (AcDbCurve*)arrDbCurve.at(i);
		if (!iDbCur)
		{
			continue;
		}
		AcGePoint3d ipStart, ipEnd;
		iDbCur->getStartPoint(ipStart);
		iDbCur->getEndPoint(ipEnd);
		if (AcGePoint2d(ipStart.x, ipStart.y).distanceTo(AcGePoint2d(pointRootStart.x, pointRootStart.y)) < 0.01 && AcGePoint2d(ipEnd.x, ipEnd.y).distanceTo(AcGePoint2d(pointRootEnd.x, pointRootEnd.y)) < 0.01)
		{
			newDbCurve = iDbCur;
			break;
		}
	}
	if (!newDbCurve)
	{
		return false;
	}

	AcDbCurve* dbCurveClone = AcDbCurve::cast(newDbCurve->clone());
	AcGeVector3d dMove = ptPoint - pointRootStart;
	AcGeVector3d move = dMove;
	dbCurveClone->transformBy(move);
	std::map<int, AcDbCurve*> mapOff;
	mapOff[0] = dbCurveClone;
	AcGePoint3d pStart, pEnd;
	dbCurveClone->getStartPoint(pStart);
	dbCurveClone->getEndPoint(pEnd);
	double dbLength = pStart.distanceTo(pEnd);
	m_geUVGrid->m_dblength = dbLength;
	m_geUVGrid->m_pStartUVMap = pStart;

	for (int i = 1; i <= row; i++)
	{
		AcDbVoidPtrArray negArr, posArr;
		if (dbCurveClone->getOffsetCurves(i * abs(height), posArr) == Acad::eOk)
		{
			if (posArr.length() == 1)
			{
				AcDbCurve* curPos = (AcDbCurve*)posArr.at(0);
				mapOff[i] = curPos;
			}
		}
		if (dbCurveClone->getOffsetCurves(-i * abs(height), negArr) == Acad::eOk)
		{
			if (negArr.length() == 1)
			{
				AcDbCurve* curNeg = (AcDbCurve*)negArr.at(0);
				mapOff[-i] = curNeg;
			}
		}
	}

	int m_row = mapOff.size() - 1;
	std::list<double> lstDist;
	double dStart = 0,
		dEnd = 0;
	newDbCurve->getDistAtPoint(pointRootStart, dStart);
	newDbCurve->getDistAtPoint(pointRootEnd, dEnd);
	double diPlus = 0;
	while (true)
	{
		if (diPlus > dEnd)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else if (abs(diPlus - dEnd) < 0.01)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else
		{
			lstDist.push_back(diPlus);
			diPlus += width;
		}
	}

	int m_collumn = lstDist.size() - 1;
	m_geUVGrid->setNodesSize(m_row, m_collumn);

	std::list<double>::iterator itDist = lstDist.begin();
	int iRow = 0, iCol = 0;
	for (; itDist != lstDist.end(); itDist++)
	{
		double di0 = *itDist;
		AcGePoint3d iPnt;
		dbCurveClone->getPointAtDist(di0, iPnt);

		std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
		iRow = 0;
		for (; itOff != mapOff.rend(); itOff++)
		{
			AcGePoint3d pClost;
			itOff->second->getClosestPointTo(iPnt, pClost);
			m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pClost.x, pClost.y));
		}
		iCol++;
	}
	m_geUVGrid->setPointStartUV(m_geUVGrid->m_arrNode[0][0]);
	m_geUVGrid->dataMapUVGrid(pEnd.convert2d(AcGePlane()), dbLength);
	for (std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin(); itMap != mapOff.end(); itMap++)
	{
		if ((*itMap).second)
		{
			delete (*itMap).second;
		}
	}
	mapOff.clear();
	delete newDbCurve;
	return true;
}

bool CAcDbUVGrid::setMapUV(double& height, double& width, double& row, AcGePoint3d & ptPoint, AcDbCurve * &dbCurvePath, AcGePoint3d & pointRootStart, AcGePoint3d & pointRootEnd, AcGePoint3d & pSet)
{
	AcGeCurve3d* geCurvePath = NULL;
	if (dbCurvePath->getAcGeCurve(geCurvePath) == Acad::eOk)
	{
		m_geUVGrid->setGeCurvePath(*geCurvePath);
	}
	m_dbCurve = AcDbCurve::cast(dbCurvePath->clone());
	return this->setMapPoint(height, width, row, ptPoint, pointRootStart, pointRootEnd, pSet);
}

bool CAcDbUVGrid::setNumberSegOfU(const int& numb)
{
	if (numb <= 0)
	{
		return false;
	}
	int numbSegV = m_geUVGrid->getNumberSegOfV();
	return true;
}

bool CAcDbUVGrid::setNumberSegOfV(const int& numb)
{
	if (numb <= 0)
	{
		return false;
	}
	int numbSegU = m_geUVGrid->getNumberSegOfU();
	return true;
}

bool CAcDbUVGrid::getFixed()
{
	return m_fixed;
}

void CAcDbUVGrid::setFixed(bool isFixed)
{
	m_fixed = isFixed;
}

bool CAcDbUVGrid::getBasePoint(AcGePoint3d & basePoint)
{
	return m_geUVGrid->getBasePoint(basePoint);
}

unsigned int CAcDbUVGrid::getNumbVerts() const
{
	assertReadEnabled();
	AcDbPolyline* dbPolyline = NULL;
	if (m_dbCurve->isKindOf(AcDbPolyline::desc()))
	{
		dbPolyline = AcDbPolyline::cast(m_dbCurve);
	}
	if (!dbPolyline)
	{
		return 0;
	}

	return dbPolyline->numVerts();
}
bool CAcDbUVGrid::setNumbVerts(const int& input)
{
	if (input <= 1)
	{
		return false;
	}

	AcDbPolyline* dbPolyline = NULL;
	if (m_dbCurve->isKindOf(AcDbPolyline::desc()))
	{
		dbPolyline = AcDbPolyline::cast(m_dbCurve);
	}
	if (!dbPolyline)
	{
		return false;
	}

	unsigned int numbVerts = dbPolyline->numVerts();
	if (numbVerts > input)
	{
		int delta = numbVerts - input;
		for (int i = 0; i < delta; i++)
		{
			int removeAt = input + i;
			dbPolyline->removeVertexAt(removeAt);
		}

		int numbSegU, numbSegV;
		numbSegU = m_geUVGrid->getNumberSegOfU();
		numbSegV = m_geUVGrid->getNumberSegOfV();
	}
	else if (numbVerts < input)
	{
		AcGePoint3d startPoint, endPoint;
		dbPolyline->getEndPoint(endPoint);
		double curveLength;
		dbPolyline->getDistAtPoint(endPoint, curveLength);

		double baseDis = m_geUVGrid->getBaseDistance();
		double uSize = m_geUVGrid->getSizeOfU();
		double vSize = m_geUVGrid->getSizeOfV();
		double segment = curveLength / (input - 1);
		std::list<AcGePoint2d> listPoint;
		for (int i = 0; i < input; i++)
		{
			double dis = segment * i;
			AcGePoint3d point;
			dbPolyline->getPointAtDist(dis, point);
			listPoint.push_back(AcGePoint2d(point.x, point.y));
		}
		AcDbPolyline newPolyline;
		int i = 0;
		for (std::list<AcGePoint2d>::iterator it = listPoint.begin(); it != listPoint.end(); it++, i++)
		{
			newPolyline.addVertexAt(i, *it);
		}

		AcGeCurve3d* geCurvePath = NULL;
		if (newPolyline.getAcGeCurve(geCurvePath) != Acad::ErrorStatus::eOk)
		{
			return false;
		}

		int numbSegU, numbSegV;
		numbSegU = m_geUVGrid->getNumberSegOfU();
		numbSegV = m_geUVGrid->getNumberSegOfV();
	}
	return true;
}

bool CAcDbUVGrid::getUVInGrid(const AcGePoint2d & point, double& u, double& v)
{
	return m_geUVGrid->getUVInGrid(point, u, v);
}

std::list<CAcDbQuadrangle2d*> CAcDbUVGrid::getDbQuadrangle2d()
{
	return m_listQuadrangle;
}

bool CAcDbUVGrid::setMapPointByAlignment(AcDbCurve * &dbCurve)
{
	m_geUVGrid->clearNodes();
	AcGePoint3d ipS, ipE;
	dbCurve->getStartPoint(ipS);
	dbCurve->getEndPoint(ipE);
	if (m_geUVGrid->m_dblength < ipS.distanceTo(ipE))
	{
		AcGePoint3d pClost;
		dbCurve->getPointAtDist(m_geUVGrid->m_dblength, pClost);
		AcGePoint3dArray arrPsplit;
		arrPsplit.append(pClost);
		AcDbVoidPtrArray arrDbCurve;
		if (dbCurve->getSplitCurves(arrPsplit, arrDbCurve) != Acad::eOk)
			return false;

		int iLen = arrDbCurve.length();
		for (int i = 0; i < iLen; i++)
		{
			AcDbCurve* iDbCur = (AcDbCurve*)arrDbCurve.at(i);
			if (!iDbCur)
			{
				continue;
			}
			AcGePoint3d ipStart;
			iDbCur->getStartPoint(ipStart);
			if (ipStart == ipS)
			{
				dbCurve = iDbCur;
				break;
			}
		}
	}

	AcGeCurve3d* geCurvePath = NULL;
	if (dbCurve->getAcGeCurve(geCurvePath) != Acad::ErrorStatus::eOk)
	{
		SAFE_DELETE(dbCurve);
	}
	this->setDbCurvePath(*geCurvePath);

	AcDbCurve* dbCurveClone = AcDbCurve::cast(dbCurve->clone());
	AcGePoint3d pStart, pEnd;
	dbCurveClone->getStartPoint(pStart);
	dbCurveClone->getEndPoint(pEnd);
	AcGeVector3d dMove = m_geUVGrid->m_pStartUVMap - pStart;
	AcGeVector3d move = dMove;
	dbCurveClone->transformBy(move);

	std::map<int, AcDbCurve*> mapOff;
	mapOff[0] = dbCurveClone;

	for (int i = 1; i <= m_geUVGrid->m_iRow * 0.5; i++)
	{
		AcDbVoidPtrArray negArr, posArr;
		if (dbCurveClone->getOffsetCurves(i, posArr) == Acad::eOk)
		{
			if (posArr.length() == 1)
			{
				AcDbCurve* curPos = (AcDbCurve*)posArr.at(0);
				mapOff[i] = curPos;
			}
		}
		if (dbCurveClone->getOffsetCurves(-i, negArr) == Acad::eOk)
		{
			if (negArr.length() == 1)
			{
				AcDbCurve* curNeg = (AcDbCurve*)negArr.at(0);
				mapOff[-i] = curNeg;
			}
		}
	}

	std::list<double> lstDist;
	double dStart = 0,
		dEnd = 0;
	dbCurveClone->getDistAtPoint(pStart, dStart);
	double endParam;
	dbCurveClone->getEndParam(endParam);
	dbCurveClone->getDistAtParam(endParam, dEnd);
	double diPlus = 0;
	while (true)
	{
		if (diPlus > dEnd)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else if (abs(diPlus - dEnd) < 0.01)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else
		{
			lstDist.push_back(diPlus);
			diPlus += m_geUVGrid->m_sizeOfColumn;
		}
	}

	int row = mapOff.size() - 1;
	int collumn = lstDist.size() - 1;
	m_geUVGrid->setNodesSize(row, collumn);

	std::list<double>::iterator itDist = lstDist.begin();
	int iRow = 0, iCol = 0;
	for (; itDist != lstDist.end(); itDist++)
	{
		double di0 = *itDist;
		AcGePoint3d iPnt;
		dbCurveClone->getPointAtDist(di0, iPnt);

		std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
		iRow = 0;
		for (; itOff != mapOff.rend(); itOff++)
		{
			AcGePoint3d pClost;
			itOff->second->getClosestPointTo(iPnt, pClost);
			m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pClost.x, pClost.y));
		}
		iCol++;
	}
	dbCurveClone->close();
	std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin();
	for (; itMap != mapOff.end(); itMap++)
	{
		delete (*itMap).second;
	}
	return true;
}

bool CAcDbUVGrid::setMapPointUVByAlignment(AcDbPolyline*& dbPolyline, const double& height, const double& width, const int& row, const int& column)
{
	if (!dbPolyline)
		return false;
	AcDbPolyline* dbl = NULL;
	extendAlignment(dbPolyline, dbl);
	if (!dbl)
		return false;

	AcGePoint3d pointStart, pointEnd;
	double endParam;
	dbl->getStartPoint(pointStart);
	dbl->getEndParam(endParam);
	dbl->getPointAtParam(endParam, pointEnd);

	std::map<int, AcDbCurve*> mapOff;
	mapOff[0] = dbl;

	int nRow = std::ceil((double)row / 2);
	for (int i = 1; i <= nRow; i++)
	{
		AcDbVoidPtrArray negArr, posArr;
		if (dbl->getOffsetCurves(i * abs(height), posArr) == Acad::eOk)
		{
			if (posArr.length() == 1)
			{
				AcDbCurve* curPos = (AcDbCurve*)posArr.at(0);
				mapOff[i] = curPos;
			}
		}

		if (row % 2 == 1 && i == nRow)
			break;

		if (dbl->getOffsetCurves(-i * abs(height), negArr) == Acad::eOk)
		{
			if (negArr.length() == 1)
			{
				AcDbCurve* curNeg = (AcDbCurve*)negArr.at(0);
				mapOff[-i] = curNeg;
			}
		}
	}

	int rowUV = mapOff.size() - 1;
	std::list<double> lstDist;
	double dStart = 0, dEnd = 0;
	dbl->getDistAtPoint(pointStart, dStart);
	dbl->getDistAtPoint(pointEnd, dEnd);
	double diPlus = 0;
	while (true)
	{
		if (diPlus > dEnd)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else if (abs(diPlus - dEnd) < 0.01)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else
		{
			lstDist.push_back(diPlus);
			diPlus += width;
		}
	}

	if (column > 0)
	{
		m_geUVGrid->setNodesSize(rowUV, column);
		m_geUVGrid->setSizeOfColumn(width);
		m_geUVGrid->setSizeOfRow(height);

		std::list<double>::iterator itDist = lstDist.begin();
		int iRow = 0, iCol = 0;
		for (; itDist != lstDist.end(); itDist++)
		{
			double di0 = *itDist;
			AcGePoint3d iPnt;
			dbl->getPointAtDist(di0, iPnt);

			std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
			iRow = 0;
			for (; itOff != mapOff.rend(); itOff++)
			{
				AcGePoint3d pClost;
				itOff->second->getClosestPointTo(iPnt, pClost);
				m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pClost.x, pClost.y));
			}
			if (iCol == column)
			{
				break;
			}
			iCol++;
		}

		std::map<int, std::list<CAcGeQuadrangle2d>> mapGeQuadrangle;
		for (int i = 0; i < rowUV; i++)
		{
			for (int j = 0; j < column; j++)
			{
				AcGePoint2d pN0 = m_geUVGrid->nodeAt(i, j),
					pN1 = m_geUVGrid->nodeAt(i, j + 1),
					pN2 = m_geUVGrid->nodeAt(i + 1, j + 1),
					pN3 = m_geUVGrid->nodeAt(i + 1, j);

				CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
				geQuad->setPoint(pN0, pN1, pN2, pN3);
				geQuad->dU = j;
				geQuad->dV = i;
				m_geUVGrid->addGeQuadrangle2d(geQuad);
			}
		}

		dbPolyline->close();
		dbl->close();
		std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin();
		for (; itMap != mapOff.end(); itMap++)
		{
			delete (*itMap).second;
		}
		return true;
	}

	int collumnUV = lstDist.size() - 1;
	m_geUVGrid->setNodesSize(rowUV, collumnUV);
	m_geUVGrid->setSizeOfColumn(width);
	m_geUVGrid->setSizeOfRow(height);
	m_geUVGrid->m_columnDefaulf = m_geUVGrid->getMapColumn();

	std::list<double>::iterator itDist = lstDist.begin();
	int iRow = 0, iCol = 0;
	for (; itDist != lstDist.end(); itDist++)
	{
		double di0 = *itDist;
		AcGePoint3d iPnt;
		dbl->getPointAtDist(di0, iPnt);

		std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
		iRow = 0;
		for (; itOff != mapOff.rend(); itOff++)
		{
			AcGePoint3d pClost;
			itOff->second->getClosestPointTo(iPnt, pClost);
			m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pClost.x, pClost.y));
		}
		iCol++;
	}

	std::map<int, std::list<CAcGeQuadrangle2d>> mapGeQuadrangle;
	for (int i = 0; i < rowUV; i++)
	{
		for (int j = 0; j < collumnUV; j++)
		{
			AcGePoint2d pN0 = m_geUVGrid->nodeAt(i, j),
				pN1 = m_geUVGrid->nodeAt(i, j + 1),
				pN2 = m_geUVGrid->nodeAt(i + 1, j + 1),
				pN3 = m_geUVGrid->nodeAt(i + 1, j);

			CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
			geQuad->setPoint(pN0, pN1, pN2, pN3);
			geQuad->dU = j;
			geQuad->dV = i;

			//add geQuad
			m_geUVGrid->addGeQuadrangle2d(geQuad);
		}
	}

	dbPolyline->close();
	dbl->close();
	std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin();
	for (; itMap != mapOff.end(); itMap++)
	{
		delete (*itMap).second;
	}

	return true;
}

bool CAcDbUVGrid::setMapPointUVByAlignmentEx(AcDbPolyline * &dbPolyline, const double& height, const double& width, const int& row, const int& column)
{
	if (!dbPolyline)
		return false;
	AcDbPolyline* dbl = NULL;
	extendAlignment(dbPolyline, dbl);
	if (!dbl)
		return false;
	AcGePoint3d pointStart, pointEnd;
	double endParam;
	dbl->getStartPoint(pointStart);
	dbl->getEndParam(endParam);
	dbl->getPointAtParam(endParam, pointEnd);

	double dblLength = 0;
	dbl->getDistAtParam(endParam, dblLength);

	std::map<int, AcDbCurve*> mapOff;
	mapOff[0] = dbl;

	int nRow = std::ceil((double)row / 2);
	for (int i = 1; i <= nRow; i++)
	{
		AcDbVoidPtrArray negArr, posArr;
		if (dbl->getOffsetCurves(i * abs(height), posArr) == Acad::eOk)
		{
			if (posArr.length() == 1)
			{
				AcDbCurve* curPos = (AcDbCurve*)posArr.at(0);
				mapOff[i] = curPos;
			}
		}

		if (row % 2 == 1 && i == nRow)
			break;
		if (dbl->getOffsetCurves(-i * abs(height), negArr) == Acad::eOk)
		{
			if (negArr.length() == 1)
			{
				AcDbCurve* curNeg = (AcDbCurve*)negArr.at(0);
				mapOff[-i] = curNeg;
			}
		}
	}

	int rowUV = mapOff.size() - 1;
	std::list<double> lstDist;
	double dStart = 0,
		dEnd = 0;
	dbl->getDistAtPoint(pointStart, dStart);
	dbl->getDistAtPoint(pointEnd, dEnd);
	double diPlus = 0;
	while (true)
	{
		if (diPlus > dEnd)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else if (abs(diPlus - dEnd) < 0.01)
		{
			lstDist.push_back(dEnd);
			break;
		}
		else
		{
			lstDist.push_back(diPlus);
			diPlus += width;
		}
	}

	if (column > 0)
	{
		m_geUVGrid->setNodesSize(rowUV, column);
		m_geUVGrid->setSizeOfColumn(width);
		m_geUVGrid->setSizeOfRow(height);

		std::list<double>::iterator itDist = lstDist.begin();
		int iRow = 0, iCol = 0;
		for (; itDist != lstDist.end(); itDist++)
		{
			double di0 = *itDist;
			AcGePoint3d iPnt;
			dbl->getPointAtDist(di0, iPnt);

			std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
			iRow = 0;
			for (; itOff != mapOff.rend(); itOff++)
			{
				double endParam = 0;
				itOff->second->getEndParam(endParam);
				double dLength = 0;
				itOff->second->getDistAtParam(endParam, dLength);
				double di1 = di0 * dLength / dblLength;

				AcGePoint3d pDist;
				itOff->second->getPointAtDist(di1, pDist);

				m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pDist.x, pDist.y));
			}
			if (iCol == column)
			{
				break;
			}
			iCol++;
		}

		std::map<int, std::list<CAcGeQuadrangle2d>> mapGeQuadrangle;
		for (int i = 0; i < rowUV; i++)
		{
			for (int j = 0; j < column; j++)
			{
				AcGePoint2d pN0 = m_geUVGrid->nodeAt(i, j),
					pN1 = m_geUVGrid->nodeAt(i, j + 1),
					pN2 = m_geUVGrid->nodeAt(i + 1, j + 1),
					pN3 = m_geUVGrid->nodeAt(i + 1, j);

				CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
				geQuad->setPoint(pN0, pN1, pN2, pN3);
				geQuad->dU = j;
				geQuad->dV = i;
				m_geUVGrid->addGeQuadrangle2d(geQuad);
			}
		}

		dbPolyline->close();
		dbl->close();
		std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin();
		for (; itMap != mapOff.end(); itMap++)
		{
			delete (*itMap).second;
		}
		return true;
	}

	int collumnUV = lstDist.size() - 1;
	m_geUVGrid->setNodesSize(rowUV, collumnUV);
	m_geUVGrid->setSizeOfColumn(width);
	m_geUVGrid->setSizeOfRow(height);
	m_geUVGrid->m_columnDefaulf = m_geUVGrid->getMapColumn();

	std::list<double>::iterator itDist = lstDist.begin();
	int iRow = 0, iCol = 0;
	for (; itDist != lstDist.end(); itDist++)
	{
		double di0 = *itDist;
		AcGePoint3d iPnt;
		dbl->getPointAtDist(di0, iPnt);

		std::map<int, AcDbCurve*>::reverse_iterator itOff = mapOff.rbegin();
		iRow = 0;
		for (; itOff != mapOff.rend(); itOff++)
		{
			double endParam = 0;
			itOff->second->getEndParam(endParam);
			double dLength = 0;
			itOff->second->getDistAtParam(endParam, dLength);
			double di1 = di0 * dLength / dblLength;

			AcGePoint3d pDist;
			itOff->second->getPointAtDist(di1, pDist);

			m_geUVGrid->setNodePosition(iRow++, iCol, AcGePoint2d(pDist.x, pDist.y));
		}
		iCol++;
	}

	std::map<int, std::list<CAcGeQuadrangle2d>> mapGeQuadrangle;
	for (int i = 0; i < rowUV; i++)
	{
		for (int j = 0; j < collumnUV; j++)
		{
			AcGePoint2d pN0 = m_geUVGrid->nodeAt(i, j),
				pN1 = m_geUVGrid->nodeAt(i, j + 1),
				pN2 = m_geUVGrid->nodeAt(i + 1, j + 1),
				pN3 = m_geUVGrid->nodeAt(i + 1, j);

			CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
			geQuad->setPoint(pN0, pN1, pN2, pN3);
			geQuad->dU = j;
			geQuad->dV = i;

			//add geQuad
			m_geUVGrid->addGeQuadrangle2d(geQuad);
		}
	}

	dbPolyline->close();
	dbl->close();
	std::map<int, AcDbCurve*>::iterator itMap = mapOff.begin();
	for (; itMap != mapOff.end(); itMap++)
	{
		delete (*itMap).second;
	}

	return true;
}

bool CAcDbUVGrid::extendAlignment(AcDbPolyline * &dbPolyline, AcDbPolyline * &dbPolylineExtend)
{
	double dEndPar = 0,
		dEndDist = 0;
	dbPolyline->getEndParam(dEndPar);
	dbPolyline->getDistAtParam(dEndPar, dEndDist);

	AcGePoint3d pStart, pEnd;
	dbPolyline->getStartPoint(pStart);
	dbPolyline->getEndPoint(pEnd);
	double dStart = 0,
		dEnd = dEndDist;
	double dNearStart = 0.001,
		dNearEnd = dEndDist - 0.001;

	AcGePoint3d pNearStart = pStart,
		pNearEnd = pEnd;
	if (dNearStart < dEndDist)
	{
		dbPolyline->getPointAtDist(dNearStart, pNearStart);
		AcGeVector3d vDirStart = (pNearStart - pStart).normal();
		pNearStart = pStart - vDirStart;
	}
	if (dNearEnd > 0)
	{
		dbPolyline->getPointAtDist(dNearEnd, pNearEnd);
		AcGeVector3d vDirEnd = (pEnd - pNearEnd).normal();
		pNearEnd = pEnd + vDirEnd;
	}

	AcDbPolyline* plExtent = new AcDbPolyline();
	int iAdd = 0;
	plExtent->addVertexAt(iAdd++, AcGePoint2d(pNearStart.x, pNearStart.y));

	int iNumVert = dbPolyline->numVerts();
	for (int i = 0; i < iNumVert; i++)
	{
		AcGePoint3d iPnt;
		double dBul = 0;
		dbPolyline->getPointAt(i, iPnt);
		dbPolyline->getBulgeAt(i, dBul);
		plExtent->addVertexAt(iAdd++, AcGePoint2d(iPnt.x, iPnt.y), dBul);
	}

	plExtent->setBulgeAt(iAdd - 1, 0);
	plExtent->addVertexAt(iAdd++, AcGePoint2d(pNearEnd.x, pNearEnd.y));
	dbPolylineExtend = plExtent;
	AcGeMatrix3d transformMatrix;
	transformMatrix.setToTranslation(AcGeVector3d(0.1, 0.1, 0.0));
	dbPolylineExtend->transformBy(transformMatrix);
	return true;
}

bool CAcDbUVGrid::setAllMapGeQuadrangle(std::list<CAcDbUVMap*>& lstDbUVMap, std::list<CAcGeUVGrid*>& lstGeUvGrip)
{
	std::map<int, std::list<CAcGeQuadrangle2d>> mapGeQuadrangle;
	std::list<CAcDbUVMap*>::iterator dbMap = lstDbUVMap.begin();
	for (; dbMap != lstDbUVMap.end(); dbMap++)
	{
		CAcGeUVGrid* geUvGrip = (*dbMap)->getGeUVMap()->getGeUVGrid();
		lstGeUvGrip.push_back(geUvGrip);
	}
	return true;
}
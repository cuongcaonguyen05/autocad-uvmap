#include "pch.h"
#include "AcDbQuadrangle2d.h"

Adesk::UInt32 CAcDbQuadrangle2d::kCurrentVersionNumber = 2;

CAcDbQuadrangle2d::CAcDbQuadrangle2d()
{
	m_geQuadrangle2d = NULL;
}

CAcDbQuadrangle2d::~CAcDbQuadrangle2d()
{
	SAFE_DELETE(m_geQuadrangle2d);
}

Adesk::Boolean CAcDbQuadrangle2d::subWorldDraw(AcGiWorldDraw* mode)
{
	if (!m_geQuadrangle2d)
	{
		return Adesk::kFalse;
	}
	if (!mode)
	{
		return Adesk::kFalse;
	}

	AcDbPolyline polyline;
	polyline.setClosed(true);
	for (int i = 0; i < QUADRANGLE_VERTEX; i++)
	{
		polyline.addVertexAt(i, m_geQuadrangle2d->getPoint(i));
	}

	AcCmEntityColor oldColor = mode->subEntityTraits().trueColor();
	AcCmEntityColor newColor;

	if (m_geQuadrangle2d->isValid == true)
	{
		newColor.setColorIndex(3);
		mode->subEntityTraits().setTrueColor(newColor);
		polyline.worldDraw(mode);
	}

	return Adesk::kTrue;
}

Acad::ErrorStatus CAcDbQuadrangle2d::dwgInFields(AcDbDwgFiler* filer)
{
	Acad::ErrorStatus es;
	// object version - must always be the first item.
	Adesk::UInt32 version = 0;
	if ((es = filer->readUInt32(&version)) != Acad::eOk)
		return es;

	if (version > CAcDbQuadrangle2d::kCurrentVersionNumber)
	{
		return Acad::eMakeMeProxy;
	}

	AcGePoint2d first;
	es = filer->readItem(&first);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->setFirstPoint(first);

	AcGePoint2d second;
	es = filer->readItem(&second);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->setSecondPoint(second);

	AcGePoint2d third;
	es = filer->readItem(&third);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->setThirdPoint(third);

	AcGePoint2d fourth;
	es = filer->readItem(&fourth);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->setFourthPoint(fourth);

	if (version < 2)
	{
		return es;
	}
	Adesk::UInt32 dU, dV;
	es = filer->readItem(&dU);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->dU = dU;

	es = filer->readItem(&dV);
	if (es != Acad::eOk)
	{
		return es;
	}
	m_geQuadrangle2d->dV = dV;

	return filer->filerStatus();
}

Acad::ErrorStatus CAcDbQuadrangle2d::dwgOutFields(AcDbDwgFiler* filer) const
{
	Acad::ErrorStatus es = Acad::eInvalidInput;

	if (!m_geQuadrangle2d)
	{
		return es;
	}

	// object version - must always be the first item.
	if ((es = filer->writeUInt32(CAcDbQuadrangle2d::kCurrentVersionNumber)) != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geQuadrangle2d->getFirstPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geQuadrangle2d->getSecondPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geQuadrangle2d->getThirdPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	es = filer->writeItem(m_geQuadrangle2d->getFourthPoint());
	if (es != Acad::eOk)
	{
		return es;
	}

	Adesk::UInt32 dU, dV;
	dU = m_geQuadrangle2d->dU;
	es = filer->writeItem(dU);
	if (es != Acad::eOk)
	{
		return es;
	}

	dV = m_geQuadrangle2d->dV;
	es = filer->writeItem(dV);
	if (es != Acad::eOk)
	{
		return es;
	}

	return filer->filerStatus();
}

bool CAcDbQuadrangle2d::setGeQuadrangle2d(const CAcGeQuadrangle2d* geQuadrangle2d)
{
	if (!geQuadrangle2d)
	{
		return false;
	}
	m_geQuadrangle2d = const_cast<CAcGeQuadrangle2d*>(geQuadrangle2d);
	return true;
}

CAcGeQuadrangle2d* CAcDbQuadrangle2d::getGeQuadrangle2d()
{
	return m_geQuadrangle2d;
}

bool CAcDbQuadrangle2d::isPointInQuadrangle2d(const AcGePoint2d& point)
{
	return m_geQuadrangle2d->isPointInQuadrangle2d(point);
}

bool CAcDbQuadrangle2d::getUVInQuadrangle(const AcGePoint2d& point, double& u, double& v)
{
	return m_geQuadrangle2d->getUVInQuadrangle(point, u, v);
}

#pragma once
#include "pch.h"
#include "AcGeQuadrangle2d.h"

class CAcDbQuadrangle2d
{
	CAcGeQuadrangle2d* m_geQuadrangle2d;
protected:
	static Adesk::UInt32 kCurrentVersionNumber;
public:
	CAcDbQuadrangle2d();
	~CAcDbQuadrangle2d();

	bool setGeQuadrangle2d(const CAcGeQuadrangle2d* geQuadrangle2d);
	CAcGeQuadrangle2d* getGeQuadrangle2d();
	bool isPointInQuadrangle2d(const AcGePoint2d& point);
	bool getUVInQuadrangle(const AcGePoint2d& point, double& u, double& v);

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* filer);
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* filer) const;
};
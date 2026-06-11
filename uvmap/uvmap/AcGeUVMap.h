#pragma once
#include "pch.h"
#include "AcGeUVFace.h"
#include "AcGeUVGrid.h"

class CAcGeUVMap
{
public:
	CAcGeUVMap();
	~CAcGeUVMap();
private:
	std::wstring m_szName;
	std::wstring m_szGroupName;
	std::wstring m_szBridgeName;

	CAcGeUVFace* m_geUVFace;
	CAcGeUVGrid* m_geUVGrid;
public:
	void setName(const std::wstring& name);
	std::wstring getName() const;

	void setGroupName(const std::wstring& name);
	std::wstring getGroupName() const;

	void setBridgeName(const std::wstring& name);
	std::wstring getBridgeName() const;

	CAcGeUVFace* getGeUVFace() const;
	bool setGeUVFace(const CAcGeUVFace* geUVFace);

	CAcGeUVGrid* getGeUVGrid() const;
	bool setGeUVGrid(/*const*/ CAcGeUVGrid* geUVGrid);

	void copyFrom(const CAcGeUVMap* geUVMap);

	void getListUV(std::vector<AcGePoint3d>& listPoint3d, std::vector<AcGePoint2d>& listUVPoint);

	CAcGeUVMap& transformBy(const AcGeMatrix3d& leftSide);
};
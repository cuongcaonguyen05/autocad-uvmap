#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include "gecurv3d.h"
#include "dbcurve.h"
#include "AcGeQuadrangle2d.h"

class CAcDbUVGrid;
class CAcDbUVMap;

struct SGeQuadrangle2d
{
	int iRow = 0;
	int iCol = 0;
	CAcGeQuadrangle2d* geQuadrangle = NULL;
};

class CAcGeUVGrid
{
	friend class CAcDbUVGrid;
	friend class CAcDbUVMap;
	friend class CDbUVMapReactor;
public:
	CAcGeUVGrid();
	~CAcGeUVGrid();
private:
	AcGePoint2d** m_arrNode;	//node[row][col]
	int m_iRow;
	int m_iCol;
	int m_columnDefaulf;
	double m_sizeOfRow;
	double m_sizeOfColumn;

	double m_sizeU;
	double m_sizeV;
	double m_baseDistance;

	int m_numbSegU;
	int m_numbSegV;
	int m_rotateUVMap;
	int m_matixUVMap;
	int m_rotateFace;
	int m_matixFace;
	double m_dblength;
	AcGePoint2d m_gePointStartUV;
	AcGeCurve3d* m_geCurvePath;
	AcGePoint3d m_pStartUVMap;
	std::map<int, std::list<CAcGeQuadrangle2d*>> m_mapGeQuadrangle;
	bool m_isEditMap;
	bool m_fixed;
public:
	std::map<int, std::list<CvGe::CvGePolygon2D>> m_mapPolygon;
	typedef std::list<SGeQuadrangle2d>::iterator iterator;
	typedef std::list<SGeQuadrangle2d>::const_iterator const_iterator;
	void clear();
	void setSizeOfU(const double& size);
	void setSizeOfV(const double& size);
	double getSizeOfU() const;
	double getSizeOfV() const;

	void setBaseDistance(const double& distance);
	double getBaseDistance() const;

	void setGeCurvePath(const AcGeCurve3d& geCurve);
	AcGeCurve3d* getGeCurvePath() const;

	int getNumberSegOfU() const;
	int getNumberSegOfV() const;

	bool getBasePoint(AcGePoint3d& basePoint);
	bool getUVInGrid(const AcGePoint2d& point, double& u, double& v);
	bool getPointAtUV(const double& u, const double& v, AcGePoint2d& point);
	void setPointStartUV(AcGePoint2d& gePointStartUV);
	AcGePoint2d getPointStartUV();
	void dataMapUVGrid(const AcGePoint2d& gePointEnd, double& dblength);
	void setRotateUVMap(const int& rotateUVMap);
	void setRotateFace(const int& rotateFace);
	int getRotateUVMap() const;
	int getRotateFace() const;
	int getRotateMatrixFace() const;
	void rotateMap(AcGePoint2d& gePointStartUV, int& rotate);

	void setNodesSize(const int& row, const int& col);
	bool setMapRow(int& row);
	int getMapRow() const;
	bool setMapColumn(int& column);
	int getMapColumn() const;
	bool addRow(int& row);
	bool addColumn(int& column);
	bool setSizeOfRow(const double& sizeOfRow);
	double getSizeOfRow();
	bool setSizeOfColumn(const double& sizeOfColumn);
	double getSizeOfColumn();
	int getColumnDf();

	void clearNodes();
	void setNodePosition(const int& row, const int& col, const AcGePoint2d& pPos);
	void editNodePosition();
	AcGePoint2d nodeAt(const int& row, const int& col) const;
	CAcGeUVGrid& transformBy(const AcGeMatrix3d& leftSide);
	std::map<int, std::list<CAcGeQuadrangle2d*>> getMapGeQuadrangle2d();

	bool initGrid(const AcGePoint2d& pOrigin,
		const double& dWidth,
		const double& dHeight);
	bool addGeQuadrangle2d(CAcGeQuadrangle2d*& geQuad);
	bool removeAll();
	bool isDrawColor(CAcGeQuadrangle2d& geQuad);
	bool addPolygon2d(const int& bol, CvGe::CvGePolygon2D& poly2d);
	bool getFixed();
	void setFixed(bool isFixed);
};
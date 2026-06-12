#include "pch.h"
#include "AcGeUVGrid.h"
#include <atlbase.h>

CAcGeUVGrid::CAcGeUVGrid()
{
	m_geCurvePath				= NULL;
	m_rotateUVMap				= 0;
	m_matixUVMap				= 0;
	m_rotateFace				= 0;
	m_matixFace					= 0;
	m_arrNode					= NULL;
	m_iRow						= 0;
	m_iCol						= 0;
	m_sizeOfRow					= 1;
	m_sizeOfColumn				= 1;
	m_isEditMap					= false;
	m_columnDefaulf				= 0;
	m_fixed						= false;
	m_sizeU						= 0;
	m_sizeV						= 0;
	m_baseDistance				= 0;
	m_numbSegU					= 0;
	m_numbSegV					= 0;
	m_dblength					= 0;
}

CAcGeUVGrid::~CAcGeUVGrid()
{
}

void CAcGeUVGrid::clear()
{
	if (!m_arrNode)
	{
		return;
	}
	for (int i = 0; i < m_iRow + 1; i++)
	{
		delete[] m_arrNode[i];
	}
	delete[] m_arrNode;
	m_arrNode = NULL;
}

void CAcGeUVGrid::setSizeOfU(const double& size)
{
	m_sizeU = size;
}

void CAcGeUVGrid::setSizeOfV(const double& size)
{
	m_sizeV = size;
}

double CAcGeUVGrid::getSizeOfU() const
{
	return m_sizeU;
}

double CAcGeUVGrid::getSizeOfV() const
{
	return m_sizeV;
}

void CAcGeUVGrid::setBaseDistance(const double& distance)
{
	m_baseDistance = distance;
}

double CAcGeUVGrid::getBaseDistance() const
{
	return m_baseDistance;
}

void  CAcGeUVGrid::setGeCurvePath(const AcGeCurve3d& geCurve)
{
	SAFE_DELETE(m_geCurvePath);
	m_geCurvePath = (AcGeCurve3d*)geCurve.copy();
}

AcGeCurve3d* CAcGeUVGrid::getGeCurvePath() const
{
	return m_geCurvePath;
}

int CAcGeUVGrid::getNumberSegOfU() const
{
	return m_numbSegU;
}

int CAcGeUVGrid::getNumberSegOfV() const
{
	return m_numbSegV;
}

bool CAcGeUVGrid::getBasePoint(AcGePoint3d& basePoint)
{
	return false;
}

bool CAcGeUVGrid::getUVInGrid(const AcGePoint2d& point, double& u, double& v)
{
	AcGePoint3d p3dGet(point.x, point.y, 0);
	for (int i = 0; i < m_iRow; i++)
	{
		for (int j = 0; j < m_iCol; j++)
		{
			AcGePoint2d p0 = nodeAt(i, j),
				p1 = nodeAt(i, j + 1),
				p2 = nodeAt(i + 1, j + 1),
				p3 = nodeAt(i + 1, j);

			CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d();
			geQuad->setPoint(p0, p1, p2, p3);
			bool bInsideQuad = geQuad->isPointInQuadrangle2d(point);
			if (!bInsideQuad)
			{
				delete geQuad;
				continue;
			}
			geQuad->getUVInQuadEx(point, u, v);
			u += j;
			v += i;
			delete geQuad;
			return true;
		}
	}

	return false;
}

bool CAcGeUVGrid::getPointAtUV(const double& u, const double& v, AcGePoint2d& point)
{
	int i = std::floor(v),
		j = std::floor(u);
	if (i < 0 || j < 0 || i >= m_iRow || j >= m_iCol)
	{
		return false;
	}
	double dU = u - j,
		dV = v - i;

	AcGePoint2d p0 = nodeAt(i, j),
		p1 = nodeAt(i, j + 1),
		p2 = nodeAt(i + 1, j + 1),
		p3 = nodeAt(i + 1, j);

	AcGePoint2d p01 = p0 + (p1 - p0) * dU,
		p32 = p3 + (p2 - p3) * dU;
	point = p01 + (p32 - p01) * dV;

	return true;
}

void CAcGeUVGrid::setPointStartUV(AcGePoint2d& gePointStartUV)
{
	m_gePointStartUV = gePointStartUV;
}

AcGePoint2d CAcGeUVGrid::getPointStartUV()
{
	return m_gePointStartUV;
}

void CAcGeUVGrid::dataMapUVGrid(const AcGePoint2d& gePointEnd, double& dblength)
{
	m_dblength = dblength;
}

void CAcGeUVGrid::setRotateUVMap(const int& rotateUVMap)
{
	m_matixUVMap = rotateUVMap - m_rotateUVMap;
	m_rotateUVMap = rotateUVMap;
}

int CAcGeUVGrid::getRotateUVMap() const
{
	return m_rotateUVMap;
}

void CAcGeUVGrid::setRotateFace(const int& rotateFace)
{
	m_matixFace = rotateFace - m_rotateFace;
	m_rotateFace = rotateFace;
}

int CAcGeUVGrid::getRotateFace() const
{
	return m_rotateFace;
}

int CAcGeUVGrid::getRotateMatrixFace() const
{
	return m_matixFace;
}

void CAcGeUVGrid::rotateMap(AcGePoint2d& gePointStartUV, int& rotate)
{
	gePointStartUV = getPointStartUV();
	rotate = m_matixUVMap;
	double angleInRadians = rotate * PI / 180.0;
	AcGeMatrix2d rotationMatrix;
	rotationMatrix.setToRotation(angleInRadians, gePointStartUV);
	m_matixUVMap = 0;

	int row = getMapRow();
	int col = getMapColumn();
	for (int i = 0; i < row + 1; i++)
	{
		for (int j = 0; j <= col; j++)
		{
			m_arrNode[i][j].transformBy(rotationMatrix);
		}
	}
}

void CAcGeUVGrid::setNodesSize(const int& row, const int& col)
{
	clearNodes();
	m_iRow = row;
	m_iCol = col;
	m_arrNode = new AcGePoint2d * [m_iRow + 1];
	for (int i = 0; i < m_iRow + 1; i++)
	{
		m_arrNode[i] = new AcGePoint2d[m_iCol + 1];
		for (int j = 0; j <= m_iCol; j++)
		{
			m_arrNode[i][j] = AcGePoint2d::kOrigin;
		}
	}
}

bool CAcGeUVGrid::addRow(int& row)
{
	if (row == 0) {
		row = 1;
	}

	//add row
	AcGePoint2d** newArrNode = NULL;
	newArrNode = new AcGePoint2d * [row + 1];
	for (int i = 0; i < row + 1; i++)
	{
		newArrNode[i] = new AcGePoint2d[m_iCol + 1];
		for (int j = 0; j <= m_iCol; j++)
		{
			AcGeVector2d direction = (m_arrNode[m_iRow][j] - m_arrNode[m_iRow - 1][j]).normal();
			if (i < m_iRow)
			{
				newArrNode[i][j] = m_arrNode[i][j];
			}
			else
			{
				newArrNode[i][j] = newArrNode[i - 1][j] + getSizeOfRow() * direction;
			}
		}
	}

	setNodesSize(row, m_iCol);
	for (int i = 0; i < m_iRow + 1; i++)
	{
		for (int j = 0; j <= m_iCol; j++)
		{
			m_arrNode[i][j] = newArrNode[i][j];

		}
	}
	editNodePosition();

	return true;
}

bool CAcGeUVGrid::addColumn(int& column)
{
	if (column == 0) {
		column = 1;
	}

	//add column
	AcGePoint2d** newArrNode = NULL;
	newArrNode = new AcGePoint2d * [m_iRow + 1];
	for (int i = 0; i < m_iRow + 1; i++)
	{
		double xMax = m_arrNode[0][m_iCol].x;
		newArrNode[i] = new AcGePoint2d[column + 1];
		for (int j = 0; j <= column; j++)
		{
			if (j <= m_iCol)
			{
				newArrNode[i][j] = m_arrNode[i][j];
			}
			else
			{
				AcGeVector2d direction = (newArrNode[i][j - 1] - newArrNode[i][j - 2]).normal();
				newArrNode[i][j] = newArrNode[i][j - 1] + getSizeOfColumn() * direction;
			}
		}
	}

	setNodesSize(m_iRow, column);
	for (int i = 0; i < m_iRow + 1; i++)
	{
		for (int j = 0; j <= m_iCol; j++)
		{
			m_arrNode[i][j] = newArrNode[i][j];
		}
	}
	editNodePosition();

	return true;
}

bool CAcGeUVGrid::setMapRow(int& row)
{
	m_iRow = row;
	return true;
}

int CAcGeUVGrid::getMapRow() const
{
	return m_iRow;
}

bool CAcGeUVGrid::setMapColumn(int& column)
{
	m_iCol = column;
	return true;
}

int CAcGeUVGrid::getMapColumn() const
{
	return m_iCol;
}

bool CAcGeUVGrid::setSizeOfRow(const double& sizeOfRow)
{
	m_sizeOfRow = sizeOfRow;
	return true;
}

double CAcGeUVGrid::getSizeOfRow()
{
	return m_sizeOfRow;
}

bool CAcGeUVGrid::setSizeOfColumn(const double& sizeOfColumn)
{
	m_sizeOfColumn = sizeOfColumn;
	return true;
}

double CAcGeUVGrid::getSizeOfColumn()
{
	return m_sizeOfColumn;
}

int CAcGeUVGrid::getColumnDf()
{
	return m_columnDefaulf;
}

void CAcGeUVGrid::clearNodes()
{
	if (!m_arrNode)
	{
		return;
	}
	for (int i = 0; i < m_iRow + 1; i++)
	{
		delete[] m_arrNode[i];
	}
	delete[] m_arrNode;
	m_arrNode = NULL;
}

void CAcGeUVGrid::setNodePosition(const int& row, const int& col, const AcGePoint2d& pPos)
{
	if (row > m_iRow || col > m_iCol)
	{
		return;
	}
	m_arrNode[row][col] = pPos;
}

void CAcGeUVGrid::editNodePosition()
{
	removeAll();
	for (int i = 0; i <= m_iRow; i++)
	{
		for (int j = 0; j <= m_iCol; j++)
		{
			AcGePoint2d iPnt = m_arrNode[i][j];
			if (i > 0 && j > 0)
			{
				CAcGeQuadrangle2d* geQuad = new CAcGeQuadrangle2d;
				geQuad->setPoint(m_arrNode[i - 1][j - 1], m_arrNode[i - 1][j], m_arrNode[i][j], m_arrNode[i][j - 1]);
				geQuad->dU = j;
				geQuad->dV = i;

				//add geQuad
				int progress = std::ceil(m_arrNode[i - 1][j - 1].x);
				std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator it = m_mapGeQuadrangle.find(progress);
				if (it != m_mapGeQuadrangle.end())
				{
					it->second.push_back(geQuad);
				}
				else
				{
					std::list<CAcGeQuadrangle2d*> listGeQuadrangle;
					listGeQuadrangle.push_back(geQuad);
					m_mapGeQuadrangle[progress] = listGeQuadrangle;
				}
			}
		}
	}

	std::map<int, std::list<CvGe::CvGePolygon2D>>::iterator itOn = m_mapPolygon.find(1);
	if (itOn == m_mapPolygon.end())
	{
		return;
	}
	std::list<CvGe::CvGePolygon2D>::iterator poly2dOn = itOn->second.begin();
	for (; poly2dOn != itOn->second.end(); poly2dOn++)
	{
		std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = m_mapGeQuadrangle.begin();
		for (; itMap != m_mapGeQuadrangle.end(); itMap++)
		{
			std::list<CAcGeQuadrangle2d*>::iterator geQua = itMap->second.begin();
			for (; geQua != itMap->second.end(); geQua++)
			{
				AcGePoint2d p = (*geQua)->getPointQuadrilateralCenter();
				bool isValid = CGeometryUtil::isPointInside(*poly2dOn, CvGe::CvGePoint2D(p.x, p.y));
				if (isValid)
				{
					(*geQua)->isValid = true;
				}
			}
		}
	}

	std::map<int, std::list<CvGe::CvGePolygon2D>>::iterator itOff = m_mapPolygon.find(2);
	if (itOff == m_mapPolygon.end())
	{
		return;
	}
	std::list<CvGe::CvGePolygon2D>::iterator poly2dOff = itOff->second.begin();
	for (; poly2dOff != itOff->second.end(); poly2dOff++)
	{
		std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = m_mapGeQuadrangle.begin();
		for (; itMap != m_mapGeQuadrangle.end(); itMap++)
		{
			std::list<CAcGeQuadrangle2d*>::iterator geQua = itMap->second.begin();
			for (; geQua != itMap->second.end(); geQua++)
			{
				AcGePoint2d p = (*geQua)->getPointQuadrilateralCenter();
				bool isValid = CGeometryUtil::isPointInside(*poly2dOff, CvGe::CvGePoint2D(p.x, p.y));
				if (isValid)
				{
					(*geQua)->isValid = false;
				}
			}
		}
	}
}

bool CAcGeUVGrid::addPolygon2d(const int& bol, CvGe::CvGePolygon2D& poly2d)
{
	std::map<int, std::list<CvGe::CvGePolygon2D>>::iterator it = m_mapPolygon.find(bol);
	if (it != m_mapPolygon.end())
	{
		it->second.push_back(poly2d);
	}
	else
	{
		std::list<CvGe::CvGePolygon2D> lstPoly2d;
		lstPoly2d.push_back(poly2d);
		m_mapPolygon[bol] = lstPoly2d;
	}
	return true;
}

AcGePoint2d CAcGeUVGrid::nodeAt(const int& row, const int& col) const
{
	return m_arrNode[row][col];
}
CAcGeUVGrid& CAcGeUVGrid::transformBy(const AcGeMatrix3d& leftSide) {
	return *this;
}

bool CAcGeUVGrid::initGrid(const AcGePoint2d& pOrigin,
	const double& dWidth,
	const double& dHeight)
{
	if (!m_arrNode)
	{
		return false;
	}
	for (int iRow = 0; iRow <= m_iRow; iRow++)
	{
		for (int iCol = 0; iCol <= m_iCol; iCol++)
		{
			AcGePoint2d ip2d = pOrigin
				+ iRow * dWidth * AcGeVector2d::kXAxis
				+ iCol * dHeight * AcGeVector2d::kYAxis;
			m_arrNode[iRow][iCol] = ip2d;
		}
	}
	m_sizeOfRow = dWidth;
	m_sizeOfColumn = dHeight;
	return true;
}

std::map<int, std::list<CAcGeQuadrangle2d*>> CAcGeUVGrid::getMapGeQuadrangle2d()
{
	return m_mapGeQuadrangle;
}

bool CAcGeUVGrid::addGeQuadrangle2d(CAcGeQuadrangle2d*& geQuad)
{
	//map
	AcGePoint2d pN0 = geQuad->getFirstPoint();
	int progress = std::ceil(pN0.x);
	std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator it = m_mapGeQuadrangle.find(progress);
	if (it != m_mapGeQuadrangle.end())
	{
		it->second.push_back(geQuad);
	}
	else
	{
		std::list<CAcGeQuadrangle2d*> listGeQuadrangle;
		listGeQuadrangle.push_back(geQuad);
		m_mapGeQuadrangle[progress] = listGeQuadrangle;
	}

	return true;
}

bool CAcGeUVGrid::removeAll()
{
	std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = m_mapGeQuadrangle.begin();
	for (; itMap != m_mapGeQuadrangle.end(); itMap++)
	{
		std::list<CAcGeQuadrangle2d*>::iterator geQua = itMap->second.begin();
		for (; geQua != itMap->second.end(); geQua++)
		{
			delete* geQua;
		}
	}
	m_mapGeQuadrangle.clear();
	return true;
}

bool CAcGeUVGrid::isDrawColor(CAcGeQuadrangle2d& geQuad)
{
	std::map<int, std::list<CAcGeQuadrangle2d*>>::iterator itMap = m_mapGeQuadrangle.begin();
	for (; itMap != m_mapGeQuadrangle.end(); itMap++)
	{
		std::list<CAcGeQuadrangle2d*>::iterator geQua = itMap->second.begin();
		for (; geQua != itMap->second.end(); geQua++)
		{
			AcGePoint2d p1 = (*geQua)->getFirstPoint();
			AcGePoint2d p2 = (*geQua)->getThirdPoint();
			AcGePoint2d pp1 = geQuad.getFirstPoint();
			AcGePoint2d pp2 = geQuad.getThirdPoint();
			if (p1 == pp1 && p2 == pp2)
			{
				return (*geQua)->isValid;
			}
		}
	}
	return false;
}

bool CAcGeUVGrid::getFixed() {
	return m_fixed;
}
void CAcGeUVGrid::setFixed(bool isFixed) {
	m_fixed = isFixed;
}
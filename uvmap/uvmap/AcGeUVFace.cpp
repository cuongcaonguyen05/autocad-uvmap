#include "pch.h"
#include "AcGeUVFace.h"

CAcGeUVFace::CAcGeUVFace()
{
}

CAcGeUVFace::~CAcGeUVFace()
{
	deleteListPointer(m_listTriangle2d);
	deleteListPointer(m_listTriangle3d);
}

CAcGeUVFace::iterator CAcGeUVFace::begin2d()
{
	return m_listTriangle2d.begin();
}

CAcGeUVFace::iterator CAcGeUVFace::end2d()
{
	return m_listTriangle2d.end();
}

CAcGeUVFace::const_iterator CAcGeUVFace::begin2d() const
{
	return m_listTriangle2d.begin();
}

CAcGeUVFace::const_iterator CAcGeUVFace::end2d() const
{
	return m_listTriangle2d.end();
}

void CAcGeUVFace::clear2d()
{
	deleteListPointer(m_listTriangle2d);
}

size_t CAcGeUVFace::size2d()
{
	return m_listTriangle2d.size();
}

void CAcGeUVFace::pushBack2d(CAcGeTriangle* geTriangle)
{
	m_listTriangle2d.push_back(geTriangle);
}

CAcGeUVFace::iterator CAcGeUVFace::begin3d()
{
	return m_listTriangle3d.begin();
}

CAcGeUVFace::iterator CAcGeUVFace::end3d()
{
	return m_listTriangle3d.end();
}

CAcGeUVFace::const_iterator CAcGeUVFace::begin3d() const
{
	return m_listTriangle3d.begin();
}

CAcGeUVFace::const_iterator CAcGeUVFace::end3d() const
{
	return m_listTriangle3d.end();
}

void CAcGeUVFace::clear3d()
{
	deleteListPointer(m_listTriangle3d);
}

size_t CAcGeUVFace::size3d()
{
	return m_listTriangle3d.size();
}

void CAcGeUVFace::pushBack3d(CAcGeTriangle* geTriangle)
{
	m_listTriangle3d.push_back(geTriangle);
}

bool CAcGeUVFace::addGeTriangle2d(const CAcGeTriangle* geTriangle)
{
	if (!geTriangle) {
		return false;
	}
	CAcGeTriangle* triangle = new CAcGeTriangle;
	triangle->copyFrom(geTriangle);
	m_listTriangle2d.push_back(triangle);
	return true;
}

bool CAcGeUVFace::addGeTriangle3d(const CAcGeTriangle* geTriangle)
{
	if (!geTriangle) {
		return false;
	}
	CAcGeTriangle* triangle = new CAcGeTriangle;
	triangle->copyFrom(geTriangle);
	m_listTriangle3d.push_back(triangle);
	return true;
}

void CAcGeUVFace::cloneFrom(const CAcGeUVFace* geTriangle)
{
	if (!geTriangle) {
		return;
	}

	deleteListPointer(m_listTriangle2d);
	deleteListPointer(m_listTriangle3d);
	for (CAcGeUVFace::const_iterator it = geTriangle->begin2d(); it != geTriangle->end2d(); it++) {
		this->addGeTriangle2d(*it);
	}

	for (CAcGeUVFace::const_iterator it = geTriangle->begin3d(); it != geTriangle->end3d(); it++) {
		this->addGeTriangle3d(*it);
	}
}

void CAcGeUVFace::clear()
{
	deleteListPointer(m_listTriangle2d);
	deleteListPointer(m_listTriangle3d);
}

void CAcGeUVFace::getBoundingBox2d(AcGePoint3d& A, AcGePoint3d& B, AcGePoint3d& C, AcGePoint3d& D) const
{
	double minX, minY, maxX, maxY;
	bool isFirst = true;
	for (CAcGeUVFace::const_iterator it = m_listTriangle2d.begin(); it != m_listTriangle2d.end(); it++) {
		CAcGeTriangle* geTriangle = *it;
		if (isFirst) {
			AcGePoint3d first = geTriangle->getFirstPoint();
			AcGePoint3d second = geTriangle->getSecondPoint();
			AcGePoint3d third = geTriangle->getThirdPoint();
			minX = maxX = first.x;
			minY = maxY = first.y;

			minX = (minX < second.x) ? minX : second.x;
			minY = (minY < second.y) ? minY : second.y;
			minX = (minX < third.x) ? minX : third.x;
			minY = (minY < third.y) ? minY : third.y;

			maxX = (maxX > second.x) ? maxX : second.x;
			maxY = (maxY > second.y) ? maxY : second.y;
			maxX = (maxX > third.x) ? maxX : third.x;
			maxY = (maxY > third.y) ? maxY : third.y;
			isFirst = false;
		}
		else {
			AcGePoint3d first = geTriangle->getFirstPoint();
			AcGePoint3d second = geTriangle->getSecondPoint();
			AcGePoint3d third = geTriangle->getThirdPoint();

			minX = (minX < first.x) ? minX : first.x;
			minY = (minY < first.y) ? minY : first.y;
			minX = (minX < second.x) ? minX : second.x;
			minY = (minY < second.y) ? minY : second.y;
			minX = (minX < third.x) ? minX : third.x;
			minY = (minY < third.y) ? minY : third.y;

			maxX = (maxX > first.x) ? maxX : first.x;
			maxY = (maxY > first.y) ? maxY : first.y;
			maxX = (maxX > second.x) ? maxX : second.x;
			maxY = (maxY > second.y) ? maxY : second.y;
			maxX = (maxX > third.x) ? maxX : third.x;
			maxY = (maxY > third.y) ? maxY : third.y;
		}
	}

	A = AcGePoint3d(minX, minY, 0);
	B = AcGePoint3d(maxX, minY, 0);
	C = AcGePoint3d(maxX, maxY, 0);
	D = AcGePoint3d(minX, maxY, 0);
}

double CAcGeUVFace::getMaxSize() const
{
	AcGePoint3d A, B, C, D;
	this->getBoundingBox2d(A, B, C, D);

	double x = B.x - A.x;
	double y = C.y - A.y;

	return (x > y) ? x : y;
}

CAcGeUVFace& CAcGeUVFace::transformBy(const AcGeMatrix3d& leftSide) {
	return *this;
}

void CAcGeUVFace::rotateFace(AcGePoint2d& gePointStartUV, int& rotate)
{
	double angleInRadians = rotate * PI / 180.0;
	AcGeMatrix2d rotationMatrix;
	rotationMatrix.setToRotation(angleInRadians, gePointStartUV);

	std::list<CAcGeTriangle*>::iterator it2d = begin2d();
	for (; it2d != end2d(); it2d++)
	{
		AcGePoint3d p1, p2, p3;
		p1 = (*it2d)->getFirstPoint();
		AcGePoint2d p1Tran = AcGePoint2d(p1.x, p1.y).transformBy(rotationMatrix);
		(*it2d)->setFirstPoint(AcGePoint3d(p1Tran.x, p1Tran.y, 0));

		p2 = (*it2d)->getSecondPoint();
		AcGePoint2d p2Tran = AcGePoint2d(p2.x, p2.y).transformBy(rotationMatrix);
		(*it2d)->setSecondPoint(AcGePoint3d(p2Tran.x, p2Tran.y, 0));

		p3 = (*it2d)->getThirdPoint();
		AcGePoint2d p3Tran = AcGePoint2d(p3.x, p3.y).transformBy(rotationMatrix);
		(*it2d)->setThirdPoint(AcGePoint3d(p3Tran.x, p3Tran.y, 0));
	}
	return;
}
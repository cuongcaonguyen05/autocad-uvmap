#pragma once
#include "AcGeTriangle.h"

class CAcGeUVFace
{
	std::list<CAcGeTriangle*> m_listTriangle3d;
	std::list<CAcGeTriangle*> m_listTriangle2d;

public:
	CAcGeUVFace();
	~CAcGeUVFace();
	
	typedef std::list<CAcGeTriangle*>::iterator iterator;
	typedef std::list<CAcGeTriangle*>::const_iterator const_iterator;

	iterator begin2d();
	iterator end2d();
	const_iterator begin2d() const;
	const_iterator end2d() const;

	void clear2d();
	size_t size2d();
	void pushBack2d(CAcGeTriangle* geTriangle);

	iterator begin3d();
	iterator end3d();
	const_iterator begin3d() const;
	const_iterator end3d() const;

	void clear3d();
	size_t size3d();
	void pushBack3d(CAcGeTriangle* geTriangle);

	void clear();
	bool addGeTriangle2d(const CAcGeTriangle* geTriangle);
	bool addGeTriangle3d(const CAcGeTriangle* geTriangle);
	void cloneFrom(const CAcGeUVFace* geTriangle);
	void getBoundingBox2d(AcGePoint3d& A, AcGePoint3d& B, AcGePoint3d& C, AcGePoint3d& D) const;
	double getMaxSize() const;

	CAcGeUVFace& transformBy(const AcGeMatrix3d& leftSide);
	void rotateFace(AcGePoint2d& gePointStartUV, int& rotate);
};
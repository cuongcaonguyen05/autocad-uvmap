#include "pch.h"
#include "CvGePolygon2D.h"

#include <math.h>

namespace CvGe
{
	// ---------------------------------------------------------------- CvGePoint2D
	CvGePoint2D::CvGePoint2D()
	{
		m_dX = 0;
		m_dY = 0;
		bStatus = false;
	}

	CvGePoint2D::CvGePoint2D(const CvGePoint2D& other)
	{
		m_dX = other.m_dX;
		m_dY = other.m_dY;
		bStatus = false;
	}

	CvGePoint2D::CvGePoint2D(double dX, double dY)
	{
		m_dX = dX;
		m_dY = dY;
		bStatus = false;
	}

	CvGePoint2D::~CvGePoint2D()
	{
	}

	void CvGePoint2D::move(double dX, double dY)
	{
		m_dX += dX;
		m_dY += dY;
	}

	double CvGePoint2D::distanceTo(const CvGePoint2D& other) const
	{
		return sqrt(pow(m_dX - other.m_dX, 2) + pow(m_dY - other.m_dY, 2));
	}

	bool CvGePoint2D::isCloseTo(const CvGePoint2D& other) const
	{
		return ((fabs(m_dX - other.m_dX) < 0.000001) && (fabs(m_dY - other.m_dY) < 0.000001));
	}

	bool CvGePoint2D::isCloseTo2(const CvGePoint2D& other) const
	{
		return ((fabs(m_dX - other.m_dX) < 0.000000001) && (fabs(m_dY - other.m_dY) < 0.000000001));
	}

	void CvGePoint2D::format(int digit)
	{
		format2(digit, m_dX);
		format2(digit, m_dY);
	}

	void CvGePoint2D::format2(int digit, double& db)
	{
		int temp = 1;
		for (int i = 0; i < digit; ++i)
			temp *= 10;
		int temp1 = (int)db;
		db -= temp1;
		db *= temp;
		if (db > 0)
			db += 0.5;
		else
			db -= 0.5;
		int j = (int)db;
		db /= temp;
		db = temp1 + (double)j / temp;
	}

	CvGePoint2D& CvGePoint2D::operator=(const CvGePoint2D& other)
	{
		m_dX = other.m_dX;
		m_dY = other.m_dY;
		return *this;
	}

	bool CvGePoint2D::operator==(const CvGePoint2D& other) const
	{
		if (m_dX != other.m_dX)
			return false;
		if (m_dY != other.m_dY)
			return false;
		return true;
	}

	// ------------------------------------------------------------- CvGePolyline2D
	CvGePolyline2D::CvGePolyline2D()
	{
	}

	CvGePolyline2D::~CvGePolyline2D()
	{
	}

	void CvGePolyline2D::addVertex(const CvGePoint2D& point)
	{
		m_Vertice.push_back(point);
	}

	void CvGePolyline2D::addVertexReverse(const CvGePoint2D& point)
	{
		m_Vertice.push_front(point);
	}

	void CvGePolyline2D::insert(iterator& it, const CvGePoint2D& point)
	{
		m_Vertice.insert(it, point);
	}

	void CvGePolyline2D::assign(iterator begin, iterator end)
	{
		m_Vertice.assign(begin, end);
	}

	void CvGePolyline2D::reverse()
	{
		m_Vertice.reverse();
	}

	void CvGePolyline2D::format()
	{
		iterator it1, it2;
		it1 = begin();
		if (it1 == end())
			return;
		it2 = it1;
		++it2;
		for (; it2 != end();)
		{
			if (it1->isCloseTo2(*it2))
			{
				it2 = erase(it2);
				continue;
			}
			++it1;
			++it2;
		}
	}

	void CvGePolyline2D::pop_back()
	{
		m_Vertice.pop_back();
	}

	void CvGePolyline2D::pop_front()
	{
		m_Vertice.pop_front();
	}

	void CvGePolyline2D::clear()
	{
		m_Vertice.clear();
	}

	CvGePolyline2D::iterator CvGePolyline2D::erase(iterator it)
	{
		return m_Vertice.erase(it);
	}

	size_t CvGePolyline2D::side() const
	{
		return m_Vertice.size();
	}

	size_t CvGePolyline2D::size() const
	{
		return m_Vertice.size();
	}

	CvGePolyline2D::iterator CvGePolyline2D::begin()
	{
		return m_Vertice.begin();
	}

	CvGePolyline2D::iterator CvGePolyline2D::end()
	{
		return m_Vertice.end();
	}

	CvGePolyline2D::const_iterator CvGePolyline2D::begin() const
	{
		return m_Vertice.begin();
	}

	CvGePolyline2D::const_iterator CvGePolyline2D::end() const
	{
		return m_Vertice.end();
	}

	CvGePolyline2D::reverse_iterator CvGePolyline2D::rbegin()
	{
		return m_Vertice.rbegin();
	}

	CvGePolyline2D::reverse_iterator CvGePolyline2D::rend()
	{
		return m_Vertice.rend();
	}

	CvGePolyline2D::const_reverse_iterator CvGePolyline2D::rbegin() const
	{
		return m_Vertice.rbegin();
	}

	CvGePolyline2D::const_reverse_iterator CvGePolyline2D::rend() const
	{
		return m_Vertice.rend();
	}

	CvGePoint2D CvGePolyline2D::back()
	{
		return m_Vertice.back();
	}

	CvGePoint2D CvGePolyline2D::front()
	{
		return m_Vertice.front();
	}

	// -------------------------------------------------------------- CvGePolygon2D
	CvGePolygon2D::CvGePolygon2D()
	{
	}

	CvGePolygon2D::~CvGePolygon2D()
	{
	}
}

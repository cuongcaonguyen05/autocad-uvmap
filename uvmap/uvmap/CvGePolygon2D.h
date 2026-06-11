#pragma once

#include <list>

#ifndef CVGE_EXPORT_API
#define CVGE_EXPORT_API
#endif

namespace CvGe
{
	class CVGE_EXPORT_API CvGePoint2D
	{
	public:
		CvGePoint2D();
		CvGePoint2D(const CvGePoint2D& other);
		CvGePoint2D(double dX, double dY);
		~CvGePoint2D();

		double distanceTo(const CvGePoint2D& other) const;
		bool   isCloseTo(const CvGePoint2D& other) const;
		bool   isCloseTo2(const CvGePoint2D& other) const;
		void   format(int digit);
		void   move(double dX, double dY);

		CvGePoint2D& operator=(const CvGePoint2D& other);
		bool operator==(const CvGePoint2D& other) const;

	private:
		void format2(int digit, double& db);

	public:
		double m_dX;
		double m_dY;
		bool   bStatus;
	};

	class CVGE_EXPORT_API CvGePolyline2D
	{
	public:
		typedef std::list<CvGePoint2D>::iterator               iterator;
		typedef std::list<CvGePoint2D>::const_iterator         const_iterator;
		typedef std::list<CvGePoint2D>::reverse_iterator       reverse_iterator;
		typedef std::list<CvGePoint2D>::const_reverse_iterator const_reverse_iterator;

		CvGePolyline2D();
		virtual ~CvGePolyline2D();

		void addVertex(const CvGePoint2D& point);
		void addVertexReverse(const CvGePoint2D& point);
		void insert(iterator& it, const CvGePoint2D& point);
		void assign(iterator begin, iterator end);
		void reverse();
		void format();
		void pop_back();
		void pop_front();
		void clear();

		iterator erase(iterator it);
		size_t   side() const;
		size_t   size() const;

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		reverse_iterator rbegin();
		reverse_iterator rend();
		const_reverse_iterator rbegin() const;
		const_reverse_iterator rend() const;

		CvGePoint2D back();
		CvGePoint2D front();

	private:
		std::list<CvGePoint2D> m_Vertice;
	};

	class CVGE_EXPORT_API CvGePolygon2D : public CvGePolyline2D
	{
	public:
		CvGePolygon2D();
		~CvGePolygon2D();
	};
}
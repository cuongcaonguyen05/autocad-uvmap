#pragma once

#include "GeoJson.h"
#include "AcGeUVGrid.h"
#include "AcGeQuadrangle2d.h"
#include "gepnt2d.h"
#include "gepnt3d.h"

namespace vizDatabase
{
	class CReadWriteJson
	{
		std::map<unsigned long, std::list<CGeoJson*>> m_listEntities;
		std::map<unsigned long, CGeoJson*> m_blockTableRecords;
		double m_red, m_green, m_blue;
		std::list<AcGePoint3d> m_lstGeo;
		std::list<AcGePoint2d> m_lstTex;
		int numItem;
		int m_currentObjectID = 0;

		bool geoTran(std::list<CGeoJson*>& listGeo, const AcGeVector2d& vecTran);
		bool mapQuadrangleTran(std::list<CAcGeUVGrid*>& lstGeUVGrip, std::list<CAcGeUVGrid*>& lstGeUVGripTran, AcGeVector2d& vecTran);
		bool getTexCoordsWithGrip(CGeoJson*& geo, std::list<CAcGeUVGrid*>& lstGeUVGrip, const double& width, const double& height);
		bool getTexCoordsWithMap(CGeoJson*& geo, std::map<int, std::list<CAcGeQuadrangle2d*>>& mapGeQuadrangle, const double& width, const double& height);

	public:
		static CReadWriteJson* create();
		CReadWriteJson();
		~CReadWriteJson();

		void setColor(const double& r, const double& g, const double& b);
		void getColor(double& r, double& g, double& b);

		bool append(CGeoJson* geo);
		bool surfaceUVMap(std::list<CAcGeUVGrid*>& lstGeUVGrip, const double& width, const double& height);
	};
}
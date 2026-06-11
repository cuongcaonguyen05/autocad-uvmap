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
		void meshWithCoord(std::list<CGeoJson*>& lstGeo, const double& dWidth, const double& dHeight);
		void meshNoCoord(std::list<CGeoJson*>& lstGeo, const double& dWidth, const double& dHeight, std::list<AcGePoint3d>& lstP3d, std::list<AcGePoint2d>& lstP2d);
		void meshUnwrap(const std::vector<float>& vecPosition,
			const std::vector<float>& lstTextcoord,
			const std::vector<unsigned int>& lstIndice,
			std::list<AcGePoint3d>& inputPoint3d,
			std::list<AcGePoint3d>& inputPoint2d);
		void meshGetScale(const std::list<AcGePoint3d>& inputPoint3d,
			const std::list<AcGePoint3d>& inputPoint2d,
			double& dScale);
		void meshDataToGeoJson(std::list<AcGePoint3d> lstP3d,
			std::list<AcGePoint2d> lstP2d,
			const double dWidth, const double dHeight);
	public:
		static CReadWriteJson* create();
		CReadWriteJson();
		~CReadWriteJson();

		void setColor(const double& r, const double& g, const double& b);
		void getColor(double& r, double& g, double& b);

		bool append(CGeoJson* geo);
		bool surfaceUVMap(std::list<CAcGeUVGrid*>& lstGeUVGrip, const double& width, const double& height);
		bool meshUVMap(const double& width, const double& height);
	};
}
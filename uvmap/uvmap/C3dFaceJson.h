#pragma once
#include "GeoJson.h"

namespace vizDatabase
{
	class C3dFaceJson : public CGeoJson
	{
	protected:
		POINT3D m_normal;
		POINT3D m_direction;
		POINT3D m_origin;
		static long m_version;

		unsigned long m_vpvItemId;
		virtual unsigned long getBinariesSize();
		C3dFaceJson(void);
	public:

		~C3dFaceJson(void);
		static C3dFaceJson* create();

		virtual bool buildJson(std::wstring& szOut);
		virtual bool buildBinaries(unsigned char* data, unsigned long& len);
		virtual bool readBinaries(unsigned char* data, unsigned long& len);
		virtual bool readJson(const std::string& szJson);
		virtual unsigned long getVPVItemId();

		void setOrigin(double x, double y, double z);
		void setDirection(double x, double y, double z);
		void setNormal(double x, double y, double z);
		static unsigned long desc();
	};
}
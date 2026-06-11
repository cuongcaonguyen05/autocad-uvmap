#pragma once
#include <gemat3d.h>

#ifndef JSON_STRING_MAX_LENGTH
#define JSON_STRING_MAX_LENGTH 2048
#endif

namespace vizDatabase
{
	namespace DbType
	{
		enum {
			DB_NONE = 0,
			DB_ARC_TYPE = 1,
			DB_POINT_TYPE = 2,
			DB_LINE2D_TYPE = 3,
			DB_POLYLINE_TYPE = 4,
			DB_POLYGON_TYPE = 5,
			DB_TEXT_TYPE = 6,
			DB_BREAKLINE2D_TYPE = 7,
			DB_BREAKLINE3D_TYPE = 8,
			DB_HOLE2D_TYPE = 9,
			DB_HOLE3D_TYPE = 10,
			DB_POLYLINE2D_TYPE = 11,
			DB_POLYLINE3D_TYPE = 12,
			DB_STATION_TYPE = 13,
			DB_STATION_POINT_TYPE = 14,
			DB_CIRCLE_TYPE = 15,
			DB_POINT2D_TYPE = 16,
			DB_POINT3D_TYPE = 17,
			DB_DTM_MODEL_TYPE = 18,
			DB_DTM_BOUND_TYPE = 19,
			DB_LINE3D_TYPE = 20,
			DB_CONTOUR_TYPE = 22,
			DB_ALIGNMENT_TYPE = 23,
			DB_VERTEX_POINT = 24,
			DB_VERTEX_ARC = 25,
			DB_VERTEX_SPIRAL = 26,
			DB_POLE = 27,
			DB_PROFILE = 28,
			DB_RED_LINE = 29,
			DB_RED_PARABOLIC = 30,
			DB_SECTION = 31,
			DB_ALIGNMENT_FOUNDATION = 32,
			DB_ALIGNMENT_FOUNDATION_ITEM = 33,
			DB_POLE_STATION_ITEM = 34,
			DB_BLOCK = 35,
			DB_BLOCK_INSTANCE = 36,
			DB_FEATURE_CODE = 37,
			DB_WALL = 38,
			DB_LONGTITUDINAL_CULVER = 39,
			DB_ROADBED = 40,
			DB_POLE_SECTION = 41,
			DB_ROW_INFO = 42,
			DB_SECTION_TABLE = 43,
			DB_PROJECT = 44,
			DB_LAYER = 45,
			DB_FILTER_LAYER = 46,
			DB_PROJECT_INFO = 47,
			DB_DRAWING = 48,
			DB_PROJECT_HEADING = 49,
			DB_ALIGNMENT = 50,
			DB_FEATURECODE = 51,
			DB_TABLE = 52,
			DB_ADS_VIEWPORT = 53,
			DB_MATERIAL = 54,
			DB_SURFACE = 55,
			DB_MATERIAL_AREA = 56,
			DB_FOREST = 57,
			DB_FENCES = 58,
			DB_TREE = 59,
			DB_MOVEMENT_PATH = 60,
			DB_GROUP = 61,
			DB_ROAD_MARKING = 62,
			DB_MESH = 63,
			DB_TARGET_SURFACE = 64,
			DB_3D_FACE = 65,
			DB_BUILDING = 66,
			DB_TREEGROUP = 67,
			DB_TARGET_ENTITY = 68,
			DB_ROADSIGN = 69,
			DB_SOLID_ALONGPATH = 70,
			DB_REGION_AREA = 71,
			DB_ROADLINE = 72,
			DB_LIGHTING = 73,
			DB_SHADOW = 74,
			DB_IFCITEM = 75,
			DB_POINTCLOUD = 76,
			DB_UVMAP = 78,
			DB_BLOCKREF = 79,
			DB_BLOCK3D = 80,
			DB_VPVPROJECT = 81,
			DB_INTS = 82,
			DB_RINTS = 83,
			// Instancing: bảng instance của 1 prototype solid (giảm ~71% geometry
			// cho CAD copy-paste). Record lưu prototypeId + list ma trận rigid.
			DB_INSTANCE_TABLE = 84
		};
	}
}

namespace vizDatabase
{
	typedef struct __point3d {
		double x;
		double y;
		double z;
	} POINT3D, RGB;

	typedef struct __point2d {
		double x;
		double y;

	} POINT2D;

	typedef struct __binaries {
		unsigned char* data;
		unsigned long len;
	} BINARIES;

	class CGeoJson
	{
	protected:
		unsigned long m_lType;
		std::list<POINT3D*> m_vertex;
		std::list<POINT2D*> m_vertex2d;
		std::list<POINT2D*> m_texCoords;
		unsigned long m_lId;

		int m_r, m_g, m_b;
		unsigned long m_lLayer;
		std::wstring m_wLayer;

		bool m_visible;
		char m_szActiveProject[JSON_STRING_MAX_LENGTH];

		std::list<POINT3D> m_listVertext;
		unsigned long m_alignmentPos;
		double m_rgbColor_r, m_rgbColor_g, m_rgbColor_b;

		unsigned long m_roadLineModelItemId;
		unsigned long m_roadSignId;
		unsigned long m_privateId;

		AcGeMatrix3d m_matrixRoadSign;

		//must not create a new single instance
		CGeoJson(void);
	public:
		typedef std::list<POINT3D*>::iterator iterator;
		typedef std::list<POINT2D*>::iterator texCoordIterator;
		iterator begin();
		iterator end();
		int vertexNumber();

		typedef std::list<POINT2D*>::iterator iterator2d;
		iterator2d begin2d();
		iterator2d end2d();

		virtual ~CGeoJson(void);

		static CGeoJson* create(unsigned long type);
		unsigned long type();
		void SetType(unsigned long type);
		void setRGBColor(double r, double g, double b);
		void getRGBColor(double& r, double& g, double& b);
		virtual bool getChild(std::list<CGeoJson*>& childs);

		bool setLayer(const unsigned long lLayer);
		unsigned long getLayer();
		bool setWLayer(const std::wstring wLayer);
		std::wstring getWLayer();
		bool setActiveProject(const char* szActiveProject);
		bool setActiveProjectW(const TCHAR* szActiveProject);

		unsigned long objectId();
		bool getObjectId(unsigned long& szId);
		bool setItemId(const unsigned long lId);
		void setVisible(bool visible);
		bool visible();

		virtual bool setGeoInfo(const POINT3D& point);
		virtual bool setGeo2dInfo(const POINT2D& point);
		virtual bool setGeoInfo(const double& x, const double& y, const double& z);
		virtual bool setGeoInfo2d(const double& x, const double& y);
		virtual bool getGeoInfo(POINT3D*& p1, POINT3D*& p2, POINT3D*& p3, POINT3D*& p4);
		virtual bool getGeo2dInfo(POINT2D*& p1, POINT2D*& p2, POINT2D*& p3, POINT2D*& p4);

		virtual bool setTexCoords(const POINT2D& point);
		virtual bool setTexCoords(const double& x, const double& y);
		virtual bool getTexCoords(POINT2D*& p1, POINT2D*& p2, POINT2D*& p3, POINT2D*& p4);

		bool setGeoInfoAt(const int& index, const POINT3D& point);
		bool setTexCoordAt(const int& index, const POINT2D& point);
		bool getVertex(POINT3D*& p1, POINT3D*& p2, POINT3D*& p3, POINT3D*& p4);
		bool clearTexCoord();
		bool clearGeoInfo();

		virtual bool buildJson(std::wstring& szOut);
		virtual bool readJson(const std::wstring& szJson);
		virtual bool readVMIni(const std::string& szIni);

		virtual bool buildBinaries(unsigned char* data, unsigned long& len);
		virtual bool readBinaries(unsigned char* data, unsigned long& len);

		virtual bool getMin(double& x, double& y, double& z);
		virtual bool getMax(double& x, double& y, double& z);

		virtual void setImageFile(const std::string& strImage);
		virtual unsigned long getBinariesSize();

		bool isKindOf(DWORD type);

		virtual bool getListVertext(std::list<POINT3D>& listVertext);

		void setAlignmentPos(unsigned long aliPos);
		unsigned long getAlignmentPos();

		virtual bool getMatrixTransform(double transform[4][4]);
		virtual double rotation();

		virtual unsigned long getRoadLineModelItemId();
		virtual void setRoadLineModelItemId(unsigned long value);


		void setColor(double r, double g, double b);
		void getColor(double& r, double& g, double& b);

		void setIdRoadSign(unsigned long id);
		unsigned long getIdRoadSign();

		bool setMatrixRoadSign(const AcGeMatrix3d& matrix);
		AcGeMatrix3d getMatrixRoadSign();

		bool setPrivateId(const unsigned long& privateId);
		unsigned long getPrivateId();
	};
}
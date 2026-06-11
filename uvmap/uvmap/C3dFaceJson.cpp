// C3dFaceJson.cpp : Hiện thực C3dFaceJson.
#include "pch.h"
#include "C3dFaceJson.h"
#include <locale>
#include <codecvt>
#include <iomanip>
#include <sstream>

using namespace vizDatabase;
long C3dFaceJson::m_version = 9;

struct POINT3DComparator {
	static constexpr double EPSILONEX = 1e-9;
	bool operator()(const POINT3D& a, const POINT3D& b) const {
		if (fabs(a.x - b.x) > EPSILONEX) return a.x < b.x;
		if (fabs(a.y - b.y) > EPSILONEX) return a.y < b.y;
		if (fabs(a.z - b.z) > EPSILONEX) return a.z < b.z;
		return false;
	}
};

C3dFaceJson::C3dFaceJson(void)
{
	m_normal.x = 0; m_normal.y = 0; m_normal.z = 1;
	m_direction.x = 1; m_direction.y = 0; m_direction.z = 0;
	m_lType = DbType::DB_3D_FACE;

	m_origin.x = 0; m_origin.y = 0; m_origin.z = -1000000;
	m_r = 0;
	m_g = 0;
	m_b = 0;
}

C3dFaceJson::~C3dFaceJson(void)
{

}

void C3dFaceJson::setNormal(double x, double y, double z) {
	m_normal.x = x; m_normal.y = y; m_normal.z = z;
}

void C3dFaceJson::setDirection(double x, double y, double z) {
	m_direction.x = x; m_direction.y = y; m_direction.z = z;
}

void C3dFaceJson::setOrigin(double x, double y, double z) {
	m_origin.x = x; m_origin.y = y; m_origin.z = z;
}

bool C3dFaceJson::buildJson(std::wstring& szOut) {
	
	return true;
}

bool C3dFaceJson::buildBinaries(unsigned char* data, unsigned long& len)
{
	//len = size;

	//data = new unsigned char[size];
	int size = 0;
	unsigned char* d = data;
	memcpy(d, &m_version, sizeof(long));
	d += sizeof(long);
	size += sizeof(long);

	unsigned long textSize = m_lLayer;

	memcpy(d, &textSize, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	unsigned long ul_temp = m_vertex.size();
	memcpy(d, &ul_temp, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	std::list<POINT3D*>::iterator it = m_vertex.begin();
	for (; it != m_vertex.end(); it++)
	{
		double d_temp = (*it)->x;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (*it)->y;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (*it)->z;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);
	}
	//for version 6
	/////////////////////////////
	memcpy(d, &m_roadLineModelItemId, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);
	/////////////////////////////

	//For versin 4
	////////////////////////
	ul_temp = m_vertex2d.size();
	memcpy(d, &ul_temp, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	std::list<POINT2D*>::iterator it2d = m_vertex2d.begin();
	for (; it2d != m_vertex2d.end(); it2d++)
	{
		double d_temp = (*it2d)->x;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (*it2d)->y;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);
	}

	////////////////////////
	//for version 3
	ul_temp = m_texCoords.size();
	memcpy(d, &ul_temp, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	std::list<POINT2D*>::iterator it2 = m_texCoords.begin();
	for (; it2 != m_texCoords.end(); it2++)
	{
		double d_temp = (*it2)->x;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (*it2)->y;
		memcpy(d, &d_temp, sizeof(double));
		d += sizeof(double);
		size += sizeof(double);

	}
	//

	memcpy(d, &m_r, sizeof(int));
	d += sizeof(int);
	size += sizeof(int);

	memcpy(d, &m_g, sizeof(int));
	d += sizeof(int);
	size += sizeof(int);

	memcpy(d, &m_b, sizeof(int));
	d += sizeof(int);
	size += sizeof(int);

	double d_temp = m_normal.x;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_normal.y;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_normal.z;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_direction.x;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_direction.y;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_direction.z;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	//oringni
	d_temp = m_origin.x;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_origin.y;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	d_temp = m_origin.z;
	memcpy(d, &d_temp, sizeof(double));
	d += sizeof(double);
	size += sizeof(double);

	unsigned long aligmentPos = m_alignmentPos;
	memcpy(d, &aligmentPos, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	unsigned long roadSignId = m_roadSignId;
	memcpy(d, &roadSignId, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			double d_temp = m_matrixRoadSign(row, col);
			memcpy(d, &d_temp, sizeof(double));
			d += sizeof(double);
			size += sizeof(double);
		}
	}

	unsigned long privateId = m_privateId;
	memcpy(d, &privateId, sizeof(unsigned long));
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	return true;
}

bool C3dFaceJson::readBinaries(unsigned char* data, unsigned long& len) {
	int size = 0;
	unsigned long* textSize = NULL;
	unsigned char* d = data;
	m_vertex.clear();

	long version = *(long*)d;
	d += sizeof(long);
	size += sizeof(long);

	textSize = (unsigned long*)d;
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	this->setLayer(*textSize);

	unsigned long* ul_temp = (unsigned long*)d;
	d += sizeof(unsigned long);
	size += sizeof(unsigned long);

	for (unsigned long i = 0; i < *ul_temp; i++)
	{
		POINT3D* p1 = new POINT3D();
		double* d_temp = NULL;

		d_temp = (double*)d;
		p1->x = *d_temp;
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (double*)d;
		p1->y = *d_temp;
		d += sizeof(double);
		size += sizeof(double);

		d_temp = (double*)d;
		p1->z = *d_temp;
		d += sizeof(double);
		size += sizeof(double);

		m_vertex.push_back(p1);
	}
	//for version 4
	////////////////////////////////////////////////
	if (version > 5) {
		m_roadLineModelItemId = *(unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);
	}
	////////////////////////////////////////////////


	//for version 4
	////////////////////////////////////////////////
	if (version > 3) {
		ul_temp = (unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);

		for (unsigned long i = 0; i < *ul_temp; i++)
		{
			POINT2D* p1 = new POINT2D();
			double* d_temp = NULL;

			d_temp = (double*)d;
			p1->x = *d_temp;
			d += sizeof(double);
			size += sizeof(double);

			d_temp = (double*)d;
			p1->y = *d_temp;
			d += sizeof(double);
			size += sizeof(double);


			m_vertex2d.push_back(p1);
		}
	}
	////////////////////////////////////////////////
	if (version > 2) {
		ul_temp = (unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);

		for (unsigned long i = 0; i < *ul_temp; i++)
		{
			POINT2D* p1 = new POINT2D();
			double* d_temp = NULL;

			d_temp = (double*)d;
			p1->x = *d_temp;
			d += sizeof(double);
			size += sizeof(double);

			d_temp = (double*)d;
			p1->y = *d_temp;
			d += sizeof(double);
			size += sizeof(double);

			m_texCoords.push_back(p1);
		}
	}

	double* alignmentPosInDouble = (double*)d;
	unsigned long ulongValue = (unsigned long)(*alignmentPosInDouble);
	if ((*alignmentPosInDouble) - ulongValue >= 0.5 && (*alignmentPosInDouble) - ulongValue < 1) {
		d += sizeof(double);
		size += sizeof(double);
	}

	int* i_temp = NULL;
	i_temp = (int*)d;
	d += sizeof(int);
	size += sizeof(int);

	m_r = *i_temp;

	i_temp = NULL;
	i_temp = (int*)d;
	d += sizeof(int);
	size += sizeof(int);

	m_g = *i_temp;

	i_temp = NULL;
	i_temp = (int*)d;
	d += sizeof(int);
	size += sizeof(int);

	m_b = *i_temp;


	//normal
	double* d_temp = NULL;

	d_temp = (double*)d;
	m_normal.x = *d_temp;
	d += sizeof(double);
	size += sizeof(double);

	d_temp = (double*)d;
	m_normal.y = *d_temp;
	d += sizeof(double);
	size += sizeof(double);


	d_temp = (double*)d;
	m_normal.z = *d_temp;
	d += sizeof(double);
	size += sizeof(double);

	//direction
	d_temp = NULL;

	d_temp = (double*)d;
	m_direction.x = *d_temp;
	d += sizeof(double);
	size += sizeof(double);


	d_temp = (double*)d;
	m_direction.y = *d_temp;
	d += sizeof(double);
	size += sizeof(double);


	d_temp = (double*)d;
	m_direction.z = *d_temp;
	d += sizeof(double);
	size += sizeof(double);


	//origin
	d_temp = NULL;
	if (version >= 2) {
		d_temp = (double*)d;
		m_origin.x = *d_temp;
		d += sizeof(double);
		size += sizeof(double);


		d_temp = (double*)d;
		m_origin.y = *d_temp;
		d += sizeof(double);
		size += sizeof(double);


		d_temp = (double*)d;
		m_origin.z = *d_temp;
		d += sizeof(double);
		size += sizeof(double);


	}

	if (version > 4)
	{
		unsigned long* aligmentPos = NULL;
		aligmentPos = (unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);
		this->setAlignmentPos(*aligmentPos);
	}

	if (version > 6)
	{
		unsigned long* roadSignId = NULL;
		roadSignId = (unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);
		this->setIdRoadSign(*roadSignId);
	}

	if (version > 7)
	{
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				double* d_temp = (double*)d;
				m_matrixRoadSign.entry[row][col] = *d_temp;
				d += sizeof(double);
				size += sizeof(double);
			}
		}
	}

	if (version > 8)
	{
		unsigned long* privateId = NULL;
		privateId = (unsigned long*)d;
		d += sizeof(unsigned long);
		size += sizeof(unsigned long);
		this->setPrivateId(*privateId);
	}

	len = size;
	return true;
}

unsigned long C3dFaceJson::getBinariesSize() {

	int size = 0;
	size += sizeof(long);

	size += sizeof(unsigned long);

	size += sizeof(unsigned long);
	size += 3 * sizeof(double) * m_vertex.size();

	size += sizeof(unsigned long);
	size += 2 * sizeof(double) * m_vertex2d.size();

	size += sizeof(unsigned long);
	size += 2 * sizeof(double) * m_texCoords.size();



	size += 3 * sizeof(int);


	size += 3 * sizeof(double);

	size += 3 * sizeof(double);


	size += 3 * sizeof(double);

	size += sizeof(unsigned long);
	size += sizeof(unsigned long);
	size += sizeof(unsigned long);

	size += 16 * sizeof(double);

	size += sizeof(unsigned long);
	/*
	unsigned long size = 0;
	size += sizeof(long);
	size += sizeof(unsigned long);						// size list point
	size += sizeof(double) * 3 * m_vertex.size();		// list point
	size += sizeof(int) * 3;							// color
	size += sizeof(unsigned long);	// Layer
	size += sizeof(double) * 3; //normal

	size += sizeof(double) * 3; //direction
	size += sizeof(double) * 3; //origin

	size += sizeof(double) * 2 * m_texCoords.size();		// list point
	*/
	return size;
}

bool C3dFaceJson::readJson(const std::string& szJson)
{
	return true;
}

unsigned long C3dFaceJson::desc()
{
	return DbType::DB_3D_FACE;
}

C3dFaceJson* C3dFaceJson::create() {
	return new C3dFaceJson();
}

unsigned long C3dFaceJson::getVPVItemId() {
	std::wstringstream ss;
	ss << m_r << _T("") << this->getLayer();
	return CAcadUtil::GetInstance()->whash(ss.str().c_str());
}
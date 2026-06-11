#include "pch.h"
#include "GeoJson.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <iomanip>
#include "C3dFaceJson.h"

using namespace vizDatabase;
CGeoJson::CGeoJson(void)
{
	memset(m_szActiveProject, 0, JSON_STRING_MAX_LENGTH);

	m_lType = -1;
	m_r = 0; m_g = 0; m_b = 0;
	m_lId = 0;
	m_alignmentPos = 0;
	m_roadLineModelItemId = 0;
	m_wLayer = L"";
	m_roadSignId = 0;
	m_privateId = 0;
	m_matrixRoadSign = AcGeMatrix3d::kIdentity;
}

vizDatabase::CGeoJson::iterator vizDatabase::CGeoJson::begin()
{
	return m_vertex.begin();
}

vizDatabase::CGeoJson::iterator vizDatabase::CGeoJson::end()
{
	return m_vertex.end();
}

CGeoJson::iterator2d CGeoJson::begin2d() {
	return m_vertex2d.begin();
}

CGeoJson::iterator2d CGeoJson::end2d() {
	return m_vertex2d.end();
}

CGeoJson::~CGeoJson(void)
{
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	for (; it != m_vertex.end(); it++) {
		delete* it;
	}
	m_vertex.clear();

	std::list<POINT2D*>::iterator it2d = m_vertex2d.begin();
	for (; it2d != m_vertex2d.end(); it2d++) {
		delete* it2d;
	}
	m_vertex2d.clear();
}

bool CGeoJson::buildJson(std::wstring& szOut) {

	return false;
}

bool CGeoJson::readJson(const std::wstring& szJson)
{
	return false;
}

bool vizDatabase::CGeoJson::readVMIni(const std::string& szIni)
{
	return false;
}

bool vizDatabase::CGeoJson::buildBinaries(unsigned char* data, unsigned long& len)
{
	data = NULL;
	len = 0;
	return false;
}

bool vizDatabase::CGeoJson::readBinaries(unsigned char* data, unsigned long& len)
{
	return false;
}

unsigned long vizDatabase::CGeoJson::getBinariesSize()
{
	return 0;
}

bool CGeoJson::getObjectId(unsigned long& lId) {
	lId = m_lId;
	return true;
}

unsigned long CGeoJson::objectId() {
	return m_lId;
}

bool CGeoJson::setLayer(const unsigned long lLayer) {

	m_lLayer = lLayer;
	return true;
}

unsigned long vizDatabase::CGeoJson::getLayer()
{
	return m_lLayer;
}

bool vizDatabase::CGeoJson::setWLayer(const std::wstring wLayer)
{
	m_wLayer = wLayer;
	return true;
}

std::wstring vizDatabase::CGeoJson::getWLayer()
{
	return m_wLayer;
}

bool CGeoJson::setItemId(const unsigned long lId) {
	m_lId = lId;
	return true;
}

bool CGeoJson::setActiveProject(const char* szActiveProject) {
	if (!szActiveProject) {
		return false;
	}

	strncpy(m_szActiveProject, szActiveProject, JSON_STRING_MAX_LENGTH - 1); // Fix #11: bounds-safe copy
	m_szActiveProject[JSON_STRING_MAX_LENGTH - 1] = '\0';
	return true;
}

bool CGeoJson::setActiveProjectW(const TCHAR* szActiveProject) {
	if (!szActiveProject) {
		return false;
	}

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	strncpy(m_szActiveProject, conv1.to_bytes(szActiveProject).c_str(), JSON_STRING_MAX_LENGTH - 1); // Fix #11
	m_szActiveProject[JSON_STRING_MAX_LENGTH - 1] = '\0';
	return true;
}

bool CGeoJson::setGeo2dInfo(const POINT2D& point) {
	POINT2D* ptr = new POINT2D();
	ptr->x = point.x;
	ptr->y = point.y;
	m_vertex2d.push_back(ptr);
	return true;
}

bool CGeoJson::setGeoInfo(const POINT3D& point) {
	if (point.x > 900000.0 || point.x < -900000.0) {
		static long s_tr2 = 0;
	}
	POINT3D* ptr = new POINT3D();
	ptr->x = point.x;
	ptr->y = point.y;
	ptr->z = point.z;
	m_vertex.push_back(ptr);
	return true;
}

bool CGeoJson::setGeoInfo(const double& x, const double& y, const double& z) {
	// TRACE toạ độ X rác (X easting thực < ~700k; >900k = artifact). Log kèm
	// "Entering method" gần nhất ở trên → lộ hàm gốc đẻ ra vertex rác.
	if (x > 900000.0 || x < -900000.0) {
		static long s_tr = 0;
	}
	POINT3D* ptr = new POINT3D();
	ptr->x = x;
	ptr->y = y;
	ptr->z = z;
	m_vertex.push_back(ptr);
	return true;
}

bool CGeoJson::setGeoInfo2d(const double& x, const double& y) {
	POINT2D* ptr = new POINT2D();
	ptr->x = x;
	ptr->y = y;

	m_vertex2d.push_back(ptr);
	return true;
}

bool CGeoJson::getGeoInfo(POINT3D*& p1, POINT3D*& p2, POINT3D*& p3, POINT3D*& p4)
{
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	if (m_vertex.size() >= 3)
	{
		p1 = *it;
		it++;
		p2 = *it;
		it++;
		p3 = *it;
		it++;
		if (it != m_vertex.end())
		{
			p4 = *it;
		}
		return true;
	}
	return false;
}

bool CGeoJson::getGeo2dInfo(POINT2D*& p1, POINT2D*& p2, POINT2D*& p3, POINT2D*& p4)
{
	std::list<POINT2D*>::iterator it = m_vertex2d.begin();
	if (m_vertex.size() >= 3)
	{
		p1 = *it;
		it++;
		p2 = *it;
		it++;
		p3 = *it;
		it++;
		if (it != m_vertex2d.end())
		{
			p4 = *it;
		}
		return true;
	}
	return false;
}

bool CGeoJson::setTexCoords(const POINT2D& point) {
	POINT2D* ptr = new POINT2D();
	ptr->x = point.x;
	ptr->y = point.y;
	m_texCoords.push_back(ptr);
	return true;
}

bool CGeoJson::setTexCoords(const double& x, const double& y) {
	POINT2D* ptr = new POINT2D();
	ptr->x = x;
	ptr->y = y;
	m_texCoords.push_back(ptr);
	return true;
}

void CGeoJson::setRGBColor(double r, double g, double b) {
	this->m_r = r;
	this->m_g = g;
	this->m_b = b;
}

void CGeoJson::getRGBColor(double& r, double& g, double& b)
{
	r = m_r;
	g = m_g;
	b = m_b;
}

CGeoJson* CGeoJson::create(unsigned long type)
{
	if (type == C3dFaceJson::desc())
	{
		return C3dFaceJson::create();
	}
	//else if (type == CDbBlocklRef::desc())
	//{
	//	return CDbBlocklRef::create();
	//}
	return nullptr;
}

unsigned long CGeoJson::type() {
	return m_lType;
}

void vizDatabase::CGeoJson::SetType(unsigned long type)
{
	m_lType = type;
}

bool CGeoJson::isKindOf(DWORD type) {
	return m_lType == type;
}

bool vizDatabase::CGeoJson::getListVertext(std::list<POINT3D>& listVertext)
{
	for (auto vertex : m_vertex) {
		listVertext.push_back(*vertex);
	}
	return false;
}

bool CGeoJson::getChild(std::list<CGeoJson*>& childs) {
	return false;
}

bool CGeoJson::getMin(double& x, double& y, double& z) {
	if (m_vertex.size() == 0) {
		return false;
	}
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	for (; it != m_vertex.end(); it++) {
		x = min(x, (*it)->x);
		y = min(y, (*it)->y);
		z = min(z, (*it)->z);
	}
	return true;
}

bool CGeoJson::getMax(double& x, double& y, double& z) {
	if (m_vertex.size() == 0) {
		return false;
	}
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	for (; it != m_vertex.end(); it++) {
		x = max(x, (*it)->x);
		y = max(y, (*it)->y);
		z = max(z, (*it)->z);
	}
	return true;
}

bool CGeoJson::getTexCoords(POINT2D*& p1, POINT2D*& p2, POINT2D*& p3, POINT2D*& p4) {
	std::list<POINT2D*>::iterator it = m_texCoords.begin();
	if (m_texCoords.size() >= 3)
	{
		p1 = *it;
		it++;
		p2 = *it;
		it++;
		p3 = *it;
		it++;
		if (it == m_texCoords.end()) return true;
		p4 = *it;
		return true;
	}
	return false;
}

bool vizDatabase::CGeoJson::setGeoInfoAt(const int& index, const POINT3D& point)
{
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	std::advance(it, index);
	if (it != m_vertex.end())
	{
		(*it)->x = point.x;
		(*it)->y = point.y;
		(*it)->z = point.z;
		return true;
	}
	return false;
}

bool vizDatabase::CGeoJson::setTexCoordAt(const int& index, const POINT2D& point)
{
	std::list<POINT2D*>::iterator it = m_texCoords.begin();
	std::advance(it, index);
	if (it != m_texCoords.end())
	{
		(*it)->x = point.x;
		(*it)->y = point.y;
		return true;
	}
	return false;
}

bool vizDatabase::CGeoJson::getVertex(POINT3D*& p1, POINT3D*& p2, POINT3D*& p3, POINT3D*& p4)
{
	std::list<POINT3D*>::iterator it = m_vertex.begin();
	if (m_vertex.size() >= 3)
	{
		p1 = *it;
		it++;
		p2 = *it;
		it++;
		p3 = *it;
		it++;
		if (it == m_vertex.end()) return true;
		p4 = *it;
		return true;
	}
	return false;
}

bool vizDatabase::CGeoJson::clearTexCoord()
{
	if (m_texCoords.size() == 0)
	{
		return false;
	}
	std::list<POINT2D*>::iterator it2d = m_texCoords.begin();
	for (; it2d != m_texCoords.end(); it2d++)
	{
		delete (*it2d);
	}
	m_texCoords.clear();
	return true;
}

bool vizDatabase::CGeoJson::clearGeoInfo()
{
	if (m_vertex.size() == 0)
	{
		return false;
	}
	std::list<POINT3D*>::iterator it3d = m_vertex.begin();
	for (; it3d != m_vertex.end(); it3d++)
	{
		delete (*it3d);
	}
	m_vertex.clear();
	return true;
}

void CGeoJson::setImageFile(const std::string& strImage) {
	//Do nothing
}

void CGeoJson::setVisible(bool visible) {
	m_visible = visible;
}

bool CGeoJson::visible() {
	return m_visible;
}

void CGeoJson::setAlignmentPos(unsigned long lId) {
	m_alignmentPos = lId;
}

unsigned long CGeoJson::getAlignmentPos() {
	return m_alignmentPos;
}

bool vizDatabase::CGeoJson::getMatrixTransform(double transform[4][4])
{
	return false;
}

double vizDatabase::CGeoJson::rotation()
{
	return 0.0;
}

unsigned long vizDatabase::CGeoJson::getRoadLineModelItemId() {
	return m_roadLineModelItemId;
}

void vizDatabase::CGeoJson::setRoadLineModelItemId(unsigned long value) {
	m_roadLineModelItemId = value;
}

void vizDatabase::CGeoJson::setColor(double r, double g, double b)
{
	m_rgbColor_r = r;
	m_rgbColor_g = g;
	m_rgbColor_b = b;
}

void vizDatabase::CGeoJson::getColor(double& r, double& g, double& b)
{
	r = m_rgbColor_r;
	g = m_rgbColor_g;
	b = m_rgbColor_b;
}

void vizDatabase::CGeoJson::setIdRoadSign(unsigned long id)
{
	m_roadSignId = id;
}

unsigned long vizDatabase::CGeoJson::getIdRoadSign()
{
	return m_roadSignId;
}

bool vizDatabase::CGeoJson::setMatrixRoadSign(const AcGeMatrix3d& matrix)
{
	m_matrixRoadSign = matrix;
	return true;
}

AcGeMatrix3d vizDatabase::CGeoJson::getMatrixRoadSign()
{
	return m_matrixRoadSign;
}

int vizDatabase::CGeoJson::vertexNumber() {
	return m_vertex.size();
}

bool vizDatabase::CGeoJson::setPrivateId(const unsigned long& privateId)
{
	m_privateId = privateId;
	return true;
}

unsigned long vizDatabase::CGeoJson::getPrivateId()
{
	return m_privateId;
}
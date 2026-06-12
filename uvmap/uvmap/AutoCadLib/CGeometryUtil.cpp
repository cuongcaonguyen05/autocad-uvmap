#include "pch.h"
#include "CGeometryUtil.h"

char g_pCharArr[] =
{ '1','2','3','4','5','6','7','8','9' };

char g_pStringArr[] =
{ '0','1','2','3','4','5','6','7','8','9'
, 'q','Q','w','W','e','E','r','R','t','T','y','Y','u','U'
, 'i','I','o','O','p','P','a','A','S','S','d','D','f','F','g','G',
'h','H','j','J','k','K','l','L','z','Z','x','X','c','C','v','V','b','B','n','N','m','M' };

int IntRand()
{
	int nMaxRand = sizeof(g_pStringArr) / sizeof(char);
	int nMinRand = 0;

	int range = (nMaxRand - nMinRand);
	return nMinRand + int(range * rand() / (RAND_MAX + 1.0));
}

bool CGeometryUtil::isPointInside(const CvGe::CvGePolygon2D& polygon, const CvGe::CvGePoint2D& point)
{
	using namespace CvGe;

	if (polygon.size() < 3)
		return false;

	int windingNumber = 0;
	for (CvGePolygon2D::const_iterator it = polygon.begin(); it != polygon.end(); ++it)
	{
		if (it->isCloseTo(point))
			return true;

		CvGePolygon2D::const_iterator next = it;
		++next;
		if (next == polygon.end())
			next = polygon.begin();

		if (it->m_dY <= point.m_dY)
		{
			if (next->m_dY > point.m_dY)
			{
				double cross = (next->m_dX - it->m_dX) * (point.m_dY - it->m_dY) -
					(point.m_dX - it->m_dX) * (next->m_dY - it->m_dY);
				if (cross > 0)
					++windingNumber;
			}
		}
		else
		{
			if (next->m_dY <= point.m_dY)
			{
				double cross = (next->m_dX - it->m_dX) * (point.m_dY - it->m_dY) -
					(point.m_dX - it->m_dX) * (next->m_dY - it->m_dY);
				if (cross < 0)
					--windingNumber;
			}
		}
	}

	return windingNumber != 0;
}

char* CGeometryUtil::getRandomString32() {
	int nSize = 32;
	char* szPathPhrase = (char*)malloc((nSize + 1) * sizeof(char));
	for (int i = 0; i < nSize; i++)
	{
		szPathPhrase[i] = g_pStringArr[IntRand()];
	}
	szPathPhrase[nSize] = NULL;
	return szPathPhrase;
}

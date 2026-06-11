#pragma once

#include <list>

#ifndef PI
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862
#endif // !PI

#ifndef SAFE_DELETE
#define  SAFE_DELETE(p) do {delete (p); (p) = NULL;} while (0)
#endif

template<typename T>
void deleteListPointer(std::list<T>& listPtr)
{
	for (auto it = listPtr.begin(); it != listPtr.end(); ++it)
	{
		delete (*it);
	}
	listPtr.clear();
}

#ifndef CCOINITIALIZER_H_
#define CCOINITIALIZER_H_

#ifndef _WIN32
typedef enum tagCOINIT {
	COINIT_APARTMENTTHREADED = 0x2,
	COINIT_MULTITHREADED = 0x0,
	COINIT_DISABLE_OLE1DDE = 0x4,
	COINIT_SPEED_OVER_MEMORY = 0x8
} COINIT;

typedef unsigned long DWORD;

#else

#include <intsafe.h>
#include <objbase.h>

#endif
class CCoInitializer
{
public:
	explicit CCoInitializer(DWORD dwCoInit = COINIT_APARTMENTTHREADED);

	~CCoInitializer();
private:
#ifdef _WIN32
	bool _coinitialized;
#endif

	CCoInitializer(const CCoInitializer&);
	CCoInitializer& operator=(const CCoInitializer&);
};

#endif
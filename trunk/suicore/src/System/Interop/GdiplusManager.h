
#include <internal/GdiPlus/GdiPlus.h>

using namespace Gdiplus;

class GdiplusManager
{
public:

	GdiplusManager()
	{
		Status ret = GdiplusStartup(&_token, &_pStartupInput, NULL);
		//assert(ret == Ok);
	}

	~GdiplusManager()
	{
		GdiplusShutdown(_token);
	}

protected:

	ULONG_PTR _token;
    HMODULE _gdihandle;
    GdiplusStartupInput _pStartupInput;
};

/////////////////////////////////////////////////////////////////////////
// stringcorehelper.cpp

#include <System/Tools/Array.h>
#include <System/Windows/CoreTool.h>

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

namespace suic
{

struct StringInfo
{
	Int32 _Mysize;
	Int32 _Myres;

    LONG _ref;

	void* data ()
	{
		return (this + 1);
	}

	void __incRef() throw()
	{
		InterlockedIncrement(&_ref);
	}

	void __decRef() throw()
    {
        if (InterlockedDecrement(&_ref) == 0)	
        {
            uiFree(this);
        }
    }
};

struct __nilStringDataW : public StringInfo
{
	__nilStringDataW ()
	{
		_Mysize = 0;
		_Myres = 0;
		_ref = 2;
        ch = L'\0';
	}
    Char ch;
};

struct __nilStringDataA : public StringInfo
{
    __nilStringDataA ()
    {
        _Mysize = 0;
        _Myres = 0;
        _ref = 2;
        ch = '\0';
    }
    char ch;
};

template<typename Ch> 
StringInfo* __allocString(Int32 size)
{
	size = (size + 1) * sizeof(Ch) + sizeof (StringInfo);
	StringInfo * _stNewData = (StringInfo *)uiMalloc(size);
	_stNewData->_Myres = size - sizeof(Ch);
	return _stNewData;
}

template<typename Ch> 
StringInfo* __setBaseString(StringInfo * _stNewData, Int32 size)
{
    _stNewData->_Mysize = size;
    _stNewData->_Myres = size;
    _stNewData->_ref = 1;
    *((Ch*)_stNewData->data() + size) = Ch();
    return _stNewData;
}

template<typename Ch> 
void Memcpy_s(Ch* dst, int N1, const Ch* src, int N2){}
template<typename Ch> 
void Memset_s(Ch* dst, Ch c, int size){}

template<> 
void Memcpy_s<Char>(Char* dst, int N1, const Char* src, int N2)
{
    wmemcpy_s(dst, N1, src, N2);
}

template<> 
void Memcpy_s<char>(char* dst, int N1, const char* src, int N2)
{
    memcpy_s(dst, N1, src, N2);
}

template<> 
void Memset_s<Char>(Char* dst, Char c, int size)
{
    ::wmemset(dst, c, size);
}

template<> 
void Memset_s<char>(char* dst, char c, int size)
{
    ::memset(dst, c, size);
}

template<typename Ch> 
StringInfo* __initString(const Ch* value, Int32 size)
{
    Int32 count = (size + 1) * sizeof(Ch) + sizeof (StringInfo);
	StringInfo * _stNewData = (StringInfo *)uiMalloc(count);
    Memcpy_s<Ch>((Ch *)_stNewData->data(), size, value, size);
    return __setBaseString<Ch>(_stNewData, size);
}

template<typename Ch> 
StringInfo* __initString(Ch value, Int32 size)
{
    Int32 count = (size + 1) * sizeof(Ch) + sizeof (StringInfo);
	StringInfo * _stNewData = (StringInfo *)uiMalloc(count);
	Memset_s<Ch>((Ch*)_stNewData->data(), value, size);
    return __setBaseString<Ch>(_stNewData, size);
}

template<typename Ch> 
StringInfo* __GrowFitString(StringInfo* _stData, Int32 newSize)
{
    StringInfo * _stNewData = __allocString<Ch>(newSize);
    Memcpy_s<Ch>((Ch*)_stNewData->data(), _stData->_Mysize
		, (Ch*)_stData->data(), _stData->_Mysize);
    return _stNewData;
}

template<typename Ch> 
StringInfo* __GrowString(StringInfo* OldData, Int32 newSize)
{
    Int32 _size = OldData->_Myres;

    if ((newSize / 3) < (_size / 2))
    {
        _size = newSize / 2 + newSize;
    }
    else
    {
        _size = newSize;
    }

    return __GrowFitString<Ch>(OldData, _size);
}

template<typename Ch> 
StringInfo* __AppendString(StringInfo* _stData, const Ch* value, Int32 count)
{
    StringInfo * _stNewData = __GrowString<Ch>(_stData, _stData->_Mysize + count);
	_stNewData->_Mysize = _stData->_Mysize + count;
	Memcpy_s<Ch>((Ch*)_stNewData->data() + _stData->_Mysize, count, value, count);
	_stNewData->_ref = 1;
	_stData->__decRef ();
	*((Ch*)_stNewData->data() + _stNewData->_Mysize) = Char();
    return _stNewData;
}

}

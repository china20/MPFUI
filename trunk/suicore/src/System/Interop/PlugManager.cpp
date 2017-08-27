
#include <System/Interop/InternalOper.h>
#include <System/Interop//PlugManager.h>

namespace suic
{

static PlugManager* g_plugmanager = NULL;

PlugManager::PlugManager()
    : _isInit(false)
{
}

PlugManager::~PlugManager()
{
    ClearPlugs();
}

PlugManager* PlugManager::Ins()
{
    if (!g_plugmanager)
    {
        g_plugmanager = new PlugManager();
    }
    return g_plugmanager;
}

void PlugManager::AddPlug(const String& path)
{
    if (!ContainsPlug(path))
    {
        if (_isInit)
        {
            HMODULE h = LoadLibrary(path.c_str());
            if (h)
            {
                _mapPlug.Add(path, h);
            }
        }
        else
        {
            _mapPlug.Add(path, NULL);
        }
    }
}

void PlugManager::RemovePlug(const String& path)
{
    HMODULE hmod = NULL;
    if (_mapPlug.TryGetValue(path, hmod))
    {
        if (hmod)
        {
            FreeLibrary(hmod);
        }
        _mapPlug.Remove(path);
    }
}

bool PlugManager::ContainsPlug(const String& path)
{
    return _mapPlug.Contains(path);
}

void PlugManager::LoadPlugXml(const String& xml)
{

}

void PlugManager::InitPlugs()
{
    /*if (!_isInit)
    {
        StringDic<HMODULE>::Enumerator enumer(&_mapPlug, 0);

        while (enumer.HasNext())
        {
            Pair<String, HMODULE>* pair = enumer.Current();

            if (!pair->val)
            {
                HMODULE h = LoadLibrary(pair->key.c_str());
                if (h)
                {
                    pair->val = h;
                }
            }
        }
        _isInit = true;
    }*/
}

void PlugManager::ClearPlugs()
{
    /*StringDic<String, HMODULE>::Enumerator enumer(&_mapPlug, 0);

    while (enumer.HasNext())
    {
        Pair<String, HMODULE>* pair = enumer.Current();

        if (pair->val)
        {
            FreeLibrary(pair->val);
        }
    }*/

    _mapPlug.Clear();
    _isInit = false;
}

}

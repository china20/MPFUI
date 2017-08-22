// 华勤科技版权所有 2010-2020
// 
// 文件名：VSManager.h
// 功  能：。
// 
// 作  者：汪荣
// 时  间：2013-01-02
// 
// ============================================================================

# ifndef _VSMANAGER_H_
# define _VSMANAGER_H_

using namespace suic;

#include <Main/Project.h>

class VSManager
{
public:

	static void UnzipTo(const suic::String& strZipPath, const suic::String& strDir, const suic::String& name);
    static bool CreateVSProject(const suic::String& strVer, const suic::String& name, const suic::String& strDir);
	static void ReplacePrjName(const suic::String& name, const suic::String& strDir);
};

#endif


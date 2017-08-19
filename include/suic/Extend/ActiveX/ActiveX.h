// ���ڿƼ���Ȩ���� 2010-2013
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-08-31
//
// SHARPUI�����������ҵ��������ϵ�ܹ���������ϵͳ���¼�ϵͳ��
// ��ģ����Ŀǰc++�������һ���ص㣬����������ʽ�ʹ�����룬�ײ�
// ��������SSE2ָ����м��٣���������ͨ���ڻ��ǰ�͸�����������ܶ�
// �ǳ����㣬���ơ����Ķ����ӿڣ���ȫ�������������ҵʹ�á�
// 
// ����蹺����֧�ַ������ѯ����ϵ
// china0851@foxmail.com��QQ:386666951��
// ====================================================================

#ifndef _UIACTIVEXHOST_H_
#define _UIACTIVEXHOST_H_

#include <Extend/suiwgxconfig.h>

#include <Framework/Controls/HwndHost.h>
#include <Extend/ActiveX/ActiveXSite.h>
#include <Extend/ActiveX/ActiveXContainer.h>

namespace suic
{

/// <summary>
///  ActiveX��ʵ��ActiveXHost�ؼ��ķ�װ��
/// </summary>
class SUIWGX_API ActiveX : public suic::HwndHost
{
public:

    ActiveX();
    ActiveX(ActiveXSite* active);
    virtual ~ActiveX();

    RTTIOfClass(ActiveX)

    suic::Handle GetControlHandle() const;

    void Dispose();

    bool IsValid() const;
    bool DoVerb(int verb);

    /// <summary>
    ///  ����ָ��ID��com�ؼ�
    /// </summary>
    /// <param name="clsid">�ַ�����ʽ�����id</param>
    /// <returns>�ɹ�����true;����false</returns>
    bool LoadActiveX(suic::String clsid);
    bool LoadActiveX(const CLSID& clsid);

    /// <summary>
    ///  ��ѯ�ؼ������Ľӿ�
    /// </summary>
    /// <remarks>
    ///     �˷�����ѯ����Ŀؼ�ʵ�ֵ�ָ���ӿ�ָ��
    /// </remarks>
    /// <param name="iid">�ؼ��ӿ�iid</param>
    /// <param name="ppRet">�ؼ��ؼ��ӿ�ָ��</param>
    /// <returns>�ɹ�����true;����false</returns>
    bool QueryControl(const IID iid, LPVOID* ppRet);

    bool Reopen();
    void Close();

protected:

    void Clear();

    virtual bool OpenOverride();
    virtual void CloseOveride();

protected:

    void OnArrange(const suic::Size& arrangeSize);
    void ArrangeCore(const suic::Rect& arrangeRect);

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnUnloaded(suic::LoadedEventArg* e);
    void OnRender(suic::Drawing * drawing);

    void OnVisibilityChanged();

protected:

    // ���class id
    CLSID _clsid;

    ActiveXSite* _site;
    ActiveXContainer _container;
    IOleObject* _oleObj;
    suic::Rect _boundRect;
};

typedef suic::shared<ActiveX> ActiveXPtr;

};

#endif

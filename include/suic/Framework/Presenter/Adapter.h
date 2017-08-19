// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����uiadapter.h
// ��  �ܣ��������������࣬����ӳ�����ݵ��������ʾ
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2012-08-31
// 
// ============================================================================

#ifndef _UIADAPTER_H_
#define _UIADAPTER_H_

#include <System/Windows/FrameworkElement.h>

namespace suic
{

/// <summary>
/// ʵ���û����ݵ�ʵ��UI�ؼ���ӳ�䣨�û�ͨ��ע���Լ�������������
/// �������ݵ���ʵ��UI�ؼ���
/// </summary>
class SUICORE_API ItemAdapter : public RefBase
{
public:

    virtual ~ItemAdapter() {}

    /// <summary>
    ///  ��������Ĵ�С�ߴ�
    /// </summary>
    /// <remarks>
    ///  ������֪��С�������ͨ���˷��������С��������ʾ���õ�
    /// </remarks>
    /// <param name="item">��������</param>
    /// <returns>������ƴ�С</returns>
    virtual Size Estimate(RefBase* item);

    /// <summary>
    ///  �����û������Լ�������UI����Ԫ��
    /// </summary>
    /// <remarks>
    ///  ������������󣬿������ش˺������غ��ʵ�UI��ʾԪ��
    ///  ���򣬷���NULL��ϵͳ��������ģ���Զ�����
    /// </remarks>
    /// <param name="container">������</param>
    /// <param name="item">������</param>
    /// <returns>��</returns>
    virtual Element* GetElement(Element* container, RefBase* item);

    /// <summary>
    ///  �����������ݸ���UI��ʾ
    /// </summary>
    /// <remarks>
    ///  ���ڲ��ַ����ı�ʱ��UIԪ�ض�Ӧ����������ܷ����ı䣬����
    ///  ͨ���˷��������û�������ʾ
    /// </remarks>
    /// <param name="container">������</param>
    /// <param name="elem">��������ӳ���Ԫ��</param>
    /// <param name="item">������</param>
    /// <param name="index">����������</param>
    /// <returns>��</returns>
    virtual void UpdateElement(Element* container, Element* elem, RefBase* item, int index);
    virtual void SelectItemStyle(Element* elem, int index);

    /// <summary>
    ///  �����û�ģ�����UIԪ��ӳ�䣬֪ͨAdapter
    /// </summary>
    /// <remarks>
    ///  �û����Ը��ݴ˺����޸����������ԣ�ֻ��ģ��Ԫ�ظı�Ż���ô˷���
    /// </remarks>
    /// <param name="owner">�������Ŀؼ�������ListBox�ȣ�</param>
    /// <param name="container">������</param>
    /// <param name="elem">��������ӳ���Ԫ��</param>
    /// <returns>��</returns>
    virtual void OnElementAdapt(const Element* owner, Element* container, Element* elem);

    virtual void OnStartAdapt(Element* owner);
    virtual void OnEndAdapt(Element* owner);

};

typedef shared<ItemAdapter> ItemAdapterPtr;

};

#endif

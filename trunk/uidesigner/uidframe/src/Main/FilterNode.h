
#ifndef _FILTERNODE_H_
#define _FILTERNODE_H_

#include <Core/ResNode.h>

using namespace suic;

class ElementRootItem;
class Project;

class FilterNode : public ResNode
{
public:

    RTTIOfClass(FilterNode)

    BeginMember(FilterNode, ResNode)
        MemberGetInt(ShowItem)
    EndMember()

    FilterNode();
    FilterNode(const String& name);

    ~FilterNode();

    int GetShowItem()
    {
        return Visibility::Visible;
    }

    bool IsSingleValue();
    suic::String GetResXml(const String& offset);

    virtual Project* GetProject() const;
    
    // 给出名称，找到第一个分类文件夹节点
    FilterNode* FindItemOnChild(const String& name);
    // 按路径查找子节点(images/button/1.png)
    FilterNode* FindItemFromPath(const String& name);

    // 移除指定的节点
    void RemoveFilterItem(FilterNode* rootElem);
    // 按路径移除子节点(images/button/1.png)
    void RemoveItemOnPath(const String& strName);

    bool IsRootNode() const;

    // 获取相对Project目录的相对路径
    String GetRelativePath() const;

    // 获取全路径
    String GetFullPath() const;

    // 增加一个分类文件夹
    void AddSubFilter(const String& strPath);

    void SetName(const String& name);
    String GetName() const;

    String ToString();

private:

    String _name;
};

#endif

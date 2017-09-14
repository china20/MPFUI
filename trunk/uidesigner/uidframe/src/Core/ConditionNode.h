///
/// 文件名：ContentControlElement.h
///
/// 功  能：定义和Element对应的资源基类
///
/// 作  者：汪荣
///
class ContentControlElement : public BaseFrameworkElement
{
public:

    static IPropertyId* ResourcesProperty;

    static void StaticInit();

    ContentControlElement();
};

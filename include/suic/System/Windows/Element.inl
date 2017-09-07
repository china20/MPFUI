
inline MeasureData* Element::GetMeasureData()
{
    return &_measureData;
}

inline EventHandlersStore* Element::GetEventHandlersStore()
{
    return _evthStore;
}

inline Rect& Element::GetDescendantBounds()
{
    return _descendantBounds;
}

inline bool Element::IsClipToBounds()
{
    return ReadDoFlag(ViewFlags::IsClipToBoundsVF);
}

inline bool Element::ReadFlag(int key) const
{
    return (0 != (_flag & key));
}

inline Element* Element::GetUIParent() const
{
    return (Element*)(_parent.GetTarget());
}

inline DpObject* Element::GetContextParent() const
{
    return GetUIParent();
}

inline void Element::ResetCanvasOffset()
{
    _canvasOffset.x = 0;
    _canvasOffset.y = 0;
}

inline Point Element::GetCanvasOffset() const
{
    return _canvasOffset;
}

inline Size Element::GetRenderSize()
{
    if (Visibility::Collapsed != GetVisibility())
    {
        return _renderSize;
    }
    else
    {
        return Size();
    }
}

inline Size Element::ComputeRenderSize()
{
    return GetRenderSize();
}

inline Rect Element::GetRenderRect()
{
    if (Visibility::Collapsed != GetVisibility())
    {
        return Rect(0, 0, _renderSize.cx, _renderSize.cy);
    }
    else
    {
        return Rect(0, 0, 0, 0);
    }
}

inline void Element::SetRenderSize(Size val)
{
    _renderSize = val;
}

inline int Element::GetActualWidth()
{
    return _renderSize.cx;
}

inline int Element::GetActualHeight()
{
    return _renderSize.cy;
}

inline bool Element::IsInitialized()
{
    return ReadFlag(CoreFlags::IsInitialized);
}

inline bool Element::IsLoaded()
{
    return ReadFlag(CoreFlags::IsLoaded);
}

inline bool Element::IsCollapsed()
{
    return ReadFlag(CoreFlags::IsCollapsed);
}

inline void Element::SetIsCollapsed(bool val)
{
    WriteFlag(CoreFlags::IsCollapsed, val);
}

inline void Element::AddHandler(const RoutedEvent* routedEvent, Handler* handler)
{
    AddHandler(routedEvent, handler, false);
}

inline suic::Point Element::GetVisualOffset() const
{
    return _offset;
}

inline void Element::SetVisualOffset(const Point& pt)
{
    _offset.x = pt.x;
    _offset.y = pt.y;
}

inline int Element::GetParentIndex() const
{
    return _parentIndex;
}

inline bool Element::GetCacheLayer()
{
    return GetValue(CacheLayerProperty)->ToBool();
}

inline void Element::SetCacheLayer(bool val)
{
    SetValue(CacheLayerProperty, BOOLTOBOOLEAN(val));
}

inline RenderContext* Element::GetRenderContext() const
{
    return _renderLayer;
}

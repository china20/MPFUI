[ContentProperty("Content")]
public class Page : FrameworkElement, IWindowService, IAddChild
{
    // Fields
    private IWindowService _currentIws;
    private static DependencyObjectType _dType;
    private bool _isTopLevel;
    private PageHelperObject _pho;
    private SetPropertyFlags _setPropertyFlags;
    private ControlTemplate _templateCache;
    public static readonly DependencyProperty BackgroundProperty = Panel.BackgroundProperty.AddOwner(typeof(Page), new FrameworkPropertyMetadata(Panel.BackgroundProperty.GetDefaultValue(typeof(Panel)), FrameworkPropertyMetadataOptions.None));
    public static readonly DependencyProperty ContentProperty = ContentControl.ContentProperty.AddOwner(typeof(Page), new FrameworkPropertyMetadata(new PropertyChangedCallback(Page.OnContentChanged)));
    public static readonly DependencyProperty FontFamilyProperty = TextElement.FontFamilyProperty.AddOwner(typeof(Page));
    public static readonly DependencyProperty FontSizeProperty = TextElement.FontSizeProperty.AddOwner(typeof(Page));
    public static readonly DependencyProperty ForegroundProperty = TextElement.ForegroundProperty.AddOwner(typeof(Page));
    public static readonly DependencyProperty KeepAliveProperty = JournalEntry.KeepAliveProperty.AddOwner(typeof(Page));
    public static readonly DependencyProperty TemplateProperty = Control.TemplateProperty.AddOwner(typeof(Page), new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsMeasure, new PropertyChangedCallback(Page.OnTemplateChanged)));
    public static readonly DependencyProperty TitleProperty = DependencyProperty.Register("Title", typeof(string), typeof(Page), new FrameworkPropertyMetadata(null, new PropertyChangedCallback(Page.OnTitleChanged)));

    // Methods
    static Page()
    {
        Window.IWindowServiceProperty.OverrideMetadata(typeof(Page), new FrameworkPropertyMetadata(new PropertyChangedCallback(Page._OnWindowServiceChanged)));
        UIElement.FocusableProperty.OverrideMetadata(typeof(Page), new FrameworkPropertyMetadata(BooleanBoxes.FalseBox));
        FrameworkElement.DefaultStyleKeyProperty.OverrideMetadata(typeof(Page), new FrameworkPropertyMetadata(typeof(Page)));
        _dType = DependencyObjectType.FromSystemTypeInternal(typeof(Page));
    }

    public Page()
    {
        PropertyMetadata metadata = TemplateProperty.GetMetadata(base.DependencyObjectType);
        ControlTemplate defaultValue = (ControlTemplate) metadata.DefaultValue;
        if (defaultValue != null)
        {
            OnTemplateChanged(this, new DependencyPropertyChangedEventArgs(TemplateProperty, metadata, null, defaultValue));
        }
    }

    private static void _OnWindowServiceChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        (d as Page).OnWindowServiceChanged(e.NewValue as IWindowService);
    }

    protected override Size ArrangeOverride(Size arrangeBounds)
    {
        base.VerifyAccess();
        if (this.VisualChildrenCount > 0)
        {
            UIElement visualChild = this.GetVisualChild(0) as UIElement;
            if (visualChild != null)
            {
                visualChild.Arrange(new Rect(new Point(), arrangeBounds));
            }
        }
        return arrangeBounds;
    }

    private void DetermineTopLevel()
    {
        FrameworkElement parent = base.Parent as FrameworkElement;
        if ((parent != null) && (parent.InheritanceBehavior == InheritanceBehavior.Default))
        {
            this._isTopLevel = true;
        }
        else
        {
            this._isTopLevel = false;
        }
    }

    private bool IsPropertySet(SetPropertyFlags property)
    {
        return (((byte) (this._setPropertyFlags & property)) != 0);
    }

    protected override Size MeasureOverride(Size constraint)
    {
        base.VerifyAccess();
        if (this.VisualChildrenCount > 0)
        {
            UIElement visualChild = this.GetVisualChild(0) as UIElement;
            if (visualChild != null)
            {
                visualChild.Measure(constraint);
                return visualChild.DesiredSize;
            }
        }
        return new Size(0.0, 0.0);
    }

    private void OnContentChanged(object oldContent, object newContent)
    {
        base.RemoveLogicalChild(oldContent);
        base.AddLogicalChild(newContent);
    }

    private static void OnContentChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        ((Page) d).OnContentChanged(e.OldValue, e.NewValue);
    }

    protected virtual void OnTemplateChanged(ControlTemplate oldTemplate, ControlTemplate newTemplate)
    {
    }

    private static void OnTemplateChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        Page fe = (Page) d;
        StyleHelper.UpdateTemplateCache(fe, (FrameworkTemplate) e.OldValue, (FrameworkTemplate) e.NewValue, TemplateProperty);
    }

    internal override void OnTemplateChangedInternal(FrameworkTemplate oldTemplate, FrameworkTemplate newTemplate)
    {
        this.OnTemplateChanged((ControlTemplate) oldTemplate, (ControlTemplate) newTemplate);
    }

    private static void OnTitleChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        ((Page) d).PropertyIsSet(SetPropertyFlags.Title);
    }

    protected internal sealed override void OnVisualParentChanged(DependencyObject oldParent)
    {
        base.VerifyAccess();
        base.OnVisualParentChanged(oldParent);
        Visual parent = VisualTreeHelper.GetParent(this) as Visual;
        if (((parent != null) && !(base.Parent is Window)) && ((this.NavigationService == null) || (this.NavigationService.Content != this)))
        {
            bool flag = false;
            FrameworkElement element = parent as FrameworkElement;
            if (element != null)
            {
                DependencyObject templatedParent = element;
                while ((element != null) && (element.TemplatedParent != null))
                {
                    templatedParent = element.TemplatedParent;
                    element = templatedParent as FrameworkElement;
                    if (element is Frame)
                    {
                        break;
                    }
                }
                if ((templatedParent is Window) || (templatedParent is Frame))
                {
                    flag = true;
                }
            }
            if (!flag)
            {
                throw new InvalidOperationException(SR.Get("ParentOfPageMustBeWindowOrFrame"));
            }
        }
    }

    private void OnWindowServiceChanged(IWindowService iws)
    {
        this._currentIws = iws;
        this.DetermineTopLevel();
        if ((this._currentIws != null) && this._isTopLevel)
        {
            this.PropagateProperties();
        }
    }

    private void PropagateProperties()
    {
        if (this._pho != null)
        {
            if (this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowTitle))
            {
                this._currentIws.Title = this.PageHelperObject._windowTitle;
            }
            if (this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowHeight) && !this._currentIws.UserResized)
            {
                this._currentIws.Height = this.PageHelperObject._windowHeight;
            }
            if (this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowWidth) && !this._currentIws.UserResized)
            {
                this._currentIws.Width = this.PageHelperObject._windowWidth;
            }
            if (this.IsPropertySet(SetPropertyFlags.ShowsNavigationUI))
            {
                this.SetShowsNavigationUI(this.PageHelperObject._showsNavigationUI);
            }
        }
    }

    private void PropertyIsSet(SetPropertyFlags property)
    {
        this._setPropertyFlags = (SetPropertyFlags) ((byte) (this._setPropertyFlags | property));
    }

    private void SetShowsNavigationUI(bool showsNavigationUI)
    {
        NavigationWindow window = this._currentIws as NavigationWindow;
        if (window != null)
        {
            window.ShowsNavigationUI = showsNavigationUI;
        }
    }

    internal bool ShouldJournalWindowTitle()
    {
        return this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowTitle);
    }

    [EditorBrowsable(EditorBrowsableState.Never)]
    public bool ShouldSerializeShowsNavigationUI()
    {
        return this.IsPropertySet(SetPropertyFlags.ShowsNavigationUI);
    }

    [EditorBrowsable(EditorBrowsableState.Never)]
    public bool ShouldSerializeTitle()
    {
        return this.IsPropertySet(SetPropertyFlags.Title);
    }

    [EditorBrowsable(EditorBrowsableState.Never)]
    public bool ShouldSerializeWindowHeight()
    {
        return this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowHeight);
    }

    [EditorBrowsable(EditorBrowsableState.Never)]
    public bool ShouldSerializeWindowTitle()
    {
        return this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowTitle);
    }

    [EditorBrowsable(EditorBrowsableState.Never)]
    public bool ShouldSerializeWindowWidth()
    {
        return this.IsPropertySet(SetPropertyFlags.None | SetPropertyFlags.WindowWidth);
    }

    void IAddChild.AddChild(object obj)
    {
        base.VerifyAccess();
        if ((this.Content != null) && (obj != null))
        {
            throw new InvalidOperationException(SR.Get("PageCannotHaveMultipleContent"));
        }
        this.Content = obj;
    }

    void IAddChild.AddText(string str)
    {
        XamlSerializerUtil.ThrowIfNonWhiteSpaceInAddText(str, this);
    }

    // Properties
    [Category("Appearance")]
    public Brush Background
    {
        get
        {
            return (Brush) base.GetValue(BackgroundProperty);
        }
        set
        {
            base.SetValue(BackgroundProperty, value);
        }
    }

    public object Content
    {
        get
        {
            base.VerifyAccess();
            return base.GetValue(ContentProperty);
        }
        set
        {
            base.VerifyAccess();
            base.SetValue(ContentProperty, value);
        }
    }

    internal override DependencyObjectType DTypeThemeStyleKey
    {
        get
        {
            return _dType;
        }
    }

    internal override int EffectiveValuesInitialSize
    {
        get
        {
            return 0x13;
        }
    }

    [Localizability(LocalizationCategory.Font, Modifiability=Modifiability.Unmodifiable), Bindable(true), Category("Appearance")]
    public FontFamily FontFamily
    {
        get
        {
            return (FontFamily) base.GetValue(FontFamilyProperty);
        }
        set
        {
            base.SetValue(FontFamilyProperty, value);
        }
    }

    [Bindable(true), Localizability(LocalizationCategory.None), Category("Appearance"), TypeConverter(typeof(FontSizeConverter))]
    public double FontSize
    {
        get
        {
            return (double) base.GetValue(FontSizeProperty);
        }
        set
        {
            base.SetValue(FontSizeProperty, value);
        }
    }

    [Category("Appearance"), Bindable(true)]
    public Brush Foreground
    {
        get
        {
            return (Brush) base.GetValue(ForegroundProperty);
        }
        set
        {
            base.SetValue(ForegroundProperty, value);
        }
    }

    public bool KeepAlive
    {
        get
        {
            return JournalEntry.GetKeepAlive(this);
        }
        set
        {
            JournalEntry.SetKeepAlive(this, value);
        }
    }

    protected internal override IEnumerator LogicalChildren
    {
        get
        {
            base.VerifyAccess();
            return new SingleChildEnumerator(this.Content);
        }
    }

    public NavigationService NavigationService
    {
        get
        {
            return NavigationService.GetNavigationService(this);
        }
    }

    private PageHelperObject PageHelperObject
    {
        get
        {
            if (this._pho == null)
            {
                this._pho = new PageHelperObject();
            }
            return this._pho;
        }
    }

    public bool ShowsNavigationUI
    {
        get
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                throw new InvalidOperationException(SR.Get("CannotQueryPropertiesWhenPageNotInTreeWithWindow"));
            }
            NavigationWindow windowService = this.WindowService as NavigationWindow;
            return ((windowService != null) && windowService.ShowsNavigationUI);
        }
        set
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                this.PageHelperObject._showsNavigationUI = value;
                this.PropertyIsSet(SetPropertyFlags.ShowsNavigationUI);
            }
            else if (this._isTopLevel)
            {
                this.SetShowsNavigationUI(value);
                this.PropertyIsSet(SetPropertyFlags.ShowsNavigationUI);
            }
        }
    }

    double IWindowService.Height
    {
        get
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                throw new InvalidOperationException(SR.Get("CannotQueryPropertiesWhenPageNotInTreeWithWindow"));
            }
            return this.WindowService.Height;
        }
        set
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                this.PageHelperObject._windowHeight = value;
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowHeight);
            }
            else if (this._isTopLevel)
            {
                if (!this.WindowService.UserResized)
                {
                    this.WindowService.Height = value;
                }
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowHeight);
            }
        }
    }

    string IWindowService.Title
    {
        get
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                throw new InvalidOperationException(SR.Get("CannotQueryPropertiesWhenPageNotInTreeWithWindow"));
            }
            return this.WindowService.Title;
        }
        set
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                this.PageHelperObject._windowTitle = value;
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowTitle);
            }
            else if (this._isTopLevel)
            {
                this.WindowService.Title = value;
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowTitle);
            }
        }
    }

    bool IWindowService.UserResized
    {
        get
        {
            Invariant.Assert(this._currentIws != null, "_currentIws cannot be null here.");
            return this._currentIws.UserResized;
        }
    }

    double IWindowService.Width
    {
        get
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                throw new InvalidOperationException(SR.Get("CannotQueryPropertiesWhenPageNotInTreeWithWindow"));
            }
            return this.WindowService.Width;
        }
        set
        {
            base.VerifyAccess();
            if (this.WindowService == null)
            {
                this.PageHelperObject._windowWidth = value;
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowWidth);
            }
            else if (this._isTopLevel)
            {
                if (!this.WindowService.UserResized)
                {
                    this.WindowService.Width = value;
                }
                this.PropertyIsSet(SetPropertyFlags.None | SetPropertyFlags.WindowWidth);
            }
        }
    }

    public ControlTemplate Template
    {
        get
        {
            return this._templateCache;
        }
        set
        {
            base.SetValue(TemplateProperty, value);
        }
    }

    internal override FrameworkTemplate TemplateCache
    {
        get
        {
            return this._templateCache;
        }
        set
        {
            this._templateCache = (ControlTemplate) value;
        }
    }

    internal override FrameworkTemplate TemplateInternal
    {
        get
        {
            return this.Template;
        }
    }

    public string Title
    {
        get
        {
            return (string) base.GetValue(TitleProperty);
        }
        set
        {
            base.SetValue(TitleProperty, value);
        }
    }

    public double WindowHeight
    {
        get
        {
            base.VerifyAccess();
            return ((IWindowService) this).Height;
        }
        set
        {
            base.VerifyAccess();
            ((IWindowService) this).Height = value;
        }
    }

    private IWindowService WindowService
    {
        get
        {
            return this._currentIws;
        }
    }

    [Localizability(LocalizationCategory.Title)]
    public string WindowTitle
    {
        get
        {
            base.VerifyAccess();
            return ((IWindowService) this).Title;
        }
        set
        {
            base.VerifyAccess();
            ((IWindowService) this).Title = value;
        }
    }

    public double WindowWidth
    {
        get
        {
            base.VerifyAccess();
            return ((IWindowService) this).Width;
        }
        set
        {
            base.VerifyAccess();
            ((IWindowService) this).Width = value;
        }
    }
}


const float half = 0.5f;
const float oneThird = 0.3333333f;
const float twoThirds = 0.6666667f;
const float oneSixth = 0.1666667f;
const float one255th = 0.003921569f;

struct HslColor
{
private:
    
    float _h;
    float _s;
    float _l;
    Byte _a;

public:

    HslColor(float h, float s, float l)
    {
        _h = h;
        _s = s;
        _l = l;
        _a = 0xff;
    }

    HslColor(Byte a, float h, float s, float l)
    {
        _h = h;
        _s = s;
        _l = l;
        _a = a;
    }

    HslColor(Color rgbColor)
    {
        Byte r = Color::R(rgbColor);
        Byte g = Color::G(rgbColor);
        Byte b = Color::B(rgbColor);

        float num4 = r * 0.003921569f;
        float num5 = g * 0.003921569f;
        float num6 = b * 0.003921569f;
        _a = Color::A(rgbColor);
        Byte num7 = (r < g) ? ((r < b) ? r : b) : ((g < b) ? g : b);
        Byte num8 = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
        float num9 = num7 * 0.003921569f;
        float num10 = num8 * 0.003921569f;

        _h = FloatUtil::FNaN;

        if (num8 != num7)
        {
            if (num8 == r)
            {
                _h = (num5 - num6) / (num10 - num9);
            }
            else if (num8 == g)
            {
                _h = 2.0f + ((num6 - num4) / (num10 - num9));
            }
            else if (num8 == b)
            {
                _h = 4.0f + ((num4 - num5) / (num10 - num9));
            }
        }

        if (_h < 0.0f)
        {
            _h += 6.0f;
        }
        
        _h *= 0.1666667f;
        _l = (num9 + num10) * 0.5f;

        if (num8 == num7)
        {
            _s = 0.0f;
        }
        else if (_l <= 0.5f)
        {
            _s = (num10 - num9) / (num10 + num9);
        }
        else
        {
            _s = (num10 - num9) / ((2.0f - num10) - num9);
        }
    }

    float GetHue() const
    {
        return _h;
    }

    void SetHue(float val)
    {
        _h = FloatUtil::IsNaN(val) ? val : UFMAX(0.0f, UFMIN(1.0f, val));
    }

    float GetSaturation() const
    {
        return _s;
    }
    void SetSaturation(float val)
    {
        _s = UFMAX(0.0f, UFMIN(1.0f, val));
    }

    float GetLuminance() const
    {
        return _l;
    }
    void SetLuminance(float val)
    {
        _l = UFMAX(0.0f, UFMIN(1.0f, val));
    }

    Byte GetAlpha() const
    {
        return _a;
    }

    void SetAlpha(Byte a)
    {
        _a = a;
    }
    
    HslColor GetLighter(int percent)
    {
        float l = _l;
        if (percent < 0)
        {
            l += (l * percent) * 0.01f;
        }
        else
        {
            l += ((1.0f - l) * percent) * 0.01f;
        }
        return HslColor(_h, _s, l);
    }

    HslColor GetDarker(int percent)
    {
        float l = _l;
        if (percent < 0)
        {
            l -= ((1.0f - l) * percent) * 0.01f;
        }
        else
        {
            l -= (l * percent) * 0.01f;
        }
        return HslColor(_h, _s, l);
    }

    HslColor GetModifyLuminance(float percent)
    {
        if ((percent < -1.0f) || (percent > 1.0f))
        {
            throw ArgumentException("The percent parameter must be between -1 and 1", __FILELINE__);
        }
        float l = _l;
        if (percent > 0.0f)
        {
            l += (1.0f - l) * percent;
        }
        else
        {
            l += l * percent;
        }
        return HslColor(_a, _h, _s, l);
    }

    HslColor GetModifySaturation(float percent)
    {
        if ((percent < -1.0f) || (percent > 1.0f))
        {
            throw ArgumentException("The percent parameter must be between -1 and 1", __FILELINE__);
        }
        float s = _s;
        if (percent > 0.0f)
        {
            s += (1.0f - s) * percent;
        }
        else
        {
            s += s * percent;
        }
        return HslColor(_a, _h, s, _l);
    }

    HslColor GetModifyHue(float percent)
    {
        if ((percent < -1.0f) || (percent > 1.0f))
        {
            throw ArgumentException("The percent parameter must be between -1 and 1", __FILELINE__);
        }
        float h = _h;
        if (percent > 0.0f)
        {
            h += (1.0f - h) * percent;
        }
        else
        {
            h += h * percent;
        }
        return HslColor(_a, h, _s, _l);
    }

    Color ToColor()
    {
        return ToColor(GetAlpha());
    }

    static HslColor FromArgb(Byte a, Byte r, Byte g, Byte b)
    {
        double num5;
        double num6;
        Byte num = UIMAX(r, UIMAX(g, b));
        Byte num2 = UIMIN(r, UIMIN(g, b));
        double num3 = ((double)num) / 255.0;
        double num4 = ((double)num2) / 255.0;

        if (num == num2)
        {
            num5 = 0.0;
        }
        else if (num == r)
        {
            num5 = (int)(((60.0 * (g - b)) / ((double) (num - num2))) + 360.0) % 360;
        }
        else if (num == g)
        {
            num5 = ((60.0 * (b - r)) / ((double) (num - num2))) + 120.0;
        }
        else
        {
            num5 = ((60.0 * (r - g)) / ((double) (num - num2))) + 240.0;
        }
        double l = 0.5 * (num3 + num4);
        if (num == num2)
        {
            num6 = 0.0;
        }
        else if (l <= 0.5)
        {
            num6 = (num3 - num4) / (2.0 * l);
        }
        else
        {
            num6 = (num3 - num4) / (2.0 - (2.0 * l));
        }
        return HslColor(a, num5, num6, l);
    }

    static double ComputeRGBComponent(double p, double q, double tC)
    {
        if (tC < 0.16666666666666666)
        {
            return (p + (((q - p) * 6.0) * tC));
        }
        if (tC < 0.5)
        {
            return q;
        }
        if (tC < 0.66666666666666663)
        {
            return (p + (((q - p) * 6.0) * (0.66666666666666663 - tC)));
        }
        return p;
    }

    static double ModOne(double value)
    {
        if (value < 0.0)
        {
            return (value + 1.0);
        }
        if (value > 1.0)
        {
            return (value - 1.0);
        }
        return value;
    }


    Color ToArgb()
    {
        double q = (GetLuminance() < 0.5) ? (GetLuminance() * (1.0 + GetSaturation())) : ((GetLuminance() + GetSaturation()) - (GetLuminance() * GetSaturation()));
        double p = (2.0 * GetLuminance()) - q;
        double num3 = GetHue() / 360.0;
        double tC = ModOne(num3 + 0.33333333333333331);
        double num5 = num3;
        double num6 = ModOne(num3 - 0.33333333333333331);
        return Color::FromArgb(GetAlpha(), (byte) Math::Round((double)(ComputeRGBComponent(p, q, tC) * 255.0)), (byte) Math::Round((double) (ComputeRGBComponent(p, q, num5) * 255.0)), (byte) Math::Round((double) (ComputeRGBComponent(p, q, num6) * 255.0)));
    }

    Color ToColor(Byte alpha)
    {
        float num = (_l < 0.5f) ? (_l * (_s + 1.0f)) : ((_l + _s) - (_l * _s));
        float num2 = (_l + _l) - num;
        float num3 = FloatUtil::IsNaN(_h) ? 0.0f : _h;
        float num4 = num3 + 0.3333333f;
        if (num4 > 1.0f)
        {
            num4--;
        }
        float num5 = num3;
        float num6 = num3 - 0.3333333f;
        if (num6 < 0.0f)
        {
            num6++;
        }
        if (num4 < 0.1666667f)
        {
            num4 = (((num - num2) * num4) * 6.0f) + num2;
        }
        else if (num4 < 0.5f)
        {
            num4 = num;
        }
        else if (num4 < 0.6666667f)
        {
            num4 = (((num - num2) * (0.6666667f - num4)) * 6.0f) + num2;
        }
        else
        {
            num4 = num2;
        }
        if (num5 < 0.1666667f)
        {
            num5 = (((num - num2) * num5) * 6.0f) + num2;
        }
        else if (num5 < 0.5f)
        {
            num5 = num;
        }
        else if (num5 < 0.6666667f)
        {
            num5 = (((num - num2) * (0.6666667f - num5)) * 6.0f) + num2;
        }
        else
        {
            num5 = num2;
        }
        if (num6 < 0.1666667f)
        {
            num6 = (((num - num2) * num6) * 6.0f) + num2;
        }
        else if (num6 < 0.5f)
        {
            num6 = num;
        }
        else if (num6 < 0.6666667f)
        {
            num6 = (((num - num2) * (0.6666667f - num6)) * 6.0f) + num2;
        }
        else
        {
            num6 = num2;
        }
        return Color::FromArgb(alpha, Byte((float)(num4 * 255.0f)), Byte((float)(num5 * 255.0f)), Byte((float) (num6 * 255.0f)));
    }

    static Color ToColor(float h, float s, float l)
    {
        HslColor color(h, s, l);
        return color.ToColor(0xFF);
    }
};


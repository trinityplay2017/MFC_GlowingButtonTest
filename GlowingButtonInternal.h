// GlowingButtonInternal.h

#pragma once
class CGlowingButtonInternal
{
public:
    CGlowingButtonInternal() = default;
    ~CGlowingButtonInternal() = default;
    CGlowingButtonInternal(const CGlowingButtonInternal&) = delete;
    CGlowingButtonInternal& operator=(const CGlowingButtonInternal&) = delete;
    CGlowingButtonInternal(CGlowingButtonInternal&&) = delete;
    CGlowingButtonInternal& operator=(CGlowingButtonInternal&&) = delete;
protected:
    HBITMAP CreateAlphaDIB(int w, int h, void** ppBits)
    {
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        return CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, ppBits, nullptr, 0);
    }
    void DrawGlowBorderGDI(CDC* pDC, const CRect& rc, int radius, COLORREF color, int alpha, float thickness)
    {
        if (alpha <= 0 || thickness < 0.5f)
            return;
        const int w = rc.Width();
        const int h = rc.Height();
        if (w <= 0 || h <= 0)
            return;
        void* pBits = nullptr;
        HBITMAP hDib = CreateAlphaDIB(w, h, &pBits);
        if (!hDib)
            return;
        CDC memDC;
        memDC.CreateCompatibleDC(pDC);
        HBITMAP hOld = (HBITMAP)memDC.SelectObject(hDib);
        memset(pBits, 0, w * h * 4);
        const int maxThick = max(1, (int)ceil(thickness));
        for (int t = maxThick; t >= 1; --t)
        {
            float factor = (float)t / maxThick;
            int a = (int)(alpha * factor * factor);
            int r = GetRValue(color) * a / 255;
            int g = GetGValue(color) * a / 255;
            int b = GetBValue(color) * a / 255;
            COLORREF premult = RGB(r, g, b);
            CPen pen(PS_SOLID | PS_GEOMETRIC, t, premult);
            CPen* pOldPen = memDC.SelectObject(&pen);
            memDC.SelectStockObject(NULL_BRUSH);
            int inset = maxThick - t;
            memDC.RoundRect(inset, inset, w - inset, h - inset, radius, radius);
            memDC.SelectObject(pOldPen);
        }
        BLENDFUNCTION bf = {};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 50;
        bf.AlphaFormat = AC_SRC_ALPHA;
        AlphaBlend(pDC->GetSafeHdc(),
            rc.left, rc.top, w, h,
            memDC.GetSafeHdc(),
            0, 0, w, h,
            bf);
        memDC.SelectObject(hOld);
        DeleteObject(hDib);
    }
    void DrawGlowBorderGDI2(CDC* pDC, const CRect& rc, int radius,
        COLORREF color, int alpha, float thickness)
    {
        const int w = rc.Width();
        const int h = rc.Height();
        if (w <= 0 || h <= 0 || thickness <= 0.f) return;
        const int band = (std::max)(1, (int)std::ceil(thickness));
        const int rad = (std::min)(radius, (std::min)(w, h) / 2);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;          
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* pBits = nullptr;
        HDC hMem = CreateCompatibleDC(pDC->GetSafeHdc());
        HBITMAP hBmp = CreateDIBSection(pDC->GetSafeHdc(), &bmi,
            DIB_RGB_COLORS, &pBits, nullptr, 0);
        if (!hBmp || !pBits)
        {
            if (hBmp) DeleteObject(hBmp);
            if (hMem) DeleteDC(hMem);
            return;
        }
        HGDIOBJ hOldBmp = SelectObject(hMem, hBmp);
        const BYTE cr = GetRValue(color);
        const BYTE cg = GetGValue(color);
        const BYTE cb = GetBValue(color);
        const float gAlpha = alpha / 255.0f;
        BYTE* px = (BYTE*)pBits;
        const float left = 0.5f;
        const float top = 0.5f;
        const float right = w - 0.5f;
        const float bottom = h - 0.5f;
        const float r = (float)rad;
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const float fx = x + 0.5f;
                const float fy = y + 0.5f;
                const float cx = (left + right) * 0.5f;
                const float cy = (top + bottom) * 0.5f;
                const float halfW = (right - left) * 0.5f;
                const float halfH = (bottom - top) * 0.5f;
                const float qx = (std::max)(std::fabs(fx - cx) - (halfW - r), 0.0f);
                const float qy = (std::max)(std::fabs(fy - cy) - (halfH - r), 0.0f);
                const float d = std::sqrt(qx * qx + qy * qy) - r;
                if (d > 0.0f || d < -thickness) continue;
                float t = 1.0f - (-d / thickness);       
                t = t * t * (3.0f - 2.0f * t);           
                if (d > -0.5f) t *= (0.5f - d);          
                const BYTE a = (BYTE)(t * gAlpha * 255.0f + 0.5f);
                if (a == 0) continue;
                BYTE* p = px + (y * w + x) * 4;          
                p[0] = cb;
                p[1] = cg;
                p[2] = cr;
                p[3] = a;
            }
        }
        BLENDFUNCTION bf = {};
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;                    
        bf.AlphaFormat = AC_SRC_ALPHA;
        ::AlphaBlend(pDC->GetSafeHdc(), rc.left, rc.top, w, h,
            hMem, 0, 0, w, h, bf);
        SelectObject(hMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hMem);
    }
    void DrawGlowBorderGDI2BAK(CDC* pDC, const CRect& rc, int radius, COLORREF color, int alpha, float thickness)
    {
        const int w = rc.Width();
        const int h = rc.Height();
        for (int i = (int)thickness; i >= 1; --i)
        {
            CPen pen(PS_SOLID, i, color);
            CPen* old = pDC->SelectObject(&pen);
            pDC->SelectStockObject(NULL_BRUSH);
            pDC->RoundRect(rc.left, rc.top, rc.right, rc.bottom, radius, radius);
            BLENDFUNCTION bf = {};
            bf.BlendOp = AC_SRC_OVER;
            bf.SourceConstantAlpha = 50;
            bf.AlphaFormat = AC_SRC_ALPHA;
            AlphaBlend(pDC->GetSafeHdc(),
                rc.left, rc.top, w, h,
                pDC->GetSafeHdc(),
                0, 0, w, h,
                bf);
            pDC->SelectObject(old);
        }
    }
    void DrawRoundedFocusRect(CDC* pDC, const CRect& rc, int radius = 5)
    {
        if (!pDC)
            return;
        CPen pen(PS_DOT, 1, RGB(0, 120, 215));
        CPen* pOldPen = pDC->SelectObject(&pen);
        CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->RoundRect(
            rc.left,
            rc.top,
            rc.right,
            rc.bottom,
            radius,
            radius
        );
        pDC->SelectObject(pOldBrush);
        pDC->SelectObject(pOldPen);
    }
    COLORREF HSVtoRGB(double h, double s, double v)
    {
        h = fmod(h, 360.0);
        if (h < 0.0) h += 360.0;
        double c = v * s;
        double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
        double m = v - c;
        double r = 0.0, g = 0.0, b = 0.0;
        if (h < 60.0) { r = c; g = x; }
        else if (h < 120.0) { r = x; g = c; }
        else if (h < 180.0) { g = c; b = x; }
        else if (h < 240.0) { g = x; b = c; }
        else if (h < 300.0) { r = x; b = c; }
        else { r = c; b = x; }
        return RGB(
            static_cast<BYTE>((r + m) * 255.0),
            static_cast<BYTE>((g + m) * 255.0),
            static_cast<BYTE>((b + m) * 255.0));
    }
    COLORREF HSVtoBLUE(double h, double s, double v)
    {
        h = fmod(h, 360.0);
        if (h < 0.0) h += 360.0;
        double c = v * s;
        double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
        double m = v - c;
        double r = 0.0, g = 0.0, b = 1.0;
        if (h < 60.0) { r = c; g = x; }
        else if (h < 120.0) { r = x; g = c; }
        else if (h < 180.0) { g = c; b = x; }
        else if (h < 240.0) { g = x; b = c; }
        else if (h < 300.0) { r = x; b = c; }
        else { r = c; b = x; }
        return RGB(
            static_cast<BYTE>((r + m) * 255.0),
            static_cast<BYTE>((g + m) * 255.0),
            static_cast<BYTE>((b + m) * 255.0));
    }
    void DrawRainbowRoundedRect(HDC hdc, int left, int top, int right, int bottom, int radius, double offset,
        const int thickness = 1,
        bool bOverride = false,
        COLORREF bOverrideClr = 0xFF0000,
        BYTE alpha = 255)
    {
        const int width = right - left;
        const int height = bottom - top;
        if (radius == 0)
        {
            const double perimeter = 2.0 * width + 2.0 * height;
            if (perimeter <= 0.0) return;
            const int steps = static_cast<int>(perimeter * 2.0) + 2;
            for (int i = 0; i <= steps; ++i)
            {
                const double t = static_cast<double>(i) / steps;
                double remaining = t * perimeter;
                int x = 0;
                int y = 0;
                if (remaining <= width)
                {
                    x = left + static_cast<int>(remaining + 0.5);
                    y = top;
                }
                else
                {
                    remaining -= width;
                    if (remaining <= height)
                    {
                        x = right - 1;
                        y = top + static_cast<int>(remaining + 0.5);
                    }
                    else
                    {
                        remaining -= height;
                        if (remaining <= width)
                        {
                            x = right - 1 - static_cast<int>(remaining + 0.5);
                            y = bottom - 1;
                        }
                        else
                        {
                            remaining -= width;
                            x = left;
                            y = bottom - 1 - static_cast<int>(remaining + 0.5);
                        }
                    }
                }
                if (x >= right)  x = right - 1;
                if (y >= bottom) y = bottom - 1;
                if (x < left)    x = left;
                if (y < top)     y = top;
                const double hue = fmod(t * 360.0 + offset, 360.0);
                const COLORREF color = bOverride ? bOverrideClr :
                    HSVtoRGB(hue, 1.0, 1.0);
                for (int dx = -thickness / 2; dx <= thickness / 2; ++dx)
                {
                    for (int dy = -thickness / 2; dy <= thickness / 2; ++dy)
                    {
                        const int px = x + dx;
                        const int py = y + dy;
                        if (alpha == 255)
                        {
                            SetPixelV(hdc, px, py, color);
                        }
                        else if (alpha != 0)
                        {
                            const COLORREF background = GetPixel(hdc, px, py);
                            const BYTE r = static_cast<BYTE>(
                                (GetRValue(color) * alpha +
                                    GetRValue(background) * (255 - alpha)) / 255);
                            const BYTE g = static_cast<BYTE>(
                                (GetGValue(color) * alpha +
                                    GetGValue(background) * (255 - alpha)) / 255);
                            const BYTE b = static_cast<BYTE>(
                                (GetBValue(color) * alpha +
                                    GetBValue(background) * (255 - alpha)) / 255);
                            SetPixelV(hdc, px, py, RGB(r, g, b));
                        }
                    }
                }
            }
            return;
        }
        constexpr double magicNum = 2.0;
        int magicNum2 = static_cast<int>(std::round(magicNum));
        if (radius * magicNum2 > width) radius = width / magicNum2;
        if (radius * magicNum2 > height) radius = height / magicNum2;
        const double straightW = width - magicNum * radius;
        const double straightH = height - magicNum * radius;
        const double perimeter = magicNum * straightW + magicNum * straightH + magicNum * PI * radius;
        if (perimeter <= 0.0) return;
        const int steps = static_cast<int>(perimeter * magicNum) + magicNum2;
        for (int i = 0; i <= steps; ++i)
        {
            const double t = static_cast<double>(i) / steps;
            double remaining = t * perimeter;
            int x = 0, y = 0;
            if (remaining <= straightW)
            {
                x = left + radius + static_cast<int>(remaining + 0.5);
                y = top;
            }
            else
            {
                remaining -= straightW;
                if (remaining <= 0.5 * PI * radius)
                {
                    const double angle = remaining / radius;
                    x = right - radius + static_cast<int>(radius * std::sin(angle) + 0.5);
                    y = top + radius - static_cast<int>(radius * std::cos(angle) + 0.5);
                }
                else
                {
                    remaining -= 0.5 * PI * radius;
                    if (remaining <= straightH)
                    {
                        x = right;
                        y = top + radius + static_cast<int>(remaining + 0.5);
                    }
                    else
                    {
                        remaining -= straightH;
                        if (remaining <= 0.5 * PI * radius)
                        {
                            const double angle = remaining / radius;
                            x = right - radius + static_cast<int>(radius * std::cos(angle) + 0.5);
                            y = bottom - radius + static_cast<int>(radius * std::sin(angle) + 0.5);
                        }
                        else
                        {
                            remaining -= 0.5 * PI * radius;
                            if (remaining <= straightW)
                            {
                                x = right - radius - static_cast<int>(remaining + 0.5);
                                y = bottom;
                            }
                            else
                            {
                                remaining -= straightW;
                                if (remaining <= 0.5 * PI * radius)
                                {
                                    const double angle = remaining / radius;
                                    x = left + radius - static_cast<int>(radius * std::sin(angle) + 0.5);
                                    y = bottom - radius + static_cast<int>(radius * std::cos(angle) + 0.5);
                                }
                                else
                                {
                                    remaining -= 0.5 * PI * radius;
                                    if (remaining <= straightH)
                                    {
                                        x = left;
                                        y = bottom - radius - static_cast<int>(remaining + 0.5);
                                    }
                                    else
                                    {
                                        remaining -= straightH;
                                        const double angle = remaining / radius;
                                        x = left + radius - static_cast<int>(radius * std::cos(angle) + 0.5);
                                        y = top + radius - static_cast<int>(radius * std::sin(angle) + 0.5);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (x >= right)  x = right - 1;
            if (y >= bottom) y = bottom - 1;
            if (x < left)    x = left;
            if (y < top)     y = top;
            const double hue = fmod(t * 360.0 + offset, 360.0);
            const COLORREF color = bOverride ? bOverrideClr :
                HSVtoRGB(hue, 1.0, 1.0);
            for (int dx = -thickness / 2; dx <= thickness / 2; ++dx)
            {
                for (int dy = -thickness / 2; dy <= thickness / 2; ++dy)
                {
                    const int px = x + dx;
                    const int py = y + dy;
                    if (alpha == 255)
                    {
                        SetPixelV(hdc, px, py, color);
                    }
                    else if (alpha != 0)
                    {
                        const COLORREF background = GetPixel(hdc, px, py);
                        const BYTE r = static_cast<BYTE>(
                            (GetRValue(color) * alpha +
                                GetRValue(background) * (255 - alpha)) / 255);
                        const BYTE g = static_cast<BYTE>(
                            (GetGValue(color) * alpha +
                                GetGValue(background) * (255 - alpha)) / 255);
                        const BYTE b = static_cast<BYTE>(
                            (GetBValue(color) * alpha +
                                GetBValue(background) * (255 - alpha)) / 255);
                        SetPixelV(hdc, px, py, RGB(r, g, b));
                    }
                }
            }
        }
    }
    void DrawRainbowRoundedRectBAK(HDC hdc, int left, int top, int right, int bottom, int radius, double offset,
        const int thickness = 1,
        bool bOverride = false,
        COLORREF bOverrideClr = 0xFF0000)
    {
        if (radius < 1) radius = 1;
        const int width = right - left;
        const int height = bottom - top;
        constexpr double magicNum = 2.0;
        constexpr int magicNum2 = 2;
        if (radius * magicNum2 > width) radius = width / magicNum2;
        if (radius * magicNum2 > height) radius = height / magicNum2;
        const double straightW = width - magicNum * radius;
        const double straightH = height - magicNum * radius;
        const double perimeter = magicNum * straightW + magicNum * straightH + magicNum * PI * radius;
        if (perimeter <= 0.0) return;
        const int steps = static_cast<int>(perimeter * magicNum) + magicNum2;
        for (int i = 0; i <= steps; ++i)
        {
            const double t = static_cast<double>(i) / steps;
            double remaining = t * perimeter;
            int x = 0, y = 0;
            if (remaining <= straightW)
            {
                x = left + radius + static_cast<int>(remaining + 0.5);
                y = top;
            }
            else
            {
                remaining -= straightW;
                if (remaining <= 0.5 * PI * radius)
                {
                    const double angle = remaining / radius;
                    x = right - radius + static_cast<int>(radius * std::sin(angle) + 0.5);
                    y = top + radius - static_cast<int>(radius * std::cos(angle) + 0.5);
                }
                else
                {
                    remaining -= 0.5 * PI * radius;
                    if (remaining <= straightH)
                    {
                        x = right;
                        y = top + radius + static_cast<int>(remaining + 0.5);
                    }
                    else
                    {
                        remaining -= straightH;
                        if (remaining <= 0.5 * PI * radius)
                        {
                            const double angle = remaining / radius;
                            x = right - radius + static_cast<int>(radius * std::cos(angle) + 0.5);
                            y = bottom - radius + static_cast<int>(radius * std::sin(angle) + 0.5);
                        }
                        else
                        {
                            remaining -= 0.5 * PI * radius;
                            if (remaining <= straightW)
                            {
                                x = right - radius - static_cast<int>(remaining + 0.5);
                                y = bottom;
                            }
                            else
                            {
                                remaining -= straightW;
                                if (remaining <= 0.5 * PI * radius)
                                {
                                    const double angle = remaining / radius;
                                    x = left + radius - static_cast<int>(radius * std::sin(angle) + 0.5);
                                    y = bottom - radius + static_cast<int>(radius * std::cos(angle) + 0.5);
                                }
                                else
                                {
                                    remaining -= 0.5 * PI * radius;
                                    if (remaining <= straightH)
                                    {
                                        x = left;
                                        y = bottom - radius - static_cast<int>(remaining + 0.5);
                                    }
                                    else
                                    {
                                        remaining -= straightH;
                                        const double angle = remaining / radius;
                                        x = left + radius - static_cast<int>(radius * std::cos(angle) + 0.5);
                                        y = top + radius - static_cast<int>(radius * std::sin(angle) + 0.5);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (x >= right)  x = right - 1;
            if (y >= bottom) y = bottom - 1;
            if (x < left)    x = left;
            if (y < top)     y = top;
            const double hue = fmod(t * 360.0 + offset, 360.0);
            const COLORREF color = bOverride ? bOverrideClr :
                HSVtoRGB(hue, 1.0, 1.0);
            for (int dx = -thickness / 2; dx <= thickness / 2; ++dx)
            {
                for (int dy = -thickness / 2; dy <= thickness / 2; ++dy)
                {
                    SetPixelV(hdc, x + dx, y + dy, color);
                }
            }
        }
    }
    COLORREF LightenColor(COLORREF color, float factor)
    {
        factor = std::clamp(factor , 0.0f,10.0f);
        int r = GetRValue(color);
        int g = GetGValue(color);
        int b = GetBValue(color);
        r = std::clamp(int(r * factor), 0, 255);
        g = std::clamp(int(g * factor), 0, 255);
        b = std::clamp(int(b * factor), 0, 255);
        return RGB(r, g, b);
    }
    COLORREF DarkenColor(COLORREF color, float factor)
    {
        factor = max(0.0f, factor);
        int r = int(GetRValue(color) * factor);
        int g = int(GetGValue(color) * factor);
        int b = int(GetBValue(color) * factor);
        r = std::clamp(r, 0, 255);
        g = std::clamp(g, 0, 255);
        b = std::clamp(b, 0, 255);
        return RGB(r, g, b);
    }
    COLORREF ChangeColor(COLORREF color, float factor)
    {
        COLORREF rt = color;
        if (factor == 1.0f)
        {
            rt = DarkenColor(color, factor);
        }
        if (factor < 1.0f)
        {
            rt = DarkenColor(color, factor);
        }
        if (factor > 1.0f)
        {
            rt = LightenColor(color, factor);
        }
        return rt;
    }
    void DrawRainbowLine(HDC hdc, int x1, int y1, int x2, int y2, double offset)
    {
        const int dx = x2 - x1;
        const int dy = y2 - y1;
        const double distance = std::sqrt(static_cast<double>(dx) * dx + static_cast<double>(dy) * dy);
        if (distance <= 0.0) return;
        const int steps = static_cast<int>(distance);
        for (int i = 0; i <= steps; ++i)
        {
            const double t = static_cast<double>(i) / steps;
            const int x = static_cast<int>(x1 + dx * t + 0.5);
            const int y = static_cast<int>(y1 + dy * t + 0.5);
            const double hue = fmod(t * 360.0 + offset, 360.0);
            SetPixelV(hdc, x, y, HSVtoRGB(hue, 1.0, 1.0));
        }
    }
    COLORREF InternalLerpColor(COLORREF c1, COLORREF c2, double t)
    {
        BYTE r1 = GetRValue(c1);
        BYTE g1 = GetGValue(c1);
        BYTE b1 = GetBValue(c1);
        BYTE r2 = GetRValue(c2);
        BYTE g2 = GetGValue(c2);
        BYTE b2 = GetBValue(c2);
        BYTE r = static_cast<BYTE>(r1 + (r2 - r1) * t);
        BYTE g = static_cast<BYTE>(g1 + (g2 - g1) * t);
        BYTE b = static_cast<BYTE>(b1 + (b2 - b1) * t);
        return RGB(r, g, b);
    }
};

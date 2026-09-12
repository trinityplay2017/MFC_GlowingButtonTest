// GlowingButton.cpp

#include "stdafx.h"
#include "GlowingButton.h"
#include "colorhelper.h"
#include "CGlowingButtonHelper.h"
#include "scintillaWnd.h"
#include "RectHelper.h"
void StartSitFailedCheck();
BEGIN_MESSAGE_MAP(CGlowingButton, CButton)
    ON_WM_TIMER()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_SIZE()
END_MESSAGE_MAP()
CGlowingButton::CGlowingButton()
    :
m_animStartTime()
{
    initialize();
    updateRound();
}
CGlowingButton::~CGlowingButton()
{
    ::DeleteObject(m_hRgn);
    ::KillTimer(m_hWnd, TIMER_ANIM_RAINBOW);
}
void CGlowingButton::initialize()
{
    m_animDuration      = 100;
    m_bIshovered        = false;
    m_bIsPressed        = false;
    m_bIsFocused        = false;
    m_bIsDisabled       = false;
    m_disabledColor     = RGB(70, 70, 70);
    m_backColor         = RGB(40, 40, 40);
    m_borderColor       = RGB(20, 20, 20);
    m_pressedColor      = RGB(123, 102, 240);
    m_hoverBackColor    = RGB(40, 40, 40);
    m_textColor         = RGB(255, 255, 255);
    m_roundRadius       = 0;
    g_prevBackColor     = m_backColor;
    g_targetBackColor   = m_backColor;
    g_rainbowOffset     = 0.0;
}
void CGlowingButton::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);
    if (cx > 0 && cy > 0)
    {   
        updateRound();
    }
}
void CGlowingButton::OnMouseHover(UINT nFlags, CPoint point)
{
    m_bIshovered = TRUE;
    Invalidate(FALSE);
    __super::OnMouseHover(nFlags, point);   
}
void CGlowingButton::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bTracking)
    {
        m_bTracking = TRUE;
        m_bIshovered = TRUE;
        Invalidate(FALSE);
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        ::TrackMouseEvent(&tme);
    }
    __super::OnMouseMove(nFlags, point);
}
void CGlowingButton::OnMouseLeave()
{
    m_bTracking = FALSE;
    m_bIshovered = FALSE;
    Invalidate(FALSE);
    __super::OnMouseLeave();
}
void CGlowingButton::updateRound()
{
    if (m_hRgn != NULL)
    {
        ::DeleteObject(m_hRgn);
        m_hRgn = NULL;
    }
    CRect rc;
    GetClientRect(&rc);
    const int radius = (int)m_roundRadius;
    m_hRgn = ::CreateRoundRectRgn(
        rc.left,
        rc.top,
        rc.right,
        rc.bottom,
        radius,
        radius);
    SetWindowRgn(m_hRgn, TRUE);
   }
void CGlowingButton::PreSubclassWindow()
{
    LONG_PTR style = ::GetClassLongPtrW(m_hWnd, GCL_STYLE);
    ::SetClassLongPtrW(m_hWnd, GCL_STYLE, style & ~CS_DBLCLKS);
    ModifyStyle(0, BS_FLAT | BS_PUSHBUTTON | BS_OWNERDRAW | BS_DEFPUSHBUTTON | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    initialize();
    updateRound();
    {
        ::SetTimer(m_hWnd, TIMER_ANIM_RAINBOW, 100, nullptr);
    }
    StartSitFailedCheck();
    __super::PreSubclassWindow();
}
void CGlowingButton::OnTimer(UINT_PTR id)
{
    if (id == TIMER_ANIM_RAINBOW)
    {
        if (m_rainbow.m_enable && m_rainbow.m_config.m_animating)
        {
            g_rainbowOffset -= m_rainbow.m_config.m_step;
            if (g_rainbowOffset < 0.0)
            {
                g_rainbowOffset += 360.0;
            }
        }
        ::InvalidateRect(m_hWnd, nullptr, FALSE);
    }
}
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
std::string ColorRefToHex(COLORREF color) 
{
    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::uppercase
        << std::setw(2) << (int)GetRValue(color)
        << std::setw(2) << (int)GetGValue(color)
        << std::setw(2) << (int)GetBValue(color);
    return ss.str();
}
DWORD llastTick;
DWORD m_dwSitStartTime;
int m_nCount;
const int m_nSitCount = 5;
void StartSitFailedCheck()
{
    llastTick = GetTickCount64();
}
bool IsOneSecondPassed(DWORD& lastTick)
{
    DWORD currentTick = GetTickCount64();
    if (currentTick - lastTick >= 400) 
    {
        lastTick = currentTick;
        return true;
    }
    return false;
}
#include <string>
#include <vector>
std::wstring GetCaption(HWND hwnd)
{
    if (!hwnd)
        return {};
    int len = ::GetWindowTextLengthW(hwnd);
    std::wstring result(len, L'\0');
    int written = ::GetWindowTextW(hwnd, result.data(), len + 1);
    result.resize(written); 
    return result;
}
void drawBackground(HWND hwnd, HDC hdc, const RECT& rc)
{
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
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
void drawText(HWND hwnd, HDC hdc, const RECT& rc,
    BOOL animate, double phaseDeg = std::numeric_limits<double>::quiet_NaN())
{
    HFONT hOldFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    int   oldBkMode = SetBkMode(hdc, TRANSPARENT);
    wchar_t text[256]{};
    ::GetWindowTextW(hwnd, text, 256);
    int len = lstrlenW(text);
    if (len == 0)
    {
        SetBkMode(hdc, oldBkMode);
        SelectObject(hdc, hOldFont);
        return;
    }
    SIZE sz{};
    GetTextExtentPoint32W(hdc, text, len, &sz);
    int x = rc.left + ((rc.right - rc.left) - sz.cx) / 2;
    int y = rc.top + ((rc.bottom - rc.top) - sz.cy) / 2;
    double phase = 0.0;
    if (animate)
    {
        if (!_isnan(phaseDeg))
        {
            phase = phaseDeg;                       
        }
        else
        {
            ULONGLONG t = GetTickCount64();         
            phase = double(t % 6000) / 6000.0 * 360.0;
        }
    }
    for (int i = 0; i < len; ++i)
    {
        double hue = (double(i) / len) * 360.0 + phase;
        hue = fmod(hue, 360.0);
        if (hue < 0.0) hue += 360.0;
        ::SetTextColor(hdc, HSVtoRGB((float)hue, 1.0f, 1.0f));
        TextOutW(hdc, x, y, &text[i], 1);
        int charW = 0;
        GetCharWidth32W(hdc, text[i], text[i], &charW);
        x += charW;
    }
    SetBkMode(hdc, oldBkMode);
    SelectObject(hdc, hOldFont);
}
void drawTextBAK(HWND hwnd, HDC hdc, const RECT& rc, BOOL animate, double offset=0.0)
{
    HFONT hOldFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    int   oldBkMode = SetBkMode(hdc, TRANSPARENT);
    wchar_t text[256]{};
    ::GetWindowTextW(hwnd, text, 256);
    int len = lstrlenW(text);
    if (len == 0)
    {
        SetBkMode(hdc, oldBkMode);
        SelectObject(hdc, hOldFont);
        return;
    }
    SIZE sz{};
    GetTextExtentPoint32W(hdc, text, len, &sz);
    int x = rc.left + ((rc.right - rc.left) - sz.cx) / 2;
    int y = rc.top + ((rc.bottom - rc.top) - sz.cy) / 2;
    float phase = 0.0f;
    if (animate)
    {
        if (offset == 0.0)
        {
            DWORD t = GetTickCount();
            phase = (float)(t % 6000) / 6000.0f * 360.0f;
        }
        else
        {
            phase = offset;
        }
    }
    for (int i = 0; i < len; ++i)
    {
        float hue = (float)i / len * 360.0f + phase;
        while (hue >= 360.0f) hue -= 360.0f;
        while (hue < 0.0f) hue += 360.0f;
        COLORREF color = HSVtoRGB(hue, 1.0f, 1.0f);
        ::SetTextColor(hdc, color);
        TextOutW(hdc, x, y, &text[i], 1);
        int charW = 0;
        GetCharWidth32W(hdc, text[i], text[i], &charW);
        x += charW;
    }
    SetBkMode(hdc, oldBkMode);
    SelectObject(hdc, hOldFont);
}
void CGlowingButton::DrawItem(LPDRAWITEMSTRUCT dis)
{
    CDC* pDC                        = CDC::FromHandle(dis->hDC);
    CRect rc                        = dis->rcItem;
    UINT state = dis->itemState;
    if (m_bIshovered)
        state |= ODS_HOTLIGHT;
    CDC memDC;
    CBitmap bmp;
    memDC.CreateCompatibleDC(pDC);
    bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
    CBitmap* pOldBitmap             = memDC.SelectObject(&bmp);
    m_bIsPressed                    = (state & ODS_SELECTED) != 0;
    m_bIsFocused                    = (state & ODS_FOCUS) != 0;
    m_bIsDisabled                   = (state & ODS_DISABLED) != 0;
    const bool pressed              = m_bIsPressed;
    const bool focused              = m_bIsFocused;
    const bool disabled             = m_bIsDisabled;
    const bool checked              = (state & ODS_CHECKED) != 0;
    const bool hovered              = m_bIshovered;
    COLORREF   clrText              = NppDarkMode::getTextColor();
    COLORREF   clrBorder            = m_borderColor;
    CRect      textRect             = rc;
    int        radius               = m_roundRadius;
    int        width                = rc.Width();
    int        height               = rc.Height();
    COLORREF   targetBack           = m_backColor;            
    COLORREF   m_checkedHoverColor  = m_backColor;
    COLORREF   m_checkedColor       = m_backColor;
    bool       bStaticBorder            = false;
    UINT       bDebugState          = 5555;
    if (disabled)                { targetBack = m_disabledColor;     bStaticBorder = true; bDebugState = 1; }
    else if (pressed && hovered) { targetBack = m_pressedColor;      bStaticBorder = true; bDebugState = 3; }
    else if (checked && hovered) { targetBack = m_checkedHoverColor; bStaticBorder = true; bDebugState = 4; }
    else if (checked)            { targetBack = m_checkedColor;      bStaticBorder = true; bDebugState = 5; }
    else if (hovered && !pressed){ targetBack = m_hoverBackColor;    bStaticBorder = true; bDebugState = 6; }
    else                         { targetBack = m_backColor;         bDebugState = 0; }
    if (targetBack != g_targetBackColor)
    {
        g_prevBackColor     = g_targetBackColor;   
        g_targetBackColor   = targetBack;
        m_animStartTime     = std::chrono::steady_clock::now();
    }
    double t = 1.0;
    if (m_animDuration > 0)
    {
        auto now    = std::chrono::steady_clock::now();
        int elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - m_animStartTime).count();
                  t = std::clamp((double)elapsed / m_animDuration, 0.0, 1.0);
    }
    COLORREF clrBack = InternalLerpColor(g_prevBackColor, g_targetBackColor, t);
    memDC.FillSolidRect(&rc, clrBack);
    const int m_borderPadding = 0;
    if (m_bFirstDraw || state != m_prevState )
    {
        m_bFirstDraw = false;
        m_prevState = state;
        CString stateStr;
        if (state & ODS_SELECTED)     stateStr += _T("ODS_SELECTED ");
        if (state & ODS_GRAYED)       stateStr += _T("ODS_GRAYED ");
        if (state & ODS_DISABLED)     stateStr += _T("ODS_DISABLED ");
        if (state & ODS_CHECKED)      stateStr += _T("ODS_CHECKED ");
        if (state & ODS_FOCUS)        stateStr += _T("ODS_FOCUS ");
        if (state & ODS_DEFAULT)      stateStr += _T("ODS_DEFAULT ");
        if (state & ODS_COMBOBOXEDIT) stateStr += _T("ODS_COMBOBOXEDIT ");
        if (state & ODS_HOTLIGHT)     stateStr += _T("ODS_HOTLIGHT ");
        if (state & ODS_INACTIVE)     stateStr += _T("ODS_INACTIVE ");
        if (state & ODS_NOACCEL)      stateStr += _T("ODS_NOACCEL ");
        if (state & ODS_NOFOCUSRECT)  stateStr += _T("ODS_NOFOCUSRECT ");
        stateStr.TrimRight();
        if (stateStr.IsEmpty())
            stateStr = _T("(no flags)");
        printScin(_T("State changed: 0x%04X -> %s"), state, (LPCTSTR)stateStr);
    }
    if (m_rainbow.m_enable)
    {
        DrawRainbowRoundedRect(memDC.GetSafeHdc(),
            m_borderPadding,
            m_borderPadding,
            (width - m_borderPadding) - 1,
            (height - m_borderPadding) - 1,
            (radius / 2) + 2,
            g_rainbowOffset,
            1,
            bStaticBorder,
            clrBorder,
            255);
    }
    else
    {
        DrawRainbowRoundedRect(memDC.GetSafeHdc(),
            m_borderPadding,
            m_borderPadding,
            (width - m_borderPadding) - 1,
            (height - m_borderPadding) - 1,
            (radius / 2) + 2,
            g_rainbowOffset,
            1,
            true,
            clrBorder,
            255);
    }
    if (focused && !disabled)
    {
    }
    memDC.SetTextColor(clrText);
    memDC.SetBkMode(TRANSPARENT);
    CFont* pOldFont = memDC.SelectObject(GetFont());
    drawText(m_hWnd, memDC, rc, true, g_rainbowOffset);
    memDC.SelectObject(pOldFont);
    pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);
    __super::DrawItem(dis);
}
void CGlowingButton::EnableRainbowBorder(bool enable, double speed)
{
    m_rainbow.m_enable = enable;
}
void CGlowingButton::SetWindowTextW(LPCWSTR str)
{ 
    ::SendMessageW(m_hWnd, WM_SETTEXT, 0, (LPARAM)str);
}
void CGlowingButton::SetWindowTextW(int val) 
{
    wchar_t str[32]={0};
    swprintf_s(str, L"%d", val);
    SetWindowTextW(str);
}
int CGlowingButton::GetWindowTextW(LPWSTR str, int cchMax)
{
    if (!str || cchMax <= 0)
        return 0;
    LRESULT n = ::SendMessageW(m_hWnd, WM_GETTEXT, (WPARAM)cchMax, (LPARAM)str);
    return (int)n;
}

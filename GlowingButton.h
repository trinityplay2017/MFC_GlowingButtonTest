// GlowingButton.h

#pragma once
#include <chrono>
#include "GlowingButtonInternal.h"
struct _RAINDBOWCOLOR
{
    bool            m_animating;
    double          m_step;
    _RAINDBOWCOLOR()
    {
        init();
    }
    void init()
    {
        m_animating = false;
        m_step = 8.0;
    }
};
struct RAINDBOWCOLOR
{
    bool            m_enable;
    _RAINDBOWCOLOR  m_config;
    RAINDBOWCOLOR()
    {
        m_enable = false;
        m_config.init();
    }
};
struct GLOWINGSTATE
{
    bool m_override;
    COLORREF m_color;
};
class CGlowingButton : public CButton, public CGlowingButtonInternal
{
private: 
    const int       TIMER_ANIM = 1;
    const int       TIMER_ANIM_RAINBOW = 2;
    COLORREF        g_prevBackColor;
    COLORREF        g_targetBackColor;
    double          g_rainbowOffset;
    HRGN            m_hRgn = NULL;
    std::chrono::steady_clock::time_point m_animStartTime;
    BOOL	        m_bIsPressedAndDisabled;
    BOOL	        m_bIsDisabled;
    BOOL	        m_bIsFocused;
    BOOL	        m_bIsPressed;
    BOOL            m_bIshovered;
    BOOL            m_bTracking = FALSE;
    UINT m_prevState = UINT_MAX;
    bool m_bFirstDraw = true;
private: 
    GLOWINGSTATE    m_stateSetup[8];
    int             m_animDuration; 
    COLORREF        m_disabledColor ;
    COLORREF        m_backColor ;
    COLORREF        m_borderColor ;
    COLORREF        m_pressedColor ;
    COLORREF        m_hoverBackColor ;
    COLORREF        m_textColor ;
    int             m_roundRadius;
public: 
    CGlowingButton(const CGlowingButton&)            = delete;
    CGlowingButton& operator=(const CGlowingButton&) = delete;
    CGlowingButton(CGlowingButton&&)                 = delete;
    CGlowingButton& operator=(CGlowingButton&&)      = delete;
    CGlowingButton();
    virtual ~CGlowingButton();
public:
    void            updateRound();
    void            SetHoverBackColor(COLORREF color)   { m_hoverBackColor = color;                 }
    void            SetTextColor(COLORREF color)        { m_textColor = color;                      }
    void            SetBorderColor(COLORREF color)      { m_borderColor = color;                    }
    void            SetBackColor(COLORREF color)        { m_backColor = color;                      }
    void            SetRoundRadius(int radius)          { m_roundRadius = radius; updateRound();    }
    void            SetWindowTextW(LPCWSTR str);
    void            SetWindowTextW(int val);
    int             GetWindowTextW(LPWSTR str, int cchMax);
    void            EnableRainbowBorder(bool enable = true, double speed = 1.0);
    void            DisableRainbowBorder() { EnableRainbowBorder(false); }
    RAINDBOWCOLOR   m_rainbow;
protected:    
    virtual void    DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) ;
    virtual void    PreSubclassWindow() ;
    afx_msg void    OnTimer(UINT_PTR nIDEvent);
    afx_msg void    OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void    OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void    OnMouseLeave();
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    void            initialize();
    DECLARE_MESSAGE_MAP()
};

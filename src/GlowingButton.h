#pragma once

#include <chrono>
#include "GlowingButtonInternal.h"

struct RainbowConfig
{
	bool   animating = false;
	double step      = 8.0;
};

struct RainbowState
{
	bool          enable = false;
	RainbowConfig config;
};

class CGlowingButton : public CButton, public CGlowingButtonInternal
{
public:
	CGlowingButton();
	~CGlowingButton() override;

	CGlowingButton(const CGlowingButton&)            = delete;
	CGlowingButton& operator=(const CGlowingButton&) = delete;
	CGlowingButton(CGlowingButton&&)                 = delete;
	CGlowingButton& operator=(CGlowingButton&&)      = delete;

	void SetHoverBackColor(COLORREF c) { m_hoverBackColor = c; }
	void SetTextColor(COLORREF c)      { m_textColor = c; }
	void SetBorderColor(COLORREF c)    { m_borderColor = c; }
	void SetBackColor(COLORREF c)      { m_backColor = c; }
	void SetPressedColor(COLORREF c)   { m_pressedColor = c; }
	void SetDisabledColor(COLORREF c)  { m_disabledColor = c; }
	void SetRoundRadius(int r);
	void SetAnimDuration(int ms)       { m_animDuration = ms; }
	void SetShowBorder(bool show)      { m_showBorder = show; }

	void SetWindowTextW(LPCWSTR str);
	void SetWindowTextW(int val);
	int  GetWindowTextW(LPWSTR str, int cchMax);

	void EnableRainbowBorder(bool enable = true, double speed = 1.0);
	void DisableRainbowBorder() { EnableRainbowBorder(false); }

	RainbowState m_rainbow;

protected:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	void PreSubclassWindow() override;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void Initialize();
	void UpdateRoundRegion();

	DECLARE_MESSAGE_MAP()

private:
	static constexpr UINT_PTR TIMER_RAINBOW = 2;

	COLORREF m_prevBackColor   = 0;
	COLORREF m_targetBackColor = 0;
	double   m_rainbowOffset   = 0.0;
	HRGN     m_hRgn            = nullptr;

	std::chrono::steady_clock::time_point m_animStartTime;

	BOOL m_bIsDisabled = FALSE;
	BOOL m_bIsFocused  = FALSE;
	BOOL m_bIsPressed  = FALSE;
	BOOL m_bIsHovered  = FALSE;
	BOOL m_bTracking   = FALSE;

	int      m_animDuration   = 100;
	COLORREF m_disabledColor  = RGB(70, 70, 70);
	COLORREF m_backColor      = RGB(40, 40, 40);
	COLORREF m_borderColor    = RGB(20, 20, 20);
	COLORREF m_pressedColor   = RGB(123, 102, 240);
	COLORREF m_hoverBackColor = RGB(40, 40, 40);
	COLORREF m_textColor      = RGB(255, 255, 255);
	int      m_roundRadius    = 0;
	bool     m_showBorder     = true;
};

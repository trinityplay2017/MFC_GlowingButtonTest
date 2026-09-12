#include "pch.h"
#include "GlowingButton.h"

#include <cmath>
#include <algorithm>

BEGIN_MESSAGE_MAP(CGlowingButton, CButton)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CGlowingButton::CGlowingButton()
{
	Initialize();
}

CGlowingButton::~CGlowingButton()
{
	if (m_hRgn)
	{
		::DeleteObject(m_hRgn);
		m_hRgn = nullptr;
	}
	if (m_hWnd)
		::KillTimer(m_hWnd, TIMER_RAINBOW);
}

void CGlowingButton::Initialize()
{
	m_animDuration    = 100;
	m_bIsHovered      = FALSE;
	m_bIsPressed      = FALSE;
	m_bIsFocused      = FALSE;
	m_bIsDisabled     = FALSE;
	m_disabledColor   = RGB(70, 70, 70);
	m_backColor       = RGB(40, 40, 40);
	m_borderColor     = RGB(20, 20, 20);
	m_pressedColor    = RGB(123, 102, 240);
	m_hoverBackColor  = RGB(40, 40, 40);
	m_textColor       = RGB(255, 255, 255);
	m_roundRadius     = 0;
	m_prevBackColor   = m_backColor;
	m_targetBackColor = m_backColor;
	m_rainbowOffset   = 0.0;
}

void CGlowingButton::SetRoundRadius(int radius)
{
	m_roundRadius = radius;
	UpdateRoundRegion();
}

void CGlowingButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	if (cx > 0 && cy > 0)
		UpdateRoundRegion();
}

void CGlowingButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		m_bTracking = TRUE;
		m_bIsHovered = TRUE;
		Invalidate(FALSE);

		TRACKMOUSEEVENT tme{};
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

void CGlowingButton::OnMouseLeave()
{
	m_bTracking  = FALSE;
	m_bIsHovered = FALSE;
	Invalidate(FALSE);
	CButton::OnMouseLeave();
}

void CGlowingButton::UpdateRoundRegion()
{
	if (!m_hWnd)
		return;

	if (m_hRgn)
	{
		::DeleteObject(m_hRgn);
		m_hRgn = nullptr;
	}

	CRect rc;
	GetClientRect(&rc);
	m_hRgn = ::CreateRoundRectRgn(rc.left, rc.top, rc.right + 1, rc.bottom + 1,
	                              m_roundRadius, m_roundRadius);
	SetWindowRgn(m_hRgn, TRUE);
}

void CGlowingButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW | BS_PUSHBUTTON | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	Initialize();
	UpdateRoundRegion();
	::SetTimer(m_hWnd, TIMER_RAINBOW, 33, nullptr); // ~30 fps
	CButton::PreSubclassWindow();
}

void CGlowingButton::OnTimer(UINT_PTR id)
{
	if (id == TIMER_RAINBOW)
	{
		if (m_rainbow.enable)
		{
			m_rainbowOffset -= m_rainbow.config.step;
			if (m_rainbowOffset < 0.0)
				m_rainbowOffset += 360.0;
			Invalidate(FALSE);
		}
	}
}

void CGlowingButton::DrawItem(LPDRAWITEMSTRUCT dis)
{
	CDC* pDC = CDC::FromHandle(dis->hDC);
	CRect rc = dis->rcItem;
	UINT state = dis->itemState;

	if (m_bIsHovered)
		state |= ODS_HOTLIGHT;

	// Double buffer
	CDC memDC;
	CBitmap bmp;
	memDC.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	m_bIsPressed  = (state & ODS_SELECTED) != 0;
	m_bIsFocused  = (state & ODS_FOCUS) != 0;
	m_bIsDisabled = (state & ODS_DISABLED) != 0;

	const bool pressed  = m_bIsPressed;
	const bool disabled = m_bIsDisabled;
	const bool hovered  = m_bIsHovered != FALSE;

	COLORREF targetBack = m_backColor;
	bool     staticBorder = false;

	if (disabled)
	{
		targetBack = m_disabledColor;
		staticBorder = true;
	}
	else if (pressed && hovered)
	{
		targetBack = m_pressedColor;
		staticBorder = true;
	}
	else if (hovered && !pressed)
	{
		targetBack = m_hoverBackColor;
		staticBorder = true;
	}

	if (targetBack != m_targetBackColor)
	{
		m_prevBackColor   = m_targetBackColor;
		m_targetBackColor = targetBack;
		m_animStartTime   = std::chrono::steady_clock::now();
	}

	double t = 1.0;
	if (m_animDuration > 0)
	{
		auto now = std::chrono::steady_clock::now();
		int elapsed = static_cast<int>(
			std::chrono::duration_cast<std::chrono::milliseconds>(now - m_animStartTime).count());
		t = std::clamp(static_cast<double>(elapsed) / m_animDuration, 0.0, 1.0);
	}

	COLORREF clrBack = InternalLerpColor(m_prevBackColor, m_targetBackColor, t);
	memDC.FillSolidRect(&rc, clrBack);

	const int borderPad = 0;
	const int w = rc.Width();
	const int h = rc.Height();
	const int rad = m_roundRadius;

	DrawRainbowRoundedRect(
		memDC.GetSafeHdc(),
		borderPad, borderPad,
		w - borderPad - 1, h - borderPad - 1,
		(rad / 2) + 2,
		m_rainbowOffset,
		1,
		!m_rainbow.enable || staticBorder,
		m_borderColor,
		255);

	// Text
	memDC.SetBkMode(TRANSPARENT);
	CFont* pOldFont = memDC.SelectObject(GetFont());

	wchar_t text[256]{};
	::GetWindowTextW(m_hWnd, text, 256);
	int len = lstrlenW(text);

	if (len > 0)
	{
		SIZE sz{};
		GetTextExtentPoint32W(memDC.GetSafeHdc(), text, len, &sz);
		int x = rc.left + (rc.Width() - sz.cx) / 2;
		int y = rc.top  + (rc.Height() - sz.cy) / 2;

		if (m_rainbow.enable)
		{
			for (int i = 0; i < len; ++i)
			{
				double hue = (static_cast<double>(i) / len) * 360.0 + m_rainbowOffset;
				hue = fmod(hue, 360.0);
				if (hue < 0.0) hue += 360.0;
				memDC.SetTextColor(HSVtoRGB(hue, 1.0, 1.0));
				TextOutW(memDC.GetSafeHdc(), x, y, &text[i], 1);
				int charW = 0;
				GetCharWidth32W(memDC.GetSafeHdc(), text[i], text[i], &charW);
				x += charW;
			}
		}
		else
		{
			memDC.SetTextColor(disabled ? RGB(140, 140, 140) : m_textColor);
			memDC.DrawText(text, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	memDC.SelectObject(pOldFont);
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBmp);
}

void CGlowingButton::EnableRainbowBorder(bool enable, double speed)
{
	m_rainbow.enable = enable;
	m_rainbow.config.step = 4.0 * speed;
	m_rainbow.config.animating = enable;
	if (m_hWnd)
		Invalidate(FALSE);
}

void CGlowingButton::SetWindowTextW(LPCWSTR str)
{
	::SendMessageW(m_hWnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(str));
}

void CGlowingButton::SetWindowTextW(int val)
{
	wchar_t buf[32]{};
	swprintf_s(buf, L"%d", val);
	SetWindowTextW(buf);
}

int CGlowingButton::GetWindowTextW(LPWSTR str, int cchMax)
{
	if (!str || cchMax <= 0)
		return 0;
	return static_cast<int>(::SendMessageW(m_hWnd, WM_GETTEXT, static_cast<WPARAM>(cchMax), reinterpret_cast<LPARAM>(str)));
}

#include "pch.h"
#include "MFC_GlowingButtonTest.h"
#include "MFC_GlowingButtonTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCGlowingButtonTestDlg::CMFCGlowingButtonTestDlg(CWnd* pParent)
	: CDialogEx(IDD_MFC_GLOWINGBUTTONTEST_DIALOG, pParent)
{
}

void CMFCGlowingButtonTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCGlowingButtonTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_TOGGLE_RAINBOW, &CMFCGlowingButtonTestDlg::OnBnClickedToggleRainbow)
END_MESSAGE_MAP()

BOOL CMFCGlowingButtonTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Subclass the placeholder buttons from the dialog resource
	m_btnNormal.SubclassDlgItem(IDC_BTN_NORMAL, this);
	m_btnRainbow.SubclassDlgItem(IDC_BTN_RAINBOW, this);
	m_btnRounded.SubclassDlgItem(IDC_BTN_ROUNDED, this);
	m_btnDisabled.SubclassDlgItem(IDC_BTN_DISABLED, this);
	m_btnToggle.SubclassDlgItem(IDC_BTN_TOGGLE_RAINBOW, this);

	// Common dark theme look
	auto setup = [](CGlowingButton& btn, LPCWSTR text, bool rainbow = false, int radius = 0)
	{
		btn.SetWindowTextW(text);
		btn.SetBackColor(RGB(40, 40, 40));
		btn.SetHoverBackColor(RGB(55, 55, 55));
		btn.SetPressedColor(RGB(123, 102, 240));
		btn.SetTextColor(RGB(240, 240, 240));
		btn.SetBorderColor(RGB(70, 70, 70));
		btn.SetRoundRadius(radius);
		if (rainbow)
			btn.EnableRainbowBorder(true, 2.0);
	};

	setup(m_btnNormal,   L"Normal");
	setup(m_btnRainbow,  L"Rainbow Border", true);
	setup(m_btnRounded,  L"Rounded + Rainbow", true, 12);
	setup(m_btnDisabled, L"Disabled");
	m_btnDisabled.EnableWindow(FALSE);

	setup(m_btnToggle, L"Toggle Rainbow", false, 6);

	return TRUE;
}

void CMFCGlowingButtonTestDlg::OnPaint()
{
	CDialogEx::OnPaint();
}

void CMFCGlowingButtonTestDlg::OnBnClickedToggleRainbow()
{
	static bool on = false;
	on = !on;
	m_btnToggle.EnableRainbowBorder(on);
	m_btnToggle.Invalidate(FALSE);
}

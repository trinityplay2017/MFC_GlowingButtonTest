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

	// Subclass all buttons
	m_btnNormal.SubclassDlgItem(IDC_BTN_NORMAL, this);
	m_btnRainbow.SubclassDlgItem(IDC_BTN_RAINBOW, this);
	m_btnRounded.SubclassDlgItem(IDC_BTN_ROUNDED, this);
	m_btnDisabled.SubclassDlgItem(IDC_BTN_DISABLED, this);
	m_btnToggle.SubclassDlgItem(IDC_BTN_TOGGLE_RAINBOW, this);
	m_btnNoBorder.SubclassDlgItem(IDC_BTN_NO_BORDER, this);
	m_btnBlueBorder.SubclassDlgItem(IDC_BTN_BLUE_BORDER, this);
	m_btnGreenHover.SubclassDlgItem(IDC_BTN_GREEN_HOVER, this);
	m_btnOrangePress.SubclassDlgItem(IDC_BTN_ORANGE_PRESS, this);
	m_btnPill.SubclassDlgItem(IDC_BTN_PILL, this);
	m_btnFlatDark.SubclassDlgItem(IDC_BTN_FLAT_DARK, this);
	m_btnAccent.SubclassDlgItem(IDC_BTN_ACCENT, this);

	// Helper: dark base theme
	auto base = [](CGlowingButton& b)
	{
		b.SetBackColor(RGB(40, 40, 40));
		b.SetHoverBackColor(RGB(55, 55, 55));
		b.SetPressedColor(RGB(123, 102, 240));
		b.SetTextColor(RGB(240, 240, 240));
		b.SetBorderColor(RGB(70, 70, 70));
	};

	// --- Row 1 ---
	base(m_btnNormal);
	m_btnNormal.SetWindowTextW(L"Normal");

	base(m_btnRainbow);
	m_btnRainbow.SetWindowTextW(L"Rainbow Border");
	m_btnRainbow.EnableRainbowBorder(true, 2.0);

	base(m_btnRounded);
	m_btnRounded.SetWindowTextW(L"Rounded + Rainbow");
	m_btnRounded.SetRoundRadius(12);
	m_btnRounded.EnableRainbowBorder(true, 2.0);

	// --- Row 2 ---
	base(m_btnNoBorder);
	m_btnNoBorder.SetWindowTextW(L"No Border");
	m_btnNoBorder.SetShowBorder(false);

	base(m_btnBlueBorder);
	m_btnBlueBorder.SetWindowTextW(L"Blue Border");
	m_btnBlueBorder.SetBorderColor(RGB(0, 140, 255));
	m_btnBlueBorder.SetRoundRadius(6);

	base(m_btnGreenHover);
	m_btnGreenHover.SetWindowTextW(L"Green Hover");
	m_btnGreenHover.SetHoverBackColor(RGB(30, 120, 60));
	m_btnGreenHover.SetBorderColor(RGB(40, 160, 80));
	m_btnGreenHover.SetRoundRadius(6);

	// --- Row 3 ---
	base(m_btnOrangePress);
	m_btnOrangePress.SetWindowTextW(L"Orange Press");
	m_btnOrangePress.SetPressedColor(RGB(220, 110, 30));
	m_btnOrangePress.SetHoverBackColor(RGB(70, 50, 30));
	m_btnOrangePress.SetBorderColor(RGB(180, 90, 20));
	m_btnOrangePress.SetRoundRadius(6);

	base(m_btnPill);
	m_btnPill.SetWindowTextW(L"Pill Shape");
	m_btnPill.SetRoundRadius(20);          // large radius = pill
	m_btnPill.SetBorderColor(RGB(100, 100, 140));
	m_btnPill.SetHoverBackColor(RGB(50, 50, 80));
	m_btnPill.SetPressedColor(RGB(80, 70, 160));

	base(m_btnFlatDark);
	m_btnFlatDark.SetWindowTextW(L"Flat Dark");
	m_btnFlatDark.SetShowBorder(false);
	m_btnFlatDark.SetBackColor(RGB(28, 28, 28));
	m_btnFlatDark.SetHoverBackColor(RGB(45, 45, 45));
	m_btnFlatDark.SetPressedColor(RGB(20, 20, 20));
	m_btnFlatDark.SetTextColor(RGB(180, 180, 180));

	// --- Row 4 ---
	base(m_btnAccent);
	m_btnAccent.SetWindowTextW(L"Accent");
	m_btnAccent.SetBackColor(RGB(123, 102, 240));
	m_btnAccent.SetHoverBackColor(RGB(140, 120, 255));
	m_btnAccent.SetPressedColor(RGB(90, 70, 200));
	m_btnAccent.SetBorderColor(RGB(160, 140, 255));
	m_btnAccent.SetTextColor(RGB(255, 255, 255));
	m_btnAccent.SetRoundRadius(8);

	base(m_btnDisabled);
	m_btnDisabled.SetWindowTextW(L"Disabled");
	m_btnDisabled.EnableWindow(FALSE);

	base(m_btnToggle);
	m_btnToggle.SetWindowTextW(L"Toggle Rainbow");
	m_btnToggle.SetRoundRadius(6);

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

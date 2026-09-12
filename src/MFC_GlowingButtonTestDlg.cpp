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
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCGlowingButtonTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCGlowingButtonTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TOGGLE_RAINBOW, &CMFCGlowingButtonTestDlg::OnBnClickedToggleRainbow)
END_MESSAGE_MAP()

BOOL CMFCGlowingButtonTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

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
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		const int cx = GetSystemMetrics(SM_CXICON);
		const int cy = GetSystemMetrics(SM_CYICON);
		CRect rc;
		GetClientRect(&rc);
		dc.DrawIcon((rc.Width() - cx + 1) / 2, (rc.Height() - cy + 1) / 2, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCGlowingButtonTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCGlowingButtonTestDlg::OnBnClickedToggleRainbow()
{
	static bool on = false;
	on = !on;
	m_btnToggle.EnableRainbowBorder(on);
	m_btnToggle.Invalidate(FALSE);
}

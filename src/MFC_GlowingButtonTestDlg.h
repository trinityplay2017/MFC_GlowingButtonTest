#pragma once

#include "GlowingButton.h"

class CMFCGlowingButtonTestDlg : public CDialogEx
{
public:
	CMFCGlowingButtonTestDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_GLOWINGBUTTONTEST_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedToggleRainbow();
	DECLARE_MESSAGE_MAP()

private:
	CGlowingButton m_btnNormal;
	CGlowingButton m_btnRainbow;
	CGlowingButton m_btnRounded;
	CGlowingButton m_btnDisabled;
	CGlowingButton m_btnToggle;
};

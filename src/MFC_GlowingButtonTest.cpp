#include "pch.h"
#include "MFC_GlowingButtonTest.h"
#include "MFC_GlowingButtonTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CMFCGlowingButtonTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CMFCGlowingButtonTestApp::CMFCGlowingButtonTestApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CMFCGlowingButtonTestApp theApp;

BOOL CMFCGlowingButtonTestApp::InitInstance()
{
	INITCOMMONCONTROLSEX icc{};
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CMFCGlowingButtonTestDlg dlg;
	m_pMainWnd = &dlg;
	const INT_PTR nResponse = dlg.DoModal();
	if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed.\n");
	}

	return FALSE;
}

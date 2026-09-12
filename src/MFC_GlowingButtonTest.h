#pragma once

#ifndef __AFXWIN_H__
	error "include 'pch.h' before including this file"
#endif

#include "Resource.h"

class CMFCGlowingButtonTestApp : public CWinApp
{
public:
	CMFCGlowingButtonTestApp();

public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CMFCGlowingButtonTestApp theApp;

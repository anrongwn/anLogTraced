
// anLogClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CanLogClientApp: 
// �йش����ʵ�֣������ anLogClient.cpp
//

class CanLogClientApp : public CWinApp
{
public:
	CanLogClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CanLogClientApp theApp;

// Cwmsys.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCwmsysApp:
// �йش����ʵ�֣������ Cwmsys.cpp
//

class CCwmsysApp : public CWinAppEx
{
public:
	CCwmsysApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCwmsysApp theApp;
#pragma once

#ifndef AE500_H
#define AE500_H

#include "Comm/SerialPort.h"
#include "SetupDlg.h"

// AE500 对话框

class AE500 : public CDialog
{
	DECLARE_DYNAMIC(AE500)

public:
	AE500(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AE500();

// 对话框数据
	enum { IDD = IDD_AE500 };

public:
	SerialPort	m_Remote;
	SetupDlg *	m_pDlgSetup;

	BOOL OnInitCameraconnect();
	BOOL CloseRemotePort();

	LRESULT OnRemoteCommand(WPARAM wParam, LPARAM lParam);
	BOOL Send(USHORT commond, float Valus, BOOL leftright);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

#endif
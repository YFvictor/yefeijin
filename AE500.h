#pragma once

#ifndef AE500_H
#define AE500_H

#include "Comm/SerialPort.h"
#include "SetupDlg.h"

// AE500 �Ի���

class AE500 : public CDialog
{
	DECLARE_DYNAMIC(AE500)

public:
	AE500(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AE500();

// �Ի�������
	enum { IDD = IDD_AE500 };

public:
	SerialPort	m_Remote;
	SetupDlg *	m_pDlgSetup;

	BOOL OnInitCameraconnect();
	BOOL CloseRemotePort();

	LRESULT OnRemoteCommand(WPARAM wParam, LPARAM lParam);
	BOOL Send(USHORT commond, float Valus, BOOL leftright);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};

#endif
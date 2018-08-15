#pragma once
#include "afxwin.h"


// SetupDlg 对话框

class SetupDlg : public CDialog
{
	DECLARE_DYNAMIC(SetupDlg)

public:
	SetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SetupDlg();

// 对话框数据
	enum { IDD = IDD_SETUP_DIALOG };

	PROPERTY_FUNCTION(CString, CCD5000COM, m_CCD5000Port);
	PROPERTY_FUNCTION(CString, AE500COM, m_AE500Port);
	PROPERTY_FUNCTION(DCB, DCBCCD5000, dcbCCD5000);
	PROPERTY_FUNCTION(DCB, DCBAE500, dcbAE500);

protected:
	DCB dcbCCD5000;
	DCB dcbAE500;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnSave();

public:
	CComboBox m_CombCCD5000Baud;
	CComboBox m_CombCCD5000Data;
	CComboBox m_CombCCD5000Stop;
	CComboBox m_CombCCD5000Parity;
	CEdit m_EditCCD5000Timeout;
	CComboBox m_CombCCD5000Port;
	CComboBox m_CombAE500Baud;
	CComboBox m_CombAE500Data;
	CComboBox m_CombAE500Stop;
	CComboBox m_CombAE500Parity;
	CEdit m_EditAE500Timeout;
	CComboBox m_CombAE500Port;

	CString m_CCD5000Port;
	DWORD m_CCD5000Baud;
	BYTE m_CCD5000Data;
	BYTE m_CCD5000Stop;
	BYTE m_CCD5000Parity;
	DWORD m_CCD5000Timeout;

	CString m_AE500Port;
	DWORD m_AE500Baud;
	BYTE m_AE500Data;
	BYTE m_AE500Stop;
	BYTE m_AE500Parity;
	DWORD m_AE500Timeout;

	CString dwAddress;
	int inport;
	CString strPCNo;
	CString EquipmentProtocolCoding;
	BOOL m_MESState;

protected:
	CString m_FileName;
	void ReadConfig();
	void WriteConfig();

};

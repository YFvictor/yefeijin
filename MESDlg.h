#pragma once
#include "afxwin.h"


// MESDlg �Ի���

class MESDlg : public CDialog
{
	DECLARE_DYNAMIC(MESDlg)

public:
	MESDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MESDlg();

// �Ի�������
	enum { IDD = IDD_MES_DIALOG };

#define PROPERTY_FUNCTION(t, f, v)	const t& f() const{return v;} t& f(){return v;}

	PROPERTY_FUNCTION(int, OutNo, m_OutNo);
private:
	int m_OutNo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();

	void ReadConfig();
	void WriteConfig();

	CString m_Filename;

public:
	CString dwAddress;
	int inport;
	CString strPCNo;
	CString EquipmentProtocolCoding;
	BOOL m_MESState;
};

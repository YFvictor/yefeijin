#pragma once
#include "afxwin.h"


// CStandardDlg �Ի���

class CStandardDlg : public CDialog
{
	DECLARE_DYNAMIC(CStandardDlg)

public:
	CStandardDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStandardDlg();

// �Ի�������
	enum { IDD = IDD_STANDARD_VAL };
    PROPERTY_FUNCTION(DWORD, Material, m_dwMaterial);
    PROPERTY_FUNCTION(DWORD, Ear, m_dwear);
	PROPERTY_FUNCTION(DWORD, Show, m_dwShow);
	PROPERTY_FUNCTION(double, Warn, m_Warn);
	PROPERTY_FUNCTION(double, Alarm, m_Alarm);

public:
	void ReadConfig();
	void WriteConfig();

public:
	DWORD	m_dwMaterial;
	DWORD   m_dwear;
	DWORD  m_dwShow;
	double  m_Warn;
	double  m_Alarm;
	double	m_updwMaterial1;
	double	m_updwMaterial2;
	double	m_updwMaterial3;
	double	m_updwEar1;
	double	m_updwEar2;
	double	m_updwEar3;
    double	m_updwEar4;
	double	m_downdwMaterial1;
	double	m_downdwMaterial2;
	double	m_downdwMaterial3;
	double	m_downdwEar1;
	double	m_downdwEar2;
	double	m_downdwEar3;
	double	m_downdwEar4;
	

protected:
	CString m_FileName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CComboBox m_combo;
};

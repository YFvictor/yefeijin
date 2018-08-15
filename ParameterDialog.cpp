// ParameterDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "ParameterDialog.h"
#include "MainDialog.h"

// ParameterDialog 对话框

IMPLEMENT_DYNAMIC(ParameterDialog, CDialog)

ParameterDialog::ParameterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ParameterDialog::IDD, pParent)
	, m_nStatistics(10000)
	, m_dUpperlimit(0.5)
	, m_dLowerlimit(0.5)
	, m_nRoolNum(2)
{
	m_FileName = GetFileName(_T("Config.ini"));
	ReadConfig();
}

ParameterDialog::~ParameterDialog()
{
}

void ParameterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ParameterDialog, CDialog)
	ON_BN_CLICKED(IDOK, &ParameterDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_SET, &ParameterDialog::OnBnClickedRadioSet)
	ON_BN_CLICKED(IDC_RADIO_AUTO, &ParameterDialog::OnBnClickedRadioAuto)
	ON_CBN_SELCHANGE(IDC_COMBO_POSITIVE, &ParameterDialog::OnCbnSelchangeComboPositive)
END_MESSAGE_MAP()


// ParameterDialog 消息处理程序

BOOL ParameterDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemInt(IDC_EDIT_TONGJI, m_nStatistics);
	CString str;
	str.Format(_T("%.1f"), m_dUpperlimit);
	SetDlgItemText(IDC_EDIT_UPPER, str);
	str.Format(_T("%.1f"), m_dLowerlimit);
	SetDlgItemText(IDC_EDIT_LOWER, str);
	str.Format(_T("%d"), m_nRoolNum);
	SetDlgItemText(IDC_EDIT_ROOL_NUM, str);

	((CButton*)GetDlgItem(IDC_RADIO_AUTO))->SetCheck(m_bIsAuto);
	((CButton*)GetDlgItem(IDC_RADIO_SET))->SetCheck(!m_bIsAuto);

	if (((CButton*)GetDlgItem(IDC_RADIO_AUTO))->GetCheck())
	{
		SetDlgItemText(IDC_EDIT_ROOL_NUM, _T(""));
		GetDlgItem(IDC_EDIT_ROOL_NUM)->EnableWindow(FALSE);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_POSITIVE))->SetCurSel(Positive);
    ((CComboBox*)GetDlgItem(IDC_COMBO_AT9))->SetCurSel(ExistAt9);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void ParameterDialog::ReadConfig()
{
	m_nStatistics = ReadFile_l(_T("Para"), _T("statistics"), 10000, m_FileName);
	m_dUpperlimit = ReadFile_f(_T("Para"), _T("upperlimit"), 0.5, m_FileName);
	m_dLowerlimit = ReadFile_f(_T("Para"), _T("lowerlimit"), 0.5, m_FileName);
	m_nRoolNum	  =	ReadFile_l(_T("Para"), _T("roolnum"),	 0, m_FileName);
	m_nRoolMode	  =	ReadFile_l(_T("Para"), _T("m_nRoolMode"), 0, m_FileName);

	m_bIsAuto	  =	ReadFile_l(_T("Para"), _T("isauto"),	 0, m_FileName);

	Positive = ReadFile_b(_T("Polarity"), _T("Positive"), Positive, m_FileName);
	ExistAt9 = ReadFile_b(_T("Polarity"), _T("ExistAt9"), ExistAt9, m_FileName);
}

void ParameterDialog::WriteConfig()
{
	WriteFile_l(_T("Para"), _T("statistics"), m_nStatistics, m_FileName);
	WriteFile_f(_T("Para"), _T("upperlimit"), m_dUpperlimit, 2, m_FileName);
	WriteFile_f(_T("Para"), _T("lowerlimit"), m_dLowerlimit, 2, m_FileName);
	WriteFile_l(_T("Para"), _T("isauto"),	  m_bIsAuto, m_FileName);
	WriteFile_b(_T("Polarity"), _T("Positive"), Positive, m_FileName);
	WriteFile_b(_T("Polarity"), _T("ExistAt9"), ExistAt9, m_FileName);
	if (!m_bIsAuto)
	{
		WriteFile_l(_T("Para"), _T("roolnum"), m_nRoolNum, m_FileName);
		WriteFile_l(_T("Para"), _T("m_nRoolMode"), m_nRoolMode, m_FileName);
	}
}

void ParameterDialog::OnBnClickedOk()
{
	CString str;
	m_nStatistics = GetDlgItemInt(IDC_EDIT_TONGJI);
	GetDlgItemText(IDC_EDIT_UPPER, str);
	m_dUpperlimit = _wtof(str);
	GetDlgItemText(IDC_EDIT_LOWER, str);
	m_dLowerlimit = _wtof(str);

	int num = 0;
	if (((CButton*)GetDlgItem(IDC_RADIO_SET))->GetCheck())
	{
		GetDlgItemText(IDC_EDIT_ROOL_NUM, str);
		num = _wtoi(str);
		if (num == 0 || num > 3)
		{
			AfxMessageBox(_T("输入数量错误!"));
			return;
		}
		m_nRoolNum = num;
	}

	if (((CButton*)GetDlgItem(IDC_RADIO_AUTO))->GetCheck())
		m_bIsAuto = TRUE;
	else
		m_bIsAuto = FALSE;

	Positive = ((CComboBox*)GetDlgItem(IDC_COMBO_POSITIVE))->GetCurSel() <= 0 ? 0 : 1;
	ExistAt9 = ((CComboBox*)GetDlgItem(IDC_COMBO_AT9))->GetCurSel() <= 0 ? 0 : 1;
	if (ExistAt9)
	{	
		m_nRoolMode =m_nRoolNum + 3;
	}
	else
	{
       m_nRoolMode = m_nRoolNum;
	}
	
	WriteConfig();

	((CMainDialog*)AfxGetMainWnd())->OnChangeDisp(m_nRoolMode);
	((CMainDialog*)AfxGetMainWnd())->ShowWindow(SW_MAXIMIZE);
}

void ParameterDialog::OnBnClickedRadioSet()
{
	if (((CButton*)GetDlgItem(IDC_RADIO_SET))->GetCheck())
	{
		CString str;
		str.Format(_T("%d"), m_nRoolNum);
		SetDlgItemText(IDC_EDIT_ROOL_NUM, str);
		GetDlgItem(IDC_EDIT_ROOL_NUM)->EnableWindow(TRUE);
	}
}

void ParameterDialog::OnBnClickedRadioAuto()
{
	if (((CButton*)GetDlgItem(IDC_RADIO_AUTO))->GetCheck())
	{
		SetDlgItemText(IDC_EDIT_ROOL_NUM, _T(""));
		GetDlgItem(IDC_EDIT_ROOL_NUM)->EnableWindow(FALSE);
	}
}

void ParameterDialog::OnCbnSelchangeComboPositive()
{

	int Cursel = ((CComboBox*)GetDlgItem(IDC_COMBO_POSITIVE))->GetCurSel();

	if (Cursel)
	{
		GetDlgItem(IDC_COMBO_AT9)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_AT9)->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_COMBO_AT9))->SetCurSel(FALSE);
	}

}

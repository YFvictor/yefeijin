// DlgStandard.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "StandardDlg.h"
#include "ReadWriteFile.h"
#include "MainDialog.h"

// CStandardDlg 对话框

IMPLEMENT_DYNAMIC(CStandardDlg, CDialog)

CStandardDlg::CStandardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStandardDlg::IDD, pParent)
{
	m_FileName = GetFileName(_T("Config.ini"));
	ReadConfig();
	if (!m_dwShow)
	{
		m_dwShow = 1000;
		WriteConfig();
	}
}

CStandardDlg::~CStandardDlg()
{
}

void CStandardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SHOW, m_combo);
}


BEGIN_MESSAGE_MAP(CStandardDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CStandardDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CStandardDlg 消息处理程序

void CStandardDlg::OnBnClickedOk()
{
	m_dwMaterial = GetDlgItemInt(IDC_EDITMATERIAL_WIDTH);
	m_dwear = GetDlgItemInt(IDC_EDITEAR_WIDTH);

	CString upear1,upear2,upear3,upear4,upmaterial1,upmaterial2,upmaterial3;
	CString downear1,downear2,downear3,downear4,downmaterial1,downmaterial2,downmaterial3;
	GetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH, upmaterial1);
	m_updwMaterial1 = _wtof(upmaterial1);
	GetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH2, upmaterial2);
	m_updwMaterial2 = _wtof(upmaterial2);
	GetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH3, upmaterial3);
	m_updwMaterial3 = _wtof(upmaterial3);
	GetDlgItemText(IDC_EDIT_UPEAR_WIDTH, upear1);
	m_updwEar1 = _wtof(upear1);
    GetDlgItemText(IDC_EDIT_UPEAR_WIDTH2, upear2);
	m_updwEar2 = _wtof(upear2);
	GetDlgItemText(IDC_EDIT_UPEAR_WIDTH3, upear3);
	m_updwEar3 = _wtof(upear3);
	GetDlgItemText(IDC_EDIT_UPEAR_WIDTH4, upear4);
    m_updwEar4 = _wtof(upear4);
	GetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH, downmaterial1);
	m_downdwMaterial1 = _wtof(downmaterial1);
	GetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH2, downmaterial2);
	m_downdwMaterial2 = _wtof(downmaterial2);
	GetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH3, downmaterial3);
	m_downdwMaterial3 = _wtof(downmaterial3);
    GetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH, downear1);
	m_downdwEar1 = _wtof(downear1);
	GetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH2, downear2);
	m_downdwEar2 = _wtof(downear2); 
	GetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH3, downear3);
	m_downdwEar3 = _wtof(downear3);
	GetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH4, downear4);
	m_downdwEar4 = _wtof(downear4);

	CString strText,strtext;
	GetDlgItemText(IDC_EDIT_EARLYWARMING, strText); // double
	m_Warn = _wtof(strText);
	GetDlgItemText(IDC_EDIT_ALARM, strtext); // double
	m_Alarm = _wtof(strtext);
	m_dwShow = m_combo.GetItemData(m_combo.GetCurSel());
	if (m_Warn >= m_Alarm)
	{
		AfxMessageBox(_T("报警公差必须大于预警公差！"));
		return;
	}
	WriteConfig();
	((CMainDialog*)AfxGetMainWnd())->PostMessage(WM_PAINT, 0, 0);
}

void CStandardDlg::ReadConfig()
{
	m_dwMaterial = ReadFile_l(_T("standardval"), _T("material"), 0, m_FileName);
	m_dwear = ReadFile_l(_T("standardval"), _T("ear"), 0, m_FileName);
	m_dwShow = ReadFile_l(_T("standardval"), _T("show"), 0, m_FileName);
	m_Warn = ReadFile_f(_T("standardval"), _T("Warn"), 0, m_FileName);
	m_Alarm = ReadFile_f(_T("standardval"), _T("Alarm"), 0, m_FileName);
	m_updwMaterial1 = ReadFile_f(_T("standardval"), _T("upmaterial1"), 0, m_FileName);
	m_updwMaterial2 = ReadFile_f(_T("standardval"), _T("upmaterial2"), 0, m_FileName);
	m_updwMaterial3 = ReadFile_f(_T("standardval"), _T("upmaterial3"), 0, m_FileName);
	m_updwEar1 = ReadFile_f(_T("standardval"), _T("upear1"), 0, m_FileName);
	m_updwEar2 = ReadFile_f(_T("standardval"), _T("upear2"), 0, m_FileName);
	m_updwEar3 = ReadFile_f(_T("standardval"), _T("upear3"), 0, m_FileName);
	m_updwEar4 = ReadFile_f(_T("standardval"), _T("upear4"), 0, m_FileName);
	m_downdwMaterial1 = ReadFile_f(_T("standardval"), _T("downmaterial1"), 0, m_FileName);
	m_downdwMaterial2 = ReadFile_f(_T("standardval"), _T("downmaterial2"), 0, m_FileName);
	m_downdwMaterial3 = ReadFile_f(_T("standardval"), _T("downmaterial3"), 0, m_FileName);
	m_downdwEar1 = ReadFile_f(_T("standardval"), _T("downear1"), 0, m_FileName);
	m_downdwEar2 = ReadFile_f(_T("standardval"), _T("downear2"), 0, m_FileName);
	m_downdwEar3 = ReadFile_f(_T("standardval"), _T("downear3"), 0, m_FileName);
	m_downdwEar4 = ReadFile_f(_T("standardval"), _T("downear4"), 0, m_FileName);
	
}

void CStandardDlg::WriteConfig()
{
	WriteFile_l(_T("standardval"), _T("material"), m_dwMaterial, m_FileName);
	WriteFile_l(_T("standardval"), _T("ear"), m_dwear, m_FileName);
	WriteFile_l(_T("standardval"), _T("show"), m_dwShow, m_FileName);
	WriteFile_f(_T("standardval"), _T("Warn"), m_Warn, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("Alarm"), m_Alarm, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upmaterial1"), m_updwMaterial1, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upmaterial2"), m_updwMaterial2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upmaterial3"), m_updwMaterial3, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upear1"), m_updwEar1, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upear2"), m_updwEar2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upear3"), m_updwEar2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("upear4"), m_updwEar2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downmaterial1"), m_downdwMaterial1, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downmaterial2"), m_downdwMaterial2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downmaterial3"), m_downdwMaterial3, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downear1"), m_downdwEar1, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downear2"), m_downdwEar2, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downear3"), m_downdwEar3, 2, m_FileName);
	WriteFile_f(_T("standardval"), _T("downear4"), m_downdwEar4, 2, m_FileName);

}

BOOL CStandardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_combo.SetItemData(m_combo.AddString(_T("10:1")), 10000);	// 一网格表示200
	m_combo.SetItemData(m_combo.AddString(_T("5:1")), 5000);	// 一网格表示100
	m_combo.SetItemData(m_combo.AddString(_T("4:1")), 4000);	// 一网格表示80
	m_combo.SetItemData(m_combo.AddString(_T("2:1")), 2000);	// 一网格表示40
	m_combo.SetItemData(m_combo.AddString(_T("1:1")), 1000);	// 一网格表示20
	m_combo.SetItemData(m_combo.AddString(_T("1:2")), 500);		// 一网格表示10
	m_combo.SetItemData(m_combo.AddString(_T("1:4")), 250);		// 一网格表示5
	m_combo.SetItemData(m_combo.AddString(_T("1:5")), 200);		// 一网格表示4
	m_combo.SetItemData(m_combo.AddString(_T("1:10")), 100);	// 一网格表示2
	m_combo.SetItemData(m_combo.AddString(_T("1:20")), 50);		// 一网格表示1
	m_combo.SetItemData(m_combo.AddString(_T("1:40")), 25);		// 一网格表示0.5
	
	int nSel = 0;
	switch (m_dwShow)
	{
	case 10000:
		nSel = 0;
		break;

	case 5000:
		nSel = 1;
		break;

	case 4000: 
		nSel = 2;
		break;

	case 2000:
		nSel = 3;
		break;

	case 1000:
		nSel = 4;
		break;

	case 500:
		nSel = 5;
		break;

	case 250:
		nSel = 6;
		break;

	case 200:
		nSel = 7;
		break;

	case 100:
		nSel = 8;
		break;

	case 50:
		nSel = 9;
		break;

	case 25:
		nSel = 10;
		break;

	default:
		break;
	}
	m_combo.SetCurSel(nSel);

	CString str;

	str.Format(_T("%d"), m_dwMaterial);
	SetDlgItemText(IDC_EDITMATERIAL_WIDTH, str);

	str.Format(_T("%d"), m_dwear);
	SetDlgItemText(IDC_EDITEAR_WIDTH, str);

	str.Format(_T("%.2f"), m_Warn);
	SetDlgItemText(IDC_EDIT_EARLYWARMING, str);

	str.Format(_T("%.2f"), m_Alarm);
	SetDlgItemText(IDC_EDIT_ALARM, str);

	str.Format(_T("%.2f"), m_updwMaterial1);
	SetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH, str);
	
	str.Format(_T("%.2f"), m_updwMaterial2);
	SetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH2, str);

	str.Format(_T("%.2f"), m_updwMaterial3);
	SetDlgItemText(IDC_EDIT_UPMATERIAL_WIDTH3, str);

	str.Format(_T("%.2f"), m_updwEar1);
	SetDlgItemText(IDC_EDIT_UPEAR_WIDTH, str);

	str.Format(_T("%.2f"), m_updwEar2);
	SetDlgItemText(IDC_EDIT_UPEAR_WIDTH2, str);

	str.Format(_T("%.2f"), m_updwEar3);
	SetDlgItemText(IDC_EDIT_UPEAR_WIDTH3, str);

	str.Format(_T("%.2f"), m_updwEar4);
	SetDlgItemText(IDC_EDIT_UPEAR_WIDTH4, str);

	str.Format(_T("%.2f"), m_downdwMaterial1);
	SetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH, str);

	str.Format(_T("%.2f"), m_downdwMaterial2);
	SetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH2, str);

	str.Format(_T("%.2f"), m_downdwMaterial3);
	SetDlgItemText(IDC_EDIT_DOWNMATERIAL_WIDTH3, str);

	str.Format(_T("%.2f"), m_downdwEar1);
	SetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH, str);

	str.Format(_T("%.2f"), m_downdwEar2);
	SetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH2, str);

	str.Format(_T("%.2f"), m_downdwEar3);
	SetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH3, str);

	str.Format(_T("%.2f"), m_downdwEar4);
	SetDlgItemText(IDC_EDIT_DOWNEAR_WIDTH4, str);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// MESDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "MESDlg.h"
#include "ReadWriteFile.h"
// MESDlg 对话框

IMPLEMENT_DYNAMIC(MESDlg, CDialog)

MESDlg::MESDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MESDlg::IDD, pParent)
	, dwAddress(_T("192.168.1.1"))
	, inport(8000)
	, strPCNo(_T("100000000010920161010000000001"))
	, EquipmentProtocolCoding(_T("0210"))
	, m_MESState(0)
{
	m_Filename = GetFileName(_T("Configure.ini"));
	ReadConfig();
}

MESDlg::~MESDlg()
{

}

void MESDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MESDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &MESDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// MESDlg 消息处理程序

BOOL MESDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_IP_MES)->SetWindowText(dwAddress);
	SetDlgItemInt(IDC_EDIT_PORT_MES, inport);
	GetDlgItem(IDC_EDIT_MES_PC)->SetWindowText(strPCNo);
	GetDlgItem(IDC_EDIT_MES_EQ)->SetWindowText(EquipmentProtocolCoding);
	((CComboBox *)GetDlgItem(IDC_COMBO_STUTES))->SetCurSel(m_MESState);

	return TRUE;
}


void MESDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void MESDlg::ReadConfig()
{
	dwAddress = ReadFile_s(_T("MES"), _T("dwAddress"), dwAddress, m_Filename);

	inport = ReadFile_l(_T("MES"), _T("port"), _T("0"), m_Filename);
	strPCNo = ReadFile_s(_T("MES"), _T("PCNo"), strPCNo, m_Filename);
	EquipmentProtocolCoding = ReadFile_s(_T("MES"), _T("EQ"), EquipmentProtocolCoding, m_Filename);
	m_MESState = ReadFile_l(_T("MES"), _T("States"), _T("0"), m_Filename);
}

void MESDlg::WriteConfig()
{
	WriteFile_s(_T("MES"), _T("dwAddress"), dwAddress, m_Filename);
	WriteFile_l(_T("MES"), _T("port"), inport, m_Filename);
	WriteFile_s(_T("MES"), _T("PCNo"), strPCNo, m_Filename);
	WriteFile_s(_T("MES"), _T("EQ"), EquipmentProtocolCoding, m_Filename);
	WriteFile_l(_T("MES"), _T("States"), m_MESState, m_Filename);
}

void MESDlg::OnBnClickedOk()
{
	GetDlgItemText(IDC_IP_MES, dwAddress);
	if (dwAddress.IsEmpty() == true)
	{
		AfxMessageBox(_T("IP地址不能为空"));
		ReadConfig();
		return;
	}
	inport = GetDlgItemInt(IDC_EDIT_PORT_MES);
	GetDlgItemText(IDC_EDIT_MES_PC, strPCNo);
	if (strPCNo.GetLength() != 30)
	{
		AfxMessageBox(_T("上位机设备编号没有30位"));
		ReadConfig();
		return;
	}
	GetDlgItemText(IDC_EDIT_MES_EQ, EquipmentProtocolCoding);
	if (EquipmentProtocolCoding.GetLength() != 4)
	{
		AfxMessageBox(_T("设备协议编码位4位"));
		ReadConfig();
		return;
	}

	m_MESState = ((CComboBox*)GetDlgItem(IDC_COMBO_STUTES))->GetCurSel();

	WriteConfig();
// 	OnOK();
}

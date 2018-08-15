// SetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "SetupDlg.h"
#include "ReadWriteFile.h"


// SetupDlg 对话框

IMPLEMENT_DYNAMIC(SetupDlg, CDialog)

SetupDlg::SetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetupDlg::IDD, pParent)
	, m_CCD5000Port(_T("COM1"))
	, m_CCD5000Baud(9600)
	, m_CCD5000Data(8)
	, m_CCD5000Stop(0)
	, m_CCD5000Parity(0)
	, m_CCD5000Timeout(5000)

	, m_AE500Port(_T("COM2"))
	, m_AE500Baud(9600)
	, m_AE500Data(8)
	, m_AE500Stop(0)
	, m_AE500Parity(0)
	, m_AE500Timeout(5000)
{
	m_FileName = GetFileName(_T("Config.ini"));
	ReadConfig();
	dcbCCD5000.BaudRate = m_CCD5000Baud;
	dcbCCD5000.StopBits = m_CCD5000Stop;
	dcbCCD5000.ByteSize = m_CCD5000Data;
	dcbCCD5000.Parity = m_CCD5000Parity;

	dcbAE500.BaudRate = m_AE500Baud;
	dcbAE500.StopBits = m_AE500Stop;
	dcbAE500.ByteSize = m_AE500Data;
	dcbAE500.Parity = m_AE500Parity;
}
SetupDlg::~SetupDlg()
{
}

void SetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CCD5000_BAUDRATE, m_CombCCD5000Baud);
	DDX_Control(pDX, IDC_CCD5000_DATABITS, m_CombCCD5000Data);
	DDX_Control(pDX, IDC_CCD5000_STOPBITS, m_CombCCD5000Stop);
	DDX_Control(pDX, IDC_CCD5000_PARITY, m_CombCCD5000Parity);
	DDX_Control(pDX, IDC_CCD5000_TIMEOUT, m_EditCCD5000Timeout);
	DDX_Control(pDX, IDC_CCD5000_PORT, m_CombCCD5000Port);
	DDX_Control(pDX, IDC_AE500_BAUDRATE, m_CombAE500Baud);
	DDX_Control(pDX, IDC_AE500_DATABITS, m_CombAE500Data);
	DDX_Control(pDX, IDC_AE500_STOPBITS, m_CombAE500Stop);
	DDX_Control(pDX, IDC_AE500_PARITY, m_CombAE500Parity);
	DDX_Control(pDX, IDC_AE500_TIMEOUT, m_EditAE500Timeout);
	DDX_Control(pDX, IDC_AE500_PORT, m_CombAE500Port);
}


BEGIN_MESSAGE_MAP(SetupDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SAVE, &SetupDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// SetupDlg 消息处理程序

BOOL SetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStringArray txSerialPorts;

	EnumSerialPorts(txSerialPorts,0);
	
	for (int i(0); i < txSerialPorts.GetSize(); i ++)
	{
		((CComboBox*)GetDlgItem(IDC_CCD5000_PORT))->InsertString(i, txSerialPorts.GetAt(i));
	}

	for (int i(0); i < txSerialPorts.GetSize(); i ++)
	{
		((CComboBox*)GetDlgItem(IDC_AE500_PORT))->InsertString(i, txSerialPorts.GetAt(i));
	}
	CString m_Str;

	m_CombCCD5000Port.SetWindowText(m_CCD5000Port);

	m_Str.Format(_T("%d"), m_CCD5000Baud);
	m_CombCCD5000Baud.SetWindowText(m_Str);
	m_CombCCD5000Data.SetCurSel(m_CCD5000Data - 4);
	m_CombCCD5000Stop.SetCurSel(m_CCD5000Stop);
	m_CombCCD5000Parity.SetCurSel(m_CCD5000Parity);

	m_Str.Format(_T("%d"), m_CCD5000Timeout);
	m_EditCCD5000Timeout.SetWindowText(m_Str);

	m_CombAE500Port.SetWindowText(m_AE500Port);

	m_Str.Format(_T("%d"), m_AE500Baud);
	m_CombAE500Baud.SetWindowText(m_Str);
	m_CombAE500Data.SetCurSel(m_AE500Data - 4);
	m_CombAE500Stop.SetCurSel(m_AE500Stop);
	m_CombAE500Parity.SetCurSel(m_AE500Parity);

	m_Str.Format(_T("%d"), m_AE500Timeout);
	m_EditAE500Timeout.SetWindowText(m_Str);

	GetDlgItem(IDC_IP_MES)->SetWindowText(dwAddress);
	SetDlgItemInt(IDC_EDIT_PORT_MES, inport);
	GetDlgItem(IDC_EDIT_MES_PC)->SetWindowText(strPCNo);
	GetDlgItem(IDC_EDIT_MES_EQ)->SetWindowText(EquipmentProtocolCoding);
	((CComboBox *)GetDlgItem(IDC_COMBO_STUTES))->SetCurSel(m_MESState);

	return TRUE;

}

void SetupDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

void SetupDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void SetupDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
}

void SetupDlg::OnBnClickedBtnSave()
{
	m_CombCCD5000Port.GetWindowText(m_CCD5000Port);
	CString m_csTR;
	m_CombCCD5000Baud.GetWindowText(m_csTR);
	m_CCD5000Baud = _ttoi(m_csTR);
	m_CCD5000Data = m_CombCCD5000Data.GetCurSel() + 4;
	m_CCD5000Parity = m_CombCCD5000Parity.GetCurSel();
	m_CCD5000Stop = m_CombCCD5000Stop.GetCurSel();
	m_EditCCD5000Timeout.GetDlgItemInt(m_CCD5000Timeout);

	m_CombAE500Port.GetWindowText(m_AE500Port);
	m_CombAE500Baud.GetWindowText(m_csTR);
	m_AE500Baud = _ttoi(m_csTR);
	m_AE500Data = m_CombAE500Data.GetCurSel() + 4;
	m_AE500Parity = m_CombAE500Parity.GetCurSel();
	m_AE500Stop = m_CombAE500Stop.GetCurSel();
	m_EditAE500Timeout.GetDlgItemInt(m_AE500Timeout);

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
}

void SetupDlg::ReadConfig()
{
	m_CCD5000Port = ReadFile_s(_T("CCD5000"), _T("Port"), m_CCD5000Port, m_FileName);
	m_CCD5000Baud = ReadFile_l(_T("CCD5000"), _T("Baud"), 115200, m_FileName);
	m_CCD5000Data = (byte)ReadFile_l(_T("CCD5000"), _T("Data"), 8, m_FileName);
	m_CCD5000Stop = (byte)ReadFile_l(_T("CCD5000"), _T("Stop"), 0, m_FileName);
	m_CCD5000Parity = (byte)ReadFile_l(_T("CCD5000"), _T("Parity"), 0, m_FileName);
	m_CCD5000Timeout = ReadFile_l(_T("CCD5000"), _T("Timeout"), 5000, m_FileName);

	m_AE500Port = ReadFile_s(_T("AE500"), _T("Port"), m_AE500Port, m_FileName);
	m_AE500Baud = ReadFile_l(_T("AE500"), _T("Baud"), 115200, m_FileName);
	m_AE500Data = (byte)ReadFile_l(_T("AE500"), _T("Data"), 8, m_FileName);
	m_AE500Stop = (byte)ReadFile_l(_T("AE500"), _T("Stop"), 0, m_FileName);
	m_AE500Parity = (byte)ReadFile_l(_T("AE500"), _T("Parity"), 0, m_FileName);
	m_AE500Timeout = ReadFile_l(_T("AE500"), _T("Timeout"), 5000, m_FileName);

	dwAddress = ReadFile_s(_T("MES"), _T("dwAddress"), dwAddress, m_FileName);
	inport = ReadFile_l(_T("MES"), _T("port"), 0, m_FileName);
	strPCNo = ReadFile_s(_T("MES"), _T("PCNo"), strPCNo, m_FileName);
	EquipmentProtocolCoding = ReadFile_s(_T("MES"), _T("EQ"), EquipmentProtocolCoding, m_FileName);
	m_MESState = ReadFile_l(_T("MES"), _T("States"), 0, m_FileName);
}

void SetupDlg::WriteConfig()
{
	WriteFile_s(_T("CCD5000"), _T("Port"), m_CCD5000Port, m_FileName);
	WriteFile_l(_T("CCD5000"), _T("Baud"), m_CCD5000Baud, m_FileName);
	WriteFile_l(_T("CCD5000"), _T("Data"), m_CCD5000Data, m_FileName);
	WriteFile_l(_T("CCD5000"), _T("Stop"), m_CCD5000Stop, m_FileName);
	WriteFile_l(_T("CCD5000"), _T("Parity"), m_CCD5000Parity, m_FileName);
	WriteFile_l(_T("CCD5000"), _T("Timeout"), m_CCD5000Timeout, m_FileName);

	WriteFile_s(_T("AE500"), _T("Port"), m_AE500Port, m_FileName);
	WriteFile_l(_T("AE500"), _T("Baud"), m_AE500Baud, m_FileName);
	WriteFile_l(_T("AE500"), _T("Data"), m_AE500Data, m_FileName);
	WriteFile_l(_T("AE500"), _T("Stop"), m_AE500Stop, m_FileName);
	WriteFile_l(_T("AE500"), _T("Parity"), m_AE500Parity, m_FileName);
	WriteFile_l(_T("AE500"), _T("Timeout"), m_AE500Timeout, m_FileName);

	WriteFile_s(_T("MES"), _T("dwAddress"), dwAddress, m_FileName);
	WriteFile_l(_T("MES"), _T("port"), inport, m_FileName);
	WriteFile_s(_T("MES"), _T("PCNo"), strPCNo, m_FileName);
	WriteFile_s(_T("MES"), _T("EQ"), EquipmentProtocolCoding, m_FileName);
	WriteFile_l(_T("MES"), _T("States"), m_MESState, m_FileName);
}


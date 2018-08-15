// AE500.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "AE500.h"


// AE500 �Ի���

IMPLEMENT_DYNAMIC(AE500, CDialog)

AE500::AE500(CWnd* pParent /*=NULL*/)
	: CDialog(AE500::IDD, pParent)
{

}

AE500::~AE500()
{
}

void AE500::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AE500, CDialog)
	ON_MESSAGE(ON_COM_RECEIVED, OnRemoteCommand)
END_MESSAGE_MAP()


// AE500 ��Ϣ�������

BOOL AE500::OnInitCameraconnect()
{
	if(!CloseRemotePort())
	{
		return FALSE;
	}

	if(!m_Remote.RegisterNotifyWindow(m_hWnd))
	{
		return false;
	}

	if(!m_Remote.OpenPort(m_pDlgSetup->AE500COM()))
	{
		MessageBox(m_pDlgSetup->AE500COM(), _T("Զ�̿��ƶ˿ڳ�ʼ��ʧ��"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	COMMCONFIG _Config;
	DWORD _Size(sizeof(_Config));
	memset(&_Config, 0, sizeof(_Config));

	if(!m_Remote.GetConfig(&_Config, _Size))
	{
		MessageBox(m_pDlgSetup->AE500COM(), _T("Զ�̿��ƶ˿ڳ�ʼ��ʧ��"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	_Config.dcb.fParity = 1;
	_Config.dcb.Parity = m_pDlgSetup->DCBAE500().Parity;
	_Config.dcb.BaudRate = m_pDlgSetup->DCBAE500().BaudRate;
	_Config.dcb.ByteSize = m_pDlgSetup->DCBAE500().ByteSize;
	_Config.dcb.StopBits = m_pDlgSetup->DCBAE500().StopBits;

	if(!m_Remote.SetConfig(&_Config, sizeof(_Config)))
	{
		MessageBox(m_pDlgSetup->AE500COM(), _T("Զ�̿��ƶ˿ڳ�ʼ��ʧ��"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

BOOL AE500::CloseRemotePort()
{
	m_Remote.ClosePort();
	m_Remote.UnregisterNotifyWindow(m_hWnd);
	return TRUE;
}

LRESULT AE500::OnRemoteCommand(WPARAM wParam, LPARAM lParam)
{
	TRACE1("Received: %i\r\n", wParam);

	BYTE lpBuffer[2048];

	DWORD dwLength(m_Remote.GetInput(lpBuffer, 2048));

	return true;
}

BOOL AE500::Send(USHORT commond, float Valus, BOOL leftright)
{
	//ADR = 0x01;
	// leftright Ϊ0; ��Left; ΪTURE, ��Right;
	BYTE m_buff[13];

	m_buff[0] = 0x01;
	m_buff[1] = 0x10;
	m_buff[2] = 0x00; // �Ĵ������ֽ�

	if (leftright == FALSE)
	{
		m_buff[3] = 0x10;
	}
	else
	{
		m_buff[3] = 0x20;
	}

	m_buff[4] = 0x00;
	m_buff[5] = 0x02;
	m_buff[6] = 0x04;
	memcpy(&m_buff[7], &Valus, sizeof(Valus));
	USHORT m_crc16;
	m_crc16 = CRC16(m_buff, 11);

	memcpy(&m_buff[11], &m_crc16, sizeof(m_crc16));

	DWORD Length;

	m_Remote.Send(m_buff, Length);
	if (Length <= 0)
	{
		AfxMessageBox(_T("����ʧ�ܣ�"));
	}
	return true;
}
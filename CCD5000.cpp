// CCD5000.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "CCD5000.h"
#include "MainDialog.h"

// CCD5000 对话框


#define MAXBLOCK 4096
#define XON  0x11     
#define XOFF 0x13

float m_ProSpeed;
float m_LimitSpeed;

IMPLEMENT_DYNAMIC(CCD5000, CDialog)

CCD5000::CCD5000(CWnd* pParent /*=NULL*/)
	: CDialog(CCD5000::IDD, pParent)
	, m_iCount(0)
{
	memset(m_buff, 0, sizeof(m_buff));
	m_FileRecordName = GetFileName(_T("Log"));

	m_ProSpeed = 0;
	m_LimitSpeed = 0;
}

CCD5000::~CCD5000()
{

	CloseRemotePort();
}

void CCD5000::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCD5000, CDialog)
	ON_MESSAGE(ON_COM_RECEIVED, OnRemoteCommand)
END_MESSAGE_MAP()


BOOL CCD5000::OnInitCameraconnect()
{
	if(!CloseRemotePort())
	{
		return FALSE;
	}

	if(!m_Remote.RegisterNotifyWindow(m_hWnd))
	{
		return false;
	}

	if(!m_Remote.OpenPort(m_pDlgSetup->CCD5000COM()))
	{
		MessageBox(m_pDlgSetup->CCD5000COM(), _T("远程控制端口初始化失败"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	COMMCONFIG config;
	DWORD size = sizeof(config);
	memset(&config, 0, sizeof(config));

	if(!m_Remote.GetConfig(&config, size))
	{
		MessageBox(m_pDlgSetup->CCD5000COM(), _T("远程控制端口初始化失败"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	config.dcb.fParity = 1;
	config.dcb.Parity = m_pDlgSetup->DCBCCD5000().Parity;
	config.dcb.BaudRate = m_pDlgSetup->DCBCCD5000().BaudRate;
	config.dcb.ByteSize = m_pDlgSetup->DCBCCD5000().ByteSize;
	config.dcb.StopBits = m_pDlgSetup->DCBCCD5000().StopBits;

	if(!m_Remote.SetConfig(&config, sizeof(config)))
	{
		MessageBox(m_pDlgSetup->CCD5000COM(), _T("远程控制端口初始化失败"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

BOOL CCD5000::CloseRemotePort()
{
	m_Remote.ClosePort();
	m_Remote.UnregisterNotifyWindow(m_hWnd);
	return TRUE;
}

// DWORD time1 = 0;
// DWORD time2 = 0;
// DWORD time3 = 0;
// 与下位机程序自定义协议
LRESULT CCD5000::OnRemoteCommand(WPARAM wParam, LPARAM lParam)
{
	//TRACE1("Received: %i\r\n", wParam);
   // time1 = GetTickCount();

	USHORT usCommond = 0;	// 命令
	USHORT uscrc,mcrc;
	UCHAR ucHand;
	BYTE lpBuffer[2048] = {0};

	//DWORD dwLength = m_Remote.GetInput(lpBuffer, 2048);
	DWORD dwLength = m_Remote.Receive(lpBuffer, 2048, 3000);
	if (dwLength > 2048 || dwLength <= 0 )
	{
		//TRACE("length errordwLength = %d\n", dwLength);
		return FALSE;
	}
//	TRACE("length  = %d\n", dwLength);
	ucHand = lpBuffer[0];
	if ((unsigned char)ucHand == 0x86)
	{
	  memcpy(&m_buff[m_iCount], lpBuffer, dwLength);
	  m_iCount += dwLength;
	  m_bReadNow = true;
	}
	else if(m_bReadNow)
	{
	  memcpy(&m_buff[m_iCount], lpBuffer, dwLength);
	  m_iCount += dwLength;
	}

 	if(m_iCount > 1024) //异常情况处理
 	{
 		m_iCount = 0;
 		m_bReadNow = false;
 		return 0 ;
 
 	}
	if(m_iCount < 110)
		return 0;

 	if (ucHand != 0x86)
 	{
		goto _End ;
 	}
	//WriteText(220, m_buff);
	uscrc = m_buff[108] + (m_buff[109] << 8);
    mcrc = CRC16(m_buff, 108);
   
	if (uscrc != mcrc)
	{
		goto _End ;
	}

	for (int i = 0, j = 4; i < 26; i++)
	{
		j=4*(i+1);
		Byte_to_Float(m_flVules[i], m_buff+j);
	}
	m_bReadNow = false;
	m_iCount = 0;
    usCommond = m_buff[1];
    switch(usCommond)
	{
	case  Materialcount_1:     //   3   13
		if (m_buff[2]==0)//
		{
			for (int i = 0; i < 6; i++)
			{
				if (i<3)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13-3];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE1MSG, Materialcount_1, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_1, 6);
		}
		else               //5
		{
			for (int i = 0; i < 10; i++)
			{
				if (i<5)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13 -5];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE1_AT9MSG, Materialcount_1+3, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_1_at9, 10);
		}
		
		break;
	case  Materialcount_2:
		if (m_buff[2]==0)// 
		{

			for (int i = 0; i < 10; i++)
			{
				if (i<5)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13-5];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE2MSG, Materialcount_2, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_2, 10);
		}
		else
		{
			for (int i = 0; i < 18; i++)
			{
				if (i<9)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13-9];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE2_AT9MSG, Materialcount_2+3, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_2_at9, 18);
		}
	
		break;
	case  Materialcount_3:
		if (m_buff[2]==0)//
		{
			for (int i = 0; i < 14; i++)
			{
				if (i<7)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13-7];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE3MSG, Materialcount_3, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_3, 14);
// 			time3 = time1 -time2;
// 			TRACE(_T("%d\n"), time3);
// 			time2 = time1;
		}
		else  //13
		{
			for (int i = 0; i < 26; i++)
			{
				if (i<13)
				{
					m_flValuesSend[i] = m_flVules[i];
				} 
				else
				{
					m_flValuesSend[i] = m_flVules[i+13-13];
				}
			}
			SendMessageA(AfxGetMainWnd()->m_hWnd, UPDATE3_AT9MSG, Materialcount_3+3, reinterpret_cast<LPARAM>(&m_flValuesSend));
			SaveData(Materialcount_3_at9, 26);
		}
		break;
	default:
		break;
	}

_End:


	return 0;
}



BOOL CCD5000::Send(USHORT commond, float Valus)
{
	DWORD dwLength;
	dwLength = 12;
	BYTE _buf[12];
	_buf[0] = 0x86;

	memcpy(&_buf[1],&commond,sizeof(commond));

	USHORT usLength;
	usLength = 4;
	memcpy(&_buf[3], &usLength, sizeof(usLength));
	 
	memcpy(&_buf[5], &Valus, sizeof(Valus));

	USHORT crv;
	crv = CRC16(_buf, 9);
	memcpy(&_buf[9],&crv, sizeof(crv));
	_buf[11] = 0x68;
	m_Remote.Send(_buf, dwLength);
	return 0;
}

void CCD5000::WriteFileRecord(float* Value, int length)
{
// 	m_section.Lock();

	SYSTEMTIME _time;
	GetLocalTime(&_time);
	int _lengthFile;
	_lengthFile = m_FileRecordName.GetLength(); 
	CString _timeStrFile;
	_timeStrFile.Format(_T("%04i-%02i-%2i数据.txt"),_time.wYear, _time.wMonth, _time.wDay);
	m_FileRecordName += _timeStrFile;

	int _lengthFiletime;
	_lengthFiletime = _timeStrFile.GetLength();

	CString _timeStr;
	_timeStr.Format(_T("%04i-%02i-%2i %02i:%02i:%02i:%03i"),_time.wYear, _time.wMonth, _time.wDay
		, _time.wHour, _time.wMinute, _time.wSecond, _time.wMilliseconds);
	for (int i(0); i < length; i ++)
	{
		CString m_Data;
		m_Data.Format(_T("Data%d"), i);
		WriteFile_f(_timeStr, m_Data, *(Value + i), 2, m_FileRecordName);
	}

	m_FileRecordName.Delete(_lengthFile,_lengthFiletime);

// 	m_section.Unlock();
}

HANDLE CCD5000::OpenLogFile(const CString& lpFileName, const CString& lpHeader)
{
	for(int i(0); i < lpFileName.GetLength(); i++)
	{
		switch(lpFileName.GetAt(i))
		{
		case _TCHAR('\\'):
		case _TCHAR('/'):
			CreateDirectory(lpFileName.Left(i + 1), NULL);
		}
	}

	HANDLE hFile(CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

	if(hFile == INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	if(GetLastError() != ERROR_ALREADY_EXISTS || !GetFileSize(hFile, NULL))
	{
		DWORD dwWritten(0);

// #ifdef _UNICODE
// 		BYTE nFlag[] = {0xFF, 0xFE};
// 
// 		if(!WriteFile(hFile, &nFlag, sizeof(nFlag), &dwWritten, NULL))
// 		{
// 			CloseHandle(hFile);
// 			DeleteFile(lpFileName);
// 			return INVALID_HANDLE_VALUE;
// 		}
		// #endif

		CStringA lpMessageA;

		lpMessageA = lpHeader;

		DWORD dwLength(lpMessageA.GetLength() * sizeof(char));

		if(!WriteFile(hFile, lpMessageA.GetString(), dwLength, &dwWritten, NULL))
		{
			CloseHandle(hFile);
			DeleteFile(lpFileName);
			return INVALID_HANDLE_VALUE;
		}
	}
	else
	{
		if(SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
		{
			CloseHandle(hFile);
			return INVALID_HANDLE_VALUE;
		}
	}
	return hFile;
}

BOOL CCD5000::WriteReport(HANDLE hFile, const CString& lpMessage)
{
	CStringA lpMessageA;

	lpMessageA = lpMessage;
	DWORD dwWritten(0);
	DWORD dwLength = lpMessageA.GetLength() * sizeof(char);
	return WriteFile(hFile, lpMessageA.GetString(), dwLength, &dwWritten, NULL);
}

BOOL CCD5000::CloseReport(HANDLE hFile)
{
	return CloseHandle(hFile);
}

CString CCD5000::Slot1LogHeader()
{
	return _T("TIME,上左极耳,上涂层,上右极耳,\
		下左极耳,下涂层,下右极耳\r\n");
}

CString CCD5000::Slot1LogHeader(BOOL at9)
{
	return _T("TIME,上左极耳,上AT9,上涂层,上AT9,\
		下左极耳,下AT9,下涂层,下AT9,\r\n");
}

CString CCD5000::Slot2LogHeader()
{
	return _T("Time,上左极耳,上左涂层,上中间极耳,上右涂层,上右极耳,\
		下左极耳,下左涂层,下中间极耳,下右涂层,下右极耳\r\n");
}

CString CCD5000::Slot2LogHeader(BOOL at9)
{
	return _T("TIME,上左极耳,上左a_at9_1,上左涂层,上左a_at9_2,上中间极耳,上右a_at9_3, 上右涂层, 上右a_at9_4, 上右极耳,\
			  下左极耳,下左a_at9_1,下左涂层,下左a_at9_2,下中间极耳,下右a_at9_3, 下右涂层, 下右a_at9_4, 下右极耳\r\n");
}

CString CCD5000::Slot3LogHeader()
{
	return _T("Time,上左极耳,上左涂层,上中间极耳,上中间涂层,上中间极耳,上右涂层,上右极耳,\
		下左极耳,下左涂层,下中间极耳,下中间涂层,下中间极耳,下右涂层,下右极耳,\r\n");
}

CString CCD5000::Slot3LogHeader(BOOL at9)
{
	return _T("TIME,上左极耳,上左a_at9_1,上左涂层,上中间2a_at9_2,上中间极耳,上中间a_at9_3, 上中间涂层, 上中间a_at9_4,\
			  上右极耳, 上右a_at9_5,上右涂层,上AT96,上右极耳\
			  下左极耳,下左a_at9_1,下左涂层,下中间2a_at9_2,下中间极耳,下中间a_at9_3, 下中间涂层, 下中间a_at9_4,\
			  下右极耳, 下右a_at9_5,下右涂层,下AT96,下右极耳\\r\n");
}

CString CCD5000::SlotLogName(LPCTSTR pcsNumMaterial)
{
	SYSTEMTIME _LocalTime;
	GetLocalTime(&_LocalTime);

	CString txFileName;

	//if (_LocalTime.wHour < 8)		// [0, 8)
	//{
	//	txFileName.Format(	_T("%s\\Logs\\%04u-%02u-%02u#%s#Slot2.csv"),
	//		m_FileRecordName,
	//		_LocalTime.wYear,
	//		_LocalTime.wMonth,
	//		_LocalTime.wDay - 1,
	//		pcsNumMaterial);
	//}
	//else if (_LocalTime.wHour < 20)	// [8, 20)
	//{
	//	txFileName.Format(	_T("%s\\Logs\\%04u-%02u-%02u#%s#Slot1.csv"),
	//		m_FileRecordName,
	//		_LocalTime.wYear,
	//		_LocalTime.wMonth,
	//		_LocalTime.wDay,
	//		pcsNumMaterial);
	//}
	//else if (_LocalTime.wHour < 24)	// [20, 24)
	{
		txFileName.Format(	_T("%s\\Logs\\%04u-%02u-%02u#%s#Slot3.csv"),
			m_FileRecordName,
			_LocalTime.wYear,
			_LocalTime.wMonth,
			_LocalTime.wDay,
			pcsNumMaterial);
	}
	return txFileName;
}

CString CCD5000::SlotReportMessage(int length)
{
	CString StrSlotReport;

	SYSTEMTIME _LocalTime;
	GetLocalTime(&_LocalTime);

	StrSlotReport.Format(_T("%04i-%02i-%02i %02i:%02i:%02i"),
		_LocalTime.wYear,
		_LocalTime.wMonth,
		_LocalTime.wDay,
		_LocalTime.wHour,
		_LocalTime.wMinute,
		_LocalTime.wSecond);

	for (int i(0); i < length; i ++)
	{
		StrSlotReport.AppendFormat(_T(",%.2f"), m_flValuesSend[i]);
	}
	StrSlotReport += _T("\r\n");

	return StrSlotReport;
}

BOOL CCD5000::SaveData(int nMaterial, int nLength)
{
	CString strLogName;
	HANDLE hFile = NULL;
	switch (nMaterial)
	{
	case Materialcount_1:
		{
			strLogName = SlotLogName(_T("one"));
			hFile = OpenLogFile(strLogName, Slot1LogHeader());
		}
		break;

	case Materialcount_2:
		{
			strLogName = SlotLogName(_T("two"));
			hFile = OpenLogFile(strLogName, Slot2LogHeader());
		}
		break;

	case Materialcount_3:
		{
			strLogName = SlotLogName(_T("three"));
			hFile = OpenLogFile(strLogName, Slot3LogHeader());
		}
		break;
	case Materialcount_1_at9:
		{
			strLogName = SlotLogName(_T("One_at9"));
			hFile = OpenLogFile(strLogName, Slot1LogHeader(TRUE));
		}
		break;
	case Materialcount_2_at9:
		{
			strLogName = SlotLogName(_T("Two_at9"));
			hFile = OpenLogFile(strLogName, Slot2LogHeader(TRUE));
		}
		break;
	case  Materialcount_3_at9:
		{
			strLogName = SlotLogName(_T("Three_at9"));
			hFile = OpenLogFile(strLogName, Slot3LogHeader(TRUE));
		}
		break;
	}

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	WriteReport(hFile, SlotReportMessage(nLength));
	CloseReport(hFile);

	return TRUE;
}

void CCD5000::WriteText(int size, BYTE *m_GDATAarray)
{
	CFileFind  m_FileFind;
	CString Str, str,strcount = _T("");
	static int k = 0;
	strcount.Format(_T("%d"), k);
	strcount += _T(".txt");
	CFile file;
	CString m_sFilePath = GetFileName(strcount);
	for (int i=0; i< size; i++)
	{
		Str.Format(_T("%02x"), m_GDATAarray[i]);
		str+= Str;
	}
	if (file.Open(m_sFilePath , CFile::modeWrite | CFile::modeCreate))
	{
		file.Write(str, str.GetLength());								
	}
	file.Close();
	k++;

}

void CCD5000::Byte_to_Float(float &f , unsigned char byte[])
{
	FloatLongType fl;  

	f = -1;
	fl.ldata[0] = byte[0];  
	fl.ldata[1] = byte[1];  
	fl.ldata[2] = byte[2];  
	fl.ldata[3] = byte[3]; 
	f= (float)fl.fdata;
}

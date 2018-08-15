
// MainDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "MainDialog.h"
#include "AboutDlg.h"
#include "sda.h"
#include "StandardDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainDialog 对话框

CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDialog::IDD, pParent)
	, m_Bar(7)
	, m_Mode(2)
	, m_ICount(0)
	, DCount(0)
	, avgL(0)
	, avgR(0)
	, m_FinsProtocol(&m_FinsGateway, 0, 0)
	, m_AlarmEvent(CreateEvent(NULL, FALSE, FALSE, NULL))
	, m_AlarmthreadExit(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_Stop(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_Start(CreateEvent(NULL, FALSE, FALSE, NULL))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Configfile = GetFileName(_T("config.ini"));
	ReadConfig();

	DBMSL = new double[65535];
	DBMSR = new double[65535];
	memset(DBMSL, 0, sizeof(DBMSL));
	memset(DBMSR, 0, sizeof(DBMSR));
	memset(m_data, 0, sizeof(m_data));
	memset(m_RcvBuff, 0, sizeof(m_RcvBuff));

	m_pDcUp = NULL;
	m_pDcLeftDown = NULL;
	m_pDcRightDown = NULL;
	m_pDcLeftUp = NULL;
	m_pDcRightUp = NULL;
	m_pThread = NULL;

	InitializeCriticalSection(&m_csShareddata);	

// 	m_bInitRgn = FALSE;
}

CMainDialog::~CMainDialog()
{
	SetEvent( m_AlarmthreadExit );
	SetEvent(m_Stop);
	WaitForSingleObject(m_Alarmthread, INFINITE);
	delete DBMSL;
	delete DBMSR;
	m_CDataSql.Exsit();
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_REAL, m_Picture);
	DDX_Control(pDX, IDC_BTN_SETUP, m_Btn_Setup);
	DDX_Control(pDX, IDC_BTN_PARA, m_btn_para);
	DDX_Control(pDX, IDC_BTN_MANUAL, m_btn_manual);
	DDX_Control(pDX, IDC_BTN_ANTO, m_btn_anto);
	DDX_Control(pDX, IDC_PIC_LEFT_DOWN, m_picLeftDown);
	DDX_Control(pDX, IDC_PIC_RIGHT_DOWN, m_picRightDown);
	DDX_Control(pDX, IDC_BTN_STANDARD, m_btn_standard);
	DDX_Control(pDX, IDC_PIC_LEFT_UP, m_picLeftUp);
	DDX_Control(pDX, IDC_PIC_RIGHT_UP, m_picRightUp);
	DDX_Control(pDX, IDC_BUTTONSQL, m_btn_datadisplay);
}

BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SETUP, &CMainDialog::OnBnClickedBtnSetup)
	ON_WM_CTLCOLOR()

	ON_MESSAGE(MODEMSG, OnModeMsg)
	ON_MESSAGE(ERRORMSG, OnErrorMsg)
	ON_MESSAGE(UPDATE1MSG, OnUpdate1Msg)
	ON_MESSAGE(UPDATE2MSG, OnUpdate2Msg)
	ON_MESSAGE(UPDATE3MSG, OnUpdate3Msg)
	ON_MESSAGE(UPDATE1_AT9MSG, OnUpdate4Msg)
	ON_MESSAGE(UPDATE2_AT9MSG, OnUpdate5Msg)
	ON_MESSAGE(UPDATE3_AT9MSG, OnUpdate6Msg)
	ON_BN_CLICKED(IDC_BTN_PARA, &CMainDialog::OnBnClickedBtnPara)
	ON_BN_CLICKED(IDC_BTN_ANTO, &CMainDialog::OnBnClickedBtnAnto)
	ON_BN_CLICKED(IDC_BTN_MANUAL, &CMainDialog::OnBnClickedBtnManual)
	ON_BN_CLICKED(IDC_BTN_STANDARD, &CMainDialog::OnBnClickedBtnStandard)
	ON_BN_CLICKED(IDC_BUTTONSQL, &CMainDialog::OnBnClickedButtonsql)
END_MESSAGE_MAP()

BOOL CMainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);        // 设置小图标

	static int widths[] =
	{
		125,    // ......
		250,    // ......
		350,    // ......
		450,    // ......
		550,    // ......
		-1,
	};

	m_pDcUp = m_Picture.GetDC();
	m_pDcLeftUp = m_picLeftUp.GetDC();
	m_pDcRightUp = m_picRightUp.GetDC();
	m_pDcLeftDown = m_picLeftDown.GetDC();
	m_pDcRightDown = m_picRightDown.GetDC();

	m_appBrush.CreateSolidBrush(RGB(223, 233, 243));

	m_Font.CreateFont(26,12, 2, 0, FW_SEMIBOLD, FALSE
		, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	m_penGuideLine.CreatePen(PS_DOT, 1, RGB(100, 100, 100));

	m_penMaterial1.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	m_penMaterial2.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	m_penMaterial3.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

	m_penEar1.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	m_penEar2.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	m_penEar3.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	m_penEar4.CreatePen(PS_SOLID, 2, RGB(0, 255, 255));

	m_background.CreateSolidBrush(RGB(0, 0, 0));

	OnChangeDisp(m_dlgPara.RoolMode());

	m_CCD5000.m_pDlgSetup = &m_SetupDlg;
	m_CCD5000.Create(m_CCD5000.IDD, this);
	m_CCD5000.OnInitCameraconnect();

	m_font.CreateFont(60, 60/2, 0, 0, FW_NORMAL, FALSE
		, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));

	//SetTimer(1, 200, NULL);

 	if (ioc_board_init() <= 0)
 	{
 		AfxMessageBox(_T("io0640初始化失败！"));
 		//ASSERT(FALSE);
 	}
	ioc_write_outport(0, 1, 0);
	ioc_write_outport(0, 2, 0);

	AlarmLight(Light_ERROR_Yellow);

	// 设置默认主窗口大小
	int cxIcon = GetSystemMetrics(SM_CXSCREEN) - 100;
	int cyIcon = GetSystemMetrics(SM_CYSCREEN) - 100;
	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, cxIcon, cyIcon, SWP_NOMOVE);
	ShowWindow(SW_MAXIMIZE);

 	m_pThread = AfxBeginThread(ThreadSendVirtual, (LPVOID)this);
 	if (m_pThread == NULL)
 	{
 		AfxMessageBox(_T("启动失败"));
 		return TRUE;
 	}


	m_FinsGateway.RegisterNotifyWindow(m_hWnd);

	//const CString lpIP(_T("192.168.1.170"));
	//DWORD dwPort(9600);

	//if (!OnConnectToPlc(reinterpret_cast<WPARAM>(lpIP.GetString()), dwPort))
	//{
	//	AfxMessageBox(_T("PLC连接失败！"));
	//	ASSERT(FALSE);
	//}

	if (m_SetupDlg.m_MESState == TRUE)
	{
		if (!m_TCPClient.CreateTcpClient(m_SetupDlg.inport, m_SetupDlg.dwAddress))
		{
			AfxMessageBox(_T("MES系统连接失败！"));
			ASSERT(FALSE);
		}
		if (m_SetupDlg.strPCNo.GetLength() != 30)
		{
			int DeviceNoLength = m_SetupDlg.strPCNo.GetLength();
			if (DeviceNoLength > 30)
			{
				m_SetupDlg.strPCNo.Delete(0, DeviceNoLength - 30);
			}
			else
			{
				for (int i(0); i < 30 - DeviceNoLength; i ++)
				{
					m_SetupDlg.strPCNo.Insert(0, '0');
				}
			}
		}
		if (m_SetupDlg.EquipmentProtocolCoding.GetLength() != 4)
		{
			int AgreementNoLength = m_SetupDlg.EquipmentProtocolCoding.GetLength();
			if (AgreementNoLength > 4)
			{
				m_SetupDlg.strPCNo.Delete(0, AgreementNoLength - 4);
			}
			else
			{
				for (int i(0); i < 4 - AgreementNoLength; i ++)
				{
					m_SetupDlg.strPCNo.Insert(0, '0');
				}
			}
		}

		m_TCPClient.DeviceNo = m_SetupDlg.strPCNo;
		m_TCPClient.AgreementNo = m_SetupDlg.EquipmentProtocolCoding;
	}

	if (m_SetupDlg.m_MESState)
	{
		SYSTEMTIME _strattime;
		GetLocalTime(&_strattime);
		m_TCPClient.SendRunningStatus(1, _strattime, 500);
	}
	
	SetTimer(0, 1000, NULL);

	m_Alarmthread = CreateThread(NULL, 0, reinterpret_cast<PTHREAD_START_ROUTINE>(AlarmSequence), this, 0, NULL);

// 	m_CDataSql.ConnectSQL();
// 
// 	if (m_Roll.ConnectSQL())
// 	{
// 		m_Roll.IsExistSQLBase();
// 		m_Roll.IsExistMainTable();
// 
// 		SetTimer(0, 200, NULL);
// 	}
// 	else
// 	{
// 		AfxMessageBox(_T("SQL链接失败！"));
// 		ASSERT(FALSE);
// 	}


	if (m_PLCFlag)
	{
		if (!m_S7COMMTCPIP.CreateTcpClient(m_PlcPort, m_PlcIp) ||
			!m_S7COMMTCPIP.ConnectToPLC(5000) )
		{
			AfxMessageBox(_T("PLC链接失败！请检查网线或本地连接设置！"));
			ASSERT(FALSE);
			m_PLCFlag = false;
		}
		else
		{
			m_PLCFlag = true;
			SetTimer(2, 200, NULL);
		}
	}

// 	CString m_strRollName;
// 	m_strRollName = _T("DATA");
// 	if (!m_Roll.IsExistEverydayTable(m_strRollName))
// 	{
// 		SYSTEMTIME _time;
// 		GetSystemTime(&_time);
// 		RollAllInfo tRollAllInfo;
// 		tRollAllInfo.CreateTime = _time;
// 		tRollAllInfo.RollName = m_strRollName;
// 		m_Roll.CreateRoll(tRollAllInfo);
// 	}


	return TRUE;
}

void CMainDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CMainDialog::BitBltAll()
{
	if(m_pDcUp)
	{
		m_pDcUp->BitBlt(0, 0, m_rectUp.Width(), m_rectUp.Height(), &m_memReal, 0, 0, SRCCOPY);
		m_pDcLeftDown->BitBlt(0, 0, m_rectLeftDown.Width(), m_rectLeftDown.Height(), &m_memLeftDown, 0, 0, SRCCOPY);
		m_pDcRightDown->BitBlt(0, 0, m_rectRightDown.Width(), m_rectRightDown.Height(), &m_memRightDown, 0, 0, SRCCOPY);
		m_pDcLeftUp->BitBlt(0, 0, m_rectLeftUp.Width(), m_rectLeftUp.Height(), &m_memLeftUp, 0, 0, SRCCOPY);
		m_pDcRightUp->BitBlt(0, 0, m_rectRightUp.Width(), m_rectRightUp.Height(), &m_memRightUp, 0, 0, SRCCOPY);

	}	
}

void CMainDialog::OnPaint()
{
	CPaintDC dc(this);
 	float pData[50];
 	memset(pData, 0, sizeof(pData));
 	m_section.Lock();
 	InitialDBB();
 	DrawReal(pData);
 	DrawScale();		// 画蓝色背景和刻度(接收到数据不用更新)
 	DrawGuideLine();	// 画辅助线	
 //	CopyRcvToBuff(pData, sizeof(pData));	// 复制数据到缓冲区
 	DrawGrap();
 	DrawNote();
 	BitBltAll();
 	m_section.Unlock();
}

HCURSOR CMainDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainDialog::OnDestroy()
{
	CDialog::OnDestroy();

	m_FinsGateway.DestroySocket();
	m_FinsGateway.UnregisterNotifyWindow(m_hWnd);

	m_FinsProtocol.Detach();

	m_font.DeleteObject();
	m_CCD5000.CloseRemotePort();

	ReleaseDC(m_pDcUp);
	ReleaseDC(m_pDcLeftDown);
	ReleaseDC(m_pDcRightDown);
	ReleaseDC(m_pDcLeftUp);
	ReleaseDC(m_pDcRightUp);

	if (m_memReal != NULL)
	{
		m_memReal.DeleteDC();
		m_bmpReal.DeleteObject();
	}
	if (m_memLeftDown != NULL)
	{
		m_memLeftDown.DeleteDC();
		m_bmpLeftDown.DeleteObject();
	}
	if (m_memRightDown != NULL)
	{
		m_memRightDown.DeleteDC();
		m_bmpRightDown.DeleteObject();
	}
	if (m_memLeftUp != NULL)
	{
		m_memLeftUp.DeleteDC();
		m_bmpLeftUp.DeleteObject();
	}
	if (m_memRightUp != NULL)
	{
		m_memRightUp.DeleteDC();
		m_bmpRightUp.DeleteObject();
	}

	m_penGuideLine.DeleteObject();
	m_appBrush.DeleteObject();

	m_penMaterial1.DeleteObject();
	m_penMaterial2.DeleteObject();
	m_penMaterial3.DeleteObject();

	m_penEar1.DeleteObject();
	m_penEar2.DeleteObject();
	m_penEar3.DeleteObject();
	m_penEar4.DeleteObject();

	if (m_SetupDlg.m_MESState)
	{
		SYSTEMTIME _strattime;
		GetLocalTime(&_strattime);
		m_TCPClient.SendRunningStatus(0, _strattime, 500);
	}

	if (m_SetupDlg.m_MESState == TRUE)
	{
		m_TCPClient.CloseClient();
	}

	m_S7COMMTCPIP.DestroySocket();
// 	ioc_board_close();
}

void CMainDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RECT _ClientRect = {0, 0, cx, cy };

	RECT _SubRectPicture;

	if(IsWindow(m_Picture))
	{
		_SubRectPicture.right = _ClientRect.right;
		_SubRectPicture.left = _ClientRect.left;
		_SubRectPicture.top = _ClientRect.top + 70;
		if (m_dlgPara.ExistAt9)
		{
		   _SubRectPicture.bottom = _SubRectPicture.top + 350;
		} 
		else
		{
			_SubRectPicture.bottom = _SubRectPicture.top + 350;
		}
		m_Picture.MoveWindow(&_SubRectPicture);

		m_Picture.GetClientRect(&m_rectUp);
	}

	int nHeight = 0;	// 数据图高度

	if (IsWindow(m_picLeftUp))
	{
		nHeight = cy - _SubRectPicture.bottom;
		nHeight = (nHeight - 10 - 10 - 8) / 2;

		CRect rc;
		rc.left = 10;
		rc.right = _ClientRect.right / 2 - 7;
		rc.top = _SubRectPicture.bottom + 10;
		rc.bottom = rc.top + nHeight;
		m_picLeftUp.MoveWindow(&rc);

		m_picLeftUp.GetClientRect(&m_rectLeftUp);
	}

	if (IsWindow(m_picRightUp))
	{
		CRect rc;
		rc.left = _ClientRect.right / 2 + 7;
		rc.right = _ClientRect.right - 10;
		rc.top = _SubRectPicture.bottom + 10;
		rc.bottom = rc.top + nHeight;
		m_picRightUp.MoveWindow(&rc);

		m_picRightUp.GetClientRect(&m_rectRightUp);
	}

	if (IsWindow(m_picLeftDown))
	{
		CRect rc;
		rc.left = 10;
		rc.right = _ClientRect.right / 2 - 7;
		rc.top = _ClientRect.bottom - (_ClientRect.bottom - _SubRectPicture.bottom) / 2;
		rc.bottom = rc.top + nHeight;
		m_picLeftDown.MoveWindow(&rc);

		m_picLeftDown.GetClientRect(&m_rectLeftDown);
	}

	if (IsWindow(m_picRightDown))
	{
		CRect rc;
		rc.left = _ClientRect.right / 2 + 7;
		rc.right = _ClientRect.right - 10;
		rc.top = _ClientRect.bottom - (_ClientRect.bottom - _SubRectPicture.bottom) / 2;
		rc.bottom = rc.top + nHeight;
		m_picRightDown.MoveWindow(&rc);

		m_picRightDown.GetClientRect(&m_rectRightDown);
	}

	// 通讯设置
	if(IsWindow(m_Btn_Setup))
	{
		RECT _SubRect;
		_SubRect.top = _ClientRect.top + 5;
		_SubRect.bottom = _SubRect.top + 60;
		_SubRect.left = _ClientRect.left + 20;
		_SubRect.right = _SubRect.left + 80;

		m_Btn_Setup.MoveWindow(&_SubRect);
	}

	// 参数设置
	if(IsWindow(m_btn_para))
	{
		RECT _SubRect;
		_SubRect.top = _ClientRect.top + 5;
		_SubRect.bottom = _SubRect.top + 60;
		_SubRect.left = _ClientRect.left + 120;
		_SubRect.right = _SubRect.left + 80;

		m_btn_para.MoveWindow(&_SubRect);
	}
	// 标准值
	if(IsWindow(m_btn_standard))
	{
		RECT _SubRect;
		_SubRect.top = _ClientRect.top + 5;
		_SubRect.bottom = _SubRect.top + 60;
		_SubRect.left = _ClientRect.left + 220;
		_SubRect.right = _SubRect.left + 80;

		m_btn_standard.MoveWindow(&_SubRect);
	}
    // 数据显示 
	if(IsWindow(m_btn_datadisplay))
	{
		RECT _SubRect;
		_SubRect.top = _ClientRect.top + 5;
		_SubRect.bottom = _SubRect.top + 60;
		_SubRect.left = _ClientRect.left + 320;
		_SubRect.right = _SubRect.left + 80;

		m_btn_datadisplay.MoveWindow(&_SubRect);
	}
// 	// 手动
// 	if(IsWindow(m_btn_manual))
// 	{
// 		RECT _SubRect;
// 		_SubRect.top = _ClientRect.top + 5;
// 		_SubRect.bottom = _SubRect.top + 60;
// 		_SubRect.left = _ClientRect.left + 110;
// 		_SubRect.right = _SubRect.left + 80;
// 	
// 		m_btn_manual.MoveWindow(&_SubRect);
// 	}
// 	
// 	// 自动
// 	if(IsWindow(m_btn_anto))
// 	{
// 		RECT _SubRect;
// 		_SubRect.top = _ClientRect.top + 5;
// 		_SubRect.bottom = _SubRect.top + 60;
// 		_SubRect.left = _ClientRect.left + 10;
// 		_SubRect.right = _SubRect.left + 80;
// 	
// 		m_btn_anto.MoveWindow(&_SubRect);
// 	}

	RedrawWindow();
}

void CMainDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 0:
		{
			if (m_SetupDlg.m_MESState == TRUE)
			{
				m_TCPClient.SendHeartBeat(500);
			}
			//double dvalue[6];
			//for (int i = 0; i < 6; i++)
			//{
			//	dvalue[i] = i+6;
			//}
			//if (m_SetupDlg.m_MESState)
			//{
			//	SYSTEMTIME _curtime;
			//	GetLocalTime(&_curtime);
			//	m_TCPClient.SendOperatingData(6, dvalue, 50000,
			//		_curtime, 500);
			//}
		}
		break;
	case 1:
		{
			KillTimer(1);

			//if (m_PLCFlag == false)
			{
				SYSTEMTIME lptime;
				GetLocalTime(&lptime);
				CString m_strRollName;
				m_strRollName.Format(_T("Report_DATA %04u-%02u-%02u"), lptime.wYear
					, lptime.wMonth, lptime.wDay);

				if (!m_Roll.IsExistEverydayTable(m_strRollName))
				{
					SYSTEMTIME _time;
					GetSystemTime(&_time);
					RollAllInfo tRollAllInfo;
					tRollAllInfo.CreateTime = _time;
					tRollAllInfo.RollName = m_strRollName;
					m_Roll.CreateRoll(tRollAllInfo);				
				}
				m_Roll.RollName() = m_strRollName;
			}
			SetTimer(1, 5000, NULL);
		}
		break;
	case 2:
		{
			KillTimer(2);

			float speed = 0;
			m_S7COMMTCPIP.GetWord(_T("DB80.84"), &speed, TYPE_FLOAT);//涂布实时速度地址
			m_ProSpeed = speed;

			SetTimer(2, 500, NULL);
		}
		break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CMainDialog::InitialDBB()
{
	if (m_memReal != NULL)
	{
		m_memReal.DeleteDC();
		m_bmpReal.DeleteObject();
	}
	if (m_memLeftUp != NULL)
	{
		m_memLeftUp.DeleteDC();
		m_bmpLeftUp.DeleteObject();
	}
	if (m_memRightUp != NULL)
	{
		m_memRightUp.DeleteDC();
		m_bmpRightUp.DeleteObject();
	}
	if (m_memLeftDown != NULL)
	{
		m_memLeftDown.DeleteDC();
		m_bmpLeftDown.DeleteObject();
	}
	if (m_memRightDown != NULL)
	{
		m_memRightDown.DeleteDC();
		m_bmpRightDown.DeleteObject();
	}

	// 创建内存DC
	if(!m_memReal.CreateCompatibleDC(m_pDcUp))
	{
		::PostQuitMessage(0);
	} 
	if(!m_memLeftUp.CreateCompatibleDC(m_pDcLeftUp))
	{
		::PostQuitMessage(0);
	}
	if(!m_memRightUp.CreateCompatibleDC(m_pDcRightUp))
	{
		::PostQuitMessage(0);
	}
	if(!m_memLeftDown.CreateCompatibleDC(m_pDcLeftDown))
	{
		::PostQuitMessage(0);
	}
	if(!m_memRightDown.CreateCompatibleDC(m_pDcRightDown))
	{
		::PostQuitMessage(0);
	}

	m_bmpReal.CreateCompatibleBitmap(m_pDcUp, m_rectUp.Width(), m_rectUp.Height());
	m_bmpLeftUp.CreateCompatibleBitmap(m_pDcLeftUp, m_rectLeftUp.Width(), m_rectLeftUp.Height());
	m_bmpRightUp.CreateCompatibleBitmap(m_pDcRightUp, m_rectRightUp.Width(), m_rectRightUp.Height());
	m_bmpLeftDown.CreateCompatibleBitmap(m_pDcLeftDown, m_rectLeftDown.Width(), m_rectLeftDown.Height());
	m_bmpRightDown.CreateCompatibleBitmap(m_pDcRightDown, m_rectRightDown.Width(), m_rectRightDown.Height());

	m_memReal.SelectObject(m_bmpReal);
	m_memLeftUp.SelectObject(m_bmpLeftUp);
	m_memRightUp.SelectObject(m_bmpRightUp);
	m_memLeftDown.SelectObject(m_bmpLeftDown);
	m_memRightDown.SelectObject(m_bmpRightDown);
}

// 根据上下宽度差值，获得料拉伸的像素
int	CMainDialog::GetStretLenth(float fDiff)
{
// 	int nStretLenth = 0;
// 	float fDiffVal = fabs(fDiff);
// 
// 	if (fDiffVal < 0.5)			// [0, 0.5)
// 		nStretLenth = 0;
// 	else if (fDiffVal < 1)		// [0.5, 1)
// 		nStretLenth = 5;
// 	else if (fDiffVal < 1.5)	// [0.5, 1.5)
// 		nStretLenth = 8;
// 	else if (fDiffVal < 5)		// [1.5, 5)
// 		nStretLenth = 12;
// 	else						// [5, +00)
// 		nStretLenth = 20;

	//return nStretLenth;
	return 0;
}

// 画背景和实时显示text
// 传入的参数执行接收数据缓存
void CMainDialog::DrawReal(const float *pData)
{
	COLORREF crl = RGB(180, 180, 255);
	m_memReal.FillSolidRect(0, 0, m_rectUp.Width(), m_rectUp.Height(), crl);
	COLORREF crlY =  RGB(186, 111, 64);// RGB(254, 254, 65);
	m_memReal.FillSolidRect(5, m_rectUp.Height()/2 - 10, m_rectUp.Width() - 10, 20, crlY);

	CString str = _T("0.0");
	CRect rc;
	const int c_nStandardMaterial = m_dlgStandard.Material();
	int nLong = 200;	// 默认图示物料长度
	const int c_nAdjust = 20;	// text调整像素

	m_memReal.SetBkColor(RGB(0, 0, 0));
	m_memReal.SetTextColor(RGB(0, 255, 0));
	COLORREF crlB = RGB(0, 0, 0);
	m_memReal.SelectObject(m_font);

	CRect rcUp, rcDown;
	float fDiffVal = 0;	// 偏差值
	int j = 10;//白色间隔
	switch (m_Mode)
	{
	case 1:
		{
//up
			const int nOneWidth = m_rectUp.Width() /8;
			nLong = nOneWidth * 2;
			rc.top = m_rectUp.top + 10;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);
			for(int i = 0, j = 1; i< 3; i++, j = i*3 + 1)
			{
				rc.left = nOneWidth * j - 100;
				rc.right = nOneWidth * j + 100;
				m_memReal.DrawText(FloatToCstring(pData[i]), _tcslen(FloatToCstring(pData[i])), &rc, DT_CENTER | DT_VCENTER);
			}
//down
			rc.bottom = m_rectUp.bottom - 10;
			rc.top = rc.bottom - 60;
			for(int i = 0, j = 1; i< 3; i++, j = i*3 + 1)
			{
				rc.left = nOneWidth * j - 100;
				rc.right = nOneWidth * j + 100;
				m_memReal.DrawText(FloatToCstring(pData[i+3]), _tcslen(FloatToCstring(pData[i+3])), &rc, DT_CENTER | DT_VCENTER);
			}
			// 物料示意
			rcUp.left = 4 * nOneWidth - nLong;
			rcUp.right = 4 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 4 * nOneWidth - nLong;
			rcDown.right = 4 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);
		}
		break;

	case 2:
		{
			const int nOneWidth = m_rectUp.Width() / 20;
			nLong = nOneWidth * 3;
//up
			rc.top = m_rectUp.top + 10;
			rc.bottom = rc.top + 60;
			for(int i = 0, j = 1; i< 4; i++, j = 5*i)
			{
				rc.left = nOneWidth * j - 100;
				rc.right = nOneWidth * j + 100;
				m_memReal.DrawText(FloatToCstring(pData[i]), _tcslen(FloatToCstring(pData[i])), &rc, DT_CENTER | DT_VCENTER);
			}
 			rc.right = m_rectUp.Width() - 10;
 			rc.left = rc.right - 200;
 			m_memReal.DrawText(FloatToCstring(pData[4]), _tcslen(FloatToCstring(pData[4])), &rc, DT_CENTER | DT_VCENTER);
//down
			rc.bottom = m_rectUp.bottom - 10;
			rc.top = rc.bottom - 60;
			for(int i = 0, j = 1; i< 4; i++, j = 5*i)
			{
				rc.left = nOneWidth * j - 100;
				rc.right = nOneWidth * j + 100;
				m_memReal.DrawText(FloatToCstring(pData[i+5]), _tcslen(FloatToCstring(pData[i +5])), &rc, DT_CENTER | DT_VCENTER);
			}
			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[9]), _tcslen(FloatToCstring(pData[9])), &rc, DT_CENTER | DT_VCENTER);
			// 物料示意
			rcUp.left = 5 * nOneWidth - nLong;
			rcUp.right = 5 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 5 * nOneWidth - nLong;
			rcDown.right = 5 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);
			// 2
			rcUp.left = 15 * nOneWidth - nLong;
			rcUp.right = 15 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 20;
			rcDown.left = 15 * nOneWidth - nLong;
			rcDown.right = 15 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 20;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);
		}
		break;

	case 3:
		{
			const int nOneWidth = m_rectUp.Width() / 18;
			nLong = nOneWidth * 2;

			rc.top = m_rectUp.top + 10;
			rc.bottom = rc.top + 60;

			rc.left = 10;
			rc.right = rc.left + 200;
			m_memReal.DrawText(FloatToCstring(pData[0]), _tcslen(FloatToCstring(pData[0])), &rc, DT_LEFT | DT_VCENTER);
			
			rc.left = nOneWidth * 3 - 100;
			rc.right = nOneWidth * 3 + 100;
			m_memReal.DrawText(FloatToCstring(pData[1]), _tcslen(FloatToCstring(pData[1])), &rc,  DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[2]), _tcslen(FloatToCstring(pData[2])), &rc, DT_CENTER | DT_VCENTER);
				
			rc.left = nOneWidth * 9 - 100;
			rc.right = nOneWidth * 9 + 100;
			m_memReal.DrawText(FloatToCstring(pData[3]), _tcslen(FloatToCstring(pData[3])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[4]), _tcslen(FloatToCstring(pData[4])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 15 - 100;
			rc.right = nOneWidth * 15 + 100;
			m_memReal.DrawText(FloatToCstring(pData[5]), _tcslen(FloatToCstring(pData[5])), &rc, DT_CENTER | DT_VCENTER);

			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[6]), _tcslen(FloatToCstring(pData[6])), &rc, DT_RIGHT | DT_VCENTER);

			rc.bottom = m_rectUp.bottom - 10;
			rc.top = rc.bottom - 60;

			rc.left = 10;
			rc.right = rc.left + 200;
			m_memReal.DrawText(FloatToCstring(pData[7]), _tcslen(FloatToCstring(pData[7])), &rc, DT_LEFT | DT_VCENTER);

			rc.left = nOneWidth * 3 - 100;
			rc.right = nOneWidth * 3 + 100;
			m_memReal.DrawText(FloatToCstring(pData[8]), _tcslen(FloatToCstring(pData[8])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[9]), _tcslen(FloatToCstring(pData[9])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 9 - 100;
			rc.right = nOneWidth * 9 + 100;
			m_memReal.DrawText(FloatToCstring(pData[10]), _tcslen(FloatToCstring(pData[10])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[11]), _tcslen(FloatToCstring(pData[11])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 15 - 100 - 15;
			rc.right = nOneWidth * 15 + 100 - 15;
			m_memReal.DrawText(FloatToCstring(pData[12]), _tcslen(FloatToCstring(pData[12])), &rc, DT_CENTER | DT_VCENTER);

			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[13]), _tcslen(FloatToCstring(pData[13])), &rc, DT_RIGHT | DT_VCENTER);
			// 物料示意，对照左边小的左移动
			rcUp.left = 3 * nOneWidth - nLong;
			rcUp.right = 3 * nOneWidth+ nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 3 * nOneWidth - nLong;
			rcDown.right = 3 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
	
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);

			// 2
			rcUp.left = 9 * nOneWidth - nLong;
			rcUp.right = 9 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 9 * nOneWidth - nLong;
			rcDown.right = 9 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);

			// 3
			rcUp.left = 15 * nOneWidth - nLong;
			rcUp.right = 15 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 15 * nOneWidth - nLong;
			rcDown.right = 15 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);
		}
		break;

	case  4 :// AT9  + 一条料    3+2 = 5
		{
			const int nOneWidth = m_rectUp.Width() /8;
			nLong = nOneWidth * 2;
// up
			rc.top = m_rectUp.top + 10 + 60;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);

			rc.left = nOneWidth * 1 - 100;
			rc.right = nOneWidth * 1 + 100;
			m_memReal.DrawText(FloatToCstring(pData[0]), _tcslen(FloatToCstring(pData[0])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 4 - 100;
			rc.right = nOneWidth * 4 + 100;
			m_memReal.DrawText(FloatToCstring(pData[2]), _tcslen(FloatToCstring(pData[2])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 7 - 100;
			rc.right = nOneWidth * 7 + 100;
			m_memReal.DrawText(FloatToCstring(pData[4]), _tcslen(FloatToCstring(pData[4])), &rc, DT_CENTER | DT_VCENTER);
//down
			rc.bottom = m_rectUp.bottom - 10 - 30;
			rc.top = rc.bottom - 60 - 30;

			rc.left = nOneWidth * 1 - 100;
			rc.right = nOneWidth * 1 + 100;
			m_memReal.DrawText(FloatToCstring(pData[5]), _tcslen(FloatToCstring(pData[5])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 4 - 100;
			rc.right = nOneWidth * 4 + 100;
			m_memReal.DrawText(FloatToCstring(pData[7]), _tcslen(FloatToCstring(pData[7])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 7 - 100;
			rc.right = nOneWidth * 7 + 100;
			m_memReal.DrawText(FloatToCstring(pData[9]), _tcslen(FloatToCstring(pData[9])), &rc, DT_CENTER | DT_VCENTER);
//at9  up
			rc.top = m_rectUp.top + 10 ;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);

			rc.left = nOneWidth * 2 - 100;
			rc.right = nOneWidth * 2 + 100;
			m_memReal.DrawText(FloatToCstring(pData[1]), _tcslen(FloatToCstring(pData[1])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[3]), _tcslen(FloatToCstring(pData[3])), &rc, DT_CENTER | DT_VCENTER);
//at9 down
			rc.bottom = m_rectUp.bottom - 10;
			rc.top = rc.bottom - 60 ;

			rc.left = nOneWidth * 2 - 100;
			rc.right = nOneWidth * 2 + 100;
			m_memReal.DrawText(FloatToCstring(pData[4]), _tcslen(FloatToCstring(pData[4])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[6]), _tcslen(FloatToCstring(pData[6])), &rc, DT_CENTER | DT_VCENTER);
			//at9 物料
			rcUp.left = 4 * nOneWidth - nLong;
			rcUp.right = 4 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 4 * nOneWidth - nLong;
			rcDown.right = 4 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
			m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));
			// 物料示意
			int j = 10;
			rcUp.left = 4 * nOneWidth - nLong + j;
			rcUp.right = 4 * nOneWidth + nLong - j;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 4 * nOneWidth - nLong + j;
			rcDown.right = 4 * nOneWidth + nLong - j;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);

		}
		break;
	case  5: // AT9   +  二条料  5+4 = 5
		{
			const int nOneWidth = m_rectUp.Width() / 20;
			nLong = nOneWidth * 3;
//up 
			rc.top = m_rectUp.top + 10 + 60;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);
			for(int i = 0, k = 1; i < 4; i++, k = i*5)
			{	
				rc.left = nOneWidth * k- 100;
				rc.right = nOneWidth * k + 100;
				m_memReal.DrawText(FloatToCstring(pData[i*2]), _tcslen(FloatToCstring(pData[i*2])), &rc, DT_CENTER | DT_VCENTER);
			}
			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[8]), _tcslen(FloatToCstring(pData[8])), &rc, DT_CENTER | DT_VCENTER);
//down
			rc.bottom = m_rectUp.bottom - 10 - 30;
			rc.top = rc.bottom - 60 - 30;

			for(int i = 0, k = 1; i < 4; i++, k = i*5)
			{
				rc.left = nOneWidth * k- 100;
				rc.right = nOneWidth * k + 100;
				m_memReal.DrawText(FloatToCstring(pData[i*2 +9]), _tcslen(FloatToCstring(pData[i*2 +9])), &rc, DT_CENTER | DT_VCENTER);
			}
			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[17]), _tcslen(FloatToCstring(pData[17])), &rc, DT_CENTER | DT_VCENTER);
	//at9
			rc.top = m_rectUp.top ;
			rc.bottom = rc.top + 60;
// 		    Gdiplus::Graphics m_Graphics(m_memReal);
// 			Gdiplus::Pen penCu(Gdiplus::Color(0, 0, 0), 2);
// 			penCu.SetDashStyle(Gdiplus::DashStyleSolid);

			rc.left = nOneWidth*2 - 100;
			rc.right = nOneWidth*2 + 100;
			m_memReal.DrawText(FloatToCstring(pData[1]), _tcslen(FloatToCstring(pData[1])), &rc, DT_CENTER | DT_VCENTER);
			//m_Graphics.DrawLine(penCu, rc.left, rc.bottom,rc.left, rc.bottom - 60);
// 			m_memReal.MoveTo(rc.left, rc.bottom);
// 			m_memReal.LineTo(rc.left, rc.bottom - 60);
			

			rc.left = nOneWidth * 8 - 100;
			rc.right = nOneWidth * 8 + 100;
			m_memReal.DrawText(FloatToCstring(pData[3]), _tcslen(FloatToCstring(pData[3])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[5]), _tcslen(FloatToCstring(pData[5])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 18 - 100;
			rc.right = nOneWidth * 18 + 100;
			m_memReal.DrawText(FloatToCstring(pData[7]), _tcslen(FloatToCstring(pData[7])), &rc, DT_CENTER | DT_VCENTER);
//down at9
			rc.bottom = m_rectUp.bottom ;
			rc.top = rc.bottom - 60;

			rc.left = nOneWidth * 2 - 100;
			rc.right = nOneWidth * 2+ 100;
			m_memReal.DrawText(FloatToCstring(pData[10]), _tcslen(FloatToCstring(pData[10])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 8 - 100;
			rc.right = nOneWidth * 8 + 100;
			m_memReal.DrawText(FloatToCstring(pData[12]), _tcslen(FloatToCstring(pData[12])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[14]), _tcslen(FloatToCstring(pData[14])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 18 - 100;
			rc.right = nOneWidth * 18 + 100;
			m_memReal.DrawText(FloatToCstring(pData[16]), _tcslen(FloatToCstring(pData[16])), &rc, DT_CENTER | DT_VCENTER);

		//1 
		rcUp.left = 5 * nOneWidth - nLong;
		rcUp.right = 5 * nOneWidth + nLong;
		rcUp.bottom = m_rectUp.Height() / 2 - 10;
		rcUp.top = rcUp.bottom - 25;

		rcDown.left = 5 * nOneWidth - nLong;
		rcDown.right = 5 * nOneWidth + nLong;
		rcDown.top = m_rectUp.Height() / 2 + 10;
		rcDown.bottom = rcDown.top + 25;

		m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
		m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));
		// 物料示意
		rcUp.left = 5 * nOneWidth - nLong + j;
		rcUp.right = 5 * nOneWidth + nLong - j;
		rcUp.bottom = m_rectUp.Height() / 2 - 10;
		rcUp.top = rcUp.bottom - 25;

		rcDown.left = 5 * nOneWidth - nLong + j;
		rcDown.right = 5 * nOneWidth + nLong - j;
		rcDown.top = m_rectUp.Height() / 2 + 10;
		rcDown.bottom = rcDown.top + 25;

		m_memReal.FillSolidRect(&rcUp, crlB);
		m_memReal.FillSolidRect(&rcDown, crlB);

		// 2->at9
		rcUp.left = 15 * nOneWidth - nLong;
		rcUp.right = 15 * nOneWidth + nLong;
		rcUp.bottom = m_rectUp.Height() / 2 - 10;
		rcUp.top = rcUp.bottom - 20;

		rcDown.left = 15 * nOneWidth - nLong;
		rcDown.right = 15 * nOneWidth + nLong;
		rcDown.top = m_rectUp.Height() / 2 + 10;
		rcDown.bottom = rcDown.top + 20;

		m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
		m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));
		// 2
		rcUp.left = 15 * nOneWidth - nLong + j;
		rcUp.right = 15 * nOneWidth + nLong - j;
		rcUp.bottom = m_rectUp.Height() / 2 - 10;
		rcUp.top = rcUp.bottom - 20;

		rcDown.left = 15 * nOneWidth - nLong + j;
		rcDown.right = 15 * nOneWidth + nLong - j;
		rcDown.top = m_rectUp.Height() / 2 + 10;
		rcDown.bottom = rcDown.top + 20;

		m_memReal.FillSolidRect(&rcUp, crlB);
		m_memReal.FillSolidRect(&rcDown, crlB);
		}
		break;
	case  6: // AT9   + 三条料    7+6 = 13

		{
			const int nOneWidth = m_rectUp.Width() / 18;
			nLong = nOneWidth * 2;

			rc.top = m_rectUp.top + 10 + 60;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);

			rc.left = 10;
			rc.right = rc.left + 200;
			m_memReal.DrawText(FloatToCstring(pData[0]), _tcslen(FloatToCstring(pData[0])), &rc, DT_LEFT | DT_VCENTER);

			rc.left = nOneWidth * 3 - 100;
			rc.right = nOneWidth * 3 + 100;
			m_memReal.DrawText(FloatToCstring(pData[2]), _tcslen(FloatToCstring(pData[2])), &rc,  DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[4]), _tcslen(FloatToCstring(pData[4])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 9 - 100;
			rc.right = nOneWidth * 9 + 100;
			m_memReal.DrawText(FloatToCstring(pData[6]), _tcslen(FloatToCstring(pData[6])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[8]), _tcslen(FloatToCstring(pData[8])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 15 - 100;
			rc.right = nOneWidth * 15 + 100;
			m_memReal.DrawText(FloatToCstring(pData[10]), _tcslen(FloatToCstring(pData[10])), &rc, DT_CENTER | DT_VCENTER);

			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[12]), _tcslen(FloatToCstring(pData[12])), &rc, DT_RIGHT | DT_VCENTER);
//down
			rc.bottom = m_rectUp.bottom - 10 - 30;
			rc.top = rc.bottom - 60 - 30;
			rc.left = 10;
			rc.right = rc.left + 200;
			m_memReal.DrawText(FloatToCstring(pData[13]), _tcslen(FloatToCstring(pData[13])), &rc, DT_LEFT | DT_VCENTER);

			rc.left = nOneWidth * 3 - 100;
			rc.right = nOneWidth * 3 + 100;
			m_memReal.DrawText(FloatToCstring(pData[15]), _tcslen(FloatToCstring(pData[15])), &rc, DT_CENTER | DT_VCENTER);


			rc.left = nOneWidth * 6 - 100;
			rc.right = nOneWidth * 6 + 100;
			m_memReal.DrawText(FloatToCstring(pData[17]), _tcslen(FloatToCstring(pData[17])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 9 - 100;
			rc.right = nOneWidth * 9 + 100;
			m_memReal.DrawText(FloatToCstring(pData[19]), _tcslen(FloatToCstring(pData[19])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 12 - 100;
			rc.right = nOneWidth * 12 + 100;
			m_memReal.DrawText(FloatToCstring(pData[21]), _tcslen(FloatToCstring(pData[21])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 15 - 100 - 15;
			rc.right = nOneWidth * 15 + 100 - 15;
			m_memReal.DrawText(FloatToCstring(pData[23]), _tcslen(FloatToCstring(pData[23])), &rc, DT_CENTER | DT_VCENTER);

			rc.right = m_rectUp.Width() - 10;
			rc.left = rc.right - 200;
			m_memReal.DrawText(FloatToCstring(pData[25]), _tcslen(FloatToCstring(pData[25])), &rc, DT_RIGHT | DT_VCENTER);
//at9 up value显示 
			rc.top = m_rectUp.top ;
			rc.bottom = rc.top + 60;
			rc.OffsetRect(-c_nAdjust, 0);
			rc.left = 10 + 100;
			rc.right = rc.left*2 + 100;
			m_memReal.DrawText(FloatToCstring(pData[1]), _tcslen(FloatToCstring(pData[1])), &rc, DT_LEFT | DT_VCENTER);

			rc.left = nOneWidth * 5 - 100;
			rc.right = nOneWidth * 5 + 100;
			m_memReal.DrawText(FloatToCstring(pData[3]), _tcslen(FloatToCstring(pData[3])), &rc,  DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 7 - 100;
			rc.right = nOneWidth * 7 + 100;
			m_memReal.DrawText(FloatToCstring(pData[5]), _tcslen(FloatToCstring(pData[5])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 11 - 100;
			rc.right = nOneWidth * 11 + 100;
			m_memReal.DrawText(FloatToCstring(pData[7]), _tcslen(FloatToCstring(pData[7])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 13 - 100;
			rc.right = nOneWidth * 13 + 100;
			m_memReal.DrawText(FloatToCstring(pData[9]), _tcslen(FloatToCstring(pData[9])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 17 - 100;
			rc.right = nOneWidth * 17 + 100;
			m_memReal.DrawText(FloatToCstring(pData[11]), _tcslen(FloatToCstring(pData[11])), &rc, DT_CENTER | DT_VCENTER);

	//down
			rc.bottom = m_rectUp.bottom ;
			rc.top = rc.bottom - 60;
			rc.left = 10 + 100;
			rc.right = rc.left *2 + 100;
			m_memReal.DrawText(FloatToCstring(pData[14]), _tcslen(FloatToCstring(pData[14])), &rc, DT_LEFT | DT_VCENTER);

			rc.left = nOneWidth * 5 - 100;
			rc.right = nOneWidth * 5 + 100;
			m_memReal.DrawText(FloatToCstring(pData[16]), _tcslen(FloatToCstring(pData[16])), &rc, DT_CENTER | DT_VCENTER);


			rc.left = nOneWidth * 7 - 100;
			rc.right = nOneWidth * 7 + 100;
			m_memReal.DrawText(FloatToCstring(pData[18]), _tcslen(FloatToCstring(pData[18])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 11 - 100;
			rc.right = nOneWidth * 11 + 100;
			m_memReal.DrawText(FloatToCstring(pData[20]), _tcslen(FloatToCstring(pData[20])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 13 - 100;
			rc.right = nOneWidth * 13 + 100;
			m_memReal.DrawText(FloatToCstring(pData[22]), _tcslen(FloatToCstring(pData[22])), &rc, DT_CENTER | DT_VCENTER);

			rc.left = nOneWidth * 17 - 100 ;
			rc.right = nOneWidth * 17 + 100;
			m_memReal.DrawText(FloatToCstring(pData[24]), _tcslen(FloatToCstring(pData[24])), &rc, DT_CENTER | DT_VCENTER);
//at9 up

			// 物料示意，白色
			rcUp.left = 3 * nOneWidth - nLong;
			rcUp.right = 3 * nOneWidth+ nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 3 * nOneWidth - nLong;
			rcDown.right = 3 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
			m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));
			// 2
			rcUp.left = 9 * nOneWidth - nLong;
			rcUp.right = 9 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 9 * nOneWidth - nLong;
			rcDown.right = 9 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
			m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));

			// 3
			rcUp.left = 15 * nOneWidth - nLong;
			rcUp.right = 15 * nOneWidth + nLong;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 15 * nOneWidth - nLong;
			rcDown.right = 15 * nOneWidth + nLong;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, RGB(255, 255, 255));
			m_memReal.FillSolidRect(&rcDown, RGB(255, 255, 255));
			// 物料示意，对照左边小的左移动 -- 画图层
			rcUp.left = 3 * nOneWidth - nLong + j;
			rcUp.right = 3 * nOneWidth+ nLong - j;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 3 * nOneWidth - nLong + j;
			rcDown.right = 3 * nOneWidth + nLong - j;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);

			// 2
			rcUp.left = 9 * nOneWidth - nLong + j;
			rcUp.right = 9 * nOneWidth + nLong - j;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 9 * nOneWidth - nLong + j;
			rcDown.right = 9 * nOneWidth + nLong - j;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;
			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);

			// 3
			rcUp.left = 15 * nOneWidth - nLong + j;
			rcUp.right = 15 * nOneWidth + nLong - j;
			rcUp.bottom = m_rectUp.Height() / 2 - 10;
			rcUp.top = rcUp.bottom - 25;

			rcDown.left = 15 * nOneWidth - nLong + j;
			rcDown.right = 15 * nOneWidth + nLong - j;
			rcDown.top = m_rectUp.Height() / 2 + 10;
			rcDown.bottom = rcDown.top + 25;

			m_memReal.FillSolidRect(&rcUp, crlB);
			m_memReal.FillSolidRect(&rcDown, crlB);
		}
		break;

	default:
		break;
	}
}

// 获得1网格(20p)代表的宽度
float CMainDialog::GetStep()
{
	float fStep = 0;	
	switch (m_dlgStandard.Show())
	{
	case 10000:
		fStep = 200;
		break;

	case 5000:
		fStep = 100;
		break;

	case 4000:
		fStep = 80;
		break;

	case 2000:
		fStep = 40;
		break;

	case 1000:
		fStep = 20;
		break;

	case 500:
		fStep = 10;
		break;

	case 250:
		fStep = 5;
		break;

	case 200:
		fStep = 4;
		break;

	case 100:
		fStep = 2;
		break;

	case 50:
		fStep = 1;
		break;

	case 25:
		fStep = 0.5;
		break;
	default:
		break;
	}

	return fStep;
}

void CMainDialog::DrawOneHScale(CDC &dc, CRect & rc)
{
	CString str;
	const int nOne = 20;
	// 左下
	int nHor = 1;
	for (int i = nOne; i < rc.Width(); i += nOne)
	{
		if (nHor % 5 != 0)
		{
			nHor++;
			continue;
		}

		if(nHor < 10)
		{
			str.Format(_T("%d"), nHor);
			dc.TextOut(i - 3, 0, str);
		}
		else if(nHor < 100)
		{
			str.Format(_T("%d"), nHor);
			dc.TextOut(i - 6, 0, str);
		}
		else
		{
			str.Format(_T("%d"), nHor);
			dc.TextOut(i - 9, 0, str);
		}
		nHor++;
	}
}

// 画纵向刻度
void CMainDialog::DrawOneVScale(CDC & dc, int nNormal, CRect & rc, BOOL bLeft/* = TRUE*/)
{
	CString str;
	const int nOne = 20;
	int nNormalMaterial = m_dlgStandard.Material();
	int nNormalEar = m_dlgStandard.Ear();

	float fStep = GetStep();
	int nMaxHeight = rc.Height()/40*40;
	//  宽度		像素
	//		  |	|
	//	  ?	  |	|			} nMaxHeight / 2 
	//		  | |
	//	fStep | | nOne
	float fVer = nNormal - ((float)nMaxHeight/2) * fStep / nOne;	// 最低端宽度
	if (fVer < 0)
		fVer = 0;

	for(int i = 0; i < rc.Height(); i += nOne)
	{
		if (fVer - (int)fVer != 0)
		{
			fVer += fStep;
			continue;
		}

		str.Format(_T("%.0f"), fVer);

		if (fVer < 100)
			dc.TextOut(bLeft ? -19 : m_rectChartRightUp.Width() - 19, i + 8, str);
		else if (fVer < 1000)
			dc.TextOut(bLeft ? -27 : m_rectChartRightUp.Width() - 27, i + 8, str);
		else if (fVer < 10000)
			dc.TextOut(bLeft ? -35 : m_rectChartRightUp.Width() - 35, i + 8, str);
		else
			dc.TextOut(bLeft ? -40 : m_rectChartRightUp.Width() - 40, i + 8, str);

		fVer += fStep;
	}
}

void CMainDialog::SetChartRect(CRect & chartRect, CRect & rect)
{
	chartRect.left = rect.left + 40;
	chartRect.top = rect.left + 5;
	chartRect.right = rect.right - 5;
	chartRect.bottom = rect.bottom - 20;
}

// 画说明部分
void CMainDialog::DrawNote()
{
	CRect rc;

// 	rc.left = m_rectChartLeftUp.right - 150;
// 	rc.top = m_rectChartLeftUp.bottom - 50;
// 	rc.right = m_rectChartLeftUp.right;
// 	rc.bottom = m_rectChartLeftUp.bottom;
// 	m_memLeftUp.FillSolidRect(&rc, RGB(200, 200, 200));
// 
// 	rc.left = m_rectChartRightUp.right - 150;
// 	rc.top = m_rectChartRightUp.bottom - 100;
// 	rc.right = m_rectChartRightUp.right;
// 	rc.bottom = m_rectChartRightUp.bottom;
// 	m_memRightUp.FillSolidRect(&rc, RGB(200, 200, 200));
// 
// 	rc.left = m_rectChartLeftDown.right - 150;
// 	rc.top = m_rectChartLeftDown.bottom - 100;
// 	rc.right = m_rectChartLeftDown.right;
// 	rc.bottom = m_rectChartLeftDown.bottom;
// 	m_memLeftDown.FillSolidRect(&rc, RGB(200, 200, 200));
// 
// 	rc.left = m_rectChartRightDown.right - 150;
// 	rc.top = m_rectChartRightDown.bottom - 100;
// 	rc.right = m_rectChartRightDown.right;
// 	rc.bottom = m_rectChartRightDown.bottom;
// 	m_memRightDown.FillSolidRect(&rc, RGB(200, 200, 200));
	m_memLeftUp.SetBkColor(RGB(200, 200, 200));
	m_memLeftUp.SetTextColor(RGB(0,0,0));
	m_memRightUp.SetBkColor(RGB(200, 200, 200));
	m_memRightUp.SetTextColor(RGB(0,0,0));

	m_memLeftDown.SetBkColor(RGB(200, 200, 200));
	m_memLeftDown.SetTextColor(RGB(0,0,0));
	m_memRightDown.SetBkColor(RGB(200, 200, 200));
	m_memRightDown.SetTextColor(RGB(0,0,0));
	
	// 
	// 上
	// 

	// 1料
	m_memLeftUp.SelectObject(m_penMaterial1);
	m_memLeftUp.MoveTo(m_rectChartLeftUp.right - 120, m_rectChartLeftUp.bottom - 50);
	m_memLeftUp.LineTo(m_rectChartLeftUp.right - 70, m_rectChartLeftUp.bottom - 50);
	m_memLeftUp.TextOut(m_rectChartLeftUp.right - 60, m_rectChartLeftUp.bottom - 60, _T("上物料1"));

	// 2料
	if (m_Mode > 1)
	{
		m_memLeftUp.SelectObject(m_penMaterial2);
		m_memLeftUp.MoveTo(m_rectChartLeftUp.right - 120, m_rectChartLeftUp.bottom - 30);
		m_memLeftUp.LineTo(m_rectChartLeftUp.right - 70, m_rectChartLeftUp.bottom - 30);
		m_memLeftUp.TextOut(m_rectChartLeftUp.right - 60, m_rectChartLeftUp.bottom - 40, _T("上物料2"));
	}
	
	// 3料
	if (m_Mode > 2)
	{
		m_memLeftUp.SelectObject(m_penMaterial3);
		m_memLeftUp.MoveTo(m_rectChartLeftUp.right - 120, m_rectChartLeftUp.bottom - 10);
		m_memLeftUp.LineTo(m_rectChartLeftUp.right - 70, m_rectChartLeftUp.bottom - 10);
		m_memLeftUp.TextOut(m_rectChartLeftUp.right - 60, m_rectChartLeftUp.bottom - 20, _T("上物料3"));
	}

	// 
	// 下
	// 
	// 1料
	m_memLeftDown.SelectObject(m_penMaterial1);
	m_memLeftDown.MoveTo(m_rectChartLeftDown.right - 120, m_rectChartLeftDown.bottom - 50);
	m_memLeftDown.LineTo(m_rectChartLeftDown.right - 70, m_rectChartLeftDown.bottom - 50);
	m_memLeftDown.TextOut(m_rectChartLeftDown.right - 60, m_rectChartLeftDown.bottom - 60, _T("下物料1"));

	// 2料
	if (m_Mode > 1)
	{
		m_memLeftDown.SelectObject(m_penMaterial2);
		m_memLeftDown.MoveTo(m_rectChartLeftDown.right - 120, m_rectChartLeftDown.bottom - 30);
		m_memLeftDown.LineTo(m_rectChartLeftDown.right - 70, m_rectChartLeftDown.bottom - 30);
		m_memLeftDown.TextOut(m_rectChartLeftDown.right - 60, m_rectChartLeftDown.bottom - 40, _T("下物料2"));
	}

	// 3料
	if (m_Mode > 2)
	{
		m_memLeftDown.SelectObject(m_penMaterial3);
		m_memLeftDown.MoveTo(m_rectChartLeftDown.right - 120, m_rectChartLeftDown.bottom - 10);
		m_memLeftDown.LineTo(m_rectChartLeftDown.right - 70, m_rectChartLeftDown.bottom - 10);
		m_memLeftDown.TextOut(m_rectChartLeftDown.right - 60, m_rectChartLeftDown.bottom - 20, _T("下物料3"));
	}

	//
	// 上
	//
	// 极耳1
	m_memRightUp.SelectObject(m_penEar1);
	m_memRightUp.MoveTo(m_rectChartRightUp.right - 160, m_rectChartRightUp.bottom - 70);
	m_memRightUp.LineTo(m_rectChartRightUp.right - 110, m_rectChartRightUp.bottom - 70);
	m_memRightUp.TextOut(m_rectChartRightUp.right - 100, m_rectChartRightUp.bottom - 80, _T("上间隙1"));

	// 极耳2
	m_memRightUp.SelectObject(m_penEar2);
	m_memRightUp.MoveTo(m_rectChartRightUp.right - 160, m_rectChartRightUp.bottom - 50);
	m_memRightUp.LineTo(m_rectChartRightUp.right - 110, m_rectChartRightUp.bottom - 50);
	m_memRightUp.TextOut(m_rectChartRightUp.right - 100, m_rectChartRightUp.bottom - 60, _T("上间隙2"));

	// 极耳3
	if (m_Mode > 1)
	{
		m_memRightUp.SelectObject(m_penEar3);
		m_memRightUp.MoveTo(m_rectChartRightUp.right - 160, m_rectChartRightUp.bottom - 30);
		m_memRightUp.LineTo(m_rectChartRightUp.right - 110, m_rectChartRightUp.bottom - 30);
		m_memRightUp.TextOut(m_rectChartRightUp.right - 100, m_rectChartRightUp.bottom - 40, _T("上间隙3"));
	}

	// 极耳4
	if (m_Mode > 2)
	{
		m_memRightUp.SelectObject(m_penEar4);
		m_memRightUp.MoveTo(m_rectChartRightUp.right - 160, m_rectChartRightUp.bottom - 10);
		m_memRightUp.LineTo(m_rectChartRightUp.right - 110, m_rectChartRightUp.bottom - 10);
		m_memRightUp.TextOut(m_rectChartRightUp.right - 100, m_rectChartRightUp.bottom - 20, _T("上间隙4"));
	}

	// 
	// 下
	//
	// 极耳1
	m_memRightDown.SelectObject(m_penEar1);
	m_memRightDown.MoveTo(m_rectChartRightDown.right - 160, m_rectChartRightDown.bottom - 70);
	m_memRightDown.LineTo(m_rectChartRightDown.right - 110, m_rectChartRightDown.bottom - 70);
	m_memRightDown.TextOut(m_rectChartRightDown.right - 100, m_rectChartRightDown.bottom - 80, _T("下间隙1"));

	// 极耳2
	m_memRightDown.SelectObject(m_penEar2);
	m_memRightDown.MoveTo(m_rectChartRightDown.right - 160, m_rectChartRightDown.bottom - 50);
	m_memRightDown.LineTo(m_rectChartRightDown.right - 110, m_rectChartRightDown.bottom - 50);
	m_memRightDown.TextOut(m_rectChartRightDown.right - 100, m_rectChartRightDown.bottom - 60, _T("下间隙2"));

	// 极耳3
	if (m_Mode > 1)
	{
		m_memRightDown.SelectObject(m_penEar3);
		m_memRightDown.MoveTo(m_rectChartRightDown.right - 160, m_rectChartRightDown.bottom - 30);
		m_memRightDown.LineTo(m_rectChartRightDown.right - 110, m_rectChartRightDown.bottom - 30);
		m_memRightDown.TextOut(m_rectChartRightDown.right - 100, m_rectChartRightDown.bottom - 40, _T("下间隙3"));
	}

	// 极耳4
	if (m_Mode > 2)
	{
		m_memRightDown.SelectObject(m_penEar4);
		m_memRightDown.MoveTo(m_rectChartRightDown.right - 160, m_rectChartRightDown.bottom - 10);
		m_memRightDown.LineTo(m_rectChartRightDown.right - 110, m_rectChartRightDown.bottom - 10);
		m_memRightDown.TextOut(m_rectChartRightDown.right - 100, m_rectChartRightDown.bottom - 20, _T("下间隙4"));
	}
}

//  背景+刻度(接收到数据不用更新)
void CMainDialog::DrawScale()
{
	m_memLeftUp.FillSolidRect(&m_rectLeftUp, RGB(0, 0, 200));
	m_memRightUp.FillSolidRect(&m_rectRightUp, RGB(0, 0, 200));
	m_memLeftDown.FillSolidRect(&m_rectLeftDown, RGB(0, 0, 200));
	m_memRightDown.FillSolidRect(&m_rectRightDown, RGB(0, 0, 200));

	SetChartRect(m_rectChartLeftUp, m_rectLeftUp);
	SetChartRect(m_rectChartLeftDown, m_rectLeftDown);
	SetChartRect(m_rectChartRightUp, m_rectRightUp);
	SetChartRect(m_rectChartRightDown, m_rectRightDown);
	
	m_memLeftDown.SetBkColor(RGB(0,0,200));
	m_memLeftDown.SetTextColor(RGB(255,255,255));

	m_memRightDown.SetBkColor(RGB(0,0,200));
	m_memRightDown.SetTextColor(RGB(255,255,255));

	m_memLeftUp.SetBkColor(RGB(0,0,200));
	m_memLeftUp.SetTextColor(RGB(255,255,255));

	m_memRightUp.SetBkColor(RGB(0,0,200));
	m_memRightUp.SetTextColor(RGB(255,255,255));

	// 自定义坐标
	SetCoorMode(m_memLeftDown, m_rectChartLeftDown.left, m_rectChartLeftDown.bottom);
	SetCoorMode(m_memRightDown, m_rectChartRightDown.left, m_rectChartRightDown.bottom);
	SetCoorMode(m_memLeftUp, m_rectChartLeftUp.left, m_rectChartLeftUp.bottom);
	SetCoorMode(m_memRightUp, m_rectChartRightUp.left, m_rectChartRightUp.bottom);

	//
	// 横向刻度
	// 
	DrawOneHScale(m_memLeftUp, m_rectChartLeftUp);
	DrawOneHScale(m_memRightUp, m_rectChartRightUp);
	DrawOneHScale(m_memLeftDown, m_rectChartLeftDown);
	DrawOneHScale(m_memRightDown, m_rectChartRightDown);

	// 
	// 纵向刻度
	// 
	int nNormalMaterial = m_dlgStandard.Material();
	int nNormalEar = m_dlgStandard.Ear();

	DrawOneVScale(m_memLeftUp, nNormalMaterial, m_rectChartLeftUp);
	DrawOneVScale(m_memRightUp, nNormalEar, m_rectChartRightUp);
	DrawOneVScale(m_memLeftDown, nNormalMaterial, m_rectChartLeftDown);
	DrawOneVScale(m_memRightDown, nNormalEar, m_rectChartRightDown);

	// 间隙右端刻度
	DrawOneVScale(m_memRightUp, m_Mode == 1 ||m_Mode == 4 ? nNormalEar : 2 * nNormalEar, m_rectChartRightUp, FALSE);
	DrawOneVScale(m_memRightDown, m_Mode == 1 ||m_Mode == 4 ? nNormalEar : 2 * nNormalEar, m_rectChartRightDown, FALSE);

	// 恢复默认逻辑坐标
	RestoreCoor(m_memLeftDown);
	RestoreCoor(m_memRightDown);
	RestoreCoor(m_memLeftUp);
	RestoreCoor(m_memRightUp);
}

// 画一个图表的辅助线
void CMainDialog::DrawOneGuideLine(CDC &dc, CRect & rc, BOOL bLeft)
{
	const int nOne = 20;	// 一个方格的长为20像素

	// 横线
	for(int i = nOne; i < rc.Height(); i += nOne)
	{
		dc.MoveTo(0, i);
		dc.LineTo(bLeft ? rc.Width() : rc.Width() - 40, i);
	}

	// 竖线
	int nWidth = bLeft ? rc.Width() : rc.Width() - 40;
	for (int i = nOne; i < nWidth; i += nOne)
	{
		dc.MoveTo(i, 0);
		dc.LineTo(i, rc.Height() + 1);
	}
}

// 画白色背景和辅助线
void CMainDialog::DrawGuideLine()
{
	// 白色背景
	m_memLeftUp.FillSolidRect(&m_rectChartLeftUp, RGB(240, 240, 240));
	m_memLeftDown.FillSolidRect(&m_rectChartLeftDown, RGB(240, 240, 240));
	CRect rcUp(m_rectChartRightUp), rcDown(m_rectChartRightDown);
	rcUp.right -= 40;
	rcDown.right -= 40;
	m_memRightUp.FillSolidRect(&rcUp, RGB(240, 240, 240));
	m_memRightDown.FillSolidRect(&rcDown, RGB(240, 240, 240));

	//
	// 画辅助线
	//
	m_memLeftUp.SelectObject(&m_penGuideLine);
	m_memRightUp.SelectObject(&m_penGuideLine);
	m_memLeftDown.SelectObject(&m_penGuideLine);
	m_memRightDown.SelectObject(&m_penGuideLine);

	SetCoorMode(m_memLeftUp, m_rectChartLeftUp.left, m_rectChartLeftUp.bottom);
	SetCoorMode(m_memRightUp, m_rectChartRightUp.left, m_rectChartRightUp.bottom);
	SetCoorMode(m_memLeftDown, m_rectChartLeftDown.left, m_rectChartLeftDown.bottom);
	SetCoorMode(m_memRightDown, m_rectChartRightDown.left, m_rectChartRightDown.bottom);

	DrawOneGuideLine(m_memLeftUp, m_rectChartLeftUp);
	DrawOneGuideLine(m_memLeftDown, m_rectChartLeftDown);
	DrawOneGuideLine(m_memRightUp, m_rectChartRightUp, FALSE);
	DrawOneGuideLine(m_memRightDown, m_rectChartRightDown, FALSE);

	RestoreCoor(m_memLeftUp);
	RestoreCoor(m_memRightUp);
	RestoreCoor(m_memLeftDown);
	RestoreCoor(m_memRightDown);
}

// 内存dc上绘制下端数据线
void CMainDialog::DrawGrap()
{
	if (m_memLeftUp== NULL)
	{
		return;
	}
	SetCoorMode(m_memLeftUp, m_rectChartLeftUp.left, m_rectChartLeftUp.bottom);
	SetCoorMode(m_memRightUp, m_rectChartRightUp.left, m_rectChartRightUp.bottom);
	SetCoorMode(m_memLeftDown, m_rectChartLeftDown.left, m_rectChartLeftDown.bottom);
	SetCoorMode(m_memRightDown, m_rectChartRightDown.left, m_rectChartRightDown.bottom);

	CRgn rgnLeftUp;
	CRgn rgnRightUp;	
	CRgn rgnLeftDown;	
	CRgn rgnRightDown;

	rgnLeftUp.CreateRectRgn(m_rectChartLeftUp.left, m_rectChartLeftUp.top, 
		m_rectChartLeftUp.right, m_rectChartLeftUp.bottom);
	rgnLeftDown.CreateRectRgn(m_rectChartLeftDown.left, m_rectChartLeftDown.top, 
		m_rectChartLeftDown.right, m_rectChartLeftDown.bottom);
	rgnRightUp.CreateRectRgn(m_rectChartRightUp.left, m_rectChartRightUp.top, 
		m_rectChartRightUp.right - 40, m_rectChartRightUp.bottom);
	rgnRightDown.CreateRectRgn(m_rectChartRightDown.left, m_rectChartRightDown.top, 
		m_rectChartRightDown.right - 40, m_rectChartRightDown.bottom);


	// 设置只在制定区域绘图
	m_memLeftUp.SelectClipRgn(&rgnLeftUp, RGN_AND);
	m_memRightUp.SelectClipRgn(&rgnRightUp, RGN_AND);
	m_memLeftDown.SelectClipRgn(&rgnLeftDown, RGN_AND);
	m_memRightDown.SelectClipRgn(&rgnRightDown, RGN_AND);

	const int nOne = 20;
	int nMaterial = m_dlgStandard.Material();
	float fStep = GetStep();
	int nMaxHeight = m_rectChartLeftDown.Height()/40*40;
	float fMin = nMaterial - ((float)nMaxHeight/2) * fStep / nOne;
	if (fMin < 0)
		fMin = 0;

	switch (m_Mode)
	{
	case 1:
	case 4:
		{
			//
			// 上1
			// 
			m_memLeftUp.SelectObject(m_penMaterial1);

			int nHeight = 0;	// 点的纵像素
			nHeight = (int)((m_RcvBuff[0][1] - fMin) * nOne / fStep);
			int nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][1] == 0)
					continue;
				nHeight = (int)((m_RcvBuff[nArr][1] - fMin) * nOne / fStep);
				m_memLeftUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 下4
			//
			m_memLeftDown.SelectObject(m_penMaterial1);

			nHeight = 0;	// 点的纵像素
			nHeight = (int)((m_RcvBuff[0][4] - fMin) * nOne / fStep);
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][4] == 0)
					continue;
				nHeight = (int)((m_RcvBuff[nArr][4] - fMin) * nOne / fStep);
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			// 极耳
			//
			// 上0
			//
			m_memRightUp.SelectObject(m_penEar1);

			int nEar = m_dlgStandard.Ear();
			nMaxHeight = m_rectChartRightUp.Height()/40*40;
			fMin = nEar - nMaxHeight / 2 * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (int)((m_RcvBuff[0][0] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][0] == 0)
					continue;
				nHeight = (int)((m_RcvBuff[nArr][0] - fMin) * nOne / fStep);
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 上2
			//
			m_memRightUp.SelectObject(m_penEar2);

			nHeight =int ((m_RcvBuff[0][2] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][2] == 0)
					continue;
				nHeight =int ((m_RcvBuff[nArr][2] - fMin) * nOne / fStep);
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下3
			//
			m_memRightDown.SelectObject(m_penEar1);

			nHeight = int ((m_RcvBuff[0][3] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][3] == 0)
					continue;
				nHeight =int( (m_RcvBuff[nArr][3] - fMin) * nOne / fStep);
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下5
			//
			m_memRightDown.SelectObject(m_penEar2);

			nHeight =int ((m_RcvBuff[0][5] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][5] == 0)
					continue;
				nHeight =int ((m_RcvBuff[nArr][5] - fMin) * nOne / fStep);
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

		}
		break;
	case 5:
	case 2:
		{
			//
			// 上1
			// 
			m_memLeftUp.SelectObject(m_penMaterial1);

			int nHeight = 0;	// 点的纵像素
			nHeight = int((m_RcvBuff[0][1] - fMin) * nOne / fStep);
			int nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][1] == 0)
					continue;
				nHeight = int ((m_RcvBuff[nArr][1] - fMin) * nOne / fStep);
				m_memLeftUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 上3
			// 
			m_memLeftUp.SelectObject(m_penMaterial2);

			nHeight = 0;	// 点的纵像素
			nHeight = int((m_RcvBuff[0][3] - fMin) * nOne / fStep);
			nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][3] == 0)
					continue;
				nHeight = int((m_RcvBuff[nArr][3] - fMin) * nOne / fStep);
				m_memLeftUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 下6
			//
			m_memLeftDown.SelectObject(m_penMaterial1);

			nHeight = 0;	// 点的纵像素
			nHeight = int ((m_RcvBuff[0][6] - fMin) * nOne / fStep);
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][6] == 0)
					continue;
				nHeight =int ((m_RcvBuff[nArr][6] - fMin) * nOne / fStep);
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 下8
			//
			m_memLeftDown.SelectObject(m_penMaterial2);

			nHeight = 0;	// 点的纵像素
			nHeight = int ((m_RcvBuff[0][8] - fMin) * nOne / fStep);
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][8] == 0)
					continue;
				nHeight = int((m_RcvBuff[nArr][8] - fMin) * nOne / fStep);
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}


			// 极耳
			//
			// 上0
			//
			m_memRightUp.SelectObject(m_penEar1);

			int nEar = m_dlgStandard.Ear();
			nMaxHeight = m_rectChartRightUp.Height()/40*40;
			fMin = nEar - nMaxHeight / 2 * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int ((m_RcvBuff[0][0] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][0] == 0)
					continue;
				nHeight =int( (m_RcvBuff[nArr][0] - fMin) * nOne / fStep);
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 上2
			//
			m_memRightUp.SelectObject(m_penEar2);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int ((m_RcvBuff[0][2] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][2] == 0)
					continue;
				nHeight = int ((m_RcvBuff[nArr][2] - fMin) * nOne / fStep);
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			


			//
			// 上4
			//
			m_memRightUp.SelectObject(m_penEar3);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int((m_RcvBuff[0][4] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][4] == 0)
					continue;
				nHeight =int((m_RcvBuff[nArr][4] - fMin) * nOne / fStep);
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			
			//
			// 下5
			//
			m_memRightDown.SelectObject(m_penEar1);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int((m_RcvBuff[0][5] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][5] == 0)
					continue;
				nHeight = int ((m_RcvBuff[nArr][5] - fMin) * nOne / fStep);
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下7
			//
			m_memRightDown.SelectObject(m_penEar2);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int ((m_RcvBuff[0][7] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][7] == 0)
					continue;
				nHeight = int((m_RcvBuff[nArr][7] - fMin) * nOne / fStep);
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下9
			//
			m_memRightDown.SelectObject(m_penEar3);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = int ((m_RcvBuff[0][9] - fMin) * nOne / fStep);  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][9] == 0)
					continue;
				nHeight =int( (m_RcvBuff[nArr][9] - fMin) * nOne / fStep);
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			
		}
		break;
	case 6:
	case 3:
		{
			// 物料

			//
			// 上1
			// 
			m_memLeftUp.SelectObject(m_penMaterial1);

			int nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][1] - fMin) * nOne / fStep;
			int nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][1] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][1] - fMin) * nOne / fStep;
				m_memLeftUp.LineTo(i, nHeight);
				
				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 上3
			// 
			m_memLeftUp.SelectObject(m_penMaterial2);

			nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][3] - fMin) * nOne / fStep;
			nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][3] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][3] - fMin) * nOne / fStep;
				m_memLeftUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 上5
			// 
			m_memLeftUp.SelectObject(m_penMaterial3);

			nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][5] - fMin) * nOne / fStep;
			nArr = 0;
			m_memLeftUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][5] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][5] - fMin) * nOne / fStep;
				m_memLeftUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 下8
			//
			m_memLeftDown.SelectObject(m_penMaterial1);

			nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][8] - fMin) * nOne / fStep;
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][8] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][8] - fMin) * nOne / fStep;
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}
			
			//
			// 下10
			//
			m_memLeftDown.SelectObject(m_penMaterial2);

			nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][10] - fMin) * nOne / fStep;
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][10] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][10] - fMin) * nOne / fStep;
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			//
			// 下12
			//
			m_memLeftDown.SelectObject(m_penMaterial3);

			nHeight = 0;	// 点的纵像素
			nHeight = (m_RcvBuff[0][12] - fMin) * nOne / fStep;
			nArr = 0;
			m_memLeftDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartLeftDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][12] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][12] - fMin) * nOne / fStep;
				m_memLeftDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}

			
			// 极耳

			//
			// 上0
			//
			m_memRightUp.SelectObject(m_penEar1);

			int nEar = m_dlgStandard.Ear();
			nMaxHeight = m_rectChartRightUp.Height()/40*40;
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][0] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][0] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][0] - fMin) * nOne / fStep;
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 上2
			//
			m_memRightUp.SelectObject(m_penEar2);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][2] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][2] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][2] - fMin) * nOne / fStep;
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			


			//
			// 上4
			//
			m_memRightUp.SelectObject(m_penEar3);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][4] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][4] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][4] - fMin) * nOne / fStep;
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 上6
			//
			m_memRightUp.SelectObject(m_penEar4);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][6] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightUp.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightUp.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][6] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][6] - fMin) * nOne / fStep;
				m_memRightUp.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			


			//
			// 下7
			//
			m_memRightDown.SelectObject(m_penEar1);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][7] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][7] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][7] - fMin) * nOne / fStep;
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下9
			//
			m_memRightDown.SelectObject(m_penEar2);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][9] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][9] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][9] - fMin) * nOne / fStep;
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下11
			//
			m_memRightDown.SelectObject(m_penEar3);

			nEar = 2 * m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][11] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(nOne, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() + nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][11] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][11] - fMin) * nOne / fStep;
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			

			//
			// 下13
			//
			m_memRightDown.SelectObject(m_penEar4);

			nEar = m_dlgStandard.Ear();
			fMin = nEar - ((float)nMaxHeight/2) * fStep / nOne;
			if (fMin < 0)
				fMin = 0;

			nHeight = (m_RcvBuff[0][13] - fMin) * nOne / fStep;  // 点的纵像素
			nArr = 0;
			m_memRightDown.MoveTo(0, nHeight);
			for (int i = nOne; i < m_rectChartRightDown.Width() - 2 * nOne; i += nOne)
			{
				nArr++;
				if (m_RcvBuff[nArr][13] == 0)
					continue;
				nHeight = (m_RcvBuff[nArr][13] - fMin) * nOne / fStep;
				m_memRightDown.LineTo(i, nHeight);

				if (nArr >= MAX_RCV_LENGTH - 1)
				{
					return;
				}
			}			
		}
		break;

	default:
		break;
	}

	m_memLeftUp.SelectClipRgn(NULL);
	m_memLeftUp.SelectClipRgn(NULL);
	m_memRightUp.SelectClipRgn(NULL);
	m_memLeftDown.SelectClipRgn(NULL);
	m_memRightDown.SelectClipRgn(NULL);

	RestoreCoor(m_memLeftUp);
	RestoreCoor(m_memRightUp);
	RestoreCoor(m_memLeftDown);
	RestoreCoor(m_memRightDown);
}

void CMainDialog::OnBnClickedBtnSetup()
{
	m_SetupDlg.DoModal();
}



CRect CMainDialog::SetupRect(int m_HStart, int m_SStart)
{
	CRect m_ClientRect;

	m_ClientRect.left = m_HStart;
	m_ClientRect.right = m_ClientRect.left + 80;
	m_ClientRect.top = m_SStart;
	m_ClientRect.bottom = m_SStart + 40;
	return m_ClientRect;
}


HBRUSH CMainDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
 		pDC->SetTextColor(RGB(0,255,0));
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SelectObject(&m_Font);
		return m_background;
	}

	return m_appBrush;
}

LRESULT CMainDialog::OnModeMsg(WPARAM wParam, LPARAM lParam)
{
	OnChangeDisp(lParam);
	Writeconfig();

	m_CCD5000.Send(wParam,0);
	return TRUE;
}

LRESULT CMainDialog::OnErrorMsg(WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	CString str;
	str.Format(_T("%04i-%02i-%02i  %02i:%02i:%02i"),
		time.wYear,time.wMonth,
		time.wDay,time.wHour,
		time.wMinute,time.wSecond);

// 	switch (lParam)
// 	{
// 	case 0:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("指令错误！"));
// 		break;
// 	case 1:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("帧头错误！"));
// 		break;
// 	case 2:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("长度错误！"));
// 		break;
// 	case 3:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("CRC校验错误！"));
// 		break;
// 	case 4:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("帧尾错误！"));
// 		break;
// 	default:
// 		InsertHRInfoItem(&m_Info,str,wParam,_T("未知错误！"));
// 		break;
// 	}
// 
	m_CCD5000.Send(wParam,1);
	return TRUE;
}

// 拷贝这次收到的数据到缓冲区
void CMainDialog::CopyRcvToBuff(float *pData, int nLength)
{
	if (!pData)
	{
		return;
	}
	int nLeng = 0;
	if (m_Mode == 1)
		nLeng = 6;
	else if (m_Mode == 2)
		nLeng = 10;
	else
		nLeng = 14;

	for (int i = MAX_RCV_LENGTH - 2; i >= 0; i--)
		memcpy(m_RcvBuff[i+1], m_RcvBuff[i], sizeof(m_RcvBuff[i]));
	
	memcpy(m_RcvBuff[0], pData, min(nLength, nLeng) * sizeof(float));
}

// 接收到串口消息后更新界面
void CMainDialog::RcvUpdate(float* pData, DWORD dwLength)
{
	if (!pData || !dwLength)
		return;

	m_section.Lock();
	InitialDBB();
	DrawReal(pData);
    DrawScale();		// 画蓝色背景和刻度(接收到数据不用更新)
	DrawGuideLine();	// 画辅助线	

	CopyRcvToBuff(pData, dwLength);	// 复制数据到缓冲区
	DrawGrap();
	DrawNote();
	BitBltAll();

	m_section.Unlock();
}

DWORD WINAPI CMainDialog::AlarmSequence(CMainDialog* lpNotify)
{
	BOOL isExitSequence(FALSE);

	HANDLE _Signals[] = {
		lpNotify->m_AlarmthreadExit,
		lpNotify->m_AlarmEvent,
	};

	DWORD dwEvent;

	double dvalue[14] = {0};
	int num = 0;

	while(TRUE)
	{
		double data[14] = {0};
		data[0] = lpNotify -> m_dlgStandard.m_updwEar1;
		data[1] = lpNotify -> m_dlgStandard.m_updwMaterial1;
		data[2] = lpNotify -> m_dlgStandard.m_updwEar2;
		data[3] = lpNotify -> m_dlgStandard.m_updwMaterial2;
		data[4] = lpNotify -> m_dlgStandard.m_updwEar3;
		data[5] = lpNotify -> m_dlgStandard.m_updwMaterial3;
		data[6] = lpNotify -> m_dlgStandard.m_updwEar4;
		data[7] = lpNotify -> m_dlgStandard.m_downdwEar1;
		data[8] = lpNotify -> m_dlgStandard.m_downdwMaterial1;
		data[9] = lpNotify -> m_dlgStandard.m_downdwEar2;
		data[10] = lpNotify -> m_dlgStandard.m_downdwMaterial2;
		data[11] = lpNotify -> m_dlgStandard.m_downdwEar3;
		data[12] = lpNotify -> m_dlgStandard.m_downdwMaterial3;
		data[13] = lpNotify -> m_dlgStandard.m_downdwEar4;

		dwEvent = WaitForMultipleObjects(sizeof(_Signals) / sizeof(HANDLE), _Signals, FALSE, INFINITE);

		switch (dwEvent)
		{
		case WAIT_OBJECT_0 + 0:
			isExitSequence = TRUE;
			break;

		default:
			isExitSequence = FALSE;

			if( m_ProSpeed < m_LimitSpeed)
			{
				lpNotify->AlarmLight(Light_ERROR_Yellow);
				Sleep(500);
				continue;
			}

			EnterCriticalSection(&lpNotify->m_csShareddata);
			num = lpNotify->m_dValue.size();
			for ( int i = 0; i < num; i++)
			{
				dvalue[i] = lpNotify->m_dValue[i];
			}

			LeaveCriticalSection(&lpNotify->m_csShareddata);
			if( num == 6 )
			{
				if ( fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[3] - data[7]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[4] - data[8]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[5] - data[9]) > lpNotify -> m_dlgStandard.m_Alarm)
				{
					lpNotify->AlarmLight(Light_ERROR_Beep_Red);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else if (fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[3] - data[7]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[4] - data[8]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[5] - data[9]) > lpNotify -> m_dlgStandard.m_Warn)
				{
					lpNotify->AlarmLight(Light_ERROR_Yellow);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else
				{
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
			}
			else if ( num == 10 )
			{
				if ( fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[3] - data[3]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[4] - data[4]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[5] - data[7]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[6] - data[8]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[7] - data[9]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[8] - data[10]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[9] - data[11]) > lpNotify -> m_dlgStandard.m_Alarm)
				{
					lpNotify->AlarmLight(Light_ERROR_Beep_Red);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else if ( fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[3] - data[3]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[4] - data[4]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[5] - data[7]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[6] - data[8]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[7] - data[9]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[8] - data[10]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[9] - data[11]) > lpNotify -> m_dlgStandard.m_Warn)
				{
					lpNotify->AlarmLight(Light_ERROR_Yellow);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else
				{
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
			}
			else if ( num == 14 )
			{
				if ( fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Alarm 
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[3] - data[3]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[4] - data[4]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[5] - data[5]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[6] - data[6]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[7] - data[7]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[8] - data[8]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[9] - data[9]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[10] - data[10]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[11] - data[11]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[12] - data[12]) > lpNotify -> m_dlgStandard.m_Alarm
					|| fabs(dvalue[13] - data[13]) > lpNotify -> m_dlgStandard.m_Alarm)
				{
					lpNotify->AlarmLight(Light_ERROR_Beep_Red);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else if (fabs(dvalue[0] - data[0]) > lpNotify -> m_dlgStandard.m_Warn 
					|| fabs(dvalue[1] - data[1]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[2] - data[2]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[3] - data[3]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[4] - data[4]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[5] - data[5]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[6] - data[6]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[7] - data[7]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[8] - data[8]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[9] - data[9]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[10] - data[10]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[11] - data[11]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[12] - data[12]) > lpNotify -> m_dlgStandard.m_Warn
					|| fabs(dvalue[13] - data[13]) > lpNotify -> m_dlgStandard.m_Warn)
				{
					lpNotify->AlarmLight(Light_ERROR_Yellow);
					Sleep(800);
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
				else
				{
					lpNotify->AlarmLight(Light_ERROR_SUCCESS);
				}
			}
		}
		if ( isExitSequence )
		{
			break;
		}
	}
	return 0;
}

// 1卷
LRESULT CMainDialog::OnUpdate1Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	float* pData = (float*)lParam;

	EnterCriticalSection(&m_csShareddata);
	m_dValue.clear();
	double dvalue[6];
	for (int i = 0; i < 6; i++)
	{
		dvalue[i] = pData[i];
		m_dValue.push_back(pData[i]);
	}
	//CString m_textt,m_textT;
	Datadisplay tDatadisplay;
	SYSTEMTIME _systemtime;
	GetLocalTime(&_systemtime);
	tDatadisplay.datetime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), _systemtime.wYear, _systemtime.wMonth, _systemtime.wDay,
		_systemtime.wHour, _systemtime.wMinute, _systemtime.wSecond);
	//m_textt.Format(_T("%s"),tDatadisplay.datetime);
	//m_textT.Format(_T("%d"),1/*tDatadisplay.ordernumber*/);
	tDatadisplay.breed = _T("");
	tDatadisplay.operationalsource = _T("");
	for (int i = 0; i < 6; i ++)
	{
		tDatadisplay.arrfdata[i] = dvalue[i];
	}
	//if ( !m_Roll.RollName().IsEmpty() )
	//{
	//	m_CDataSql.InsetData(m_Roll.RollName(),tDatadisplay);
	//}

	LeaveCriticalSection(&m_csShareddata);

	SetEvent( m_AlarmEvent );

	if (m_SetupDlg.m_MESState)
	{
		SYSTEMTIME _curtime;
		GetLocalTime(&_curtime);
		m_TCPClient.SendOperatingData(6, dvalue, 5000,
			_curtime, 500);
	}
	
	if (m_dlgPara.IsAuto())
		m_Mode = 1;
	RcvUpdate(pData, 6);

	if (DCount > 100 && 
		(DBMSL[DCount] - avgL > m_dlgPara.Upperlimit() || DBMSL[DCount] - avgL < m_dlgPara.Lowerlimit())
		)
	{
		return 0;
	}

	DBMSL[DCount]  = pData[0] - pData[3];
	DBMSR[DCount]  = pData[2] - pData[5];

	SetWord(_T(""), pData[1]);
	SetWord(_T(""), pData[4]);

	DCount++ ;
	
	for (int i(0); i < DCount; i ++)
	{
		avgL = avgL + *(DBMSL + i);
		avgR = avgR + *(DBMSR + i);
	}

	avgL = avgL/DCount;
	avgR = avgR/DCount;

	if (DCount >= m_dlgPara.Statistics())
	{
// 		ioc_write_outbit(0, 3, 0);
		// 发送平均值
		memset(DBMSL, 0, sizeof(DBMSL));
		memset(DBMSR, 0, sizeof(DBMSR));
		avgL = 0;
		avgR = 0;
		DCount = 0;
	}
	return TRUE;
}

// 2卷
LRESULT CMainDialog::OnUpdate2Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	float* pData = (float*)lParam;

	//double dvalue[10];
	//for (int i = 0; i < 10; i++)
	//{
	//	dvalue[i] = pData[i];
	//}

	EnterCriticalSection(&m_csShareddata);
	m_dValue.clear();
	double dvalue[10];
	for (int i = 0; i < 10; i++)
	{
		dvalue[i] = pData[i];
		m_dValue.push_back(pData[i]);
	} 

	//CString m_textt,m_textT;
	Datadisplay tDatadisplay;
	SYSTEMTIME _systemtime;
	GetLocalTime(&_systemtime);
	tDatadisplay.datetime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), _systemtime.wYear, _systemtime.wMonth, _systemtime.wDay,
		_systemtime.wHour, _systemtime.wMinute, _systemtime.wSecond);
	tDatadisplay.breed = _T("");
	tDatadisplay.operationalsource = _T("");
	for (int i = 0; i < 10; i ++)
	{
		tDatadisplay.arrfdata[i] = dvalue[i];
	}
	//if ( !m_Roll.RollName().IsEmpty() )
	//{
	//	m_CDataSql.InsetData(m_Roll.RollName(),tDatadisplay);
	//}
	LeaveCriticalSection(&m_csShareddata);

	SetEvent( m_AlarmEvent );

	if (m_SetupDlg.m_MESState)
	{
		SYSTEMTIME _curtime;
		GetLocalTime(&_curtime);
		m_TCPClient.SendOperatingData(10, dvalue, 5000,
			_curtime, 500);
	}

	if (m_dlgPara.IsAuto())
		m_Mode = 2;
	RcvUpdate(pData, 10);

	if (DCount > 100 && 
		(DBMSL[DCount] - avgL > m_dlgPara.Upperlimit() || DBMSL[DCount] - avgL < m_dlgPara.Lowerlimit())
		)
	{
		return 0;
	}
	DBMSL[DCount]  = pData[0] - pData[5];
	DBMSR[DCount]  = pData[4] - pData[9];

	SetWord(_T(""), pData[1]);
	SetWord(_T(""), pData[4]);

	DCount++ ;

	for (int i(0); i < DCount; i ++)
	{
		avgL = avgL + *(DBMSL + i);
		avgR = avgR + *(DBMSR + i);
	}

	avgL = avgL/DCount;
	avgR = avgR/DCount;

	if (DCount >= m_dlgPara.Statistics())
	{
// 		ioc_write_outbit(0, 3, 0);
		// 发送平均值
		memset(DBMSL, 0, sizeof(DBMSL));
		memset(DBMSR, 0, sizeof(DBMSR));
		avgL = 0;
		avgR = 0;
		DCount = 0;
	}
	return TRUE;
}

// 3卷
LRESULT CMainDialog::OnUpdate3Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	if (m_dlgPara.IsAuto())
		m_Mode = 3;

	RcvUpdate((float*)lParam, 14);

	float* pData = (float*)lParam;

	//double dvalue[14];
	//for (int i = 0; i < 14; i++)
	//{
	//	dvalue[i] = pData[i];
	//}
	EnterCriticalSection(&m_csShareddata);
	m_dValue.clear();
	double dvalue[14];
	for (int i = 0; i < 14; i++)
	{
		dvalue[i] = pData[i];
		m_dValue.push_back(pData[i]);
	}
	//CString m_textt,m_textT;
	Datadisplay tDatadisplay;
	SYSTEMTIME _systemtime;
	GetLocalTime(&_systemtime);
	tDatadisplay.datetime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), _systemtime.wYear, _systemtime.wMonth, _systemtime.wDay,
		_systemtime.wHour, _systemtime.wMinute, _systemtime.wSecond);
	tDatadisplay.breed = _T("");
	tDatadisplay.operationalsource = _T("");
	for (int i = 0; i < 14; i ++)
	{
		tDatadisplay.arrfdata[i] = dvalue[i];
	}
	//if ( !m_Roll.RollName().IsEmpty() )
	//{
	//	m_CDataSql.InsetData(m_Roll.RollName(),tDatadisplay);
	//}
	LeaveCriticalSection(&m_csShareddata);

	SetEvent( m_AlarmEvent );

	if (m_SetupDlg.m_MESState)
	{
		SYSTEMTIME _curtime;
		GetLocalTime(&_curtime);
		m_TCPClient.SendOperatingData(14, dvalue, 5000,
			_curtime, 500);
	}

	SetWord(_T(""), pData[1]);
	SetWord(_T(""), pData[4]);

	return TRUE;
}
//1卷AT9
LRESULT CMainDialog::OnUpdate4Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	RcvUpdate((float*)lParam, 30);
	return 0;
}
//2卷-AT9
LRESULT CMainDialog::OnUpdate5Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	RcvUpdate((float*)lParam, 30);
	return 0 ;
}
//3卷-at9
LRESULT CMainDialog::OnUpdate6Msg(WPARAM wParam, LPARAM lParam)
{
	if(m_Mode != wParam)
		OnChangeDisp(wParam);
	RcvUpdate((float*)lParam, 30);
	return 0;
}


void CMainDialog::Writeconfig()
{
	WriteFile_l(_T("MODE"), _T("ID"), m_Mode, m_Configfile);
}

void CMainDialog::ReadConfig()
{
	m_Mode = ReadFile_l(_T("MODE"), _T("ID"), 1, m_Configfile);
	m_PLCFlag = (bool)ReadFile_b(_T("PLC"), _T("PLCFlag"), 0, m_Configfile);
	m_PlcPort = ReadFile_l(_T("PLC"), _T("PlcPort"), 102, m_Configfile);
	m_PlcIp = ReadFile_s(_T("PLC"), _T("PlcIp"), _T("192.168.11.11"), m_Configfile);
	m_LimitSpeed = ReadFile_f(_T("PLC"), _T("LimitSpeed"), 5, m_Configfile);
}

void CMainDialog::OnChangeDisp(int mode)
{
	m_Mode = mode;
	RedrawWindow();
}

void CMainDialog::InsertHRInfoItem(CListCtrl* pList, CString _time, INT XID,CString _info)
{
	m_ICount ++;
	int m_row ;
	m_row = pList->GetItemCount();
	if(m_row > 19)
	{
		pList->DeleteItem(0);
	}
	//获取当前的纪录条数.
	int nIndex = pList->GetItemCount();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT ;	
	lvItem.iItem = nIndex;				//行数
	lvItem.iSubItem = 0;
	CString temp ;
	temp.Format(_T("%d"),m_ICount);
	lvItem.pszText = (LPWSTR)temp.GetString();		//第一列
	//在最后一行插入记录值.
	pList->InsertItem(&lvItem);	
	//设置该行的其他列的值.
	pList->SetItemText(nIndex,1,_time);
	switch(XID)
	{
	case 1000:
		temp = _T("模式切换");
		break;
	case 1001:
		temp = _T("一条料模式");
		break;
	case 1002:
		temp = _T("两条料模式");
		break;
	case 1003:
		temp = _T("三条料模式");
		break;
	default:
		temp = _T("未知模式");
		break;
	}
	temp.Format(_T("%d"),XID);
	pList->SetItemText(nIndex,2,temp);
	pList->SetItemText(nIndex,3,_info);
	pList->EnsureVisible(pList->GetItemCount()-1,FALSE);
}

//void CMainDialog::OnBnClickedButton1()
//{
//	m_CCD5000.Send(1000, 2.0);
//}

void CMainDialog::OnBnClickedBtnPara()
{
	m_dlgPara.DoModal();
}

void CMainDialog::OnBnClickedBtnAnto()
{
// 	ioc_write_outbit(0, 1, 1);
// 	ioc_write_outbit(0, 2, 0);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString m_Strtime;
	m_Strtime.Format(_T("%04i-%02i-%02i  %02i:%02i:%02i"),
		time.wYear,time.wMonth,
		time.wDay,time.wHour,
		time.wMinute,time.wSecond);
// 	InsertHRInfoItem(&m_Info, m_Strtime, 800, _T("进人自动模式！"));
}

void CMainDialog::OnBnClickedBtnManual()
{
// 	ioc_write_outbit(0, 1, 0);
// 	ioc_write_outbit(0, 2, 1);

	SYSTEMTIME m_Time;
	GetLocalTime(&m_Time);
	CString m_Strtime;
	m_Strtime.Format(_T("%04i-%02i-%02i  %02i:%02i:%02i"),
		m_Time.wYear,m_Time.wMonth,
		m_Time.wDay,m_Time.wHour,
		m_Time.wMinute,m_Time.wSecond);

// 	InsertHRInfoItem(&m_Info, m_Strtime, 800, _T("进人手动模式！"));
}

void CMainDialog::OnBnClickedBtnStandard()
{
	m_dlgStandard.DoModal();
}

void CMainDialog::SetCoorMode(CDC &dc, int x, int y)
{
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt(1, 1);
	dc.SetViewportExt(1, -1);
	dc.SetViewportOrg(x, y);
}

void CMainDialog::RestoreCoor(CDC &dc)
{
	dc.SetMapMode(MM_TEXT);
	dc.SetWindowExt(1, 1);
	dc.SetViewportExt(1, 1);
	dc.SetViewportOrg(0, 0);
}


UINT CMainDialog::ThreadSendVirtual(LPVOID pParam)
{
	CMainDialog *pDlg = (CMainDialog*)pParam;
	Sleep(1000);
	while (WaitForSingleObject( pDlg->m_Stop, 0 ) == WAIT_TIMEOUT)
	{
		HANDLE _Signals[] = { pDlg->m_Stop, pDlg->m_Start};
		DWORD __Result(0L);
		SetEvent(pDlg->m_Start);
		Sleep(100);
		__Result = WaitForMultipleObjects(sizeof(_Signals) / sizeof(HANDLE), _Signals, FALSE, INFINITE);
		bool isExit(false);
		switch(__Result)
		{
		case WAIT_OBJECT_0:
		case WAIT_FAILED:
			isExit = true;
			break;
		case WAIT_OBJECT_0 + 1:
			isExit = false;
			break;
		default:
			isExit = true;
			break;
		}
		pDlg->m_CCD5000.SendMessage(ON_COM_RECEIVED, 0, 0);
		if (isExit == true)
		{
			continue;
		}
	}
	return 0;
}

BOOL CMainDialog::OnConnectToPlc(WPARAM wParam, LPARAM lParam)
{
	// 	if(m_FinsGateway.IsSocketOpened())
	// 	{
	// 		return AlreadyConnected;
	// 	}

	if(	!m_FinsGateway.DestroySocket()	||
		!m_FinsGateway.CreateTCPClient()	||
		!m_FinsGateway.Connect(reinterpret_cast<LPCTSTR>(wParam), lParam, 5000))
	{
		m_FinsGateway.DestroySocket();
		PostMessage(ON_COM_CONNECTION, reinterpret_cast<WPARAM>(&m_FinsGateway), FALSE);
		return FALSE;
	}

	if(!m_FinsProtocol.Connect(0, 5000))
	{
		m_FinsGateway.DestroySocket();
		PostMessage(ON_COM_CONNECTION, reinterpret_cast<WPARAM>(&m_FinsGateway), FALSE);
		return FALSE;
	}

	m_FinsCommError = 0;
	PostMessage(ON_COM_CONNECTION, reinterpret_cast<WPARAM>(&m_FinsGateway), TRUE);

	return TRUE;
}

BOOL CMainDialog::GetWord(LPCTSTR lpAddress) const
{

	if(!m_FinsGateway.IsSocketOpened())
	{
		return FALSE;
	}

	WORD _Result(0);

	if(!m_FinsProtocol.RD(lpAddress, &_Result, 1, 1000))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMainDialog::GetWord(LPCTSTR lpAddress, LPWORD lpBuffer, LONG lCount) const
{

	if(!m_FinsGateway.IsSocketOpened())
	{
		return FALSE;
	}

	if(static_cast<DWORD>(lCount) != m_FinsProtocol.RD(lpAddress, lpBuffer, lCount, 1000))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMainDialog::SetWord(LPCTSTR lpAddress, WORD nValue)
{
	if(!m_FinsGateway.IsSocketOpened())
	{
		return FALSE;
	}

	if(!m_FinsProtocol.WD(lpAddress, &nValue, 1, 1000))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMainDialog::SetWord(LPCTSTR lpAddress, LPWORD lpBuffer, LONG lCount)
{
 	if(!m_FinsGateway.IsSocketOpened())
 	{
 		return FALSE;
 	}

	if(static_cast<DWORD>(lCount) != m_FinsProtocol.WD(lpAddress, lpBuffer, lCount, 1000))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMainDialog::AlarmLight( Light_Alarm light )
{
	switch (light)
	{
	case Light_ERROR_SUCCESS:
		ioc_write_outbit(0, 5, 0);
		ioc_write_outbit(0, 6, 1);
		ioc_write_outbit(0, 7, 1);
		//ioc_write_outbit(0, 10, 0);
		break;

	case Light_ERROR_Yellow:
		ioc_write_outbit(0, 5, 1);
		ioc_write_outbit(0, 6, 0);
		ioc_write_outbit(0, 7, 1);
		//ioc_write_outbit(0, 10, 1);
		break;

	//case Light_ERROR_Red:
	//	ioc_write_outbit(0, 7, 1);
	//	ioc_write_outbit(0, 8, 0);
	//	ioc_write_outbit(0, 9, 1);
	//	ioc_write_outbit(0, 10, 1);
	//	break;

	//case Light_ERROR_Beep:
	//	ioc_write_outbit(0, 5, 0);
	//	ioc_write_outbit(0, 6, 1);
	//	ioc_write_outbit(0, 7, 1);
	//	//ioc_write_outbit(0, 10, 1);
	//	break;

	case Light_ERROR_Beep_Red:
		ioc_write_outbit(0, 5, 1);
		ioc_write_outbit(0, 6, 1);
		ioc_write_outbit(0, 7, 0);
		//ioc_write_outbit(0, 10, 1);
		break;
	default:
		break;
	}

	return 0;
}
void CMainDialog::OnBnClickedButtonsql()
{
	m_datadisplaydlg.DoModal();
}


// MainDialog.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "SetupDlg.h"
#include "CCD5000.h"
#include "afxcmn.h"
#include "ReadWriteFile.h"
#include "ParameterDialog.h"
#include "StandardDlg.h"
#include "Comm/Ethernet.h"
#include "Comm/FINSTCP.h"
#include "TCPClient.h"
#include "ADOConn.h"
#include <vector>
#include "DataDisplaydlg.h"
#include "Roll.h"
#include "DataSql.h"
#include "S7COMMTCPIP.h"

typedef enum
{
	Light_ERROR_SUCCESS = 0,
	Light_ERROR_Yellow = 0x0001,
	Light_ERROR_Red = 0x0002,
	Light_ERROR_Beep = 0x0004,
	Light_ERROR_Beep_Red = 0x0006
}Light_Alarm;

// CMainDialog 对话框
class CMainDialog : public CDialog
{
// 构造
public:
	CMainDialog(CWnd* pParent = NULL);	// 标准构造函数
	~CMainDialog();

// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnPara();
	afx_msg void OnBnClickedBtnAnto();
	afx_msg void OnBnClickedBtnManual();
	afx_msg void OnBnClickedBtnSetup();
public:
	CRect SetupRect(int m_HStart, int m_SStart);
	LRESULT OnModeMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnErrorMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate1Msg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate2Msg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate3Msg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate4Msg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate5Msg(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdate6Msg(WPARAM wParam, LPARAM lParam);
	void RcvUpdate(float* pData, DWORD dwLength);
	static UINT  ThreadSendVirtual( LPVOID pParam);

	void SetCoorMode(CDC &dc, int x, int y);// 自定义原点坐标模式
	void RestoreCoor(CDC &dc);				// 恢复GDI映射

	void Writeconfig();
	void ReadConfig();
	void OnChangeDisp(int mode);		// 根据物料数显示上端界面
	void InsertHRInfoItem(CListCtrl* pList, CString _time, INT XID, CString _info);

	void InitialDBB();

	int	 GetStretLenth(float fDiff);

	void SetChartRect(CRect & chartRect, CRect & rect);		// 获得图表的矩形区域
	void DrawReal(const float *pData );

	float GetStep();	// 获得一网格的宽度大小
	void DrawOneHScale(CDC &dc, CRect & rc);
	void DrawOneVScale(CDC &dc, int nNormal, CRect & rc, BOOL bLeft = TRUE);	// 绘制一个图表的垂直刻度
	void DrawNote(); 
	void DrawScale();

	void DrawOneGuideLine(CDC &dc, CRect & rc, BOOL bLeft = TRUE);
	void DrawGuideLine();

	void DrawGrap();
	
	void BitBltAll();

	void CopyRcvToBuff(float *pData, int nLength);

	BOOL AlarmLight( Light_Alarm light );
	static DWORD WINAPI AlarmSequence(CMainDialog* lpNotify);
	Roll m_Roll;


private:
	CRITICAL_SECTION m_csShareddata;

	HANDLE   m_AlarmthreadExit;
	HANDLE   m_Alarmthread;
	HANDLE   m_AlarmEvent;
	HANDLE   m_Stop;
	HANDLE   m_Start;
	
	CDataSql m_CDataSql;

	std::vector<double>   m_dValue;

	float			m_RcvBuff[MAX_RCV_LENGTH][20];		// 接收数据缓冲区
	int				DCount;
	int				m_StatusBarHeigth;
	int				m_Bar; // m_Bar = 5为2条；  m_Bar = 7为3条；m_Bar = 3为1条 ;
	int				m_Mode;
	int				m_ICount;
	float			m_data[14];		// 无作用
	double *		DBMSL;
	double *		DBMSR;
	double 			avgL;
	double 			avgR;
	CString			m_Configfile;
	CWinThread	*	m_pThread;


	SetupDlg		m_SetupDlg;
	ParameterDialog m_dlgPara;

	TCPClient			m_TCPClient;

	CStatic			m_Picture;
	CStatic 		m_picLeftUp;
	CStatic 		m_picRightUp;
	CStatic 		m_picLeftDown;
	CStatic 		m_picRightDown;

	CCriticalSection m_section;

	CDC *			m_pDcUp;
	CDC *			m_pDcLeftDown;
	CDC *			m_pDcRightDown;
	CDC *			m_pDcLeftUp;
	CDC *			m_pDcRightUp;

	CDC				m_memReal;
	CDC				m_memLeftUp;
	CDC				m_memRightUp;
	CDC				m_memLeftDown;
	CDC				m_memRightDown;

	CBrush			m_appBrush;
	CBrush			m_background;

	CBitmap			m_bmpReal;
	CBitmap			m_bmpLeftDown;
	CBitmap			m_bmpRightDown;
	CBitmap			m_bmpLeftUp;
	CBitmap			m_bmpRightUp;
	
	CRect			m_rectUp;		
	CRect			m_rectLeftDown;	
	CRect			m_rectRightDown;
	CRect			m_rectLeftUp;	
	CRect			m_rectRightUp;

	CRect			m_rectChartLeftUp;	
	CRect			m_rectChartRightUp;
	CRect			m_rectChartLeftDown;	
	CRect			m_rectChartRightDown;

	CFont			m_font;
	CFont			m_Font;

	CButton 		m_btn_para;
	CButton 		m_btn_manual;
	CButton 		m_btn_anto;
	CButton 		m_Btn_Setup;
	CButton         m_btn_datadisplay;
	CButton			m_btn_standard;

	CPen			m_penGuideLine;			// 辅助线
	CPen			m_penMaterial1;			
	CPen			m_penMaterial2;			
	CPen			m_penMaterial3;	
	
	CPen			m_penEar1;
	CPen			m_penEar2;
	CPen			m_penEar3;
	CPen			m_penEar4;

	CCD5000			m_CCD5000;
	CStandardDlg    m_dlgStandard;
	CDataDisplaydlg  m_datadisplaydlg;
public:
	afx_msg void OnBnClickedBtnStandard();


	mutable Ethernet m_FinsGateway;
	mutable FINSTCP m_FinsProtocol;
	mutable DWORD m_FinsCommError;

	BOOL OnConnectToPlc(WPARAM wParam, LPARAM lParam);

	BOOL GetWord(LPCTSTR lpAddress) const;
	BOOL GetWord(LPCTSTR lpAddress, LPWORD lpBuffer, LONG lCount) const;
	BOOL SetWord(LPCTSTR lpAddress, WORD nValue);
	BOOL SetWord(LPCTSTR lpAddress, LPWORD lpBuffer, LONG lCount);

	afx_msg void OnBnClickedButtonsql();
	int m_roolinformaton;
	bool m_PLCFlag;
	CString m_PlcIp;
	int m_PlcPort;

	S7COMMTCPIP	  m_S7COMMTCPIP;
};

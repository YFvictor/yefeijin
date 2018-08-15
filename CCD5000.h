#pragma once

#ifndef CCD5000_H
#define CCD5000_H

#include "Comm/SerialPort.h"
#include "SetupDlg.h"
#include "ReadWriteFile.h"

enum _Widthcount
{
	Widthcount_Three = 3,                                                        //宽度3
	Widthcount_Five = 5,
	Widthcount_Seven = 7,
	Widthcount_Nine = 9,
	Widthcount_thirteen = 13,
};

enum _Materialcount
{
	Materialcount_1 = 1,                                                         //材料1
	Materialcount_2 = 2,
	Materialcount_3 = 3,
	Materialcount_1_at9 = 4,                                                         
	Materialcount_2_at9 = 5,
	Materialcount_3_at9 = 6,
};

typedef union                                                                              
{  
	float fdata;  
	unsigned char ldata[4];  
}FloatLongType; 

// CCD5000 对话框

extern	float m_ProSpeed;
extern	float m_LimitSpeed;

class CCD5000 : public CDialog
{
	DECLARE_DYNAMIC(CCD5000)

public:
	CCD5000(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCD5000();

// 对话框数据
	enum { IDD = IDD_CD5000 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitCameraconnect();
	BOOL CloseRemotePort();

	SerialPort	m_Remote;
	SetupDlg *	m_pDlgSetup;

	LRESULT OnRemoteCommand(WPARAM wParam, LPARAM lParam);
	BOOL Send(USHORT commond, float Valus);

	BYTE m_buff[2048];
	float m_flVules[128];
	float m_flValuesSend[128];
	int m_iCount;
	CString m_FileRecordName;

	void WriteFileRecord(float* Value, int length);

	BOOL CloseReport(HANDLE hFile);
	BOOL WriteReport(HANDLE hFile, const CString& lpMessage);
	HANDLE OpenLogFile(const CString& lpFileName, const CString& lpHeader);
	CString Slot1LogHeader();
	CString Slot1LogHeader(BOOL at9);
	CString Slot2LogHeader();
	CString Slot2LogHeader(BOOL at9);
	CString Slot3LogHeader();
	CString Slot3LogHeader(BOOL at9);
	CString SlotLogName(LPCTSTR pcsNumMaterial);
	CString SlotReportMessage(int length);

	int SaveData(int nMaterial, int nLenth);

private:
	void  WriteText(int size, BYTE *m_GDATAarray);
	void  Byte_to_Float(float &f , unsigned char byte[]); 
	bool  m_bReadNow;
};

#endif
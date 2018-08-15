#include "StdAfx.h"
#include "CSV.h"

CSV::CSV(void)
{
}

CSV::~CSV(void)
{
}

HANDLE CSV::OpenLogFile(const CString& lpFileName, const CString& lpHeader)
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

	HANDLE hFile(CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

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

		DWORD dwLength(lpHeader.GetLength() * sizeof(_TCHAR));

// 		if(!WriteFile(hFile, lpHeader.GetString(), dwLength, &dwWritten, NULL))
// 		{
// 			CloseHandle(hFile);
// 			DeleteFile(lpFileName);
// 			return INVALID_HANDLE_VALUE;
// 		}

		char dest[1024];

		int mbLength = WideCharToMultiByte( CP_ACP, 0, lpHeader, -1,
			dest, 1024, NULL, NULL );

 		if(!WriteFile(hFile, dest, mbLength - 1, &dwWritten, NULL))
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

CString CSV::SlotLogHeader()
{
	CString m_cstr;

	m_cstr = _T("日期时间,序号,种类,操作源,数据1,数据2,数据3,数据4,数据5,数据6,数据7,数据8,数据9,数据10,数据11,数据12,数据13,数据14,\r\n");
	return m_cstr;
}

CString CSV::SlotLogName(CString m_ExePath, CString nema)
{
	SYSTEMTIME _LocalTime;
	GetLocalTime(&_LocalTime);

	CString txFileName;
	txFileName.Format(	_T("%s\\%s (%04u-%02u-%02u).csv"),
		m_ExePath,
		nema,
		_LocalTime.wYear,
		_LocalTime.wMonth,
		_LocalTime.wDay);
	return txFileName;
}

BOOL CSV::WriteReport(HANDLE hFile, const CString& lpMessage)
{
	DWORD dwWritten(0);
	DWORD dwLength(lpMessage.GetLength() * sizeof(_TCHAR));
	
	char dest[1024];

	int mbLength = WideCharToMultiByte( CP_ACP, 0, lpMessage, -1,
		dest, 1024, NULL, NULL );

	return WriteFile(hFile, dest, mbLength - 1, &dwWritten, NULL);
}

BOOL CSV::CloseReport(HANDLE hFile)
{
	return CloseHandle(hFile);
}

BOOL CSV::SetRow( HANDLE hFile, SYSTEMTIME lptime, float lpmain, float lpvice )
{
	CString m_cstr;
	m_cstr.Format(_T(" %04u-%02u-%02u %02u:%02u:%02u.%03u,"),
		lptime.wYear,
		lptime.wMonth,
		lptime.wDay,
		lptime.wHour,
		lptime.wMinute,
		lptime.wSecond,
		lptime.wMilliseconds);
	m_cstr.AppendFormat(_T(" %.1f"), lpmain, lpvice);

	m_cstr += _T("\r\n");

	WriteReport(hFile, m_cstr);

	return TRUE;
}

BOOL CSV::SetRow(HANDLE hFile, WorkOrderInfo lPHead, ListWeight lPlistweight)
{
	CString m_cstr;
	m_cstr.Format(_T("%04u-%02u-%02u,%02u:%02u:%02u, %s, %0.2f, %0.2f, %0.2f,"),
		lPHead.Time.wYear,
		lPHead.Time.wMonth,
		lPHead.Time.wDay,
		lPHead.Time.wHour,
		lPHead.Time.wMinute,
		lPHead.Time.wSecond, 
		lPHead.Breed, lPlistweight.wValue1, lPlistweight.wValue2, lPlistweight.wValue3);

	m_cstr += _T("\r\n");

	WriteReport(hFile, m_cstr);

	return TRUE;
}
BOOL CSV::SetRoW( HANDLE hFile, Datadisplay pDatadisplay )
{
	CString m_cstr;
	m_cstr.Format(_T(" %s, %d, %s, %s, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f,"),
		pDatadisplay.datetime,
		pDatadisplay.ordernumber,
		pDatadisplay.breed,
		pDatadisplay.operationalsource,
		pDatadisplay.arrfdata[0],
		pDatadisplay.arrfdata[1],
		pDatadisplay.arrfdata[2],
		pDatadisplay.arrfdata[3],
		pDatadisplay.arrfdata[4],
		pDatadisplay.arrfdata[5],
		pDatadisplay.arrfdata[6],
		pDatadisplay.arrfdata[7],
		pDatadisplay.arrfdata[8],
		pDatadisplay.arrfdata[9],
		pDatadisplay.arrfdata[10],
		pDatadisplay.arrfdata[11],
		pDatadisplay.arrfdata[12],
		pDatadisplay.arrfdata[13]);

	m_cstr += _T("\r\n");

	WriteReport(hFile, m_cstr);

	return TRUE;
}
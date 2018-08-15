#pragma once

#include "WorkOrderRecord.h"
#include "stdafx.h"

class CSV
{
public:
	CSV(void);
	~CSV(void);

public:

	CString SlotLogName(CString m_ExePath, CString nema);
	CString SlotLogHeader();
	HANDLE OpenLogFile(const CString& lpFileName, const CString& lpHeader);
	BOOL SetRow(HANDLE hFile, SYSTEMTIME lptime, float lpmain, float lpvice);
	BOOL SetRow(HANDLE hFile, WorkOrderInfo lPHead, ListWeight lPlistweight);
	BOOL CloseReport(HANDLE hFile);
	BOOL SetRoW( HANDLE hFile,Datadisplay pDatadisplay );

protected:
	BOOL WriteReport(HANDLE hFile, const CString& lpMessage);
};

#include "StdAfx.h"
#include "WorkOrderRecord.h"

WorkOrderRecord::WorkOrderRecord(void)
{
	m_WorkOrderName = _T("WidthData");
}

WorkOrderRecord::~WorkOrderRecord(void)
{

}

BOOL WorkOrderRecord::ConnectSQL()
{
	BOOL m_Result;

	m_Result = m_ADOConn.OnInitADOConn();

	return m_Result;
}

void WorkOrderRecord::ExitSQL()
{
	m_ADOConn.ExitConnect();
}

BOOL WorkOrderRecord::InsertRecord(WorkOrderInfo info, float fvalue1, float fvalue2, float fvalue3)
{
	CString m_Str;
	CString m_strtime;
	m_strtime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"), info.Time.wYear,
		info.Time.wMonth,
		info.Time.wDay,
		info.Time.wHour,
		info.Time.wMinute,
		info.Time.wSecond,
		info.Time.wMilliseconds);

	m_WorkOrderName = _T("WidthData");
	m_Str.Format(_T("INSERT INTO [UserMan].[dbo].[%s] "), m_WorkOrderName);
	m_Str += _T("([Time], [Breed], [wValue1], [wValue2], [wValue3]) ");
	m_Str.AppendFormat(_T("VALUES('%s', '%s', %0.2f, %0.2f, %0.2f)")
		, m_strtime, info.Breed, fvalue1, fvalue2, fvalue3);

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(m_Str.GetString());
	if (! m_Result)
	{
		AfxMessageBox(_T("²åÈëÊý¾ÝÊ§°Ü£¡"));
		return FALSE;
	}
	return TRUE;
}


BOOL WorkOrderRecord::ClearALL()
{
	WorkOrderInfoALL.clear();
	ListWeightALL.clear();
	return TRUE;
}

BOOL WorkOrderRecord::UpdateTen()
{
	ClearALL();

	CString m_Str;m_Str += _T("SELECT TOP (10) *");
	m_Str.AppendFormat(_T(" FROM [UserMan].[dbo].[%s] order by  id DESC"), m_WorkOrderName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(m_Str.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	try
	{
		WorkOrderInfo m_WorkOrderInfo;
		ListWeight m_ListWeight;
		for(; !_Recordset->adoEOF; _Recordset->MoveNext())
		{
			_variant_t _time(_Recordset->GetCollect(_T("Time")));

			if(_time.vt == VT_NULL)
			{
				memset(&m_WorkOrderInfo.Time, 0, sizeof(SYSTEMTIME));
			}
			else
			{
				COleDateTime _Time(_time);
				_Time.GetAsSystemTime(m_WorkOrderInfo.Time);
			}

			_variant_t _Breed(_Recordset->GetCollect(_T("Breed")));
			m_WorkOrderInfo.Breed = _Breed.vt == VT_NULL ? 0 : (CString)_Breed;

			_variant_t _weight1(_Recordset->GetCollect(_T("wValue1")));
			m_ListWeight.wValue1 = _weight1.vt == VT_NULL ? 0 : (double)_weight1;

			_variant_t _weight2(_Recordset->GetCollect(_T("wValue2")));
			m_ListWeight.wValue2 = _weight2.vt == VT_NULL ? 0 : (double)_weight2;

			_variant_t _weight3(_Recordset->GetCollect(_T("wValue3")));
			m_ListWeight.wValue3 = _weight3.vt == VT_NULL ? 0 : (double)_weight3;

			WorkOrderInfoALL.push_back(m_WorkOrderInfo);
			ListWeightALL.push_back(m_ListWeight);
		}

		_Recordset->Close();
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL WorkOrderRecord::UpdateFondTime( SYSTEMTIME lpbegin, SYSTEMTIME lpend, CString csbreed )
{
	ClearALL();

	CString _Command;
	CString m_CStrbegin;
	CString m_CStrend;
	m_CStrbegin.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"), lpbegin.wYear
		, lpbegin.wMonth
		, lpbegin.wDay
		, lpbegin.wHour
		, lpbegin.wMinute
		, lpbegin.wSecond
		, lpbegin.wMilliseconds);

	m_CStrend.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"), lpend.wYear
		, lpend.wMonth
		, lpend.wDay
		, lpend.wHour
		, lpend.wMinute
		, lpend.wSecond
		, lpend.wMilliseconds);

	if (csbreed.IsEmpty())
	{
		_Command.Format(_T("SELECT * FROM [UserMan].[dbo].[%s] where [Time] > '%s'  and [Time] < '%s' order by  id")
			, m_WorkOrderName, m_CStrbegin, m_CStrend);
	}
	else
	{
		_Command.Format(_T("SELECT * FROM [UserMan].[dbo].[%s] where [Time] > '%s'  and [Time] < '%s' and [Breed] = '%s' order by  id")
			, m_WorkOrderName, m_CStrbegin, m_CStrend, csbreed);
	}


	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(_Command.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	try
	{
		WorkOrderInfo m_WorkOrderInfo;
		ListWeight m_ListWeight;
		for(; !_Recordset->adoEOF; _Recordset->MoveNext())
		{
			_variant_t _time(_Recordset->GetCollect(_T("Time")));

			if(_time.vt == VT_NULL)
			{
				memset(&m_WorkOrderInfo.Time, 0, sizeof(SYSTEMTIME));
			}
			else
			{
				COleDateTime _Time(_time);
				_Time.GetAsSystemTime(m_WorkOrderInfo.Time);
			}

			_variant_t _Breed(_Recordset->GetCollect(_T("Breed")));
			m_WorkOrderInfo.Breed = _Breed.vt == VT_NULL ? 0 : (CString)_Breed;

			_variant_t _weight1(_Recordset->GetCollect(_T("wValue1")));
			m_ListWeight.wValue1 = _weight1.vt == VT_NULL ? 0 : (double)_weight1;

			_variant_t _weight2(_Recordset->GetCollect(_T("wValue2")));
			m_ListWeight.wValue2 = _weight2.vt == VT_NULL ? 0 : (double)_weight2;

			_variant_t _weight3(_Recordset->GetCollect(_T("wValue3")));
			m_ListWeight.wValue3 = _weight3.vt == VT_NULL ? 0 : (double)_weight3;

			WorkOrderInfoALL.push_back(m_WorkOrderInfo);
			ListWeightALL.push_back(m_ListWeight);
		}

		_Recordset->Close();
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

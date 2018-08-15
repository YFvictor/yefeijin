#include "StdAfx.h"
#include "DataSql.h"

CDataSql::CDataSql(void)
{
}

CDataSql::~CDataSql(void)
{
}

BOOL CDataSql::ConnectSQL()
{
	return m_ADOConn.OnInitADOConn();
}

void CDataSql::Exsit()
{
	m_ADOConn.ExitConnect();
}

BOOL CDataSql::InsetData(CString m_ROOLname,Datadisplay lDatadisplay)
{
	CString m_textb;
	/*m_ROOLname = _T("DATA");*/
	m_textb.Format(_T("INSERT INTO [Cwmsys].[dbo].[%s]\
					  ([DATETIME]\
					  ,[ORDERNUMBER]\
					  ,[BREED]\
					  ,[OPERATIONALSOURCE]\
					  ,[DATA1]\
					  ,[DATA2]\
					  ,[DATA3]\
					  ,[DATA4]\
					  ,[DATA5]\
					  ,[DATA6]\
					  ,[DATA7]\
					  ,[DATA8]\
					  ,[DATA9]\
					  ,[DATA10]\
					  ,[DATA11]\
					  ,[DATA12]\
					  ,[DATA13]\
					  ,[DATA14])\
					  VALUES\
					  ('%s'\
					  ,'%d'\
					  ,'%s'\
					  ,'%s'"), m_ROOLname,lDatadisplay.datetime, lDatadisplay.ordernumber
					  , lDatadisplay.breed, lDatadisplay.operationalsource);

	CString strtextdata;
	
	for (size_t i(0); i < sizeof(lDatadisplay.arrfdata) / sizeof(int); i ++)
	{
		strtextdata.AppendFormat(_T(",%.2f"), lDatadisplay.arrfdata[i]);
	}

	m_textb = m_textb + strtextdata + _T(")");

	return m_ADOConn.ExecuteSQL(m_textb.GetString());
}

BOOL CDataSql::SelectData(CString strRollName, std::vector<Datadisplay>& vecDatadisplay)
{
	CString m_textb;

	m_textb.Format(_T("SELECT * FROM [Cwmsys].[dbo].[%s] WHERE [ORDERNUMBER] = '%s'"), strRollName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(m_textb.GetString());

	Datadisplay pDatadisplay;

	CString m_text;

	for(; !_Recordset->adoEOF; _Recordset->MoveNext())
	{
		try
		{
			_variant_t _Value_t = _Recordset->GetCollect(_T("DATETIME"));
			pDatadisplay.datetime = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("ORDERNUMBER"));
			m_text.Format(_T("%d"), pDatadisplay.ordernumber);
			pDatadisplay.ordernumber = _Value_t.vt == VT_NULL ? 0 : (int)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("BREED"));
			pDatadisplay.breed = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("OPERATIONALSOURCE"));
			pDatadisplay.operationalsource = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA1"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[0]);
			pDatadisplay.arrfdata[0] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA2"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[1]);
			pDatadisplay.arrfdata[1] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA3"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[2]);
			pDatadisplay.arrfdata[2] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA4"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[3]);
			pDatadisplay.arrfdata[3] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA5"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[4]);
			pDatadisplay.arrfdata[4] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA6"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[5]);
			pDatadisplay.arrfdata[5] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA7"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[6]);
			pDatadisplay.arrfdata[6] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA8"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[7]);
			pDatadisplay.arrfdata[7] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA9"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[8]);
			pDatadisplay.arrfdata[8] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA10"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[9]);
			pDatadisplay.arrfdata[9] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA11"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[10]);
			pDatadisplay.arrfdata[10] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA12"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[11]);
			pDatadisplay.arrfdata[11] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA13"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[12]);
			pDatadisplay.arrfdata[12] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA14"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[13]);
			pDatadisplay.arrfdata[13] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			vecDatadisplay.push_back(pDatadisplay);

		}
		catch(...)
		{
		}
	}

	return TRUE;
}

BOOL CDataSql::DelData(CString strRollName, CString strORDERNUMBER)
{
	CString m_texta;
	m_texta.Format(_T("DELETE FROM [Cwmsys].[dbo].[%s] WHERE [ORDERNUMBER] = '%d' "), strRollName, strORDERNUMBER);
	return m_ADOConn.ExecuteSQL(m_texta.GetString());
}

BOOL CDataSql::Update(CString strRollName, std::vector<Datadisplay>& datadisplay)
{
	datadisplay.clear();
	CString m_Text;

	m_Text.Format(_T("SELECT * FROM [Cwmsys].[dbo].[%s] order by  id DESC "), strRollName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(m_Text.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	Datadisplay pDatadisplay;
	CString m_text;
	for(; !_Recordset->adoEOF; _Recordset->MoveNext())
	{
		/*int nIndex = m_list.GetItemCount();*/
		try
		{
			_variant_t _Value_t = _Recordset->GetCollect(_T("DATETIME"));
			pDatadisplay.datetime = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("ORDERNUMBER"));
			m_text.Format(_T("%d"), pDatadisplay.ordernumber);
			pDatadisplay.ordernumber = _Value_t.vt == VT_NULL ? 0 : (int)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("BREED"));
			pDatadisplay.breed = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("OPERATIONALSOURCE"));
			pDatadisplay.operationalsource = _Value_t.vt == VT_NULL ? 0 : (CString)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA1"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[0]);
			pDatadisplay.arrfdata[0] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA2"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[1]);
			pDatadisplay.arrfdata[1] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA3"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[2]);
			pDatadisplay.arrfdata[2] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA4"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[3]);
			pDatadisplay.arrfdata[3] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA5"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[4]);
			pDatadisplay.arrfdata[4] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA6"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[5]);
			pDatadisplay.arrfdata[5] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA7"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[6]);
			pDatadisplay.arrfdata[6] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA8"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[7]);
			pDatadisplay.arrfdata[7] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA9"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[8]);
			pDatadisplay.arrfdata[8] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA10"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[9]);
			pDatadisplay.arrfdata[9] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA11"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[10]);
			pDatadisplay.arrfdata[10] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA12"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[11]);
			pDatadisplay.arrfdata[11] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA13"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[12]);
			pDatadisplay.arrfdata[12] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			_Value_t = _Recordset->GetCollect(_T("DATA14"));
			m_text.Format(_T("%.2f"), pDatadisplay.arrfdata[13]);
			pDatadisplay.arrfdata[13] = _Value_t.vt == VT_NULL ? 0 : (float)_Value_t;

			datadisplay.push_back(pDatadisplay);

		}
		catch(...)
		{			
		}
	}
	return TRUE;
}


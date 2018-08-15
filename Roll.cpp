#include "StdAfx.h"
#include "Roll.h"

Roll::Roll(void)
{
	InitializeCriticalSection(&m_Lock);

	m_RollName = _T("Null");
}

Roll::~Roll(void)
{
	DeleteCriticalSection(&m_Lock);
}

BOOL Roll::ConnectSQL()
{
    BOOL _True;

	_True = m_ADOConn.OnInitADOConn();

	return _True;
}

void Roll::ExitSQL()
{
	m_ADOConn.ExitConnect();
}

BOOL Roll::SetRollName(CString RollName)
{
	CString _Command;

	_Command.Format(_T("UPDATE [Cwmsys].[dbo].[RollInformation] SET [RollName] = \'%s\' WHERE [RollName] = \'%s\'"), RollName, m_RollName);
	BOOL m_OK(FALSE);
	m_OK = m_ADOConn.ExecuteSQL(_Command.GetString());
	if (!m_OK)
	{
		AfxMessageBox(_T("RollNameÉèÖÃÊ§°Ü£¡"));
		return FALSE;
	}

	_Command.Format(_T("EXEC Cwmsys.sys.sp_rename '%s','%s'"), m_RollName, RollName );

	m_OK = m_ADOConn.ExecuteSQL(_Command.GetString());
	if (!m_OK)
	{
		AfxMessageBox(_T("RollNameÉèÖÃÊ§°Ü£¡"));
		return FALSE;
	}
	_Command.Format(_T("ALTER TABLE [Cwmsys].[dbo].[%s] DROP CONSTRAINT [PK_%s]"), RollName, m_RollName);
	_Command.AppendFormat(_T("ALTER TABLE [Cwmsys].[dbo].[%s] ADD  CONSTRAINT [PK_%s] PRIMARY KEY CLUSTERED "), RollName, RollName);
	_Command += _T("([ID] ASC)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF,\
				   ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]");
	m_OK = m_ADOConn.ExecuteSQL(_Command.GetString());
	if (!m_OK)
	{
		AfxMessageBox(_T("RollNameÉèÖÃÊ§°Ü£¡"));
		return FALSE;
	}
	return TRUE;
}

BOOL Roll::SetBeginTime(const SYSTEMTIME& lpTime)
{
	CString _Command;

	_Command.Format(_T("UPDATE [Cwmsys].[dbo].[RollInformation] SET [CREATETIME] = \'%04u-%02u-%02u %02u:%02u:%02u.%03u\' WHERE [RollName] = \'%s\'"),
		lpTime.wYear,
		lpTime.wMonth,
		lpTime.wDay,
		lpTime.wHour,
		lpTime.wMinute,
		lpTime.wSecond,
		lpTime.wMilliseconds,
		m_RollName);
	BOOL m_OK(FALSE);
	m_OK = m_ADOConn.ExecuteSQL(_Command.GetString());
	if (!m_OK)
	{
		AfxMessageBox(_T("CreateTimeÉèÖÃÊ§°Ü£¡"));
		return FALSE;
	}

	return TRUE;
}
BOOL Roll::Update()
{
	_bstr_t _Command;
	_Command = "SELECT [RollName],[CREATETIME] FROM [Cwmsys].[dbo].[RollInformation] order by CREATETIME desc";

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(_Command);

	if(!_Recordset)
	{
		return FALSE;
	}
	SYSTEMTIME lpTime;
	RollAllInfo lRollSinlgeInfo;
	for(; !_Recordset->adoEOF; _Recordset->MoveNext())
	{
		try
		{
			_variant_t _RollName(_Recordset->GetCollect(_T("RollName")));
			lRollSinlgeInfo.RollName = (LPCTSTR)((_bstr_t)_RollName);

			_variant_t _CreatTime(_Recordset->GetCollect(_T("CREATETIME")));

			if(_CreatTime.vt == VT_NULL)
			{
				memset(&lpTime, 0, sizeof(SYSTEMTIME));
			}
			else
			{
				COleDateTime _Time(_CreatTime);
				_Time.GetAsSystemTime(lpTime);
				lRollSinlgeInfo.CreateTime = lpTime;
			}
			m_RoolInformation.push_back(lRollSinlgeInfo);

		}
		catch(...)
		{
		}
	}
	return TRUE;
}

BOOL Roll::GetSingledata(CString rollName)
{
	m_RoolInformation.clear();

    CString _Command;
	_Command.Format(_T("SELECT * FROM [Cwmsys].[dbo].[RollInformation] WHERE [RollName] = '%s' "), rollName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(_Command.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	SYSTEMTIME lpTime;
	RollAllInfo lRollSinlgeInfo;
	for(; !_Recordset->adoEOF; _Recordset->MoveNext())
	{
		try
		{
			_variant_t _RollName(_Recordset->GetCollect(_T("RollName")));
			lRollSinlgeInfo.RollName = (LPCTSTR)((_bstr_t)_RollName);

			_variant_t _CreatTime(_Recordset->GetCollect(_T("CREATETIME")));

			if(_CreatTime.vt == VT_NULL)
			{
				memset(&lpTime, 0, sizeof(SYSTEMTIME));
			}
			else
			{
				COleDateTime _Time(_CreatTime);
				_Time.GetAsSystemTime(lpTime);
				lRollSinlgeInfo.CreateTime = lpTime;
			}
			m_RoolInformation.push_back(lRollSinlgeInfo);

		}
		catch(...)
		{
		}
	}
	return TRUE;
}

BOOL Roll::GetRollName(CString& RollName)
{

	RollName = m_RollName;
	return TRUE;
}

BOOL Roll::GetBeginTime(SYSTEMTIME& lpTime)
{
	CString _Command;
	_Command.Format(_T("SELECT[CREATETIME]FROM [Cwmsys].[dbo].[RollInformation]WHERE [RollName] = \'%s\'"), m_RollName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(_Command.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	try
	{
		_variant_t _CreatTime(_Recordset->GetCollect(_T("CreatTime")));

		if(_CreatTime.vt == VT_NULL)
		{
			memset(&lpTime, 0, sizeof(SYSTEMTIME));
		}
		else
		{
			COleDateTime _Time(_CreatTime);
			_Time.GetAsSystemTime(lpTime);
		}

		_Recordset->Close();
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Roll::IsExistEverydayTable(CString rollName)
{
	CString _Command;
	_Command.Format( _T("select Count (*) as kk from [Cwmsys].[dbo].[sysobjects] where name= '%s'"), rollName);

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(_Command.GetString());

	if (!_Recordset)
	{
		return FALSE;
	}
	BOOL m_Result(FALSE);
	_variant_t _PPID(_Recordset->GetCollect("kk"));
	m_Result = (int)_PPID;

	if (m_Result)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Roll::IsExistMainTable()
{
	_bstr_t _Command;
	_Command = _T("if not exists (select * from [Cwmsys].[dbo].[sysobjects] where name = 'RollInformation')\
				  CREATE TABLE [Cwmsys].[dbo].[RollInformation](\
				  [RollName] [nvarchar](50)NOT NULL,\
				  [CREATETIME] [datetime]NOT NULL,\
				  CONSTRAINT [IX_RollInformation] UNIQUE NONCLUSTERED \
				  (\
				  [RollName] ASC\
				  )WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF,\
				  IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, \
				  ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]\
				  ) ON [PRIMARY]");

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(_Command);
	if (!m_Result)
	{
		return false;
	}

	return TRUE;
}

BOOL Roll::IsExistSQLBase()
{
	SHCreateDirectoryExW(NULL, _T("D:\\SQLdata"), NULL);
	_bstr_t _Command;
	_Command = _T("if not exists (select * from sys.sysdatabases where name = 'Cwmsys')\
				  CREATE DATABASE [Cwmsys] CONTAINMENT = NONE ON  PRIMARY \
				  ( NAME = N'ReportServerTempDB', FILENAME = N'D:\\SQLdata\\Cwmsys.mdf' , SIZE = 4160KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )\
				  LOG ON \
				  ( NAME = N'Cwmsys_log', FILENAME = N'D:\\SQLdata\\Cwmsys_log.ldf' , SIZE = 1344KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)");

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(_Command);

	if (!m_Result)
	{
		return false;
	}

	return TRUE;
}

BOOL Roll::CreateTable( CString tablename )
{
	CString m_Str;
	m_Str.Format(_T("CREATE TABLE [Cwmsys].[dbo].[%s]([ID] [int] IDENTITY(1,1) NOT NULL,"), tablename);
	m_Str += _T("[DATETIME] [datetime] NOT NULL, [Breed] [nvarchar](50) NOT NULL, ");
	m_Str += _T("[OPERATIONALSOURCE] [nvarchar](50) NOT NULL, [ORDERNUMBER] [int] NOT NULL, ");

	m_Str += _T("[DATA1] [float] NULL, [DATA2] [float] NULL, [DATA3] [float] NULL, [DATA4] [float] NULL, [DATA5] [float] NULL, ");
	m_Str += _T("[DATA6] [float] NULL, [DATA7] [float] NULL, [DATA8] [float] NULL, [DATA9] [float] NULL, [DATA10] [float] NULL, ");
	m_Str += _T("[DATA11] [float] NULL, [DATA12] [float] NULL, [DATA13] [float] NULL, [DATA14] [float] NULL, ");

	m_Str.AppendFormat(_T("CONSTRAINT [PK_%s] PRIMARY KEY CLUSTERED ( [ID] ASC )WITH (PAD_INDEX = OFF,"), tablename);
	m_Str += _T("STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]) ON [PRIMARY]");

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(m_Str.GetString());
	if (!m_Result)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Roll::DeleteTable( CString tablename )
{
	CString m_cstr;

	m_cstr.Format(_T("DROP TABLE [Cwmsys].[dbo].[%s]"), tablename);

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(m_cstr.GetString());
	if (!m_Result)
	{
		return false;
	}

	return TRUE;
}

BOOL Roll::InsertRowinfo(RollAllInfo m_RollAllInfo)
{
	CString m_cstr;
	CString m_strtime;
	m_strtime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"), m_RollAllInfo.CreateTime.wYear,
		m_RollAllInfo.CreateTime.wMonth,
		m_RollAllInfo.CreateTime.wDay,
		m_RollAllInfo.CreateTime.wHour,
		m_RollAllInfo.CreateTime.wMinute,
		m_RollAllInfo.CreateTime.wSecond,
		m_RollAllInfo.CreateTime.wMilliseconds);
	m_cstr = _T("INSERT INTO [Cwmsys].[dbo].[RollInformation] ([RollName], [CREATETIME])");
	m_cstr.AppendFormat(_T("VALUES('%s', '%s')"), m_RollAllInfo.RollName
		, m_strtime);

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(m_cstr.GetString());
	if (!m_Result)
	{
// 		AfxMessageBox(_T("´´½¨RowÊ§°Ü£¡"));
		return FALSE;
	}

	return TRUE;
}

BOOL Roll::DeleteRowinfo( CString rollname )
{
	CString m_cstr;

	m_cstr.Format(_T("DELETE FROM [Cwmsys].[dbo].[RollInformation] WHERE [RollName] = '%s'"), rollname);

	BOOL m_Result(FALSE);

	m_Result = m_ADOConn.ExecuteSQL(m_cstr.GetString());
	if (!m_Result)
	{
// 		AfxMessageBox(_T("É¾³ýROWÊ§°Ü£¡"));
		return false;
	}

	return TRUE;
}


BOOL Roll::CreateRoll(RollAllInfo m_RollAInfo)
{
    if (!InsertRowinfo(m_RollAInfo))
	{
		return FALSE;
	}
	if (!CreateTable(m_RollAInfo.RollName))
	{
		DeleteRowinfo(m_RollAInfo.RollName);
		return FALSE;
	}

	m_RollName = m_RollAInfo.RollName;

	return TRUE;
}

BOOL Roll::DeleteRoll( CString rollname )
{
	DeleteRowinfo(rollname);
	DeleteTable(rollname);
	return TRUE;
}

BOOL Roll::ClearSingleInfo()
{
	/*m_RollSinlgeInfo.clear();*/
	return true;
}
BOOL Roll::GetRoolInformation()
{
	m_RoolInformation.clear();

	CString cc;
	cc.Format(_T( "SELECT * FROM [Cwmsys].[dbo].[RollInformation]"));

	_RecordsetPtr _Recordset;
	_Recordset = m_ADOConn.GetRecordSet(cc.GetString());

	if(!_Recordset)
	{
		return FALSE;
	}
	SYSTEMTIME lpTime;
	RollAllInfo _RollAllInfo;
	for(; !_Recordset->adoEOF; _Recordset->MoveNext())
	{
		try
		{
			_variant_t _PPID(_Recordset->GetCollect(_T("Roolname")));
			_RollAllInfo.RollName = (LPCTSTR)((_bstr_t)_PPID);

			_variant_t _CreatTime(_Recordset->GetCollect(_T("Time")));

			if(_CreatTime.vt == VT_NULL)
			{
				memset(&lpTime, 0, sizeof(SYSTEMTIME));
			}
			else
			{
				COleDateTime _Time(_CreatTime);
				_Time.GetAsSystemTime(lpTime);
				_RollAllInfo.CreateTime = lpTime;
			}
			m_RoolInformation.push_back(_RollAllInfo);

		}
		catch(...)
		{
		}
	}

	return TRUE;
}

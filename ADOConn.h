#pragma once
#import "c:\\program files\\common files\\system\\ado\\msado15.dll" no_namespace rename("EOF", "adoEOF")

class ADOConn
{
public:
	ADOConn(void);
	virtual ~ADOConn(void);
public:
	//添加一个指向Connection对象的指针:
	_ConnectionPtr m_pConnection;
	//添加一个指向Recordset对象的指针:
	_RecordsetPtr m_pRecordset;
public:
	// 初始化—连接数据库
	BOOL  OnInitADOConn();
	// 执行查询
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
	// 执行SQL语句，Insert Update _variant_t
	BOOL ExecuteSQL(_bstr_t bstrSQL);
	void ExitConnect();
};

#pragma once
#import "c:\\program files\\common files\\system\\ado\\msado15.dll" no_namespace rename("EOF", "adoEOF")

class ADOConn
{
public:
	ADOConn(void);
	virtual ~ADOConn(void);
public:
	//���һ��ָ��Connection�����ָ��:
	_ConnectionPtr m_pConnection;
	//���һ��ָ��Recordset�����ָ��:
	_RecordsetPtr m_pRecordset;
public:
	// ��ʼ�����������ݿ�
	BOOL  OnInitADOConn();
	// ִ�в�ѯ
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL);
	// ִ��SQL��䣬Insert Update _variant_t
	BOOL ExecuteSQL(_bstr_t bstrSQL);
	void ExitConnect();
};

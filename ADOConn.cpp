#include "StdAfx.h"
#include "ADOConn.h"

ADOConn::ADOConn(void)
{
	m_pRecordset = NULL;
}

ADOConn::~ADOConn(void)
{
}

BOOL  ADOConn::OnInitADOConn()
{
	// ��ʼ��OLE/COM�⻷�� 
	::CoInitialize(NULL);

	try
	{
		// ����Connection����
		m_pConnection.CreateInstance("ADODB.Connection");
		// ���������ַ�����������BSTR�ͻ���_bstr_t����
// 		_bstr_t strConnect = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Password=123;Data Source=127.0.0.1\\SQLEXPRESS";
// 		_bstr_t strConnect = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Password=123;Data Source=ADMINPC\\SQLEXPRESS";
// 		_bstr_t strConnect = "Provider=SQLNCLI10.1;Persist Security Info=False;User ID=sa;Password=1;Data Source=127.0.0.1";
		_bstr_t strConnect = "Provider=SQLNCLI11.1;Password=123;Persist Security Info=True;User ID=sa;Initial Catalog=;Data Source=127.0.0.1\\ABC";
// 		_bstr_t strConnect = "Provider=SQLNCLI11.1;Password=1;Persist Security Info=True;User ID=sa;Initial Catalog=UserMan;Data Source=ADMIN-PC\\SQLEXPRESS";
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
	}
	// ��׽�쳣
	catch(_com_error e)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(e.Description());
		return FALSE;
	}
	return TRUE;
}

// ִ�в�ѯ
_RecordsetPtr&  ADOConn::GetRecordSet(_bstr_t bstrSQL)
{
	try
	{
		// �������ݿ⣬���Connection����Ϊ�գ��������������ݿ�
		if(m_pConnection == NULL)
			OnInitADOConn();
		// ������¼������
		if(m_pConnection->State == adStateOpen)
		{
			m_pRecordset.CreateInstance(__uuidof(Recordset));
			// ȡ�ñ��еļ�¼
			m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenForwardOnly, adLockReadOnly, adCmdText);
		}
	}
	// ��׽�쳣
	catch(_com_error e)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(e.Description());
	}
	// ���ؼ�¼��
	return m_pRecordset;
}

// ִ��SQL��䣬Insert Update _variant_t
BOOL ADOConn::ExecuteSQL(_bstr_t bstrSQL)
{
	//	_variant_t RecordsAffected;
	try
	{
		// �Ƿ��Ѿ��������ݿ�
		if(m_pConnection == NULL)
			OnInitADOConn();
		// Connection�����Execute����:(_bstr_t CommandText,
		// VARIANT * RecordsAffected, long Options ) 
		// ����CommandText�������ִ���ͨ����SQL���
		// ����RecordsAffected�ǲ�����ɺ���Ӱ�������, 
		// ����Options��ʾCommandText�����ͣ�adCmdText-�ı����adCmdTable-����
		// adCmdProc-�洢���̣�adCmdUnknown-δ֪
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
		return true;
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
		return false;
	}
}

void ADOConn::ExitConnect()
{
	// �رռ�¼��������
// 	if (m_pRecordset != NULL)
// 		m_pRecordset->Close();
// 	m_pConnection.Release();
// 	m_pConnection->Close();
	// �ͷŻ���
	::CoUninitialize();
}
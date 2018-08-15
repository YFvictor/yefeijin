#pragma once

#include "ADOConn.h"
#include "Roll.h"
#include <vector>


class CDataSql
{
public:
	CDataSql(void);
	~CDataSql(void);

#define PROPERTY_FUNCTION(t, f, v)	const t& f() const{return v;} t& f(){return v;}

public:

	BOOL ConnectSQL();
	void Exsit();

	BOOL InsetData(CString strRollName,Datadisplay lDatadisplay);
	BOOL SelectData(CString strRollName, std::vector<Datadisplay>& vecDatadisplay);
	BOOL DelData(CString strRollName, CString strORDERNUMBER);
	BOOL Update(CString strRollName, std::vector<Datadisplay>& datadisplay);
	std::vector<Datadisplay> m_datadisplay;

private:

	ADOConn m_ADOConn;
};

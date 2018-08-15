#pragma once
#include "ADOConn.h"
#include <vector>

typedef struct _WorkOrderInfo
{
	SYSTEMTIME	Time;
	CString		Breed;
} WorkOrderInfo, *LPWorkOrderInfo;

typedef struct _ListWeight
{
	double wValue1;
	double wValue2;
	double wValue3;
} ListWeight, *LPListWeight;

class WorkOrderRecord
{
public:
	WorkOrderRecord(void);
	~WorkOrderRecord(void);


#define PROPERTY_FUNCTION(t, f, v)	const t& f() const{return v;} t& f(){return v;}

	PROPERTY_FUNCTION(CString, WorkOrderName, m_WorkOrderName);
public:
	BOOL ConnectSQL();
	void ExitSQL();

	BOOL InsertRecord(WorkOrderInfo info, float fvalue1, float fvalue2, float fvalue3);

	BOOL UpdateTen();
	BOOL UpdateFondTime(SYSTEMTIME lpbegin, SYSTEMTIME lpend, CString csbreed);
	BOOL ClearALL();

private:

	ADOConn m_ADOConn;
	CString m_WorkOrderName;

public:
	std::vector<WorkOrderInfo> WorkOrderInfoALL;
	std::vector<ListWeight> ListWeightALL;
};

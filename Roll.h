#pragma once
#include "ADOConn.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
// typedef struct _RollSinlgeInfo
// {
// 	CString RollName;
// 	SYSTEMTIME lptime;
// }RollSinlgeInfo, *PRollSinlgeInfo, *LPRollSinlgeInfo;

//////////////////////////////////////////////////////////////////////////
typedef struct _RollAllInfo
{
	CString			RollName;
	SYSTEMTIME		CreateTime;
// 	SYSTEMTIME		EndTime;
// 	int				BeginPosition;
// 	int 			EndPosition;
// 	int				Velocity;
// 	int				Size;
// 	int				Space;
} RollAllInfo, *LPRollAllInfo;
//////////////////////////////////////////////////////////////////////////

class Roll
{
public:
	Roll(void);
	~Roll(void);

	ADOConn m_ADOConn;
	BOOL ConnectSQL();
	void ExitSQL();

public:
#define PROPERTY_FUNCTION(t, f, v)	const t& f() const{return v;} t& f(){return v;}

	PROPERTY_FUNCTION(CString, RollName, m_RollName);

public:

	BOOL CreateRoll(RollAllInfo m_RollAllInfo);
	BOOL DeleteRoll(CString rollname);
	BOOL SetRollName(CString rollname);
	BOOL SetBeginTime(const SYSTEMTIME& lpTime);
	BOOL Update();
	BOOL GetSingledata(CString rollName);
	BOOL GetRollName(CString& rollname);
	BOOL GetBeginTime(SYSTEMTIME& lpTime);
	BOOL IsExistEverydayTable(CString rollName);
	BOOL IsExistMainTable();
	BOOL IsExistSQLBase();

private:

	BOOL InsertRowinfo(RollAllInfo m_RollAInfo);
	BOOL DeleteRowinfo(CString rollname);
	BOOL CreateTable(CString tablename);
	BOOL DeleteTable(CString tablename);
	CString m_RollName;
	CRITICAL_SECTION m_Lock;

public:
	/*std::vector<RollSinlgeInfo> m_RollSinlgeInfo;*/
	BOOL ClearSingleInfo();
	BOOL GetRoolInformation();
	std::vector<RollAllInfo> m_RoolInformation;
};

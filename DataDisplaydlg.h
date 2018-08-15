#pragma once
#include "afxcmn.h"
#include "ADOConn.h"
#include "Roll.h"
#include "CSV.h"
#include <vector>
#include "DataSql.h"
#include <afxdb.h>

// CDataDisplaydlg 对话框

class CDataDisplaydlg : public CDialog
{
	DECLARE_DYNAMIC(CDataDisplaydlg)

#define PROPERTY_FUNCTION(t, f, v)	const t& f() const{return v;} t& f(){return v;}

	PROPERTY_FUNCTION(CString, FileNameRoll, m_FileNameRoll);

public:
	CDataDisplaydlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataDisplaydlg();

// 对话框数据
	enum { IDD = IDD_DATADISPLAY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ADOConn m_ADOConn;
	CListCtrl m_list;
	CListCtrl m_LIST;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonadd();
	afx_msg void RefreshDisplay();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedButtonquery();
	afx_msg void OnBnClickedButtonexportdata();
	afx_msg void OnBnClickedButtonmainquery();
	afx_msg void OnBnClickedButtondatadel();
	afx_msg void OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_FileNameRoll;
	std::vector<Datadisplay> m_datadisplay;
	std::vector<RollAllInfo> m_RollAllInfo;

	CString m_strCurRollName;
protected:
	Roll m_Roll;
	CDataSql m_datasql;
};

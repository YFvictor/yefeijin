#pragma once
#include "ReadWriteFile.h"
#include "SetupDlg.h"

// ParameterDialog 对话框

class ParameterDialog : public CDialog
{
	DECLARE_DYNAMIC(ParameterDialog)

public:
	ParameterDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ParameterDialog();

// 对话框数据
	enum { IDD = IDD_PARA_DIALOG };
public:

	PROPERTY_FUNCTION(int, Statistics, m_nStatistics);
	PROPERTY_FUNCTION(double, Upperlimit, m_dUpperlimit);
	PROPERTY_FUNCTION(double, Lowerlimit, m_dLowerlimit);
	PROPERTY_FUNCTION(int, RoolNum, m_nRoolNum);
	PROPERTY_FUNCTION(int, RoolMode, m_nRoolMode);
	PROPERTY_FUNCTION(BOOL, IsAuto, m_bIsAuto);
protected:
	int		m_nStatistics;
	double	m_dUpperlimit;
	double	m_dLowerlimit;
	BOOL	m_bIsAuto;			// 自动选择物料卷数
	int		m_nRoolNum;			// 物料卷数
	int     m_nRoolMode;        // 返回模式
public:
	bool		Positive;
	bool		ExistAt9;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CString m_FileName;
	void ReadConfig();
	void WriteConfig();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
//	afx_msg void OnNMThemeChangedRadioSet(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnSetfocusRadioSet();
//	afx_msg void OnBnHotItemChangeRadioSet(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMGetCustomSplitRectRadioSet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioSet();
	afx_msg void OnBnClickedRadioAuto();
	afx_msg void OnCbnSelchangeComboPositive();
};

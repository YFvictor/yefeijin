// DataDisplaydlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "DataDisplaydlg.h"
#include <vector>


// CDataDisplaydlg 对话框

IMPLEMENT_DYNAMIC(CDataDisplaydlg, CDialog)

CDataDisplaydlg::CDataDisplaydlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataDisplaydlg::IDD, pParent)
	, m_strCurRollName(_T(""))
{
}

CDataDisplaydlg::~CDataDisplaydlg()
{
}

void CDataDisplaydlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_LIST3, m_LIST);
}


BEGIN_MESSAGE_MAP(CDataDisplaydlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTONADD, &CDataDisplaydlg::OnBnClickedButtonadd)
	ON_BN_CLICKED(IDC_DEL, &CDataDisplaydlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_BUTTONQUERY, &CDataDisplaydlg::OnBnClickedButtonquery)
	ON_BN_CLICKED(IDC_BUTTONEXPORTDATA, &CDataDisplaydlg::OnBnClickedButtonexportdata)
	ON_BN_CLICKED(IDC_BUTTONMAINQUERY, &CDataDisplaydlg::OnBnClickedButtonmainquery)
	ON_BN_CLICKED(IDC_BUTTONDATADEL, &CDataDisplaydlg::OnBnClickedButtondatadel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CDataDisplaydlg::OnNMDblclkList3)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CDataDisplaydlg::OnLvnGetdispinfoList1)
END_MESSAGE_MAP()
// CDataDisplaydlg 消息处理程序
BOOL CDataDisplaydlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_Roll.ConnectSQL())
	{
		m_Roll.IsExistSQLBase();
		m_Roll.IsExistMainTable();

		SetTimer(0, 200, NULL);
	}
	else
	{
		AfxMessageBox(_T("SQL链接失败！"));
		ASSERT(FALSE);
	}

	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	m_LIST.SetExtendedStyle(dwExStyle);
	m_list.SetExtendedStyle(dwExStyle);

	m_LIST.InsertColumn(0,_T("表名"), LVCFMT_LEFT, 180);
	m_LIST.InsertColumn(1,_T("创建时间"), LVCFMT_LEFT, 100);

	m_list.InsertColumn(0,_T("时间"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1,_T("序号"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(2,_T("种类"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3,_T("操作员"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(4,_T("数据1"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(5,_T("数据2"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(6,_T("数据3"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(7,_T("数据4"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(8,_T("数据5"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(9,_T("数据6"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(10,_T("数据7"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(11,_T("数据8"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(12,_T("数据9"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(13,_T("数据10"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(14,_T("数据11"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(15,_T("数据12"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(16,_T("数据13"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(17,_T("数据14"), LVCFMT_LEFT, 100);

	m_Roll.ConnectSQL();
	m_Roll.Update();
	
	CString m_texttt;
	SYSTEMTIME lptime;
	GetLocalTime(&lptime);

	for (size_t i(0); i < m_Roll.m_RoolInformation.size(); i ++)
	{
		int nIndex = m_LIST.GetItemCount();

		m_LIST.InsertItem(nIndex, m_Roll.m_RoolInformation[i].RollName);

		m_texttt.Format(_T("%04u-%02u-%02u "), m_Roll.m_RoolInformation[i].CreateTime.wYear,
			m_Roll.m_RoolInformation[i].CreateTime.wMonth,m_Roll.m_RoolInformation[i].CreateTime.wDay);

		m_LIST.SetItemText(nIndex, 1, m_texttt);
	}

	m_datasql.ConnectSQL();

// 	if (m_Roll.m_RoolInformation.size() > 0 )
// 	{
// 		m_datasql.Update(m_Roll.m_RoolInformation[0].RollName, m_datadisplay);
// 		m_list.SetItemCount(m_datadisplay.size());
// 	}
// 
	return TRUE;
}

void CDataDisplaydlg::OnBnClickedDel()
{
	int nItem = m_LIST.GetNextItem(-1,LVNI_SELECTED);
	CString str;
	str = m_LIST.GetItemText(nItem,0);
	if (nItem == -1)
	{
		AfxMessageBox(_T("没有选择要删除的信息!"));
		return ;
	}
	m_Roll.DeleteRoll(str);	
	RefreshDisplay();
}

void CDataDisplaydlg::OnBnClickedButtondatadel()
{
// 	int Item = m_list.GetNextItem(-1,LVNI_SELECTED);
// 	CString strt;
// 	strt =m_list.GetItemText(Item,0);
// 	if (Item == -1)
// 	{
// 		AfxMessageBox(_T("没有选择要删除的信息!"));
// 		return ;
// 	}
// 	m_datasql.DelData(m_strCurRollName, strt);
// 
// 	RefreshDisplay();

}

void CDataDisplaydlg::OnBnClickedButtonmainquery()
{
	CString roolname;
 	GetDlgItemText(IDC_EDIT_ROOLNAME,roolname);
	m_Roll.GetSingledata(roolname);
    m_LIST.DeleteAllItems();

	CString m_texttt;
	SYSTEMTIME lptime;
	GetLocalTime(&lptime);

	for (size_t i(0); i < m_Roll.m_RoolInformation.size(); i ++)
	{
		int nIndex = m_LIST.GetItemCount();

		m_LIST.InsertItem(nIndex, m_Roll.m_RoolInformation[i].RollName);

		m_texttt.Format(_T("%04u-%02u-%02u "), m_Roll.m_RoolInformation[i].CreateTime.wYear,
			m_Roll.m_RoolInformation[i].CreateTime.wMonth,m_Roll.m_RoolInformation[i].CreateTime.wDay);

		m_LIST.SetItemText(nIndex, 1, m_texttt);
	}
	if (roolname.IsEmpty())
	{
		AfxMessageBox(_T("请输入查询信息！"));
	}
	else if (m_Roll.m_RoolInformation.size() == 0)
	{
		AfxMessageBox(_T("没有查询结果"));
	}
}

void CDataDisplaydlg::OnBnClickedButtonquery()
{
// 	std::vector<Datadisplay> vecDatadisplay;
// 	CString ordernumber;
// 	GetDlgItemText(IDC_EDIT_ORDERNUMBER,ordernumber);
// 	m_datasql.SelectData(ordernumber, vecDatadisplay);
// 	m_list.SetItemCount(m_datadisplay.size());
// 	m_list.DeleteAllItems();
// 
// 	if (m_datadisplay.size() == 0)
// 	{
// 		AfxMessageBox(_T("没有查询结果"));
// 	}
}
void CDataDisplaydlg::OnBnClickedButtonadd()
{
// 	Datadisplay tDatadisplay;
// 
// 	tDatadisplay.datetime;
// 	tDatadisplay.breed;
// 	tDatadisplay.operationalsource;
// 	tDatadisplay.ordernumber;
// 
// 	tDatadisplay.arrfdata[0];
// 	tDatadisplay.arrfdata[1];
// 	tDatadisplay.arrfdata[2];
// 	tDatadisplay.arrfdata[3];
// 	tDatadisplay.arrfdata[4];
// 	tDatadisplay.arrfdata[5];
// 	tDatadisplay.arrfdata[6];
// 	tDatadisplay.arrfdata[7];
// 	tDatadisplay.arrfdata[8];
// 	tDatadisplay.arrfdata[9];
// 	tDatadisplay.arrfdata[10];
// 	tDatadisplay.arrfdata[11];
// 	tDatadisplay.arrfdata[12];
// 	tDatadisplay.arrfdata[13];
// 
// 	CDataSql m_datasql;
// 	m_datasql.ConnectSQL();
// 	m_datasql.InsetData(m_strCurRollName,tDatadisplay);
// 	m_datasql.Exsit();
// 
// 	RefreshDisplay();
}

void CDataDisplaydlg::OnBnClickedButtonexportdata()
{
	if (m_datadisplay.size() <= 0)
	{
		AfxMessageBox(_T("列表中没有记录需要保存！"));
		return;
	}
	CFileDialog dlg( FALSE, 
		_T("csv"), 
		_T("FileList"), 
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("逗号分隔值文件文件(*.csv)|*.csv|Excel 文件(*.xls)|*.xls||"));

	dlg.m_ofn.lpstrTitle = _T("文件另存为");

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strFilename;
	strFilename = dlg.GetPathName();
	CString m_name;
	m_name = dlg.GetFileExt();
	if (m_name == _T("csv"))
	{
		CSV m_CSV;
		HANDLE hFile = m_CSV.OpenLogFile(strFilename, m_CSV.SlotLogHeader());
		for (size_t i(0); i < m_datadisplay.size(); i++)
		{
			m_CSV.SetRoW(hFile, m_datadisplay[i]);
		}
		m_CSV.CloseReport(hFile);
	}
	else if (m_name == _T("xls"))
	{
		DWORD dwRe = GetFileAttributes(strFilename);
		if ( dwRe != (DWORD)-1 )
		{
			DeleteFile(strFilename);
		}
		CDatabase database;
		CString sDriver = L"MICROSOFT EXCEL DRIVER (*.XLS)";
		CString sSql,strInsert;
		TRY
		{
			sSql.Format(L"DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver, strFilename, strFilename);

			if( database.OpenEx(sSql,CDatabase::noOdbcDialog) )
			{
				int iColumnNum,iRowCount;
				LVCOLUMN lvCol;
				CString strColName;                               //用于保存列标题名
				int i,j;                                          //列、行循环参数

				iColumnNum = m_list.GetHeaderCtrl() -> GetItemCount();
				iRowCount = m_list.GetItemCount();
				sSql = _T(" CREATE TABLE DSO_DX ( ");
				strInsert =_T(" INSERT INTO DSO_DX ( ") ;         //获得列标题名
				lvCol.mask = LVCF_TEXT;                           //必需设置，说明LVCOLUMN变量中pszText参数有效
				lvCol.cchTextMax = 32;                            //必设，pszText参数所指向的字符串的大小
				lvCol.pszText = strColName.GetBuffer(32);         //必设，pszText 所指向的字符串的实际存储位置。
				                                                  //以上三个参数设置后才能通过 GetColumn()函数获得列标题的名
				for( i=0 ; i< iColumnNum ; i++ )
				{
					if ( !(m_list.GetColumn(i,&lvCol)) )
						return;
					if ( i<iColumnNum-1 )
					{
						sSql = sSql + lvCol.pszText + _T(" TEXT , ");
						strInsert = strInsert + lvCol.pszText + _T(" , ");
					}
					else
					{
						sSql = sSql + lvCol.pszText + _T(" TEXT ) ");
						strInsert = strInsert + lvCol.pszText + _T(" )  VALUES ( ");
					}
				}                                                  //创建Excel表格文件
				database.ExecuteSQL(sSql);                         //循环提取记录并插入到EXCEL中
				sSql = strInsert;
				CString chTemp;
				for ( j=0 ; j<iRowCount ; j++ )
				{
					// 				memset(&chTemp,0,33);
					for ( i=0 ; i<iColumnNum ; i++ )
					{
						chTemp  = m_list.GetItemText(j,i);
						if ( i < (iColumnNum-1) )
						{
							sSql = sSql + L"'" + chTemp + L"' , ";
						}
						else
						{
							sSql = sSql + L"'" + chTemp + L"' ) ";
						}
					}                                               //将记录插入到表格中
					database.ExecuteSQL(sSql);
					sSql = strInsert; 
				}
			}                                                       // 关闭Excel表格文件
			database.Close();
			AfxMessageBox(_T("保存查询结果为Excel文件成功！"));
		}
		CATCH_ALL(e)
		{
			//错误类型很多，根据需要进行报错。
			AfxMessageBox(_T("Excel文件保存失败！"));
		}
		END_CATCH_ALL;

	}
	
}

void CDataDisplaydlg::RefreshDisplay()
{
	m_LIST.DeleteAllItems();
	m_Roll.m_RoolInformation.clear();
    m_Roll.Update();

	CString m_text;

	for (size_t i(0); i < m_Roll.m_RoolInformation.size(); i ++)
	{
		int nIndex = m_LIST.GetItemCount();

		m_LIST.InsertItem(nIndex, m_Roll.m_RoolInformation[i].RollName);

		m_text.Format(_T("%04u-%02u-%02u %02u:%02u:%02u.%03u"), m_Roll.m_RoolInformation[i].CreateTime.wYear,
			m_Roll.m_RoolInformation[i].CreateTime.wMonth,m_Roll.m_RoolInformation[i].CreateTime.wDay,
			m_Roll.m_RoolInformation[i].CreateTime.wHour,m_Roll.m_RoolInformation[i].CreateTime.wMinute,
			m_Roll.m_RoolInformation[i].CreateTime.wSecond,m_Roll.m_RoolInformation[i].CreateTime.wMilliseconds);;
		m_LIST.SetItemText(nIndex, 1, m_text);
	}
	m_list.DeleteAllItems();
	m_datadisplay.clear();

	if (m_strCurRollName.IsEmpty())
	{
		return;
	}
	m_datasql.Update(m_strCurRollName, m_datadisplay);
	m_list.SetItemCount(m_datadisplay.size());

}



void CDataDisplaydlg::OnNMDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int mItem = m_LIST.GetNextItem(-1, LVNI_SELECTED);
	if (mItem < 0)
	{
		return;
	}
	CString strText;

	strText = m_LIST.GetItemText(mItem, 0);
	
	
	m_datasql.Update(strText,m_datadisplay);
	m_list.SetItemCount(m_datadisplay.size());
	m_strCurRollName = strText;

	*pResult = 0;
}

void CDataDisplaydlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem= &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;

	if (pItem->mask & LVIF_TEXT)
	{
		CString text;

		//Which column?
		switch(pItem->iSubItem)
		{
		case 0:
			text = m_datadisplay[itemid].datetime;
			break;
		case 1:
			text.Format(_T("%d"), m_datadisplay[itemid].ordernumber);
			break;
		case 2:
			text = m_datadisplay[itemid].breed;
			break;
		case 3:
			text = m_datadisplay[itemid].operationalsource;
			break;
		case 4:
            text.Format(_T("%.2f"), m_datadisplay[itemid].arrfdata[0]);
			break;
		case 5:
			text.Format(_T("%.2f"), m_datadisplay[itemid].arrfdata[1]);
			break;
		case 6:
			text.Format(_T("%.2f"), m_datadisplay[itemid].arrfdata[2]);
			break;
		case 7:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[3]);
			break;
		case 8:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[4]);
			break;
		case 9:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[5]);
			break;
		case 10:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[6]);
			break;
		case 11:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[7]);
			break;
		case 12:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[8]);
			break;
		case 13:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[9]);
			break;
		case 14:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[10]);
			break;
		case 15:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[11]);
			break;
		case 16:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[12]);
			break;
		case 17:
			text.Format(_T("%.2f"),m_datadisplay[itemid].arrfdata[13]);
			break;
		}

		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);
	}
	*pResult = 0;
}
void CDataDisplaydlg::OnDestroy()
{
	CDialog::OnDestroy();
	m_Roll.ExitSQL();
	m_datasql.Exsit();
}
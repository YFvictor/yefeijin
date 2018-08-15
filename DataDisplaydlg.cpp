// DataDisplaydlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "DataDisplaydlg.h"
#include <vector>


// CDataDisplaydlg �Ի���

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
// CDataDisplaydlg ��Ϣ�������
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
		AfxMessageBox(_T("SQL����ʧ�ܣ�"));
		ASSERT(FALSE);
	}

	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_HEADERDRAGDROP | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
	m_LIST.SetExtendedStyle(dwExStyle);
	m_list.SetExtendedStyle(dwExStyle);

	m_LIST.InsertColumn(0,_T("����"), LVCFMT_LEFT, 180);
	m_LIST.InsertColumn(1,_T("����ʱ��"), LVCFMT_LEFT, 100);

	m_list.InsertColumn(0,_T("ʱ��"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(1,_T("���"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(2,_T("����"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3,_T("����Ա"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(4,_T("����1"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(5,_T("����2"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(6,_T("����3"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(7,_T("����4"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(8,_T("����5"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(9,_T("����6"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(10,_T("����7"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(11,_T("����8"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(12,_T("����9"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(13,_T("����10"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(14,_T("����11"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(15,_T("����12"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(16,_T("����13"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(17,_T("����14"), LVCFMT_LEFT, 100);

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
		AfxMessageBox(_T("û��ѡ��Ҫɾ������Ϣ!"));
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
// 		AfxMessageBox(_T("û��ѡ��Ҫɾ������Ϣ!"));
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
		AfxMessageBox(_T("�������ѯ��Ϣ��"));
	}
	else if (m_Roll.m_RoolInformation.size() == 0)
	{
		AfxMessageBox(_T("û�в�ѯ���"));
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
// 		AfxMessageBox(_T("û�в�ѯ���"));
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
		AfxMessageBox(_T("�б���û�м�¼��Ҫ���棡"));
		return;
	}
	CFileDialog dlg( FALSE, 
		_T("csv"), 
		_T("FileList"), 
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("���ŷָ�ֵ�ļ��ļ�(*.csv)|*.csv|Excel �ļ�(*.xls)|*.xls||"));

	dlg.m_ofn.lpstrTitle = _T("�ļ����Ϊ");

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
				CString strColName;                               //���ڱ����б�����
				int i,j;                                          //�С���ѭ������

				iColumnNum = m_list.GetHeaderCtrl() -> GetItemCount();
				iRowCount = m_list.GetItemCount();
				sSql = _T(" CREATE TABLE DSO_DX ( ");
				strInsert =_T(" INSERT INTO DSO_DX ( ") ;         //����б�����
				lvCol.mask = LVCF_TEXT;                           //�������ã�˵��LVCOLUMN������pszText������Ч
				lvCol.cchTextMax = 32;                            //���裬pszText������ָ����ַ����Ĵ�С
				lvCol.pszText = strColName.GetBuffer(32);         //���裬pszText ��ָ����ַ�����ʵ�ʴ洢λ�á�
				                                                  //���������������ú����ͨ�� GetColumn()��������б������
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
				}                                                  //����Excel����ļ�
				database.ExecuteSQL(sSql);                         //ѭ����ȡ��¼�����뵽EXCEL��
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
					}                                               //����¼���뵽�����
					database.ExecuteSQL(sSql);
					sSql = strInsert; 
				}
			}                                                       // �ر�Excel����ļ�
			database.Close();
			AfxMessageBox(_T("�����ѯ���ΪExcel�ļ��ɹ���"));
		}
		CATCH_ALL(e)
		{
			//�������ͺܶ࣬������Ҫ���б���
			AfxMessageBox(_T("Excel�ļ�����ʧ�ܣ�"));
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
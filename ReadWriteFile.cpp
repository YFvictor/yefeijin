/****************************************************************************
***  author: 曹洪
***  time  : 20160308
***  adress: 公司
***  作用  ：第一，简化流程；
第二，调用简单
****************************************************************************/

#include "stdafx.h"

#define RW_API _declspec(dllexport)
#include "ReadWriteFile.h"


CString FloatToCstring(float f)
{
	CString str = _T("");
	try
	{
		if(f>100)
		{
          str.Format(_T("%.1f"), f);
		}
		else if( f>=0 && f<=100)
		{
		  str.Format(_T("%.2f"), f);
		}
		return str;
	}
	catch (CString str1)
	{
		return str1;
	}

}

CString GetFileName(CString m_FileName)
{
	CString  m_CStr;
	LPTSTR  _Extension(m_CStr.GetBuffer(MAX_PATH));

	switch (::GetModuleFileName(NULL, _Extension, MAX_PATH))
	{
	case 0:
	case MAX_PATH:
		ASSERT(FALSE);
		break;

	default:
		*(::PathFindFileName(_Extension)) = _TCHAR('\0');
	};

	m_CStr.ReleaseBuffer();

	m_FileName = m_CStr + m_FileName;
	return m_FileName;
}

double ReadFile_f(CString lpAppName, CString lpKeyName, double lpDefault, CString m_FileName)
{
	_TCHAR _Buffer[1024];
	memset(_Buffer, 0, sizeof(_Buffer));
	CString _Default;
	_Default.Format(_T("%.16f"), lpDefault);
	GetPrivateProfileString(lpAppName, lpKeyName, _Default, _Buffer, sizeof(_Buffer) / sizeof(_TCHAR), m_FileName);
	return _tstof(_Buffer);
}

void WriteFile_f(CString lpAppName, CString lpKeyName, double lpdouble, int size, CString m_FileName)
{
	CString _Buffer;
	switch (size)
	{
	case 1:
		_Buffer.Format(_T("%.1f"), lpdouble);
		break;
	case 2:
		_Buffer.Format(_T("%.2f"), lpdouble);
		break;
	case 3:
		_Buffer.Format(_T("%.3f"), lpdouble);
		break;
	case 4:
		_Buffer.Format(_T("%.4f"), lpdouble);
		break;
	case 5:
		_Buffer.Format(_T("%.5f"), lpdouble);
		break;
	case 6:
		_Buffer.Format(_T("%.6f"), lpdouble);
		break;
	case 7:
		_Buffer.Format(_T("%.7f"), lpdouble);
		break;
	case 8:
		_Buffer.Format(_T("%.8f"), lpdouble);
		break;
	case 9:
		_Buffer.Format(_T("%.9f"), lpdouble);
		break;
	case 10:
		_Buffer.Format(_T("%.10f"), lpdouble);
		break;
	case 11:
		_Buffer.Format(_T("%.11f"), lpdouble);
		break;
	case 12:
		_Buffer.Format(_T("%.12f"), lpdouble);
		break;
	case 13:
		_Buffer.Format(_T("%.13f"), lpdouble);
		break;
	case 14:
		_Buffer.Format(_T("%.14f"), lpdouble);
		break;
	case 15:
		_Buffer.Format(_T("%.15f"), lpdouble);
		break;
	case 16:
		_Buffer.Format(_T("%.16f"), lpdouble);
		break;
	default:
		_Buffer.Format(_T("%.16f"), lpdouble);
		break;
	}
	WritePrivateProfileString(lpAppName, lpKeyName, _Buffer, m_FileName);
}

long ReadFile_l(CString lpAppName, CString lpKeyName, long lpDefault, CString m_FileName)
{
	_TCHAR _Buffer[1024];
	memset(_Buffer, 0, sizeof(_Buffer));
	CString _Default;
	_Default.Format(_T("%d"), lpDefault);

	GetPrivateProfileString(lpAppName, lpKeyName, _Default, _Buffer, sizeof(_Buffer) / sizeof(_TCHAR), m_FileName);

	return _tstol(_Buffer);
}

void WriteFile_l(CString lpAppName, CString lpKeyName, long lplong, CString m_FileName)
{
	CString _Buffer;
	_Buffer.Format(_T("%d"), lplong);
	WritePrivateProfileString(lpAppName, lpKeyName, _Buffer, m_FileName);
}

RW_API bool ReadFile_b( CString lpAppName, CString lpKeyName, bool lpDefault, CString m_FileName )
{
	_TCHAR _Buffer[1024];
	memset(_Buffer, 0, sizeof(_Buffer));
	CString _Default;
	_Default.Format(_T("%d"), lpDefault);
	GetPrivateProfileString(lpAppName, lpKeyName, _Default, _Buffer, sizeof(_Buffer) / sizeof(_TCHAR), m_FileName);
	if (_tstol(_Buffer) == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

RW_API void WriteFile_b( CString lpAppName, CString lpKeyName, bool lplong, CString m_FileName )
{
	CString _Buffer;
	_Buffer.Format(_T("%d"), lplong);
	WritePrivateProfileString(lpAppName, lpKeyName, _Buffer, m_FileName);
}

CString ReadFile_s(CString lpAppName, CString lpKeyName, CString lpDefault, CString m_FileName)
{
	_TCHAR _Buffer[1024];
	memset(_Buffer, 0, sizeof(_Buffer));
	GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, _Buffer, sizeof(_Buffer) / sizeof(_TCHAR), m_FileName);
	return (CString)(_Buffer);
}

void WriteFile_s(CString lpAppName, CString lpKeyName, CString lpcstring, CString m_FileName)
{
	WritePrivateProfileString(lpAppName, lpKeyName, lpcstring, m_FileName);
}

USHORT ReadFile_us(CString lpAppName, CString lpKeyName, USHORT lpDefault, CString m_FileName)
{
	_TCHAR _Buffer[1024];
	memset(_Buffer, 0, sizeof(_Buffer));
	CString _Default;
	_Default.Format(_T("%d"), lpDefault);

	GetPrivateProfileString(lpAppName, lpKeyName, _Default, _Buffer, sizeof(_Buffer) / sizeof(_TCHAR), m_FileName);

	return (USHORT)_tstol(_Buffer);
}

void WriteFile_us(CString lpAppName, CString lpKeyName, USHORT lplong, CString m_FileName)
{
	CString _Buffer;
	_Buffer.Format(_T("%d"), lplong);
	WritePrivateProfileString(lpAppName, lpKeyName, _Buffer, m_FileName);
}

DWORD EnumSerialPorts(CStringArray &lpPorts, DWORD dwOperation  = SPCTRL_ENUMOPT_PHYSICS)
{
	HKEY hKey(NULL);
	LONG lResult(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), NULL, KEY_READ, &hKey));

	switch(lResult)
	{
	case ERROR_SUCCESS:										//	打开注册表成功

		for(DWORD i(0); lResult == ERROR_SUCCESS; i++)
		{
			DWORD dwValueName(16384), dwValue(1024 * sizeof(TCHAR));
			TCHAR txValueName[16384], txValue[1024];

			lResult = RegEnumValue(hKey, i, txValueName, &dwValueName, NULL, NULL, (LPBYTE)txValue, &dwValue);

			if(lResult != ERROR_SUCCESS)
			{
				continue;
			}

			*(txValueName + 18) = _T('\0');

			switch(dwOperation)
			{
			case SPCTRL_ENUMOPT_PHYSICS:

				if(!lstrcmpi(txValueName, _T("\\Device\\SerialPort")))
				{
					lpPorts.Add(txValue);
				}
				break;

			case SPCTRL_ENUMOPT_VIRTUAL:

				if(lstrcmpi(txValueName, _T("\\Device\\SerialPort")))
				{
					lpPorts.Add(txValue);
				}
				break;

			default:
				lpPorts.Add(txValue);
			}
		}

		RegCloseKey(hKey);

		if(lResult != ERROR_NO_MORE_ITEMS)
		{
			return lResult;
		}

	case ERROR_FILE_NOT_FOUND:								//	打开注册表失败（系统中未安装串口）
		break;

	default:												//	打开注册表失败，返回错误代码
		return lResult;
	}

	return ERROR_SUCCESS;
}

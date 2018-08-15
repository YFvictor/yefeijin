
#ifdef RW_API
#else
#define RW_API _declspec(dllexport)
#endif

#define SPCTRL_ENUMOPT_ALL						0														//	枚举所有串口
#define SPCTRL_ENUMOPT_PHYSICS					1														//	仅物理串口
#define SPCTRL_ENUMOPT_VIRTUAL					2														//	仅枚举虚拟串口

RW_API CString FloatToCstring(float f);

RW_API CString GetFileName(CString m_FileName);

//////////////////////////////////////////////////////////////////////////
//// double

RW_API double ReadFile_f(CString lpAppName, CString lpKeyName, double lpDefault, CString m_FileName);
RW_API void WriteFile_f(CString lpAppName, CString lpKeyName, double lpdouble, int size, CString m_FileName);

//////////////////////////////////////////////////////////////////////////
//// long

RW_API long ReadFile_l(CString lpAppName, CString lpKeyName, long lpDefault, CString m_FileName);
RW_API void WriteFile_l(CString lpAppName, CString lpKeyName, long lplong, CString m_FileName);

//////////////////////////////////////////////////////////////////////////
//// bool

RW_API bool ReadFile_b(CString lpAppName, CString lpKeyName, bool lpDefault, CString m_FileName);
RW_API void WriteFile_b(CString lpAppName, CString lpKeyName, bool lplong, CString m_FileName);

//////////////////////////////////////////////////////////////////////////
//// CString

RW_API CString ReadFile_s(CString lpAppName, CString lpKeyName, CString lpDefault, CString m_FileName);
RW_API void WriteFile_s(CString lpAppName, CString lpKeyName, CString lpcstring, CString m_FileName);

//////////////////////////////////////////////////////////////////////////
//// USHORT

RW_API USHORT ReadFile_us(CString lpAppName, CString lpKeyName, USHORT lpDefault, CString m_FileName);
RW_API void WriteFile_us(CString lpAppName, CString lpKeyName, USHORT lplong, CString m_FileName);

RW_API DWORD EnumSerialPorts(CStringArray &lpPorts, DWORD dwOperation/* = SPCTRL_ENUMOPT_PHYSICS*/);	//	枚举系统中已安装的串口

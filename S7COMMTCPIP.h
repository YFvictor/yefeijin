#pragma  once

#ifdef S7COMMTCPIP_EXPORTS
#define S7COMMTCPIP_DLL __declspec(dllexport)
#else
#define S7COMMTCPIP_DLL __declspec(dllimport)
#endif

typedef enum 
{
	TYPE_INT = 0,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_BOOL,
	TYPE_SHORT,
	TYPE_BYTE,
	TYPE_UNKNOW
}e_PLC_DATA_TYPE;

class S7COMMTCPIP_DLL S7COMMTCPIP
{
public:
	S7COMMTCPIP(void);
	~S7COMMTCPIP(void);

public:

	BOOL CreateTcpClient(UINT nSocketPort = 0, LPCTSTR lpszSocketAddress = NULL);
	BOOL IsSocketOpened() const;
	BOOL ConnectToPLC(DWORD dwtimeout = 5000);
	BOOL DestroySocket();

	BOOL GetWord(LPCTSTR lpAddress, void* lpBuffer, e_PLC_DATA_TYPE ntype);
	BOOL GetWord(LPCTSTR lpAddress, void* lpBuffer, LONG lCount, e_PLC_DATA_TYPE ntype);
	BOOL SetWord(LPCTSTR lpAddress, void* lpBuffer, e_PLC_DATA_TYPE ntype);
	BOOL SetWord(LPCTSTR lpAddress, void* lpBuffer, LONG lCount, e_PLC_DATA_TYPE ntype);

private:

	SOCKET m_Socket;
	BOOL m_TCPIPStutes;
	BOOL m_TCPEixt;

	CRITICAL_SECTION m_Lock;
};
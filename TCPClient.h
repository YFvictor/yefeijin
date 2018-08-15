#pragma once

typedef struct 
{
	BYTE		MsgHaed;							// 1个字节
	char		MsgDirection;						// 1个字节
	CStringA	MsgAgreementNo;						// 4个字节
	CStringA	MsgID;								// 5个字节
	char		MsgResponse;						// 1个字节
	CStringA	MsgCmd;								// 4个字节
	CStringA	MsgDeviceNo;						// 30个字节
	CStringA	MsgBody;							// 可变
	CStringA	MsgCheckcode;						// 2个字节
	BYTE		MsgTrail;							// 1个字节
}MESMessage;

class TCPClient
{
public:
	TCPClient(void);
	~TCPClient(void);

	SOCKET m_SocketClient;

	BOOL CreateTcpClient(UINT nSocketPort = 0, LPCTSTR lpszSocketAddress = NULL);

	BOOL CloseClient();

 	static DWORD WINAPI ReceiveThread(TCPClient* lpNotify);

	BOOL SendHeartBeat(DWORD dwtimeout);													//可直接发送数字1标识上位机在线
	int SendLogin(CStringA username, CStringA userpwd, DWORD dwtimeout);					//0.允许登录 1.账号或密码错误 2.权限不足 3.无返回 
	BOOL SendAlarm(int Alarmid, int status, SYSTEMTIME& locdtime, DWORD dwtimeout);				//1:当前有报警,0:报警解除
	BOOL SendRunningStatus(int status, SYSTEMTIME& locdtime, DWORD dwtimeout);						//1.开机 2.维修 0.关机
	BOOL SendOperatingData(int vel, double avgvalue, int scannrate, SYSTEMTIME& locdtime, DWORD dwtimeout);
	BOOL SendOperatingData(int vel, double avgvalue[80], int scannrate, SYSTEMTIME& locdtime, DWORD dwtimeout );
	BOOL SendUpkeep(char* locdtime, short usdatalen);
	BOOL SendMaintain(char* locdtime, short usdatalen);
	BOOL SendCraftParameter(char*puchmsg, short usdatalen);
	BOOL SendProductiondata(char*puchmsg, short usdatalen);

	BOOL ReceiveHeartBeat(char msg, size_t msglen);
	BOOL ReceiveLogin(char msg, size_t msglen);													
	BOOL ReceiveAlarm(char msg, size_t msglen);
	BOOL ReceiveRunningStatus(char msg, size_t msglen);
	BOOL ReceiveOperatingData(char msg, size_t msglen);
	BOOL ReceiveSendUpkeep(char msg, size_t msglen);
	BOOL ReceiveMaintain(char msg, size_t msglen);
	BOOL ReceiveCraftParameter(char msg, size_t msglen);
	BOOL ReceiveProductiondata(char msg, size_t msglen);

	BOOL GetErrorMessage();
	
	BOOL Reconnect();


public:
	CString DeviceNo;
	int MessageId;
	CString AgreementNo;
	BOOL m_TCPIPStutes;

private:

	void checkCode(CStringA& checkcode, char * puchmsg, short usdatalen);
	void checkCode(char* one, char *ten, char * puchmsg, short usdatalen);
	int Commondid;

	SOCKADDR_IN addrSrv;
};

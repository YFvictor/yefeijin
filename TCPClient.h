#pragma once

typedef struct 
{
	BYTE		MsgHaed;							// 1���ֽ�
	char		MsgDirection;						// 1���ֽ�
	CStringA	MsgAgreementNo;						// 4���ֽ�
	CStringA	MsgID;								// 5���ֽ�
	char		MsgResponse;						// 1���ֽ�
	CStringA	MsgCmd;								// 4���ֽ�
	CStringA	MsgDeviceNo;						// 30���ֽ�
	CStringA	MsgBody;							// �ɱ�
	CStringA	MsgCheckcode;						// 2���ֽ�
	BYTE		MsgTrail;							// 1���ֽ�
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

	BOOL SendHeartBeat(DWORD dwtimeout);													//��ֱ�ӷ�������1��ʶ��λ������
	int SendLogin(CStringA username, CStringA userpwd, DWORD dwtimeout);					//0.�����¼ 1.�˺Ż�������� 2.Ȩ�޲��� 3.�޷��� 
	BOOL SendAlarm(int Alarmid, int status, SYSTEMTIME& locdtime, DWORD dwtimeout);				//1:��ǰ�б���,0:�������
	BOOL SendRunningStatus(int status, SYSTEMTIME& locdtime, DWORD dwtimeout);						//1.���� 2.ά�� 0.�ػ�
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

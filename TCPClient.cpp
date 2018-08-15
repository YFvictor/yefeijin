#include "StdAfx.h"
#include <Winsock2.h>
#include "TCPClient.h"

TCPClient::TCPClient(void)
{
	MessageId = 1;
	Commondid = 0;
	m_TCPIPStutes = FALSE;
}

TCPClient::~TCPClient(void)
{
}

BOOL TCPClient::CreateTcpClient( UINT nSocketPort /*= 0*/, LPCTSTR lpszSocketAddress /*= NULL*/ )
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) 
	{
		return FALSE;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup( );
		return FALSE; 
	}

	char addr[255];
	WideCharToMultiByte( CP_ACP, 0, lpszSocketAddress, -1,
		addr, sizeof(addr), NULL, NULL );

	m_SocketClient = socket(AF_INET,SOCK_STREAM,0);

	addrSrv.sin_addr.S_un.S_addr=inet_addr(addr);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(nSocketPort);

	if (connect(m_SocketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
// 		WSACleanup( );
		return FALSE;
	}

// 	if(NULL == CreateThread(NULL, 0, reinterpret_cast<PTHREAD_START_ROUTINE>(ReceiveThread), this, 0, NULL))
// 	{
// 		return FALSE;
// 	}

	m_TCPIPStutes = TRUE;

	return TRUE;
}

DWORD WINAPI TCPClient::ReceiveThread( TCPClient* lpNotify )
{
	char kk[256];
	while(TRUE)
	{
		int lenght = recv(lpNotify->m_SocketClient, kk, sizeof(kk), 0);
		if (lenght <= 0)
		{
			break;
		}
		TRACE1("%d\r\n",lenght); 
		lpNotify->m_TCPIPStutes = FALSE;
	}
	return 0;
}

BOOL TCPClient::CloseClient()
{
	m_TCPIPStutes = FALSE;
	closesocket(m_SocketClient);
	WSACleanup();
	return TRUE;
}

void TCPClient::checkCode(CStringA& checkcode, char * puchmsg, short usdatalen )
{
	int sum = 0;
	for (int i = 0; i < usdatalen; i++)
	{
		sum += puchmsg[i];
	}

	int checkCode1 = sum % 10;
// 	checkCode1 = checkCode1 + 48;

	int checkCode2 = sum / 10 % 10;
// 	checkCode2 = checkCode2 + 48;

	checkcode.Format("%d%d", checkCode2, checkCode1);
}

void TCPClient::checkCode(char* one, char *ten, char * puchmsg, short usdatalen)
{
	int sum = 0;
	for (int i = 0; i < usdatalen; i++)
	{
		sum += puchmsg[i];
	}

	int checkCode1 = sum % 10;
	checkCode1 = checkCode1 + 48;

	int checkCode2 = sum / 10 % 10;
	checkCode2 = checkCode2 + 48;

	*one = checkCode1;
	*ten = checkCode2;
}

BOOL TCPClient::SendHeartBeat(DWORD dwtimeout)
{
	if (!m_TCPIPStutes)
	{
		return FALSE;
	}

	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "001A";
	m_MesMsg.MsgDeviceNo = DeviceNo;
	m_MesMsg.MsgBody = '1';
	m_MesMsg.MsgTrail = 0x03;

	char bufsend[256];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 46);

	memcpy(&bufsend[47], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[49], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 50, NULL) <= 0)
	{
		m_SocketClient = socket(AF_INET,SOCK_STREAM, 0);
		if (connect(m_SocketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
		{
			WSACleanup( );
			return FALSE;
		}
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return FALSE;
	}

	checkCode(m_MesMsg.MsgCheckcode, &bufrecv[1], 46);

	CStringA m_checkcode;

	m_checkcode = &bufrecv[47];
	m_checkcode.Delete(2, m_checkcode.GetLength() - 2);

	if (m_checkcode != m_MesMsg.MsgCheckcode)
	{
		return FALSE;
	}

	m_MesMsg.MsgBody = &bufrecv[46];
	m_MesMsg.MsgBody.Delete(0, m_MesMsg.MsgBody.GetLength() - 1);

	if (m_MesMsg.MsgBody == "0")
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

int TCPClient::SendLogin( CStringA username, CStringA userpwd, DWORD dwtimeout)
{
	if (!m_TCPIPStutes)
	{
		return 0x03;
	}

	MessageId ++ ;

	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "010A";
	m_MesMsg.MsgDeviceNo = DeviceNo;

	if (username.GetLength() < 10)
	{
		for (int i(0); i < 10 - username.GetLength(); i ++)
		{
			username.Insert(0, '0');
		}
	}

	if (userpwd.GetLength() < 16)
	{
		for (int i(0); i < 16 - userpwd.GetLength(); i ++)
		{
			userpwd.Insert(0, '0');
		}
	}

	m_MesMsg.MsgBody = username + userpwd;
	m_MesMsg.MsgTrail = 0x03;

	char bufsend[256];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 71);

	memcpy(&bufsend[72], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[74], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 75, NULL) <= 0)
	{
		return 0x03;
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return 0x03;
	}

	checkCode(m_MesMsg.MsgCheckcode, &bufrecv[1], 46);

	CStringA m_checkcode;

	m_checkcode = &bufrecv[47];
	m_checkcode.Delete(2, m_checkcode.GetLength() - 2);

	if (m_checkcode != m_MesMsg.MsgCheckcode)
	{
		return 0x03;
	}

	m_MesMsg.MsgBody = &bufrecv[46];
	m_MesMsg.MsgBody.Delete(0, m_MesMsg.MsgBody.GetLength() - 1);

	if (m_MesMsg.MsgBody = "0")
	{
		return 0x00;
	}
	else if (m_MesMsg.MsgBody = "1")
	{
		return 0x01;
	}
	else if (m_MesMsg.MsgBody = "2")
	{
		return 0x02;
	}

	return 0x03;
}

BOOL TCPClient::SendAlarm( int Alarmid, int status, SYSTEMTIME& locdtime, DWORD dwtimeout)
{
	if (!m_TCPIPStutes)
	{
		return FALSE;
	}

	MessageId ++ ;

	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "401A";
	m_MesMsg.MsgDeviceNo = DeviceNo;

	m_MesMsg.MsgBody = DeviceNo;
	m_MesMsg.MsgBody.AppendFormat("%04i%01i%04i-%02i-%02i %02i:%02i:%02i", Alarmid, status, locdtime.wYear
		,locdtime.wMonth, locdtime.wDay, locdtime.wHour, locdtime.wMinute, locdtime.wSecond);

	m_MesMsg.MsgTrail = 0x03;

	char bufsend[256];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 99);

	memcpy(&bufsend[100], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[102], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 103, NULL) <= 0)
	{
		return FALSE;
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return FALSE;
	}

	m_MesMsg.MsgBody = &bufrecv[46];
	m_MesMsg.MsgBody.Delete(1, m_MesMsg.MsgBody.GetLength() - 1);

	checkCode(m_MesMsg.MsgCheckcode, &bufrecv[1], 46);

	CStringA m_checkcode;
	m_checkcode = &bufrecv[47];
	m_checkcode.Delete(2, m_checkcode.GetLength() - 1);
	if (m_checkcode != m_MesMsg.MsgCheckcode)
	{
		return FALSE;
	}
	if (m_MesMsg.MsgBody == "0")
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL TCPClient::SendRunningStatus( int status, SYSTEMTIME& locdtime, DWORD dwtimeout )
{
	if (!m_TCPIPStutes)
	{
		return FALSE;
	}

	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "501A";
	m_MesMsg.MsgDeviceNo = DeviceNo;

	m_MesMsg.MsgBody = DeviceNo;
	m_MesMsg.MsgBody.AppendFormat("%01i%04i-%02i-%02i %02i:%02i:%02i", status, locdtime.wYear
		,locdtime.wMonth, locdtime.wDay, locdtime.wHour, locdtime.wMinute, locdtime.wSecond);

	m_MesMsg.MsgTrail = 0x03;

	char bufsend[256];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 95);

	memcpy(&bufsend[96], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[98], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 99, NULL) <= 0)
	{
		return FALSE;
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TCPClient::SendOperatingData(int vel, double avgvalue, int scannrate, SYSTEMTIME& locdtime, DWORD dwtimeout )
{
	if (!m_TCPIPStutes)
	{
		return FALSE;
	}

	MessageId ++ ;

	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "551A";
	m_MesMsg.MsgDeviceNo = DeviceNo;

	m_MesMsg.MsgBody = DeviceNo;
	m_MesMsg.MsgBody.AppendFormat("%010i", vel);

	if (avgvalue > 10000.0 ||
		avgvalue < - 10000.0)
	{
		return FALSE;
	}

	CStringA avgvaluestr;
	avgvaluestr.Format("%.2f", avgvalue);

	int avgvalenght;
	avgvalenght = avgvaluestr.GetLength();
	for (int i(0); i < 10 - avgvalenght; i ++)
	{
		avgvaluestr.Insert(0, "0");
	}

	m_MesMsg.MsgBody += avgvaluestr;

	m_MesMsg.MsgBody.AppendFormat("%05i%04i-%02i-%02i %02i:%02i:%02i",scannrate, locdtime.wYear, 
		locdtime.wMonth, locdtime.wDay, locdtime.wHour, locdtime.wMinute, locdtime.wSecond);

	m_MesMsg.MsgTrail = 0x03;

	char bufsend[256];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 119);

	memcpy(&bufsend[120], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[122], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 123, NULL) <= 0)
	{
		return FALSE;
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TCPClient::SendOperatingData(int vel, double avgvalue[80], int scannrate, SYSTEMTIME& locdtime, DWORD dwtimeout )
{
	if (!m_TCPIPStutes)
	{
		return FALSE;
	}

	MessageId ++ ;

	if (MessageId > 99999)
	{
		MessageId = 1;
	}

	MESMessage m_MesMsg;

	m_MesMsg.MsgHaed = 0x02;
	m_MesMsg.MsgDirection = '1';
	m_MesMsg.MsgAgreementNo = AgreementNo;
	m_MesMsg.MsgID.Format("%05i", MessageId);
	m_MesMsg.MsgResponse = '1';
	m_MesMsg.MsgCmd = "551A";
	m_MesMsg.MsgDeviceNo = DeviceNo;

	m_MesMsg.MsgBody = DeviceNo;
	m_MesMsg.MsgBody.AppendFormat("%010i", vel);

	for (int i = 0; i < vel; i++)
	{
		if (avgvalue[i] > 10000.0 ||
			avgvalue[i] < - 10000.0)
		{
			return FALSE;
		}

		CStringA avgvaluestr;
		avgvaluestr.Format("%.2f", avgvalue[i]);

		int avgvalenght;
		avgvalenght = avgvaluestr.GetLength();
		for (int i(0); i < 10 - avgvalenght; i ++)
		{
			avgvaluestr.Insert(0, "0");
		}

		m_MesMsg.MsgBody += avgvaluestr;
	}

	m_MesMsg.MsgBody.AppendFormat("%05i%04i-%02i-%02i %02i:%02i:%02i",scannrate, locdtime.wYear, 
		locdtime.wMonth, locdtime.wDay, locdtime.wHour, locdtime.wMinute, locdtime.wSecond);

	m_MesMsg.MsgTrail = 0x03;

	char bufsend[1024];

	memset(bufsend, 0, sizeof(bufsend));

	memcpy(&bufsend[0], &m_MesMsg.MsgHaed, 1);

	memcpy(&bufsend[1], &m_MesMsg.MsgDirection, sizeof(m_MesMsg.MsgDirection));

	memcpy(&bufsend[2], m_MesMsg.MsgAgreementNo, m_MesMsg.MsgAgreementNo.GetLength());

	memcpy(&bufsend[6], m_MesMsg.MsgID, m_MesMsg.MsgID.GetLength());

	memcpy(&bufsend[11], &m_MesMsg.MsgResponse, sizeof(m_MesMsg.MsgResponse));

	memcpy(&bufsend[12], m_MesMsg.MsgCmd, m_MesMsg.MsgCmd.GetLength());

	memcpy(&bufsend[16], m_MesMsg.MsgDeviceNo, m_MesMsg.MsgDeviceNo.GetLength());

	memcpy(&bufsend[46], m_MesMsg.MsgBody, m_MesMsg.MsgBody.GetLength());

	checkCode(m_MesMsg.MsgCheckcode, &bufsend[1], 119 + (vel-1)*10);

	memcpy(&bufsend[120 + (vel - 1) * 10], m_MesMsg.MsgCheckcode, m_MesMsg.MsgCheckcode.GetLength());

	memcpy(&bufsend[122 + (vel - 1) * 10], &m_MesMsg.MsgTrail, sizeof(m_MesMsg.MsgTrail));

	if (send(m_SocketClient, (char*)bufsend, 123 + (vel - 1) * 10, NULL) <= 0)
	{
		return FALSE;
	}

	char bufrecv[256];
	struct timeval timeout = {dwtimeout, 0 };

	int ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	ret = setsockopt(m_SocketClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	int recCount = recv(m_SocketClient, bufrecv, sizeof(bufrecv), 0);

	if (recCount <= 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TCPClient::SendUpkeep( char* locdtime, short usdatalen )
{
	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}
	char buf[256];
	memset(buf, 0, sizeof(buf));

	buf[0] = 0x02;

	char direction = '1';
	buf[1] = direction;

	char ENMultiByte[8];

	WideCharToMultiByte( CP_ACP, 0, AgreementNo, -1,
		ENMultiByte, 8, NULL, NULL);

	memcpy(&buf[2], ENMultiByte, 4);

	char checkCode1 = MessageId % 10;
	buf[6] = checkCode1 + 48;

	checkCode1 = MessageId / 10 % 10;
	buf[7] = checkCode1 + 48;

	checkCode1 = MessageId / 100 % 10;
	buf[8] = checkCode1 + 48;

	checkCode1 = MessageId / 1000 % 10;
	buf[9] = checkCode1 + 48;

	checkCode1 = MessageId / 10000 % 10;
	buf[10] = checkCode1 + 48;

	char Response = '1';
	buf[11] = Response;

	return TRUE;
}

BOOL TCPClient::SendMaintain( char* locdtime, short usdatalen )
{
	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}
	char buf[256];
	memset(buf, 0, sizeof(buf));

	buf[0] = 0x02;

	char direction = '1';
	buf[1] = direction;

	char ENMultiByte[8];

	WideCharToMultiByte( CP_ACP, 0, AgreementNo, -1,
		ENMultiByte, 8, NULL, NULL);

	memcpy(&buf[2], ENMultiByte, 4);

	char checkCode1 = MessageId % 10;
	buf[6] = checkCode1 + 48;

	checkCode1 = MessageId / 10 % 10;
	buf[7] = checkCode1 + 48;

	checkCode1 = MessageId / 100 % 10;
	buf[8] = checkCode1 + 48;

	checkCode1 = MessageId / 1000 % 10;
	buf[9] = checkCode1 + 48;

	checkCode1 = MessageId / 10000 % 10;
	buf[10] = checkCode1 + 48;

	char Response = '1';
	buf[11] = Response;

	return TRUE;
}

BOOL TCPClient::SendCraftParameter( char*puchmsg, short usdatalen )
{
	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}
	char buf[256];
	memset(buf, 0, sizeof(buf));

	buf[0] = 0x02;

	char direction = '1';
	buf[1] = direction;

	char ENMultiByte[8];

	WideCharToMultiByte( CP_ACP, 0, AgreementNo, -1,
		ENMultiByte, 8, NULL, NULL);

	memcpy(&buf[2], ENMultiByte, 4);

	char checkCode1 = MessageId % 10;
	buf[6] = checkCode1 + 48;

	checkCode1 = MessageId / 10 % 10;
	buf[7] = checkCode1 + 48;

	checkCode1 = MessageId / 100 % 10;
	buf[8] = checkCode1 + 48;

	checkCode1 = MessageId / 1000 % 10;
	buf[9] = checkCode1 + 48;

	checkCode1 = MessageId / 10000 % 10;
	buf[10] = checkCode1 + 48;

	char Response = '1';
	buf[11] = Response;

	char Command[5] = {"701A"};
	memcpy(&buf[12], Command, 4);

	char PCNoMultiByte[32];

	WideCharToMultiByte( CP_ACP, 0, DeviceNo, -1,
		PCNoMultiByte, 1024, NULL, NULL);

	memcpy(&buf[16], PCNoMultiByte, 30);

	memcpy(&buf[46], puchmsg, 30);

	checkCode(&buf[78], &buf[77], &buf[1], 76);

	buf[79] = 0x03;

	if (send(m_SocketClient, (char*)buf, 80, NULL))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TCPClient::SendProductiondata( char*puchmsg, short usdatalen )
{
	MessageId ++ ; 
	if (MessageId > 99999)
	{
		MessageId = 1;
	}
	char buf[256];
	memset(buf, 0, sizeof(buf));

	buf[0] = 0x02;

	char direction = '1';
	buf[1] = direction;

	char ENMultiByte[8];

	WideCharToMultiByte( CP_ACP, 0, AgreementNo, -1,
		ENMultiByte, 8, NULL, NULL);

	memcpy(&buf[2], ENMultiByte, 4);

	char checkCode1 = MessageId % 10;
	buf[6] = checkCode1 + 48;

	checkCode1 = MessageId / 10 % 10;
	buf[7] = checkCode1 + 48;

	checkCode1 = MessageId / 100 % 10;
	buf[8] = checkCode1 + 48;

	checkCode1 = MessageId / 1000 % 10;
	buf[9] = checkCode1 + 48;

	checkCode1 = MessageId / 10000 % 10;
	buf[10] = checkCode1 + 48;

	char Response = '1';
	buf[11] = Response;

	return TRUE;
}

BOOL TCPClient::ReceiveHeartBeat(char msg, size_t msglen)
{
	return TRUE;
}

BOOL TCPClient::ReceiveLogin(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveAlarm(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveRunningStatus(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveOperatingData(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveSendUpkeep(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveMaintain(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveCraftParameter(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::ReceiveProductiondata(char msg, size_t msglen)
{

	return TRUE;
}

BOOL TCPClient::Reconnect()
{
	CloseClient();
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) 
	{
		return FALSE;
	}


	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 )
	{
		WSACleanup( );
		return FALSE; 
	}

	m_SocketClient = socket(AF_INET,SOCK_STREAM, 0);

	if (connect(m_SocketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		WSACleanup( );
		return FALSE;
	}

	return TRUE;
}

BOOL TCPClient::GetErrorMessage()
{
	int ErrorCode = WSAGetLastError();
	
	switch(ErrorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:

		return 0;
		break;
	default:

		return 1;
		break;
	}
}

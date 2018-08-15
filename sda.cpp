// sda.cpp : 实现文件
//

#include "stdafx.h"
#include "Cwmsys.h"
#include "sda.h"


// sda

sda::sda()
{
}

sda::~sda()
{
}


// sda 成员函数

void sda::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnClose(nErrorCode);
}

void sda::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnAccept(nErrorCode);
}

void sda::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnConnect(nErrorCode);
}

void sda::OnOutOfBandData(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnOutOfBandData(nErrorCode);
}

void sda::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnReceive(nErrorCode);
}

void sda::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CSocket::OnSend(nErrorCode);
}

int sda::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}

int sda::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CSocket::Send(lpBuf, nBufLen, nFlags);
}

// sda.cpp : ʵ���ļ�
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


// sda ��Ա����

void sda::OnClose(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnClose(nErrorCode);
}

void sda::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnAccept(nErrorCode);
}

void sda::OnConnect(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnConnect(nErrorCode);
}

void sda::OnOutOfBandData(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnOutOfBandData(nErrorCode);
}

void sda::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnReceive(nErrorCode);
}

void sda::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CSocket::OnSend(nErrorCode);
}

int sda::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CSocket::Receive(lpBuf, nBufLen, nFlags);
}

int sda::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CSocket::Send(lpBuf, nBufLen, nFlags);
}

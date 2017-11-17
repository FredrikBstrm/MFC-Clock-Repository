#pragma once

// UdpNtpSocket command target

class UdpNtpSocket : public CAsyncSocket
{
private:
	bool m_ReadyToSendFirstPacket;
	bool m_HasReceivedFirstPacket;
	CDialog * m_pDlg;
public:
	UdpNtpSocket();
	virtual ~UdpNtpSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	bool SendNtpPacket();
	void SetParentDlg(CDialog * m_pDlg);
	bool GetHasReceivedFirstPacket();
};



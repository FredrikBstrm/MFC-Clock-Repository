// UdpNtpSocket.cpp : implementation file
//

#include "stdafx.h"
#include "CurrentTime.h"
#include "CurrentTimeDlg.h"
#include "UdpNtpSocket.h"

//NTP time server URL and port (the time servers are listening on port number 123 using UDP).
#define NTP_URL "time-a.nist.gov"
#define NTP_PORT 123

//NTP time-stamp of 1 Jan 1970.
#define NTP_TIMESTAMP_DELTA 2208988800ull

//Fields in the NTP packet that need to be set before sending the packet.
#define LEAP_INDICATOR 0
#define VERSION_NUMBER 3
#define MODE 3

//NTP packet.
typedef struct
{
	unsigned char li : 2;            // Only two bits. Leap indicator.
	unsigned char vn : 3;            // Only three bits. Version number of the protocol.
	unsigned char mode : 3;          // Only three bits. Mode. Client will pick mode 3 for client.

	unsigned __int8 stratum;         // Eight bits. Stratum level of the local clock.
	unsigned __int8 poll;            // Eight bits. Maximum interval between successive messages.
	unsigned __int8 precision;       // Eight bits. Precision of the local clock.

	unsigned __int32 rootDelay;      // 32 bits. Total round trip delay time.
	unsigned __int32 rootDispersion; // 32 bits. Max error aloud from primary clock source.
	unsigned __int32 refId;          // 32 bits. Reference clock identifier.

	unsigned __int32 refTm_s;        // 32 bits. Reference time-stamp seconds.
	unsigned __int32 refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

	unsigned __int32 origTm_s;       // 32 bits. Originate time-stamp seconds.
	unsigned __int32 origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

	unsigned __int32 rxTm_s;         // 32 bits. Received time-stamp seconds.
	unsigned __int32 rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

	unsigned __int32 txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
	unsigned __int32 txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;                        // Total: 384 bits or 48 bytes.

// UdpNtpSocket

UdpNtpSocket::UdpNtpSocket()
{
	m_ReadyToSendFirstPacket = false;
	m_HasReceivedFirstPacket = false;

	//Create the socket
	BOOL bRet = Create(NTP_PORT, SOCK_DGRAM, FD_WRITE | FD_READ, NULL);
	if (bRet != TRUE)
	{
		UINT uErr = GetLastError();
		TCHAR szError[256];
		wsprintf(szError, _T("UdpNtpSocket Create() failed: %d"), uErr);
		AfxMessageBox(szError);
	}
}

UdpNtpSocket::~UdpNtpSocket()
{
}


// UdpNtpSocket member functions

void UdpNtpSocket::SetParentDlg(CDialog *pDlg)
{
	m_pDlg = pDlg;
}

void UdpNtpSocket::OnSend(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	m_ReadyToSendFirstPacket = true;    // The socket is now ready to send.

	//Send the first NTP packet to the time server.
	SendNtpPacket();
	CAsyncSocket::OnSend(nErrorCode);
}

bool UdpNtpSocket::SendNtpPacket()
{
	if (!m_ReadyToSendFirstPacket)
	{
		return(false);
	}

	//Create and zero out the NTP packet.
	ntp_packet ntp_packet_to_send;
	memset(&ntp_packet_to_send, 0, sizeof(ntp_packet));

	//Set the first three fields (li, vn and mode).
	ntp_packet_to_send.li = LEAP_INDICATOR;
	ntp_packet_to_send.vn = VERSION_NUMBER;
	ntp_packet_to_send.mode = MODE;

	LPCTSTR ntpUrl = _T(NTP_URL);
	int bytesSent;

	//Send the NTP packet.
	if ((bytesSent = SendTo((const char*)&ntp_packet_to_send, sizeof(ntp_packet_to_send), NTP_PORT, ntpUrl)) == SOCKET_ERROR)
	{
		UINT uErr = GetLastError();
		if (uErr != WSAEWOULDBLOCK)
		{
			TCHAR szError[256];
			wsprintf(szError, _T("UdpNtpSocket failed to send: %d"), uErr);
			AfxMessageBox(szError);
		}
		return(false);
	}
	return(true);
}

void UdpNtpSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	//Create the NTP packet that will store the incoming data.
	ntp_packet received_ntp_packet;

	CString ntpUrl;
	UINT ntpPort;

	int bytesReceived;

	//Receive the NTP packet
	bytesReceived = ReceiveFrom((char*)&received_ntp_packet, sizeof(ntp_packet), ntpUrl, ntpPort);

	switch (bytesReceived)
	{
	case 0:  // Connection was closed.
		Close();
		break;
	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			AfxMessageBox(_T("Error occurred"));
			Close();
		}
		break;
	default: // Normal case: ReceiveFrom() returned the expected number of bytes (the size of the ntp packet).

		// These two fields contain the time-stamp seconds as the packet left the NTP server.
	    // The number of seconds correspond to the seconds passed since 1900.
		// ntohl() converts the bit/byte order from the network's to host's "endianness".

		received_ntp_packet.txTm_s = ntohl(received_ntp_packet.txTm_s); // Time-stamp seconds.
		received_ntp_packet.txTm_f = ntohl(received_ntp_packet.txTm_f); // Time-stamp fraction of a second.

		// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
		// Subtract 70 years worth of seconds from the seconds since 1900.
		// This leaves the seconds since the UNIX epoch of 1970.
		// (1900)------------------(1970)**************************************(Time Packet Left the Server).

		time_t txTm = (time_t)(received_ntp_packet.txTm_s - NTP_TIMESTAMP_DELTA);
		
		//Update the digital- and analog clocks.
		((CCurrentTimeDlg*)m_pDlg)->UpdateClocks(txTm);

		//Notify the dialog box that the first NTP packet has been received.
		m_HasReceivedFirstPacket = true;
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

bool UdpNtpSocket::GetHasReceivedFirstPacket()
{
	return m_HasReceivedFirstPacket;
}
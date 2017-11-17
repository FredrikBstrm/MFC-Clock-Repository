
// CurrentTimeDlg.h : header file
//

#pragma once

#include "UdpNtpSocket.h"
#include "AnalogClock.h"
#include "afxwin.h"

// CCurrentTimeDlg dialog
class CCurrentTimeDlg : public CDialogEx
{
// Construction
public:
	CCurrentTimeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CURRENTTIME_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	UdpNtpSocket m_UdpNtpSocket;

	CString m_NewYorkDigitalClock;
	CString m_StockholmDigitalClock;
	CString m_MelbourneDigitalClock;

	AnalogClock m_NewYorkAnalogClock;
	AnalogClock m_StockholmAnalogClock;
	AnalogClock m_MelbourneAnalogClock;

	time_t m_CurrentUtcTime;

	int m_SecondsSinceLatestQuery;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void UpdateClocks(time_t txTm);
};

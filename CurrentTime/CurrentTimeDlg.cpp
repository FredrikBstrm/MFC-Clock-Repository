
// CurrentTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CurrentTime.h"
#include "CurrentTimeDlg.h"
#include "afxdialogex.h"

//ID for timer.
#define ID_TIMER_CLOCK 1

//Delay for timer (milliseconds).
#define DELAY 1000

//Time between each query to the time server (seconds).
#define SECONDS_BETWEEN_EACH_QUERY 64

//Time difference compared to UTC (hours).
#define NEW_YORK (-5)
#define STOCKHOLM (+1)
#define MELBOURNE (+11)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCurrentTimeDlg dialog



CCurrentTimeDlg::CCurrentTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CURRENTTIME_DIALOG, pParent)
	, m_NewYorkDigitalClock(_T(""))
	, m_StockholmDigitalClock(_T(""))
	, m_MelbourneDigitalClock(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCurrentTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEW_YORK_DIGITAL_CLOCK, m_NewYorkDigitalClock);
	DDX_Text(pDX, IDC_STOCKHOLM_DIGITAL_CLOCK, m_StockholmDigitalClock);
	DDX_Text(pDX, IDC_MELBOURNE_DIGITAL_CLOCK, m_MelbourneDigitalClock);
	DDX_Control(pDX, IDC_NEW_YORK_ANALOG_CLOCK, m_NewYorkAnalogClock);
	DDX_Control(pDX, IDC_STOCKHOLM_ANALOG_CLOCK, m_StockholmAnalogClock);
	DDX_Control(pDX, IDC_MELBOURNE_ANALOG_CLOCK, m_MelbourneAnalogClock);
}

BEGIN_MESSAGE_MAP(CCurrentTimeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCurrentTimeDlg message handlers

BOOL CCurrentTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//Set this dialog as parent dialog
	m_UdpNtpSocket.SetParentDlg(this);

	//Reset seconds since latest query
	m_SecondsSinceLatestQuery = 0;

	//Set the color of the analog clocks
	COLORREF color;
	color = RGB(0, 0, 255); //Blue
	m_NewYorkAnalogClock.SetClockPointerColor(color);
	color = RGB(0, 255, 0); //Green
	m_StockholmAnalogClock.SetClockPointerColor(color);
	color = RGB(255, 0, 0); //Red
	m_MelbourneAnalogClock.SetClockPointerColor(color);

	//Start the timer
	SetTimer(ID_TIMER_CLOCK, DELAY, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCurrentTimeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCurrentTimeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCurrentTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
	//Check if the first NTP packet from the time server has been received
	if (m_UdpNtpSocket.GetHasReceivedFirstPacket())
	{
		if (m_SecondsSinceLatestQuery == SECONDS_BETWEEN_EACH_QUERY)
		{
			//Add one second to the current time, update the clocks and query the time server
			m_SecondsSinceLatestQuery = 0;
			UpdateClocks(m_CurrentUtcTime + 1);
			m_UdpNtpSocket.SendNtpPacket();
		}
		else
		{
			//Add one second to the current time and update the clocks
			m_SecondsSinceLatestQuery++;
			UpdateClocks(m_CurrentUtcTime + 1);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CCurrentTimeDlg::UpdateClocks(time_t txTm)
{
	//Convert the seconds since the UNIX epoch of 1970 to current hour, minute and second (UTC).
	tm ptm;
	gmtime_s(&ptm, &txTm);

	//Set the hour in New York, Stockholm and Melbourne.
	int iNewYorkHour = abs((ptm.tm_hour + NEW_YORK)) % 24;
	int iStockholmHour = abs((ptm.tm_hour + STOCKHOLM)) % 24;
	int iMelbourneHour = abs((ptm.tm_hour + MELBOURNE)) % 24;

	int iMinute = ptm.tm_min;
	int iSecond = ptm.tm_sec;

	//Update the analog clocks.
	m_NewYorkAnalogClock.UpdateAnalogClock(iNewYorkHour, iMinute, iSecond);
	m_StockholmAnalogClock.UpdateAnalogClock(iStockholmHour, iMinute, iSecond);
	m_MelbourneAnalogClock.UpdateAnalogClock(iMelbourneHour, iMinute, iSecond);

	CString strNewYorkHour, strStockholmHour, strMelbourneHour, strMinute, strSecond;

	//Convert time from integer mode to "digital clock" mode (8 -> 08, 3 -> 03 ....).
	if (iNewYorkHour < 10)
	{
		strNewYorkHour.Format(_T("0%d"), iNewYorkHour);
	}
	else
	{
		strNewYorkHour.Format(_T("%d"), iNewYorkHour);
	}

	if (iStockholmHour < 10)
	{
		strStockholmHour.Format(_T("0%d"), iStockholmHour);
	}
	else
	{
		strStockholmHour.Format(_T("%d"), iStockholmHour);
	}

	if (iMelbourneHour < 10)
	{
		strMelbourneHour.Format(_T("0%d"), iMelbourneHour);
	}
	else
	{
		strMelbourneHour.Format(_T("%d"), iMelbourneHour);
	}

	if (iMinute < 10)
	{
		strMinute.Format(_T("0%d"), iMinute);
	}
	else
	{
		strMinute.Format(_T("%d"), iMinute);
	}

	if (iSecond < 10)
	{
		strSecond.Format(_T("0%d"), iSecond);
	}
	else
	{
		strSecond.Format(_T("%d"), iSecond);
	}

	//Update the digital clocks
	m_NewYorkDigitalClock.Format(_T("%s:%s:%s"), strNewYorkHour, strMinute, strSecond);
	m_StockholmDigitalClock.Format(_T("%s:%s:%s"), strStockholmHour, strMinute, strSecond);
	m_MelbourneDigitalClock.Format(_T("%s:%s:%s"), strMelbourneHour, strMinute, strSecond);
	UpdateData(FALSE);

	//Update the current time (UTC)
	m_CurrentUtcTime = txTm;
}

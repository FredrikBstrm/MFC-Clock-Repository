// AnalogClock.h : header file
//
#pragma once

// AnalogClock window

class AnalogClock : public CStatic
{
// Construction
public:
	AnalogClock();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual COLORREF GetClockPointerColor();
	virtual void SetClockPointerColor(COLORREF color);
	virtual ~AnalogClock();
	void UpdateAnalogClock(int hour, int minute, int second);

protected:
	void DrawClockFace (CDC* pDC);
    void DrawClockPointer (CDC* pDC, int nLength, int nDegrees, COLORREF clrColor);

	int m_PreviousSecond;        
    int m_PreviousMinute;
    int m_PreviousHour;


	// Generated message map functions
protected:
	COLORREF m_ClockPointerColor;
	afx_msg void OnPaint();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

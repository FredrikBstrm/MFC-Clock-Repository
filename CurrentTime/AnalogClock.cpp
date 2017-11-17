// Clock.cpp : implementation file
//

#include "stdafx.h"
#include "AnalogClock.h"
#include <afxwin.h>
#include <math.h>

//Size of the circles that creates the clock face.
#define CIRCLE_SIZE 20

//Length of the clock pointers.
#define SECOND_POINTER_LENGTH 400
#define MINUTE_POINTER_LENGTH 300
#define HOUR_POINTER_LENGTH 200

// AnalogClock

AnalogClock::AnalogClock()
{
    m_PreviousSecond = 0;
    m_PreviousMinute = 0;
    m_PreviousHour = 0;

	m_ClockPointerColor = RGB(0, 0, 0);
}

AnalogClock::~AnalogClock()
{
}

BEGIN_MESSAGE_MAP(AnalogClock, CStatic)
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void AnalogClock::DrawClockFace (CDC* pDC)
{
	
	static CPoint point[12] = { CPoint (   0,  450),    // 12
								CPoint ( 225,  390),    //  1
								CPoint ( 390,  225),    //  2 
								CPoint ( 450,    0),    //  3 
								CPoint ( 390, -225),    //  4 
								CPoint ( 225, -390),    //  5 
								CPoint (   0, -450),    //  6 
								CPoint (-225, -390),    //  7
								CPoint (-390, -225),    //  8
								CPoint (-450,    0),    //  9
								CPoint (-390,  225),    // 10
								CPoint (-225,  390) };  // 11

	//Draw the clock face.
    pDC->SelectStockObject(BLACK_BRUSH);

    for (int i=0; i<12; i++)
        pDC->Ellipse(point[i].x - CIRCLE_SIZE, point[i].y + CIRCLE_SIZE, point[i].x + CIRCLE_SIZE, point[i].y - CIRCLE_SIZE);
}

void AnalogClock::DrawClockPointer (CDC* pDC, int length, int degrees, COLORREF color)
{
	CPoint point;

	//Convert degrees to radians.
    double radians = (double) degrees * 0.017453292;

	//Calculate the endpoint of the clock pointer (x and y coordinates).
    point.x = (int) (length * sin (radians));
    point.y = (int) (length * cos (radians));

	//Draw the clock pointer.
    CPen pen(PS_SOLID, 8, color);
    CPen * pOldPen = pDC->SelectObject(&pen);

    pDC->MoveTo(0, 0);
    pDC->LineTo(point);

    pDC->SelectObject (pOldPen);
}

void AnalogClock::OnPaint() 
{
	// Center the origin and switch to the MM_ISOTROPIC mapping mode.
	CRect rect;
    GetClientRect(&rect);

    CPaintDC dc(this);
    dc.SetMapMode(MM_ISOTROPIC);
    dc.SetWindowExt(1000, 1000);
    dc.SetViewportExt(rect.Width (), -rect.Height ());
    dc.SetViewportOrg(rect.Width () / 2, rect.Height () / 2);

	//Draw the clock face.
    DrawClockFace(&dc);

	//Draw the clock pointers.
    DrawClockPointer(&dc, HOUR_POINTER_LENGTH, (m_PreviousHour * 30) + (m_PreviousMinute / 2), m_ClockPointerColor);
    DrawClockPointer(&dc, MINUTE_POINTER_LENGTH, m_PreviousMinute * 6, m_ClockPointerColor);
    DrawClockPointer(&dc, SECOND_POINTER_LENGTH, m_PreviousSecond * 6, m_ClockPointerColor);
}

void AnalogClock::UpdateAnalogClock(int hour, int minute, int second)
{
	//Check if prev sec, min and hour corresponds to the current time. The function returns right away if that is the case.
	if ((second == m_PreviousSecond) && (minute == m_PreviousMinute) && (hour == m_PreviousHour))
	{
		return;
	}

	// Center the origin and switch to the MM_ISOTROPIC mapping mode.
	CRect rect;
	GetClientRect(&rect);

	CClientDC dc(this);
	dc.SetMapMode(MM_ISOTROPIC);
	dc.SetWindowExt(1000, 1000);
	dc.SetViewportExt(rect.Width(), -rect.Height());
	dc.SetViewportOrg(rect.Width() / 2, rect.Height() / 2);

	//Get transparent color to be able to erase current clock pointers, if needed.
	COLORREF eraseColor = ::GetSysColor(COLOR_3DFACE);

	//Convert hour from integer mode to "analog clock" mode (17 -> 5, 23 -> 11 ....).
	hour = hour % 12;

	//If hours have changed, erase the hour pointer.
	if (hour != m_PreviousHour)
	{
		DrawClockPointer(&dc, HOUR_POINTER_LENGTH, (m_PreviousHour * 30) + (m_PreviousMinute / 2), eraseColor);
		m_PreviousHour = hour;
	}

	//If minutes have changed, erase the minute- and hour pointer.
	if (minute != m_PreviousMinute)
	{
		DrawClockPointer(&dc, HOUR_POINTER_LENGTH, (m_PreviousHour * 30) + (m_PreviousMinute / 2), eraseColor);
		DrawClockPointer(&dc, MINUTE_POINTER_LENGTH, m_PreviousMinute * 6, eraseColor);
		m_PreviousMinute = minute;
		m_PreviousHour = hour;
	}

	// If seconds have changed, erase the second pointer and redraw all clock pointers.
	if (second != m_PreviousSecond)
	{
		DrawClockPointer(&dc, SECOND_POINTER_LENGTH, m_PreviousSecond * 6, eraseColor);
		DrawClockPointer(&dc, SECOND_POINTER_LENGTH, second * 6, m_ClockPointerColor);
		DrawClockPointer(&dc, MINUTE_POINTER_LENGTH, minute * 6, m_ClockPointerColor);
		DrawClockPointer(&dc, HOUR_POINTER_LENGTH, (hour * 30) + (minute / 2), m_ClockPointerColor);
		m_PreviousSecond = second;
	}
}

void AnalogClock::OnClose() 
{
	CStatic::OnClose();
}

void AnalogClock::SetClockPointerColor(COLORREF color)
{
	m_ClockPointerColor = color;
}

COLORREF AnalogClock::GetClockPointerColor()
{
	return m_ClockPointerColor;
}

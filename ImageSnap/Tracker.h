// Tracker.h: interface for the CTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACKER_H__F0AEBF35_5FA5_4E70_B267_4DAA5C0C5036__INCLUDED_)
#define AFX_TRACKER_H__F0AEBF35_5FA5_4E70_B267_4DAA5C0C5036__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTracker : public CRectTracker  
{
public:
	virtual void AdjustRect( int nHandle, LPRECT lpRect );
	CTracker();
	virtual ~CTracker();

};

#endif // !defined(AFX_TRACKER_H__F0AEBF35_5FA5_4E70_B267_4DAA5C0C5036__INCLUDED_)

// Tracker.cpp: implementation of the CTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracker::CTracker():CRectTracker()
{
	m_nHandleSize=5;
}

CTracker::~CTracker()
{

}

void CTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	//CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	//CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	//CMSCIPView *pView = (CMSCIPView *) pChild->GetActiveView();
	
	//限制ROI，宽度是4的倍数，高度是2的倍数。
	int m_nMinWidth=4*1;//ROI最小宽度
	int m_nMinHeight=4*1;//ROI最小高度
	switch(nHandle)
	{
	case hitLeft:
		{ 
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->left=lpRect->right-m_nMinWidth;
			if(((lpRect->right-lpRect->left)%4!=0))
              lpRect->left+=(lpRect->right-lpRect->left)%4;
		
			break;
		}
	case hitRight:
		{
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->right=lpRect->left+m_nMinWidth;
			if(((lpRect->right-lpRect->left)%4!=0))
			  lpRect->right-=(lpRect->right-lpRect->left)%4;
		
			break;
		}
    case hitTop:
		{ 
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->top=lpRect->bottom-m_nMinHeight;
			if(((lpRect->bottom-lpRect->top)%2!=0))
              lpRect->top+=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
	case hitBottom:
		{
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->bottom=lpRect->top+m_nMinHeight;
			if(((lpRect->bottom-lpRect->top)%2!=0))
			  lpRect->bottom-=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
	case hitTopLeft:
		{ 
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->left=lpRect->right-m_nMinWidth;
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->top=lpRect->bottom-m_nMinHeight;
			if(((lpRect->right-lpRect->left)%4!=0))
              lpRect->left+=(lpRect->right-lpRect->left)%4;
			if(((lpRect->bottom-lpRect->top)%2!=0))
              lpRect->top+=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
	case hitTopRight:
		{
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->top=lpRect->bottom-m_nMinHeight;
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->right=lpRect->left+m_nMinWidth;
			if(((lpRect->right-lpRect->left)%4!=0))
			  lpRect->right-=(lpRect->right-lpRect->left)%4;
			if(((lpRect->bottom-lpRect->top)%2!=0))
              lpRect->top+=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
    case hitBottomRight:
		{ 
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->bottom=lpRect->top+m_nMinHeight;
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->right=lpRect->left+m_nMinWidth;
			if(((lpRect->right-lpRect->left)%4!=0))
			  lpRect->right-=(lpRect->right-lpRect->left)%4;
			if(((lpRect->bottom-lpRect->top)%2!=0))
              lpRect->bottom-=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
	case hitBottomLeft:
		{
			if(((lpRect->bottom-lpRect->top)<m_nMinHeight))
				lpRect->bottom=lpRect->top+m_nMinHeight;
			if(((lpRect->right-lpRect->left)<m_nMinWidth))
				lpRect->left=lpRect->right-m_nMinWidth;
			if(((lpRect->right-lpRect->left)%4!=0))
			  lpRect->left+=(lpRect->right-lpRect->left)%4;
			if(((lpRect->bottom-lpRect->top)%2!=0))
			  lpRect->bottom-=(lpRect->bottom-lpRect->top)%2;
			
			break;
		}
		
	default:
		break;
	}
	
	
	
}

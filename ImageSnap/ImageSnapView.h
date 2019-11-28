
// ImageSnapView.h : CImageSnapView ��Ľӿ�
//


#pragma once
#include "opencv2/opencv.hpp"
#include "DoubleBufferSys.h"
#include <mutex>

using namespace std;
using namespace cv;

#include "tracker.h"
#include "DlgSetSelect.h"

#include "stdafx.h"
#include "Lable.h"

class CImageSnapView : public CScrollView
{
protected: // �������л�����
	CImageSnapView();
	DECLARE_DYNCREATE(CImageSnapView)

//����
public:
	
	IplImage* m_pWorkImg;   //ͼ��
	Point now;
	int     m_dibFlag;      //λͼ�仯��־
	int     m_ImageType;    
	float   m_ZoomFactor;   //���ű���
	BOOL	m_AbleToLeftBtn = FALSE;//��������Ч
	BOOL	m_AbleToMove = FALSE;//����ƶ�����Ч
	BOOL    m_AbleDelete = FALSE;//ɾ���۵�����
	BOOL    m_AbleToSet = FALSE;//����������������
	BOOL    m_AbleToChange = FALSE;//�������߶�������
	BOOL    m_rect = TRUE;//���þ��α�עģʽ
	BOOL    m_curve = FALSE;//�������߱�עģʽ
	BOOL    m_polygon = FALSE;//���ö���α�עģʽ
	BOOL    m_ablerect = FALSE;
	LPBITMAPINFO m_lpBmi;
	CTracker m_tracker;
	CRect    m_rectTracker; // ������Ƥ�������߼�����
	CScrollBar m_wndScrollBar;
	vector<CurvePoint>cp; //�������ߵ�λ
	vector<Point>pp; //�������ε�λ
	DoubleBufferSys dbbufSys; //˫�������
	struct PolygonPoint
	{
		int dist;
		int polygonindex;
		int pointindex;
	};
	vector <_Object> m_ObjList;
	vector<_Curve> m_Curve;
	vector<_Polygon> m_Polygon;

	CDC* m_pDC;
protected:
	IplImage *	m_pSrcImg;
	HACCEL m_hAccel;


//����
public:
	void InitData();


// ����
public:
	CImageSnapDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CImageSnapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	int m_ObjorPart;//��ע�����

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	/*void DrawLine(CDC* pDC,BOOL isRubber, CPoint ptStart, CPoint ptEnd);
	void DrawPixel(CPoint pt);*/
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetSelect();
	afx_msg void OnPriorImage();
	afx_msg void OnNextImage();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnZoom();
	afx_msg void OnZoomout();
	afx_msg void OnZoomin();
	afx_msg void OnSetObj();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateRect(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePolygon(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCurve(CCmdUI *pCmdUI);
	afx_msg void OnRect();
	afx_msg void OnPolygon();
	afx_msg void OnCurve();
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCreateImgLabel();
	afx_msg void OnDeleteImgLabel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	std::mutex m_Mutex_move;
	afx_msg void OnDeletALL();
	afx_msg void OnDeletAllObject();
	afx_msg void OnFileSave();
};

#ifndef _DEBUG  // ImageSnapView.cpp �еĵ��԰汾
inline CImageSnapDoc* CImageSnapView::GetDocument() const
   { return reinterpret_cast<CImageSnapDoc*>(m_pDocument); }
#endif


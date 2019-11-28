
// ImageSnapView.h : CImageSnapView 类的接口
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
protected: // 仅从序列化创建
	CImageSnapView();
	DECLARE_DYNCREATE(CImageSnapView)

//变量
public:
	
	IplImage* m_pWorkImg;   //图像
	Point now;
	int     m_dibFlag;      //位图变化标志
	int     m_ImageType;    
	float   m_ZoomFactor;   //缩放比例
	BOOL	m_AbleToLeftBtn = FALSE;//鼠标左键有效
	BOOL	m_AbleToMove = FALSE;//鼠标移动点有效
	BOOL    m_AbleDelete = FALSE;//删除折点切线
	BOOL    m_AbleToSet = FALSE;//运行设置曲线属性
	BOOL    m_AbleToChange = FALSE;//更改曲线二级属性
	BOOL    m_rect = TRUE;//设置矩形标注模式
	BOOL    m_curve = FALSE;//设置曲线标注模式
	BOOL    m_polygon = FALSE;//设置多边形标注模式
	BOOL    m_ablerect = FALSE;
	LPBITMAPINFO m_lpBmi;
	CTracker m_tracker;
	CRect    m_rectTracker; // 保存橡皮条矩形逻辑坐标
	CScrollBar m_wndScrollBar;
	vector<CurvePoint>cp; //储存曲线点位
	vector<Point>pp; //储存多边形点位
	DoubleBufferSys dbbufSys; //双缓存对象
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


//函数
public:
	void InitData();


// 属性
public:
	CImageSnapDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CImageSnapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	int m_ObjorPart;//标注的类别

// 生成的消息映射函数
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

#ifndef _DEBUG  // ImageSnapView.cpp 中的调试版本
inline CImageSnapDoc* CImageSnapView::GetDocument() const
   { return reinterpret_cast<CImageSnapDoc*>(m_pDocument); }
#endif


// ImageScrollView.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageSnap.h"
#include "ImageScrollView.h"


// ImageScrollView

IMPLEMENT_DYNCREATE(ImageScrollView, CScrollView)

ImageScrollView::ImageScrollView()
{

}

ImageScrollView::~ImageScrollView()
{
}


BEGIN_MESSAGE_MAP(ImageScrollView, CScrollView)
END_MESSAGE_MAP()


// ImageScrollView 绘图

void ImageScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void ImageScrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
}


// ImageScrollView 诊断

#ifdef _DEBUG
void ImageScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void ImageScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// ImageScrollView 消息处理程序

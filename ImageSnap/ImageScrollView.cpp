// ImageScrollView.cpp : ʵ���ļ�
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


// ImageScrollView ��ͼ

void ImageScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void ImageScrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  �ڴ���ӻ��ƴ���
}


// ImageScrollView ���

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


// ImageScrollView ��Ϣ�������

#pragma once



// ImageScrollView 视图

class ImageScrollView : public CScrollView
{
	DECLARE_DYNCREATE(ImageScrollView)

protected:
	ImageScrollView();           // 动态创建所使用的受保护的构造函数
	virtual ~ImageScrollView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
};



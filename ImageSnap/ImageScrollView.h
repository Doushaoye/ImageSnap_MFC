#pragma once



// ImageScrollView ��ͼ

class ImageScrollView : public CScrollView
{
	DECLARE_DYNCREATE(ImageScrollView)

protected:
	ImageScrollView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~ImageScrollView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	virtual void OnInitialUpdate();     // �����ĵ�һ��

	DECLARE_MESSAGE_MAP()
};



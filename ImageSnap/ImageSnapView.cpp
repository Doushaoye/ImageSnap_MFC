// ImageSnapView.cpp : CImageSnapView 类的实现
//
#include "stdafx.h"
#include "ImageSnap.h"
#include "MainFrm.h"
#include "PropertiesWnd.h"
#include "ImageSnapDoc.h"
#include "ImageSnapView.h"
#include "memdc.h"
#include "Processing.h"
#include "DlgZoom.h"
#include <fstream>
#include "putText.h"
#include "Setproperty.h"

#define ROUND(a) long(a+0.5)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
// CImageSnapView

IMPLEMENT_DYNCREATE(CImageSnapView, CScrollView)

BEGIN_MESSAGE_MAP(CImageSnapView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_IMAGE_PRIOR, &CImageSnapView::OnPriorImage)
	ON_COMMAND(ID_IMAGE_NEXT, &CImageSnapView::OnNextImage)
	ON_COMMAND(ID_SET_SELECT, &CImageSnapView::OnSetSelect)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_ZOOM, &CImageSnapView::OnZoom)
	ON_COMMAND(IDM_ZOOMOUT, &CImageSnapView::OnZoomout)
	ON_COMMAND(IDM_ZOOMIN, &CImageSnapView::OnZoomin)
	ON_COMMAND(ID_SET_OBJ, &CImageSnapView::OnSetObj)
	ON_WM_LBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_Rect, &CImageSnapView::OnUpdateRect)
	ON_UPDATE_COMMAND_UI(ID_Polygon, &CImageSnapView::OnUpdatePolygon)
	ON_UPDATE_COMMAND_UI(ID_Curve, &CImageSnapView::OnUpdateCurve)
	ON_COMMAND(ID_Rect, &CImageSnapView::OnRect)
	ON_COMMAND(ID_Polygon, &CImageSnapView::OnPolygon)
	ON_COMMAND(ID_Curve, &CImageSnapView::OnCurve)
//	ON_WM_KEYDOWN()
ON_COMMAND(ID_32803, &CImageSnapView::OnCreateImgLabel)
ON_COMMAND(ID_32804, &CImageSnapView::OnDeleteImgLabel)
ON_WM_KEYDOWN()
ON_WM_CHAR()
ON_WM_VSCROLL()
ON_WM_HSCROLL()
//ON_WM_MOUSEHWHEEL()
ON_WM_MOUSEWHEEL()
ON_COMMAND(ID_32807, &CImageSnapView::OnDeletAllObject)
ON_COMMAND(ID_FILE_SAVE, &CImageSnapView::OnFileSave)
END_MESSAGE_MAP()

int curve_i = -1;
int polygon_i = -1;
int rect_i = -1;
int point_i = -1;
int rect_i_lock = -1;
int polygon_i_lock = -1;
int curve_i_lock = -1;
int point_i_lock = -1;

template <typename T>
//重载+号定义vector相加
vector<T> &operator +(vector<T> &v1, vector<T> &v2){
	v1.insert(v1.end(), v2.begin(), v2.end());
	return v1;
}
// CImageSnapView 构造/析构

void bezier(cv::Mat &dst,cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4, int rate)
{
	printf("绘制曲线开始\n");
	std::vector<cv::Point> v_bezier;
	unsigned int i = 0;
	for (double t = 0; t < 1; t = t + 1.0 / rate,i++)
	{
		cv::Point p;
		if (i == 0)
		{
			p = p1;
		}
		else
		{
			p.x = p1.x*pow((1.0 - t), 3) + 3 * p2.x*t*pow((1.0 - t), 2) + 3 * p3.x*pow(t, 2)*(1.0 - t) + p4.x*pow(t, 3);
			p.y = p1.y*pow((1.0 - t), 3) + 3 * p2.y*t*pow((1.0 - t), 2) + 3 * p3.y*pow(t, 2)*(1.0 - t) + p4.y*pow(t, 3);
		}

		v_bezier.push_back(p);

	}
	v_bezier.push_back(p4);

	polylines(dst, v_bezier, FALSE, Scalar(0, 69, 255), 1);
	printf("绘制曲线结束\n");
	return;
}


int getDist(cv::Point2f p,cv::Point p2)
{
	return (p.x - p2.x)*(p.x - p2.x) + (p.y - p2.y)*(p.y - p2.y);
}



CImageSnapView::CImageSnapView()
{
	// TODO: 在此处添加构造代码
	m_pWorkImg    = NULL;
	m_pSrcImg     = NULL;
	m_lpBmi = 0;
	m_istip = FALSE;
	m_hAccel=::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOREPORT));
	m_ZoomFactor = 1.0;
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);
	m_tooltip.SetTipTextColor(RGB(0, 0, 255));//font color
	m_tooltip.SetDelayTime(50);//delay time
	InitData();
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

CImageSnapView::~CImageSnapView()
{
	InitData();
}

BOOL CImageSnapView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}
// CImageSnapView 绘制
void B3_curves(cv::Mat image, std::vector<cv::Point>&xq)//绘制三次B样条曲线
{
	std::vector<cv::Point> p;
	unsigned int i, rate = 10000;//rate是平滑程度
	long lx, ly;
	double F03, F13, F23, F33;
	lx = ROUND((xq[0].x + 4.0*xq[1].x + xq[2].x) / 6.0);//t＝0的起点x坐标
	ly = ROUND((xq[0].y + 4.0*xq[1].y + xq[2].y) / 6.0);//t＝0的起点y坐标
	p.push_back(cv::Point(lx, ly));
	for (i = 1; i<xq.size() - 2; i++)//6段样条曲线
	{
		for (double t = 0; t <= 1; t += 1.0 / rate)
		{
			F03 = (-t*t*t + 3 * t*t - 3 * t + 1) / 6;//计算F0,3(t)
			F13 = (3 * t*t*t - 6 * t*t + 4) / 6;//计算F1,3(t)
			F23 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6;//计算F2,3(t)
			F33 = t*t*t / 6;//计算B3,3(t)
			lx = ROUND(xq[i - 1].x * F03 + xq[i].x * F13 + xq[i + 1].x * F23 + xq[i + 2].x * F33);
			ly = ROUND(xq[i - 1].y * F03 + xq[i].y * F13 + xq[i + 1].y * F23 + xq[i + 2].y * F33);
			p.push_back(cv::Point(lx, ly));
		}
	}

	cv::polylines(image, p, FALSE, cv::Scalar(0, 255, 255), 1, 8, 0);
}
void CImageSnapView::OnDraw(CDC* pDC)
{
	printf("OnDraw开始\n");
	CImageSnapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		printf("OnDraw获取pDoc失败\n");
		return;
	}
	//CRect rect;
	//GetClientRect(&rect);
	//CDC CMemDC; //用于缓冲作图的内存DC
	//CMemDC.CreateCompatibleDC(pDC);
	//CDC* MemDC = &CMemDC;
	////
	//CBitmap bmp;                                                 //内存中承载临时图象的位图

	//          //依附窗口DC创建兼容内存DC

	//bmp.CreateCompatibleBitmap(MemDC, rect.Width(), rect.Height());//创建兼容位图

	//CMemDC.SelectObject(&bmp);                          //将位图选择进内存DC

	//MemDC.FillSolidRect(rect, RGB(255,255,255));//按原来背景填充客户区，不然会是黑色
	// TODO: 在此处为本机数据添加绘制代码
	CMemoryDC MemDC(pDC);
	//  有磁盘输入图像
	if (pDoc->m_pWorkImg!=NULL)	
	{	     
		//  尚未显示
		if (pDoc->m_Display==0) 
		{                
			InitData();
			IplImage *ImgTmp = cvCloneImage(pDoc->m_pWorkImg);
			cvFlip(ImgTmp);
			m_dibFlag=::ImageClone(ImgTmp,&m_pWorkImg);    //  复制到工作位图
			cvReleaseImage(&ImgTmp);

			m_pSrcImg = cvCloneImage(m_pWorkImg);

			m_ImageType=::ImageType(m_pWorkImg);
			pDoc->m_Display=1;
			
		}
	}

	if (m_dibFlag) 
	{                        //  DIB 结构改变
		if (m_lpBmi)
			free(m_lpBmi);
		m_lpBmi=CtreateMapInfo(m_pWorkImg,m_dibFlag);
		m_dibFlag=0;

		CSize  sizeTotal;
		sizeTotal = CSize(m_pWorkImg->width*m_ZoomFactor, m_pWorkImg->height*m_ZoomFactor);
		SetScrollSizes(MM_TEXT,sizeTotal);  //  设置滚动条
		Invalidate();
	}

	static cv::Scalar colors[] = 
	{
		{{0,0,255}},
		{{0,128,255}},
		{{0,255,255}},
		{{0,255,0}},
		{{255,128,0}},
		{{255,255,0}},
		{{255,0,0}},
		{{255,0,255}}
	};


	char *pBits;
	if (m_pWorkImg)
	{
		cvCopy(m_pSrcImg, m_pWorkImg);
		//cvCopyImage(m_pSrcImg,m_pWorkImg);
		cvFlip(m_pWorkImg);

		if (pDoc->m_Image.enable)
		{
			Mat m_pWorkImg_m = m_pWorkImg;
			for (unsigned int i = 0;i< pDoc->m_Image.o_property.size(); i++)
			{
				string s = pDoc->m_Image.o_property[i].property_name + ": " + pDoc->m_Image.o_property[i].property_options;
				//putTextZH(m_pWorkImg_m, s.c_str(), cvPoint(20,20+i*20),CV_RGB(255,25,25), 15, "微软雅黑");
			}
		}
		for (unsigned int i = 0;i<pDoc->m_ObjList.size();i++)
		{
			CvScalar color = colors[i % 8];
			if (FALSE == pDoc->m_ObjList[i].enable)
			{
				continue;
			}
			
			cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), cvPoint(pDoc->m_ObjList[i].objbox.right, pDoc->m_ObjList[i].objbox.bottom), color, ceil(1.0 / m_ZoomFactor));
			Mat m_pWorkImg_m = m_pWorkImg;
			int a = pDoc->m_ObjList[i].iobjclass;
			char is[10];
			string s = pDoc->lines_zh[a].second;
			_itoa_s(i+1, is, 10);
			string objecti = is;
			s = objecti +","+ s;
			int s_size = s.size();
			int fontsize = 16;
			//cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), cvPoint(pDoc->m_ObjList[i].objbox.left + s_size * fontsize/2, pDoc->m_ObjList[i].objbox.top + fontsize + 5), colors[i % 8], -1);
			if (m_pWorkImg == NULL)
			{
				ReleaseDC(MemDC);
				printf("draw m_pWorkImg NULL\n");
				return;
			}
			if ((pDoc->m_ObjList[i].objbox.right - pDoc->m_ObjList[i].objbox.left) < 50)
			{
				//putText(m_pWorkImg_m, objecti, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top + 10), 1, 1, color,2);
				putTextZH(m_pWorkImg_m, objecti.c_str(), cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), color, fontsize, "微软雅黑");

			}
			else if (pDoc->m_ObjList[i].classname == "lights")
			{
				putText(m_pWorkImg_m, objecti, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top + 10), 1, 1, color,2);
			}
			else
			{
				//putText(m_pWorkImg_m, objecti, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top + 10), 1, 1, color,2);
				putTextZH(m_pWorkImg_m, s.c_str(), cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), color, fontsize, "微软雅黑");
			}
			
			char strinfo[16] = {0};

			for (unsigned int j=0;j<pDoc->m_ObjList[i].partlist.size();j++)
			{
				if (FALSE == pDoc->m_ObjList[i].partlist[j].enable)
				{
					continue;
				}
				if (pDoc->m_ObjList[i].partlist[j].ablerect == TRUE)
				{
					cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].partlist[j].partrect.left, pDoc->m_ObjList[i].partlist[j].partrect.top), cvPoint(pDoc->m_ObjList[i].partlist[j].partrect.right, pDoc->m_ObjList[i].partlist[j].partrect.bottom), color, ceil(1.0 / m_ZoomFactor));
					sprintf_s(strinfo, "%d", j + 1);
					string ps = strinfo;
					int p_size = ps.size();
					int p_fontsize = 8;
					//cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].partlist[j].partrect.left, pDoc->m_ObjList[i].partlist[j].partrect.top), cvPoint(pDoc->m_ObjList[i].partlist[j].partrect.left + p_size * p_fontsize / 1.5, pDoc->m_ObjList[i].partlist[j].partrect.top + p_fontsize + 5), colors[i % 8], -1);

					putText(m_pWorkImg_m, strinfo, cvPoint(pDoc->m_ObjList[i].partlist[j].partrect.left, pDoc->m_ObjList[i].partlist[j].partrect.top), 1, 1, color);
				}
				else
				{
					for (unsigned int k = 0; k < pDoc->m_ObjList[i].partlist[j].polygon.size(); k++)
					{
						if (k != pDoc->m_ObjList[i].partlist[j].polygon.size() - 1)
						{
							cvLine(m_pWorkImg, pDoc->m_ObjList[i].partlist[j].polygon[k], pDoc->m_ObjList[i].partlist[j].polygon[k + 1], color, ceil(1.0 / m_ZoomFactor));
						}
						else
						{
							cvLine(m_pWorkImg, pDoc->m_ObjList[i].partlist[j].polygon[k], pDoc->m_ObjList[i].partlist[j].polygon[0], color, ceil(1.0 / m_ZoomFactor));
						}
					}
					sprintf_s(strinfo, "Part_%d_%d", i + 1, j + 1);
					string ps = strinfo;
					int p_size = ps.size();
					int p_fontsize = 8;
					//cvRectangle(m_pWorkImg, pDoc->m_ObjList[i].partlist[j].polygon[0], cvPoint(pDoc->m_ObjList[i].partlist[j].polygon[0].x + p_size * p_fontsize / 1.5, pDoc->m_ObjList[i].partlist[j].polygon[0].y + p_fontsize + 5), colors[i % 8], -1);
					//putTextZH(m_pWorkImg_m, strinfo, pDoc->m_ObjList[i].partlist[j].polygon[0], color, p_fontsize, "微软雅黑");
					putText(m_pWorkImg_m, strinfo, pDoc->m_ObjList[i].partlist[j].polygon[0], 1, 1, color);
				}
			}
			
		}
		for (unsigned int i = 0; i <pp.size(); i++)
		{
			cvCircle(m_pWorkImg, pp[i], 3, CV_RGB(255, 0, 0), -1);
			if (i<pp.size()-1)
				cvLine(m_pWorkImg, pp[i], pp[i+1], CV_RGB(0, 255, 0), 1);
		}
		if (pp.size()>0)
		{
			cvLine(m_pWorkImg, pp.back(),Point(now.x,now.y), CV_RGB(0, 255, 0), 1);
		}
		for (unsigned int i = 0; i <cp.size(); i++)
		{
			cvCircle(m_pWorkImg, cp[i].c1, 2, CV_RGB(0, 255, 0), -1);
			cvCircle(m_pWorkImg, cp[i].c2, 2, CV_RGB(0, 255, 0), -1);
			cvCircle(m_pWorkImg, cp[i].p, 3, CV_RGB(255, 0, 0), -1);
			cvLine(m_pWorkImg, cp[i].p, cp[i].c1, CV_RGB(0, 255, 0),1);
			cvLine(m_pWorkImg, cp[i].p, cp[i].c2, CV_RGB(0, 255, 0), 1);
		}
		for (unsigned int i = 1; i < cp.size(); i++)
		{
			Mat m_pWorkImg_m = m_pWorkImg;
			bezier(m_pWorkImg_m, cp[i - 1].p, cp[i - 1].c2, cp[i].c1, cp[i].p, 500);
		}
		//if (cp.size() > 1)
		//{
		//	Mat m_pWorkImg_m = m_pWorkImg;
		//	//bezier(m_pWorkImg_m, cp[cp.size() - 2].p, cp[cp.size() - 2].c2, cp[cp.size() - 1].c1, cp[cp.size() - 1].p,500);
		//	//cvLine(m_pWorkImg, cp[cp.size() - 1].p, cp[cp.size() - 2].p, CV_RGB(255, 0, 0));
		//	//DrawLine(pDC, 0, CPoint(cp.back().p.x, cp.back().p.y), CPoint(now.x, now.y));
		//}
		for (unsigned int i = 0; i < pDoc->m_Curve.size(); i++)
		{
			if (FALSE == pDoc->m_Curve[i].enable)
			{
				continue;
			}

			Mat m_pWorkImg_m = m_pWorkImg;
			
			for (unsigned int j = 0; j < pDoc->m_Curve[i].objcurve.size()-1; j++)
			{
				bezier(m_pWorkImg_m, pDoc->m_Curve[i].objcurve[j].p, pDoc->m_Curve[i].objcurve[j].c2, pDoc->m_Curve[i].objcurve[j+1].c1, pDoc->m_Curve[i].objcurve[j+1].p, 500);

			}
				//cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), cvPoint(pDoc->m_ObjList[i].objbox.right, pDoc->m_ObjList[i].objbox.bottom), colors[i % 8], ceil(3.0 / m_ZoomFactor));
			string property_s;
			if (pDoc->m_Curve[i].o_property.size() == 2)
			{
				property_s = pDoc->m_Curve[i].o_property[1].property_options;
			}
			
			int a = pDoc->m_Curve[i].iobjclass;
			char is[10];
			string s = pDoc->lines_zh[a].second;
			_itoa_s(i + 1 + pDoc->m_ObjList.size() + pDoc->m_Polygon.size(), is, 10);
			string objecti = is;
			s = objecti + "," + s + property_s;
			//int s_size = s.size();
			//colors[i % 8];
			int fontsize = 12;
			//cvRectangle(m_pWorkImg, pDoc->m_Polygon[i].objpolygon[0], cvPoint(pDoc->m_Polygon[i].objpolygon[0].x + s_size * fontsize / 1.5, pDoc->m_Polygon[i].objpolygon[0].y + fontsize + 5), colors[i % 8], -1);
			putTextZH(m_pWorkImg_m, s.c_str(), Point(pDoc->m_Curve[i].objcurve[1].p.x, pDoc->m_Curve[i].objcurve[1].p.y - 10), Scalar(82, 255, 99), fontsize, "微软雅黑");
			
		}
		for (unsigned int i = 0; i < pDoc->m_Polygon.size(); i++)
		{
			CvScalar color = colors[i + pDoc->m_ObjList.size() % 8];
			if (FALSE == pDoc->m_Polygon[i].enable)
			{
				continue;
			}
			for (unsigned int j = 0; j < pDoc->m_Polygon[i].objpolygon.size()-1; j++)
			{
				cvLine(m_pWorkImg, pDoc->m_Polygon[i].objpolygon[j], pDoc->m_Polygon[i].objpolygon[j + 1], color, ceil(1.0 / m_ZoomFactor));
				/*if (j != pDoc->m_Polygon[i].objpolygon.size())
				{
					
				}
				else
				{
					cvLine(m_pWorkImg, pDoc->m_Polygon[i].objpolygon[j], pDoc->m_Polygon[i].objpolygon[0], color, ceil(1.0 / m_ZoomFactor));
				}*/
			}
			//cvRectangle(m_pWorkImg, cvPoint(pDoc->m_ObjList[i].objbox.left, pDoc->m_ObjList[i].objbox.top), cvPoint(pDoc->m_ObjList[i].objbox.right, pDoc->m_ObjList[i].objbox.bottom), colors[i % 8], ceil(3.0 / m_ZoomFactor));
			cv::Mat m_pWorkImg_m = m_pWorkImg;
			int a = pDoc->m_Polygon[i].iobjclass;
			char is[10];
			string s = pDoc->lines_zh[a].second;
			_itoa_s(i + 1 + pDoc->m_ObjList.size(), is, 10);
			string objecti = is;
			s = objecti + "," + s;
			int s_size = s.size();
			int fontsize = 16;
			//cvRectangle(m_pWorkImg, pDoc->m_Polygon[i].objpolygon[0], cvPoint(pDoc->m_Polygon[i].objpolygon[0].x + s_size * fontsize / 1.5, pDoc->m_Polygon[i].objpolygon[0].y + fontsize + 5), colors[i % 8], -1);
			putTextZH(m_pWorkImg_m, s.c_str(), pDoc->m_Polygon[i].objpolygon[0], color, fontsize, "微软雅黑");
			char strinfo[16] = { 0 };
		

			for (unsigned int j = 0; j<pDoc->m_Polygon[i].partlist.size(); j++)
			{
				if (FALSE == pDoc->m_Polygon[i].partlist[j].enable)
				{
					continue;
				}
				if (pDoc->m_Polygon[i].partlist[j].ablerect == TRUE)
				{
					cvRectangle(m_pWorkImg, cvPoint(pDoc->m_Polygon[i].partlist[j].partrect.left, pDoc->m_Polygon[i].partlist[j].partrect.top), cvPoint(pDoc->m_Polygon[i].partlist[j].partrect.right, pDoc->m_Polygon[i].partlist[j].partrect.bottom), color, ceil(1.0 / m_ZoomFactor));
					sprintf_s(strinfo, "Part_%d_%d", i + 1 + pDoc->m_ObjList.size(), j + 1);
					string ps = strinfo;
					int p_size = ps.size();
					int p_fontsize = 8;
					//cvRectangle(m_pWorkImg, cvPoint(pDoc->m_Polygon[i].partlist[j].partrect.left, pDoc->m_Polygon[i].partlist[j].partrect.top), cvPoint(pDoc->m_Polygon[i].partlist[j].partrect.left + p_size * p_fontsize / 1.5, pDoc->m_Polygon[i].partlist[j].partrect.top + p_fontsize + 5), colors[i % 8], -1);
					putTextZH(m_pWorkImg_m, strinfo, cvPoint(pDoc->m_Polygon[i].partlist[j].partrect.left + p_size / 2, pDoc->m_Polygon[i].partlist[j].partrect.top), color, p_fontsize, "微软雅黑");
				}
				else
				{
					for (unsigned int k = 0; k < pDoc->m_Polygon[i].partlist[j].polygon.size(); k++)
					{
						if (k != pDoc->m_Polygon[i].partlist[j].polygon.size() - 1)
						{
							cvLine(m_pWorkImg, pDoc->m_Polygon[i].partlist[j].polygon[k], pDoc->m_Polygon[i].partlist[j].polygon[k + 1], color, ceil(1.0 / m_ZoomFactor));
						}
						else
						{
							cvLine(m_pWorkImg, pDoc->m_Polygon[i].partlist[j].polygon[k], pDoc->m_Polygon[i].partlist[j].polygon[0], color, ceil(1.0 / m_ZoomFactor));
						}
					}
					sprintf_s(strinfo, "Part_%d_%d", i + 1 + pDoc->m_ObjList.size(), j + 1);
					string ps = strinfo;
					int p_size = ps.size();
					int p_fontsize = 8;
					//cvRectangle(m_pWorkImg, pDoc->m_Polygon[i].partlist[j].polygon[0], cvPoint(pDoc->m_Polygon[i].partlist[j].polygon[0].x + p_size * p_fontsize / 2, pDoc->m_Polygon[i].partlist[j].polygon[0].y + p_fontsize + 5), colors[i % 8], -1);
					putTextZH(m_pWorkImg_m, strinfo, pDoc->m_Polygon[i].partlist[j].polygon[0], color, p_fontsize, "微软雅黑");
				}
			}
		}
		/*for (unsigned int i = 0; i < pDoc->m_Curve.size(); i++)
		{
			Mat m_pWorkImg_m = m_pWorkImg;
			B3_curves(m_pWorkImg_m, pDoc->m_Curve[i].objcurve);
		}*/

		/*IplImage2Bmp(dcMem.GetSafeHdc(),m_pWorkImg,&bmp);
		dcMem.SelectObject(&bmp);*/
		//pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台


		//::SetStretchBltMode(dbbufSys.GetMemDC(), COLORONCOLOR);
		cvFlip(m_pWorkImg);
		pBits=m_pWorkImg->imageData;
		int iOldMode = SetStretchBltMode(MemDC->GetSafeHdc(), COLORONCOLOR);
		//  刷新窗口画面
		CPoint pos(GetScrollPosition());
		StretchDIBits(MemDC->m_hDC,
			(int)(0 - pos.x), (int)(0 - pos.y), (int)m_pWorkImg->width*m_ZoomFactor, (int)m_pWorkImg->height*m_ZoomFactor,
			0,0,m_pWorkImg->width,m_pWorkImg->height,
			pBits,m_lpBmi,DIB_RGB_COLORS,SRCCOPY);

		SetStretchBltMode(MemDC->GetSafeHdc(), iOldMode);

		//pDC->BitBlt(0, 0, (int)m_pWorkImg->width*m_ZoomFactor, (int)m_pWorkImg->height*m_ZoomFactor, &MemDC, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
		m_tracker.m_rect.left = m_rectTracker.left*m_ZoomFactor - pos.x;
		m_tracker.m_rect.right = m_rectTracker.right*m_ZoomFactor - pos.x;
		m_tracker.m_rect.top = m_rectTracker.top*m_ZoomFactor - pos.y;
		m_tracker.m_rect.bottom = m_rectTracker.bottom*m_ZoomFactor - pos.y;
		MemDC->LPtoDP(m_tracker.m_rect); // tracker wants device coordinates
		if (m_tracker.m_rect.Width() > 0 && m_tracker.m_rect.Height() > 0)
		{
			m_tracker.Draw(MemDC);

		}
	}
	ReleaseDC(MemDC);
	printf("OnDraw完成\n");
	return;

}


// CImageSnapView 打印


void CImageSnapView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CImageSnapView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImageSnapView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CImageSnapView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CImageSnapView::OnRButtonUp(UINT nFlags, CPoint point)
{
	printf("OnRButtonUp()\n");
	ClientToScreen(&point);
	OnContextMenu(this, point);
	printf("OnRButtonUp完成\n");
}

void CImageSnapView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CImageSnapView 诊断

#ifdef _DEBUG
void CImageSnapView::AssertValid() const
{
	CView::AssertValid();
}

void CImageSnapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageSnapDoc* CImageSnapView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageSnapDoc)));
	return (CImageSnapDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageSnapView 消息处理程序

void CImageSnapView::InitData()
{
	printf("ViewInit\n");
	
	//freopen("CONOUT$","w+t",stdout);
	std::vector<std::pair<int, std::string> > lines_;
	std::ifstream infile3("21cls_English.txt");
	int nId;
	int iId = 0;
	string clsname;
	while (getline(infile3, clsname))//(infile >> nId >> clsname)
	{
		//std::string GB_str;
		//GB_str = UTF8ToGB(clsname.c_str());
		int space_pos = clsname.find_first_of(' ');
		std::string temp_name = clsname.substr(space_pos + 1);
		lines_.push_back(std::make_pair(iId, temp_name));
		iId++;

	}
	infile3.close();

	for (int i = 0; i < iId; i++)
	{
		strcpy(objclass[i],lines_[i].second.c_str());
		//printf("%s\n", lines_[i].second.c_str());
	}


	if (m_lpBmi)
	{
		free(m_lpBmi);
		m_lpBmi = 0;
	}

	if (NULL != m_pWorkImg)
	{
		cvReleaseImage(&m_pWorkImg);
		m_pWorkImg = 0;
	}

	if (NULL != m_pSrcImg)
	{
		cvReleaseImage(&m_pSrcImg);
		m_pSrcImg = 0;
	}

	m_dibFlag  = 0;
	m_ImageType= 0;
	m_ObjorPart = 0;
	m_AbleToLeftBtn = FALSE;//鼠标左键有效
	m_AbleToMove = FALSE;//鼠标移动点有效
	m_AbleDelete = FALSE;
	//m_rect = TRUE;//设置矩形标注模式
	//m_curve = FALSE;//设置曲线标注模式
	//m_polygon = FALSE;//设置多边形标注模式
	//m_ZoomFactor =1.0;

	m_rectTracker = m_tracker.m_rect = CRect(0,0,0,0);
	printf("ViewInit完成\n");
}

BOOL LessSort(CImageSnapView::PolygonPoint a, CImageSnapView::PolygonPoint b)
{ return (a.dist<b.dist); }
void CImageSnapView::OnMouseMove(UINT nFlags, CPoint point)
{
	//lock_guard<mutex> lock(m_Mutex_move);
	m_Mutex_move.lock();
	printf("Move_in\n");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	vector<int>rDist;
	vector<PolygonPoint>pDist;
	rDist.clear();
	pDist.clear();
	//vector<int>::iterator min;
	CPoint pos(GetScrollPosition());
	CImageSnapDoc* pDoc = GetDocument();
	if (NULL!=m_pWorkImg)
	{
		CRect ImageRect(0, 0, (int)m_pWorkImg->width*m_ZoomFactor, (int)m_pWorkImg->height*m_ZoomFactor);
		POINT pt = {point.x+pos.x,point.y+pos.y};
		cv::Point2f ptZOOM(pt.x*1.0 / m_ZoomFactor, pt.y*1.0 / m_ZoomFactor);
		now = Point(ptZOOM.x, ptZOOM.y);
		if (m_rect)
		{
			if (!m_AbleToMove)
			{
				if (pDoc->m_ObjList.size() > 0)
				{
					printf("m_ObjListsize\n");
					for (unsigned int i = 0; i < pDoc->m_ObjList.size(); i++)
					{
						if (pDoc->m_ObjList[i].enable!=FALSE)
						{
							int l1 = (int)(ptZOOM.x - pDoc->m_ObjList[i].objbox.left)*(ptZOOM.x - pDoc->m_ObjList[i].objbox.left) + (ptZOOM.y - pDoc->m_ObjList[i].objbox.top)*(ptZOOM.y - pDoc->m_ObjList[i].objbox.top);
							int l2 = (int)(ptZOOM.x - pDoc->m_ObjList[i].objbox.right)*(ptZOOM.x - pDoc->m_ObjList[i].objbox.right) + (ptZOOM.y - pDoc->m_ObjList[i].objbox.top)*(ptZOOM.y - pDoc->m_ObjList[i].objbox.top);
							int l3 = (int)(ptZOOM.x - pDoc->m_ObjList[i].objbox.left)*(ptZOOM.x - pDoc->m_ObjList[i].objbox.left) + (ptZOOM.y - pDoc->m_ObjList[i].objbox.bottom)*(ptZOOM.y - pDoc->m_ObjList[i].objbox.bottom);
							int l4 = (int)(ptZOOM.x - pDoc->m_ObjList[i].objbox.right)*(ptZOOM.x - pDoc->m_ObjList[i].objbox.right) + (ptZOOM.y - pDoc->m_ObjList[i].objbox.bottom)*(ptZOOM.y - pDoc->m_ObjList[i].objbox.bottom);
							rDist.push_back((l1));
							rDist.push_back((l2));
							rDist.push_back((l3));
							rDist.push_back((l4));
						}
						else
						{
							rDist.push_back((501));
							rDist.push_back((501));
							rDist.push_back((501));
							rDist.push_back((501));
						}
					}
					printf("m_ObjListsizeout\n");
					auto min = min_element(begin(rDist), end(rDist));
					int min_pos = distance(begin(rDist), min);
					point_i = min_pos % 4;
					rect_i = min_pos / 4;
					if ((*min) < 25)
					{
						m_istip = TRUE;
						m_str;
						m_str.Format("宽=%d, 高=%d", pDoc->m_ObjList[rect_i].objbox.Width(), pDoc->m_ObjList[rect_i].objbox.Height());
						m_AbleToLeftBtn = TRUE;
						SetCursor(LoadCursor(NULL, IDC_SIZEALL));//改变为十字箭头光标
					}
					else
					{
						point_i = 0;
						rect_i = 0;
						m_AbleToLeftBtn = FALSE;
						if (!m_ablerect)
						SetCursor(LoadCursor(NULL, IDC_ARROW));
					}
				}
			}
			else if (m_AbleToMove)
			{
				printf("switch\n");
				switch (point_i_lock)
				{
				case 0:
				{
					pDoc->m_ObjList[rect_i_lock].objbox.left = ptZOOM.x;
					pDoc->m_ObjList[rect_i_lock].objbox.top = ptZOOM.y;
					break;
				}
				case 1:
				{
					pDoc->m_ObjList[rect_i_lock].objbox.right = ptZOOM.x;
					pDoc->m_ObjList[rect_i_lock].objbox.top = ptZOOM.y;
					break;
				}
				case 2:
				{
					pDoc->m_ObjList[rect_i_lock].objbox.left = ptZOOM.x;
					pDoc->m_ObjList[rect_i_lock].objbox.bottom = ptZOOM.y;
					break;
				}
				case 3:
				{
					pDoc->m_ObjList[rect_i_lock].objbox.right = ptZOOM.x;
					pDoc->m_ObjList[rect_i_lock].objbox.bottom = ptZOOM.y;
					break;
				}
				default:
					break;
				}
				printf("switchout\n");
				m_pDC = GetDC();
				OnDraw(m_pDC);
				ReleaseDC(m_pDC);
			}
			
			
		}
		if (m_polygon)
		{
			if (!m_AbleToMove)
			{
				if (pp.size() > 0)
				{
					if (!m_AbleToMove && pp.size()>2)
					{
						int dist = getDist(now, pp.front());
						if (dist < 25)
						{
							m_AbleToLeftBtn = TRUE;
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));//改变为十字箭头光标
						}
						else
						{
							m_AbleToLeftBtn = FALSE;
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}

					}
				}
				else
				{
					if (pDoc->m_Polygon.size() > 0)
					{
						PolygonPoint ppindex;
						vector<cv::Point>allcorner;
						for (unsigned int i = 0; i < pDoc->m_Polygon.size(); i++)
						{
							if (pDoc->m_Polygon[i].enable != FALSE)
							{
								for (unsigned int j = 0; j < pDoc->m_Polygon[i].objpolygon.size(); j++)
								{
									ppindex.dist = getDist(ptZOOM, pDoc->m_Polygon[i].objpolygon[j]);
									ppindex.pointindex = j;
									ppindex.polygonindex = i;
									pDist.push_back(ppindex);
								}
							}
							else
							{
								for (unsigned int j = 0; j < pDoc->m_Polygon[i].objpolygon.size(); j++)
								{
									ppindex.dist = 501;
									ppindex.pointindex = j;
									ppindex.polygonindex = i;
									pDist.push_back(ppindex);
								}
							}
						}
						sort(pDist.begin(), pDist.end(), LessSort);
						point_i = pDist.front().pointindex;
						polygon_i = pDist.front().polygonindex;
						if (pDist.front().dist < 25)
						{
							m_AbleToLeftBtn = TRUE;
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));//改变为十字箭头光标
						}
						else
						{
							point_i = -1;
							rect_i = -1;
							m_AbleToLeftBtn = FALSE;
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
					}
					
				}
			
			}
			else
			{
				pDoc->m_Polygon[polygon_i_lock].objpolygon[point_i_lock].x = ptZOOM.x;
				pDoc->m_Polygon[polygon_i_lock].objpolygon[point_i_lock].y = ptZOOM.y;
				m_pDC = GetDC();
				OnDraw(m_pDC);
				ReleaseDC(m_pDC);
			}
			//OnDraw(GetDC());

		}
		if (m_curve)
		{
			if (cp.size() != 0)
			{
				if ((!m_AbleToMove) && (cp.size()>0))
				{
					int dist_back = getDist(now, cp.back().p);
					int dist = getDist(now, cp.front().p);
					if (dist < 10)
					{
						m_AbleToLeftBtn = TRUE;
						m_AbleDelete = FALSE;
						SetCursor(LoadCursor(NULL, IDC_SIZEALL));//改变为十字箭头光标
					}
					else if (dist_back < 10)
					{
						m_AbleToLeftBtn = TRUE;
						m_AbleDelete = TRUE;
						SetCursor(LoadCursor(NULL, IDC_HELP));//改变为?箭头光标
					}
					else
					{
						point_i = 0;
						m_AbleToLeftBtn = FALSE;
						m_AbleDelete = FALSE;
						SetCursor(LoadCursor(NULL, IDC_ARROW));
					}

				}
				else if ((cp.size()>0) && m_AbleToMove)
				{
					//m_AbleToLeftBtn = FALSE;
					m_AbleDelete = FALSE;
					cp.back().c2 = now;
					cp.back().c1 = Point(cp.back().p.x * 2 - now.x, cp.back().p.y * 2 - now.y);
					m_pDC = GetDC();
					OnDraw(m_pDC);
					ReleaseDC(m_pDC);
				}
			}
			else
			{
				for ( int i = 0; i < pDoc->m_Curve.size(); i++)
				{
					if (!pDoc->m_Curve[i].enable)
					{
						continue;
					}
					for (int j = 0; j < pDoc->m_Curve[i].objcurve.size(); j++)
					{
						int dist = getDist(now, pDoc->m_Curve[i].objcurve[j].p);
						if (dist < 20)
						{
							m_AbleToChange = TRUE;
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));//改变为十字箭头光标
							curve_i = i;
							i = pDoc->m_Curve.size();
							break;
						}
						else
						{
							m_AbleToChange = FALSE;
						}
					}
				}
			}
			
			
		}
		printf("Move_out\n");
		/*char   PositionString[60];
		CMainFrame*   MFrame = (CMainFrame*)AfxGetMainWnd();
		sprintf_s(PositionString, "X:%d,Y:%d", pt.x, pt.y);
		MFrame->m_wndStatusBar.SetPaneText(0, PositionString);*/
	}
	else
	{
		printf("m_pWorkImg NULL!\n");
	}
	/*rDist.swap(rDist);
	pDist.swap(pDist);*/
	m_Mutex_move.unlock();
	//Sleep(10);
	CScrollView::OnMouseMove(nFlags, point);
}

void CImageSnapView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	printf("OnLButtonDown()\n");
	if (NULL != m_pWorkImg)
	{
		CPoint pos(GetScrollPosition());
		CImageSnapDoc* pDoc = GetDocument();
		CRect ImageRect(0, 0, (int)m_pWorkImg->width*m_ZoomFactor, (int)m_pWorkImg->height*m_ZoomFactor);
		POINT pt = {point.x+pos.x,point.y+pos.y};
		if (m_rect)
		{
			if (m_AbleToLeftBtn)
			{
				point_i_lock = point_i;
				rect_i_lock = rect_i;
				m_AbleToMove = TRUE;
				if (nFlags&MK_CONTROL)
				{
					if (rect_i_lock < 0)
					{
						m_AbleToMove = FALSE;
						return;
					}
					pDoc->m_ObjList[rect_i_lock].enable = FALSE;
					m_AbleToMove = FALSE;
					pDoc->SetModifiedFlag(TRUE);
				}
				else if (nFlags&MK_SHIFT)
				{
					if (rect_i_lock < 0)
					{
						m_AbleToMove = FALSE;
						return;
					}
					m_AbleToMove = FALSE;
					CSetproperty pSet;
					CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
					pSet.m_classname = pDoc->m_ObjList[rect_i_lock].classname;
					if (pSet.DoModal() == IDOK)
					{
						vector<ss_property> p_property;
						for (unsigned int i = 0; i < pSet.m_property.size(); i++)
						{
							ss_property imageSet;
							imageSet.property_name = pSet.m_property[i].m_property;
							imageSet.property_options = pSet.m_property[i].m_ablexuanxiang;
							imageSet.property_id = pSet.m_property[i].m_propertyid;
							p_property.push_back(imageSet);
						}
						pDoc->m_ObjList[rect_i_lock].o_property = p_property;
					}
					pFrame->m_wndProperties.UpdataAllData(pDoc->m_ObjList, pDoc->m_ObjList.size() - 1, pDoc->m_ObjList[rect_i_lock].partlist.size() - 1);
					pDoc->SetModifiedFlag(TRUE);
				}
				
			}
			else
			{
				if (ImageRect.PtInRect(pt))
				{
					m_ablerect = TRUE;
					m_tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeOutside;
					if (m_tracker.HitTest(point) != CRectTracker::hitNothing)
					{
						m_tracker.Track(this, point, TRUE);
					}
					else
					{
						m_tracker.TrackRubberBand(this, point);
					}
					m_tracker.m_rect.NormalizeRect();

					m_rectTracker.left = (m_tracker.m_rect.left + pos.x)*1.0 / m_ZoomFactor;
					m_rectTracker.right = (m_tracker.m_rect.right + pos.x)*1.0 / m_ZoomFactor;
					m_rectTracker.top = (m_tracker.m_rect.top + pos.y)*1.0 / m_ZoomFactor;
					m_rectTracker.bottom = (m_tracker.m_rect.bottom + pos.y)*1.0 / m_ZoomFactor;
					char   PositionString[60];
					CMainFrame*   MFrame = (CMainFrame*)AfxGetMainWnd();
					sprintf_s(PositionString, "长:%d,宽:%d", m_rectTracker.right - m_rectTracker.left, m_rectTracker.bottom - m_rectTracker.top);
					MFrame->m_wndStatusBar.SetPaneText(0, PositionString);

				}

			}
			m_pDC = GetDC();
			OnDraw(m_pDC);
			ReleaseDC(m_pDC);
			//pDoc->UpdateAllViews(NULL);
		}
		else if (m_polygon)
		{
			if (m_AbleToLeftBtn)
			{
				if (pp.size() > 2)
				{
					pp.push_back(pp.front());
					m_AbleToLeftBtn = FALSE;
					OnSetSelect();
				}
				else
				{
					point_i_lock = point_i;
					polygon_i_lock = polygon_i;
					m_AbleToMove = TRUE;
					if (nFlags&MK_CONTROL)
					{
						if (polygon_i_lock < 0)
						{
							m_AbleToMove = FALSE;
							return;
						}
							
						pDoc->m_Polygon[polygon_i_lock].enable = FALSE;
						m_AbleToMove = FALSE;
						pDoc->SetModifiedFlag();
					}
					else if (nFlags&MK_SHIFT)
					{
						if (polygon_i_lock < 0)
						{
							m_AbleToMove = FALSE;
							return;
						}
						m_AbleToMove = FALSE;
						CSetproperty pSet;
						CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
						pSet.m_classname = pDoc->m_Polygon[polygon_i_lock].classname;
						if (pSet.DoModal() == IDOK)
						{
							vector<ss_property> p_property;
							for (unsigned int i = 0; i < pSet.m_property.size(); i++)
							{
								ss_property imageSet;
								imageSet.property_name = pSet.m_property[i].m_property;
								imageSet.property_options = pSet.m_property[i].m_ablexuanxiang;
								imageSet.property_id = pSet.m_property[i].m_propertyid;
								p_property.push_back(imageSet);
							}
							pDoc->m_Polygon[polygon_i_lock].o_property = p_property;
						}
						pFrame->m_wndProperties.UpdataAllData(pDoc->m_Polygon, pDoc->m_Polygon.size() - 1, pDoc->m_Polygon[polygon_i_lock].partlist.size() - 1);
						pDoc->SetModifiedFlag(TRUE);
					}
				}
				
			}
			else
			{
				pp.push_back(Point((int)(pt.x / m_ZoomFactor), (int)(pt.y / m_ZoomFactor)));
				//DrawPixel(point);
				
			}
			m_pDC = GetDC();
			OnDraw(m_pDC);
			ReleaseDC(m_pDC);
		}
		else if (m_curve)
		{
			if (m_AbleToLeftBtn)
			{
				
				if (m_AbleDelete)
				{
					
					if (cp.size() > 0)
					{
						if (cp.back().c2 != cp.back().p)
						{
							cp.back().c2 = cp.back().p;

						}
						else
						{
							cp.back().c1 = cp.back().p;
						}
					}
					m_AbleDelete = FALSE;
					m_AbleToLeftBtn = FALSE;
				}
				else
				{
					if (cp.size() > 2)
					{
						cp.push_back(cp.front());
						m_AbleToSet = TRUE;
						m_AbleToMove = TRUE;
					}
					m_AbleToLeftBtn = FALSE;

					//m_AbleToLeftBtn = TRUE;
					//OnSetSelect();
				}
				m_pDC = GetDC();
				OnDraw(m_pDC);
				ReleaseDC(m_pDC);
			}
			else
			{
				if (!m_AbleToChange)
				{

					m_AbleToMove = TRUE;
					CurvePoint p;
					p.p = Point((int)(pt.x / m_ZoomFactor), (int)(pt.y / m_ZoomFactor));
					p.c1 = Point((int)(pt.x / m_ZoomFactor), (int)(pt.y / m_ZoomFactor));
					p.c2 = Point((int)(pt.x / m_ZoomFactor), (int)(pt.y / m_ZoomFactor));
					cp.push_back(p);
					//DrawPixel(point);
					m_pDC = GetDC();
					OnDraw(m_pDC);
					ReleaseDC(m_pDC);
				}

			}
			if (m_AbleToChange)
			{
				curve_i_lock = curve_i;
				if (nFlags&MK_CONTROL)
				{
					if (curve_i_lock < 0)
					{
						return;
					}
					pDoc->m_Curve[curve_i_lock].enable = FALSE;
					pDoc->SetModifiedFlag();
				}
				else if (nFlags&MK_SHIFT)
				{
					if (curve_i_lock < 0)
						return;
					CSetproperty pSet;
					CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
					pSet.m_classname = pDoc->m_Curve[curve_i_lock].classname;
					if (pSet.DoModal() == IDOK)
					{
						vector<ss_property> c_property;
						for (unsigned int i = 0; i < pSet.m_property.size(); i++)
						{
							ss_property imageSet;
							imageSet.property_name = pSet.m_property[i].m_property;
							imageSet.property_options = pSet.m_property[i].m_ablexuanxiang;
							imageSet.property_id = pSet.m_property[i].m_propertyid;
							c_property.push_back(imageSet);
						}
						pDoc->m_Curve[curve_i_lock].o_property = c_property;
					}
					pFrame->m_wndProperties.UpdataAllData(pDoc->m_Curve, pDoc->m_Curve.size() - 1, -1);
					pDoc->SetModifiedFlag(TRUE);
					m_AbleToChange = !m_AbleToChange;
				}
			
				m_pDC = GetDC();
				OnDraw(m_pDC);
				ReleaseDC(m_pDC);
				
			}
		}
		
	}
	
	printf("OnLButtonDown完成\n");
	CScrollView::OnLButtonDown(nFlags, point);
}



BOOL CImageSnapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_tracker.SetCursor(pWnd, nHitTest))
	{
		return TRUE;
	}

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

void CImageSnapView::OnSetSelect()
{
	printf("OnSetSelect开始\n");
	// TODO: 在此添加命令处理程序代码
	m_ablerect = FALSE;
	CImageSnapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		printf("OnSetSelect获取pDoc失败\n");
		return;
	}
		
	
	if (m_rect)
		pDoc->SetSelect(m_ObjorPart);
	else if (m_polygon)
		pDoc->SetSelectPoly(m_ObjorPart);
	else if (m_curve)
	{
		if (cp.size() < 2)
		{
			cp.clear();
			m_pDC = GetDC();
			OnDraw(m_pDC);
			ReleaseDC(m_pDC);
		}
		else
		{
			pDoc->SetSelectCurve(m_ObjorPart);
		}
		
	}
		
	m_ObjorPart = 0;

	printf("OnSetSelect完成\n");
	return;
}

void CImageSnapView::OnPriorImage()
{
	m_Mutex_move.lock();
	// TODO: 在此添加命令处理程序代码
	printf("OnPriorImage()\n");
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_inherit = FALSE;
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		printf("pDoc错误\n");
		return;
	}
		

	if (pDoc->m_nPos >0 && pDoc->m_nPos < pDoc->m_nPicNum)
	{
		printf("pre成功\n");
		ischange=TRUE;
		pDoc->m_nPos--;
		pDoc->OpenFile();
	}
	else
	{
		AfxMessageBox("到头了");
	}
	printf("OnPriorImage完成\n");
	m_Mutex_move.unlock();
	return;
	
}

void CImageSnapView::OnNextImage()
{

	m_Mutex_move.lock();
	// TODO: 在此添加命令处理程序代码
	printf("OnNextImage()\n");
	CImageSnapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		printf("pDoc错误\n");
		return;
	}
		

	if (pDoc->m_nPos >=0 && pDoc->m_nPos < pDoc->m_nPicNum -1)
	{
		printf("next成功\n");
		ischange=TRUE;
		pDoc->m_nPos++;
		pDoc->OpenFile();
	}
	else
	{
		AfxMessageBox("到头了");
	}
	printf("OnNextImage完成\n");

	m_Mutex_move.unlock();
	return;

}


BOOL CImageSnapView::PreTranslateMessage(MSG* pMsg)
{
	 //TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN )
	{
		UINT nKey = (int)pMsg->wParam;

		//if (VK_SPACE ==nKey)
		{
			if(m_hAccel!=NULL) 
				if(::TranslateAccelerator(m_hWnd,m_hAccel,pMsg)) 
					return TRUE;
		}
	}
	if (m_istip)
	{
		CPoint pt;
		CRect clsRect;
		pt = pMsg->pt;
		m_tooltip.RelayEvent(pMsg);

		this->ScreenToClient(&pt);

		m_tooltip.AddTool(this, m_str);
		m_istip = FALSE;
	}

	return CScrollView::PreTranslateMessage(pMsg);


	//return CView::PreTranslateMessage(pMsg);
}


void CImageSnapView::OnRButtonDown(UINT nFlags, CPoint point)
{
	printf("OnRButtonDown()\n");
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_rect)
	{
		OnSetSelect();
	}
	else if (m_polygon)
	{
		//if (pp.size() < 3)
		//{
		//	pp.clear();
		//	Invalidate();
		//}
		//else
		//{
		//	OnSetSelect();
		//}
		//
	}
	else if (m_curve)
	{
			OnSetSelect();
	}
	printf("OnRButtonDown完成\n");
	CScrollView::OnRButtonDown(nFlags, point);
}

void CImageSnapView::OnZoom()
{
	// TODO: 在此添加命令处理程序代码
	//CDlgZoom dlg;
	//if (IDOK ==dlg.DoModal())
	//{
	//	//dlg.UpdateData(FALSE);
	//	m_ZoomFactor = dlg.m_zoomfactor;
	//}
	//Invalidate();
	//return;

	m_ZoomFactor=1.0;
	Invalidate();
	
}


void CImageSnapView::OnZoomout()
{
	// TODO: 在此添加命令处理程序代码
	
	if (m_ZoomFactor>=16) return;
	if		(m_ZoomFactor == 0.50f) m_ZoomFactor = 0.75f;
	else if (m_ZoomFactor == 0.75f) m_ZoomFactor = 1.00f;
	else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 1.50f;
	else if (m_ZoomFactor == 1.50f)	m_ZoomFactor = 2.00f;
	else							m_ZoomFactor*=2;
	CSize  sizeTotal;
	sizeTotal = CSize(m_pWorkImg->width*m_ZoomFactor, m_pWorkImg->height*m_ZoomFactor);
	SetScrollSizes(MM_TEXT, sizeTotal);  //  设置滚动条
	Invalidate();
}


void CImageSnapView::OnZoomin()
{
	// TODO: 在此添加命令处理程序代码
	if (m_ZoomFactor<=0.0625) return;
	if		(m_ZoomFactor == 2.00f) m_ZoomFactor = 1.50f;
	else if (m_ZoomFactor == 1.50f) m_ZoomFactor = 1.00f;
	else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 0.75f;
	else if (m_ZoomFactor == 0.75f)	m_ZoomFactor = 0.50f;
	else							m_ZoomFactor/=2;
	CSize  sizeTotal;
	sizeTotal = CSize(m_pWorkImg->width*m_ZoomFactor, m_pWorkImg->height*m_ZoomFactor);
	SetScrollSizes(MM_TEXT, sizeTotal);  //  设置滚动条
	Invalidate();
}


void CImageSnapView::OnSetObj()
{
	SendMessage(WM_RBUTTONDOWN);
	m_ObjorPart = 1;
	//::SendMessage(this->m_hWnd, WM_RBUTTONUP, MK_RBUTTON, MAKELONG(point.x,point.y));
	//SendMessageA(WM_RBUTTONDOWN, MK_RBUTTON, MAKELONG(point.x/2, point.y/2));
	// TODO:  在此添加命令处理程序代码
}


void CImageSnapView::OnLButtonUp(UINT nFlags, CPoint point)
{
	printf("OnLButtonUp()\n");
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//CPropertiesWnd * p = &(CMainFrame*)AfxGetApp()->m_pMainWnd->
	if (m_AbleToMove)
	{
		CImageSnapDoc* pDoc = GetDocument();
		if (m_rect)
		{
			pDoc->m_ischange = 1;
			pDoc->m_rect_i = rect_i_lock;
			m_AbleToMove = FALSE;
			pDoc->SetSelect2(m_ObjorPart);
			pDoc->UpdateAllViews(NULL);
			m_ObjorPart = 0;
		}
		else if (m_polygon)
		{
			pDoc->m_ischange = 1;
			pDoc->m_polygon_i = polygon_i_lock;
			m_AbleToMove = FALSE;
			pDoc->SetSelect3(m_ObjorPart);
			pDoc->UpdateAllViews(NULL);
			m_ObjorPart = 0;
		}
		else if (m_curve)
		{
			if (cp.size() > 2)
			{
				if (m_AbleToSet)
				{
					OnSetSelect();
					
				}
			}
			m_AbleToSet = FALSE;
			m_AbleDelete = FALSE;
			m_AbleToMove = FALSE;
			//OnSetSelect();
			
		}
		
		
	}
	printf("OnLButtonUp完成\n");
	CScrollView::OnLButtonUp(nFlags, point);
}


void CImageSnapView::OnUpdateRect(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_rect);
	
}


void CImageSnapView::OnUpdatePolygon(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_polygon);
}


void CImageSnapView::OnUpdateCurve(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_curve);
}


void CImageSnapView::OnRect()
{
	// TODO:  在此添加命令处理程序代码
	if (m_rect == TRUE)
		return;
	m_rect = !m_rect;
	m_polygon = FALSE;
	m_curve = FALSE;
	m_AbleToLeftBtn = FALSE;//鼠标左键有效
	m_AbleToMove = FALSE;//鼠标移动点有效
	m_AbleDelete = FALSE;
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_curve = m_curve;
	pDoc->m_polygon = m_polygon;
	pDoc->m_rect = m_rect;
	int curobj = pDoc->m_ObjList.size() - 1;
	pFrame->m_wndProperties.UpdataAllData(pDoc->m_ObjList, curobj, -1);
}


void CImageSnapView::OnPolygon()
{
	// TODO:  在此添加命令处理程序代码
	if (m_polygon == TRUE)
		return;
	m_polygon = !m_polygon;
	m_curve = FALSE;
	m_rect = FALSE;
	m_AbleToLeftBtn = FALSE;//鼠标左键有效
	m_AbleToMove = FALSE;//鼠标移动点有效
	m_AbleDelete = FALSE;
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_curve = m_curve;
	pDoc->m_polygon = m_polygon;
	pDoc->m_rect = m_rect;
	int curobj = pDoc->m_Polygon.size() - 1;
	pFrame->m_wndProperties.UpdataAllData(pDoc->m_Polygon, curobj, -1);
}


void CImageSnapView::OnCurve()
{
	// TODO:  在此添加命令处理程序代码
	if (m_curve == TRUE)
		return;
	m_curve = !m_curve;
	m_polygon = FALSE;
	m_rect = FALSE;
	m_AbleToLeftBtn = FALSE;//鼠标左键有效
	m_AbleToMove = FALSE;//鼠标移动点有效
	m_AbleDelete = FALSE;
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_curve = m_curve;
	pDoc->m_polygon = m_polygon;
	pDoc->m_rect = m_rect;
	int curobj = pDoc->m_Curve.size() - 1;
	pFrame->m_wndProperties.UpdataAllData(pDoc->m_Curve, curobj, -1);
}

// 绘制直线  
//void CImageSnapView::DrawLine(CDC* pDC,BOOL isRubber, CPoint ptStart, CPoint ptEnd)
//{
//	//CDC* pDC = GetDC();
//	if (isRubber)
//	{
//		pDC->SetROP2(R2_NOT);
//	}
//	CPen pen(PS_SOLID, 3, RGB(255, 0, 0));
//	CPen *pOldPen = pDC->SelectObject(&pen);
//	pDC->MoveTo(ptStart.x, ptStart.y);
//	pDC->LineTo(ptEnd.x, ptEnd.y);
//	pDC->SelectObject(pOldPen);
//	ReleaseDC(pDC);
//}
//
//void CImageSnapView::DrawPixel(CPoint pt)
//{
//	CDC* pDC = GetDC();
//	CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
//	CPen *pOldPen = pDC->SelectObject(&pen);
//	CBrush brush(RGB(255, 0, 0));
//	CBrush *pOldBrush = pDC->SelectObject(&brush);
//	int r = 3;
//	pDC->Ellipse(pt.x - r, pt.y - r, pt.x + r, pt.y + r);
//	pDC->SelectObject(pOldPen); 
//	pDC->SelectObject(pOldBrush);
//	ReleaseDC(pDC);
//}

//void CImageSnapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO:  在此添加消息处理程序代码和/或调用默认值
//	CImageSnapDoc* pDoc = GetDocument();
//	if (nChar == VK_DELETE)
//	{
//		
//		if (m_rect)
//		{
//			pDoc->m_ObjList[rect_i_lock].enable = FALSE;
//			pDoc->UpdateAllViews(NULL);
//		}
//	}
//	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
//}
BOOL CImageSnapView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CImageSnapView::OnCreateImgLabel()
{
	// TODO:  在此添加命令处理程序代码
	CSetproperty pSet;
	CImageSnapDoc* pDoc = GetDocument();
	pSet.m_classname = "image";
	if (pSet.DoModal() == IDOK)
	{
		pDoc->m_Image.enable = TRUE;
		pDoc->m_Image.o_property.clear();
		for (unsigned int i = 0; i < pSet.m_property.size(); i++)
		{
			ss_property imageSet;
			imageSet.property_name = pSet.m_property[i].m_property;
			imageSet.property_options = pSet.m_property[i].m_ablexuanxiang;
			imageSet.property_id = pSet.m_property[i].m_propertyid;
			pDoc->m_Image.o_property.push_back(imageSet);
		}

	}
	pDoc->SetModifiedFlag(TRUE);
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
}


void CImageSnapView::OnDeleteImgLabel()
{
	// TODO:  在此添加命令处理程序代码
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_Image.enable = FALSE;
	pDoc->SetModifiedFlag(TRUE);
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
}


void CImageSnapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//printf("OnKeyDown()\n");
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CImageSnapView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	printf("OnChar()\n");
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_BACK)
	{
		if (m_curve)
		{
			if (cp.size()>0)
				cp.pop_back();
		}
		else if (m_polygon)
		{
			if (pp.size()>0)
				pp.pop_back();
		}

		m_pDC = GetDC();
		OnDraw(m_pDC);
		ReleaseDC(m_pDC);
	}
	printf("OnChar()完成");
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}


void CImageSnapView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}



void CImageSnapView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


//void CImageSnapView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
//{
//	// 此功能要求 Windows Vista 或更高版本。
//	// _WIN32_WINNT 符号必须 >= 0x0600。
//	// TODO:  在此添加消息处理程序代码和/或调用默认值
//	OnDraw(GetDC());
//	CScrollView::OnMouseHWheel(nFlags, zDelta, pt);
//}


BOOL CImageSnapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


void CImageSnapView::OnDeletAllObject()
{
	// TODO:  在此添加命令处理程序代码
	CImageSnapDoc* pDoc = GetDocument();
	pDoc->m_Curve.clear();
	pDoc->m_Polygon.clear();
	pDoc->m_ObjList.clear();
	pDoc->SetModifiedFlag(TRUE);
	m_pDC = GetDC();
	OnDraw(m_pDC);
	ReleaseDC(m_pDC);
}


void CImageSnapView::OnFileSave()
{
	m_Mutex_move.lock();
	// TODO: 在此添加命令处理程序代码
	printf("OnNextImage()\n");
	CImageSnapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		printf("pDoc错误\n");
		return;
	}


	if (pDoc->m_nPos >= 0 && pDoc->m_nPos < pDoc->m_nPicNum - 1)
	{
		printf("next成功\n");
		ischange = TRUE;
		pDoc->OpenFile();
	}
	printf("OnNextImage完成\n");

	m_Mutex_move.unlock();
	return;
}


// ImageSnapDoc.h : CImageSnapDoc 类的接口
//


#pragma once

#include "opencv2/opencv.hpp"
#include "Lable.h"

using namespace std;
using namespace cv;



class CImageSnapDoc : public CDocument
{
protected: // 仅从序列化创建
	CImageSnapDoc();
	DECLARE_DYNCREATE(CImageSnapDoc)

//变量
public:
	IplImage*	m_pWorkImg;
	int         m_Display;      //图像显示标志
	int         m_ischange;
	vector <_Object> m_ObjList;
	vector<_Curve> m_Curve;
	vector<_Polygon> m_Polygon;
	_Image m_Image;
	BOOL m_rect;
	BOOL m_polygon;
	BOOL m_curve;
	BOOL m_save;
	BOOL m_inherit = FALSE;
	BOOL isFile = FALSE;
	int				m_nPos;				// 当前文件在容器中的索引
	int				m_nPicNum;			// 当前目录的文件个数
	int          m_rect_i;   //当前修改矩形
	int          m_curve_i;
	int          m_polygon_i;
	int          m_point;
	CSize  m_sizeDoc;
	int m_width;
	int m_height;
	int m_depth;

protected:
	IplImage *	m_pSrcImg;
	CString m_filename;
	CString m_fileext;

	CString			m_strPath;			// 当前打开目录
	CStringArray	m_FilesNameAry;		// 保存当前目录下所有文件名的容器





//函数
protected:
    void InitData();
	void SetSysPath();
	CString GetCurPath();
	//void ReadJsonSet(string classname);
	void ReadXmlInfo(CString filename);
	void OnSaveJson();
	void OnSaveJson2();
	BOOL ReadJsonInfo(CString filename);
	BOOL ReadJsonInfo2(CString filename);
	void GetAllFileNames(const CString& path, CStringArray& ary);
	void AllJson2xml();
	CString GetFilePath(int nIndex);

protected:
	
	inline void SafeReleaseImage(IplImage **pImg)
	{
		if (NULL != *pImg)
		{
			cvReleaseImage(pImg);
			*pImg = NULL;
		}
	}
public:
	void SetSelect(int sign);
	void SetSelect2(int sign);
	void SetSelectPoly(int sign);
	void SetSelect3(int sign);
	void SetSelectCurve(int sign);
	void SetModifiedFlag(BOOL bModified = TRUE);
	void OpenFile();
	CSize GetDocSize() const;



// 属性
public:
	std::vector<std::pair<int, std::string> > lines_zh;
	std::vector<std::pair<int, std::string> > lines_en;
// 操作
public:
	CString strXmlFile;
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CImageSnapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();

	afx_msg void OnSetSave();
	void OnSetSave2();
protected:
	virtual BOOL SaveModified();
public:
	afx_msg void OnReMarked();
	afx_msg void OnInherit();
	afx_msg void OnUpdateInherit(CCmdUI *pCmdUI);
	afx_msg void OnJson2Xml();
	afx_msg void OnFindchange();
	afx_msg void OnXml2Json();
	afx_msg void OnAnalyse();
};



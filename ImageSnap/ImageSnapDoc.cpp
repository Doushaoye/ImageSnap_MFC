// ImageSnapDoc.cpp : CImageSnapDoc 类的实现
//

#include "stdafx.h"
#include "ImageSnap.h"
#include <iostream>  
#include <fstream>  
#include <memory>
#include <direct.h>
#include "MainFrm.h"
#include "ImageSnapDoc.h"
#include "ImageSnapView.h"
#include "DlgSetSelect.h"
#include "MarkupMSXML.h"
#include <fstream>
#include "Json/json.h"
#include "Setproperty.h"
#include <codecvt>
#include <regex>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageSnapDoc

IMPLEMENT_DYNCREATE(CImageSnapDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageSnapDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, &CImageSnapDoc::OnFileOpen)
	ON_COMMAND(ID_SET_SAVE, &CImageSnapDoc::OnSetSave)
	ON_COMMAND(ID_RE_MARKED, &CImageSnapDoc::OnReMarked)
	ON_COMMAND(ID_32805, &CImageSnapDoc::OnInherit)
	ON_UPDATE_COMMAND_UI(ID_32805, &CImageSnapDoc::OnUpdateInherit)
	ON_COMMAND(ID_32808, &CImageSnapDoc::OnJson2Xml)
	ON_COMMAND(ID_32809, &CImageSnapDoc::OnFindchange)
	ON_COMMAND(ID_32810, &CImageSnapDoc::OnXml2Json)
	ON_COMMAND(ID_32811, &CImageSnapDoc::OnAnalyse)
END_MESSAGE_MAP()


// CImageSnapDoc 构造/析构
class path_save
{
public:
	std::string file_name;
	std::string file_path;
};
string UTF8ToGB(const char* str)
{
	string result;
	WCHAR *strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

int find_all_files(const char * lpPath, std::vector<path_save>&lines_,std::string search_str = ".")
{
	std::regex reg(search_str);
	path_save txt_line;
	using namespace std;
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind, lpPath);
	strcat(szFind, "/*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return -1;
	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				//发现子目录，递归之
				char szFile[MAX_PATH] = { 0 };
				strcpy(szFile, lpPath);
				strcat(szFile, "/");
				strcat(szFile, FindFileData.cFileName);
				find_all_files(szFile, lines_);
			}
		}
		else
		{
			//找到文件，处理之
			char szFilePath[MAX_PATH] = { 0 };
			sprintf_s(szFilePath, MAX_PATH, "%s/%s", lpPath, FindFileData.cFileName);
			//string filename;
			//filename = szFilePath;
			//判断文件后缀
			char* p = strrchr(FindFileData.cFileName, '.');
			if (p)
			{
				if (regex_search(FindFileData.cFileName,reg))
				{
					txt_line.file_name = FindFileData.cFileName;
					txt_line.file_path = szFilePath;
					lines_.push_back(txt_line);

					//std::cout << szFilePath << std::endl;
				}
			}
		}
	} while (::FindNextFile(hFind, &FindFileData));

	::FindClose(hFind);

	return 0;
}
int sumObject()
{
	using namespace Json;
	vector<path_save> Json_save;
	string strXmlFile = ".\\data";
	find_all_files(strXmlFile.c_str(), Json_save);
	for (int i = 0; i < Json_save.size(); i++)
	{
		CFileFind   find;
		if (!find.FindFile(strXmlFile.c_str()))
			return FALSE;


		ifstream ifs;
		ifs.open(strXmlFile);
		Value root;
		CharReaderBuilder fh;
		JSONCPP_STRING errs;
		parseFromStream(fh, ifs, &root, &errs);
	}
	
}

CImageSnapDoc::CImageSnapDoc()
{
	// TODO: 在此添加一次性构造代码

	m_pSrcImg  = 0;
	m_pWorkImg = 0;

	m_nPos = 0;				// 当前文件在容器中的索引
	m_nPicNum = 0;			// 当前目录的文件个数
	m_FilesNameAry.RemoveAll();


	InitData();


}

CImageSnapDoc::~CImageSnapDoc()
{
	InitData();
}

BOOL CImageSnapDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CImageSnapDoc 序列化

void CImageSnapDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CImageSnapDoc 诊断

#ifdef _DEBUG
void CImageSnapDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageSnapDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageSnapDoc 命令

void CImageSnapDoc::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码

	char BASED_CODE szOpenFilter[] = "图象文件|*.bmp; *.dib; *.gif; *.jpg; *.jpe; *.jpeg; *.pcx; *.tga; *.tif; *.tiff|位图文件 (*.bmp;*.dib)|*.bmp; *.dib|GIF文件 (*.gif)|*.gif|JPEG文件 (*.jpg;*.jpe;*.jpeg)|*.jpg; *.jpe; *.jpeg|PCX文件 (*.pcx)|*.pcx|TGA文件 (*.tga)|*.tga|TIFF文件 (*.tif;*.tiff)|*.tif; *.tiff|All Files (*.*)|*.*||";

	CFileDialog FileDlg(TRUE, "", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szOpenFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		m_filename = FileDlg.GetFileTitle();
		m_fileext = FileDlg.GetFileExt();
		CString strPath = FileDlg.GetPathName();


		m_FilesNameAry.RemoveAll();

		int nIndex = strPath.ReverseFind('\\');
		m_strPath = strPath.Left(nIndex);
		CString strFileName = strPath.Right(strPath.GetLength() - nIndex - 1);

		GetAllFileNames(m_strPath, m_FilesNameAry);
		m_nPicNum = (int)m_FilesNameAry.GetCount();

		m_nPos = 0;

		while( m_nPos < m_nPicNum && m_FilesNameAry[m_nPos] != strFileName)
			m_nPos++;

		m_bModified = FALSE;
		OpenFile();

	}

}

//void CImageSnapDoc::ReadJsonSet(string classname)
//{
//	using namespace Json;
//	using namespace std;
//	string m_jsonpath = "./set.json";
//	ifstream ifs;
//	ifs.open(m_jsonpath);
//	Value root;
//	CharReaderBuilder fh;
//	JSONCPP_STRING errs;
//	parseFromStream(fh, ifs, &root, &errs);
//
//	if (root.isMember(classname))
//	{
//		Value::Members rootMember = root[classname].getMemberNames();
//		Value v_class = root[classname];
//		for (Value::Members::iterator it = rootMember.begin(); it != rootMember.end(); ++it)
//		{
//			s_property s1;
//			s1.m_property = *it;
//			if (v_class.isMember(*it) && v_class[*it].isArray())
//			{
//				for (unsigned int j = 0; j < v_class[*it].size(); j++)
//				{
//					s1.m_xuanxiang.push_back(v_class[*it][j].asString());
//				}
//			}
//			m_property.push_back(s1);
//		}
//	}
//
//}

void CImageSnapDoc::InitData()
{
	m_Display = 0;
	m_ischange = 0;
	m_ObjList.clear();
	m_Polygon.clear();
	m_Curve.clear();
	m_Image.o_property.clear();
	m_Image.enable = TRUE;

	//****
	//load classname
	lines_zh.clear();
	lines_en.clear();

	std::ifstream infile("21cls.txt");
	std::ifstream infile1("21cls_English.txt");
	int nId;
	string clsname;
	int i = 0; 
	
	while (getline(infile,clsname))//(infile >> nId >> clsname)
	{
		//std::string GB_str;
		//GB_str = UTF8ToGB(clsname.c_str());
		int space_pos = clsname.find_first_of(' ');
		std::string temp_name = clsname.substr(space_pos+1);
		lines_zh.push_back(std::make_pair(i, temp_name));
		i++;

	}
	infile.close();

	int nId1;
	string clsname1;
	i = 0;
	while (getline(infile1, clsname1))//(infile1>> nId1 >> clsname1)
	{
		int space_pos = clsname1.find_first_of(' ');
		std::string temp_name = clsname1.substr(space_pos + 1);
		lines_en.push_back(std::make_pair(i, temp_name));
		i++;
	}
	infile1.close();
	//****

	SafeReleaseImage(&m_pSrcImg);
	SafeReleaseImage(&m_pWorkImg);
}


//设置bndbox
void CImageSnapDoc::SetSelect(int sign)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *) pFrame->GetActiveView();
	CSetproperty pSet;
	if (pView->m_rectTracker.IsRectEmpty())
	{
		return;
	}
	int curobj = -1;
	int curpart = -1;

	CDlgSetSelect dlgSetSelect(this);

	if (sign == 1)
		dlgSetSelect.m_iOOP = 1;
	dlgSetSelect.m_iOOP = dlgSetSelect.m_iOOP || dlgSetSelect.m_bPart;

	if (IDOK==dlgSetSelect.DoModal())
	{
		//目标
		
		
		if (0 == dlgSetSelect.m_iOOP)
		{
			_Object tmp;
			tmp.iobjclass = dlgSetSelect.m_iObjCls;
			tmp.pose = dlgSetSelect.m_iObjPos;
			tmp.classname = objclass[dlgSetSelect.m_iObjCls];
			pSet.m_classlabel = dlgSetSelect.m_iObjCls;
			pSet.m_classname = objclass[dlgSetSelect.m_iObjCls];
			tmp.truncated = (BOOL)(abs(dlgSetSelect.m_iObjTru-1));
			tmp.occluded = (BOOL)(abs(dlgSetSelect.m_iObjOcc-1));
			tmp.difficult = dlgSetSelect.m_iObjDif;
			tmp.objbox = pView->m_rectTracker;
			tmp.objbox.left =MIN(m_pWorkImg->width-1,MAX(0,tmp.objbox.left));
			tmp.objbox.top =MIN(m_pWorkImg->height-1,MAX(0,tmp.objbox.top));
			tmp.objbox.right =MIN(m_pWorkImg->width-1,MAX(tmp.objbox.left,tmp.objbox.right));
			tmp.objbox.bottom =MIN(m_pWorkImg->height-1,MAX(tmp.objbox.top,tmp.objbox.bottom));
			tmp.ischange = 1;
			tmp.enable = TRUE;
			if (IDOK == pSet.DoModal())
			{
				for (unsigned int i = 0; i < pSet.m_property.size(); i++)
				{
					ss_property Set;
					Set.property_name = pSet.m_property[i].m_property;
					Set.property_options = pSet.m_property[i].m_ablexuanxiang;
					Set.property_id = pSet.m_property[i].m_propertyid;
					tmp.o_property.push_back(Set);
				}

			}
			m_ObjList.push_back(tmp);
			curobj = m_ObjList.size() - 1;
		}
		//部件
		else
		{
			//取得当前目标
			unsigned int i = dlgSetSelect.m_iPartObj;
			_Part tmppart;
			tmppart.ipartclass = dlgSetSelect.m_iPartCls;
			tmppart.partrect = pView->m_rectTracker;

			//CWnd* pWnd = CWnd::FromHandle(dlgSetSelect.m_hWnd);
			//pWnd->UpdateData(TRUE);
			//char color[100]= { 0 };
			//::GetDlgItemTextA(dlgSetSelect.m_hWnd, IDC_EDIT1, color,100);
			//dlgSetSelect.m_Ecolor.GetWindowTextA(color,100);
			
			//strcpy(tmppart.color,color);
			//pWnd->UpdateData(FALSE);
			//tmppart.partrect.left =MIN(m_ObjList[i].objbox.right,MAX(m_ObjList[i].objbox.left,tmppart.partrect.left));
			//tmppart.partrect.top =MIN(m_ObjList[i].objbox.bottom,MAX(m_ObjList[i].objbox.top,tmppart.partrect.top));
			//tmppart.partrect.right =MIN(m_ObjList[i].objbox.right,MAX(tmppart.partrect.left,tmppart.partrect.right));
			//tmppart.partrect.bottom =MIN(m_ObjList[i].objbox.bottom,MAX(tmppart.partrect.top,tmppart.partrect.bottom));

			tmppart.ablerect = TRUE;
			tmppart.enable = TRUE;
			if (i < m_ObjList.size())
			{
				m_ObjList[i].partlist.push_back(tmppart);
				curobj = i;
				curpart = m_ObjList[i].partlist.size() - 1;
			}
			else if (i - m_ObjList.size()<m_Polygon.size() && i - m_ObjList.size()>=0)
			{
				int j = i - m_ObjList.size();
				m_Polygon[j].partlist.push_back(tmppart);
				curobj = j;
				curpart = m_Polygon[j].partlist.size() - 1;
			}
			else if (i - m_ObjList.size() - m_Polygon.size()<m_Curve.size() && i - m_ObjList.size() - m_Polygon.size()>=0)
			{ 
				int j = i - m_ObjList.size() - m_Polygon.size();
				m_Curve[j].partlist.push_back(tmppart);
				curobj = j;
				curpart = m_Curve[j].partlist.size() - 1;
			}
				
		}
		

		pView->m_rectTracker.SetRect(0,0,0,0);

		SetModifiedFlag();

	}
	UpdateAllViews(NULL);
	pFrame->m_wndProperties.UpdataAllData(m_ObjList,curobj,curpart);

	return;
}

//bndbox拖拽修改
void CImageSnapDoc::SetSelect2(int sign) 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *)pFrame->GetActiveView();

	int curobj = -1;
	int curpart = -1;

	CDlgSetSelect dlgSetSelect(this);

	if (sign == 1)
		dlgSetSelect.m_iOOP = 1;
	dlgSetSelect.m_iOOP = dlgSetSelect.m_iOOP || dlgSetSelect.m_bPart;
	if (0 == dlgSetSelect.m_iOOP)
	{
		_Object tmp;
		tmp.classname = m_ObjList[m_rect_i].classname;
		tmp.iobjclass = m_ObjList[m_rect_i].iobjclass;
		tmp.pose = m_ObjList[m_rect_i].pose;
		tmp.truncated = m_ObjList[m_rect_i].truncated;
		tmp.occluded = m_ObjList[m_rect_i].occluded;
		tmp.difficult = m_ObjList[m_rect_i].difficult;
		tmp.objbox = m_ObjList[m_rect_i].objbox;
		tmp.partlist = m_ObjList[m_rect_i].partlist;
		tmp.o_property = m_ObjList[m_rect_i].o_property;
		tmp.objbox.left =MIN(m_pWorkImg->width-1,MAX(0,tmp.objbox.left));
		tmp.objbox.top =MIN(m_pWorkImg->height-1,MAX(0,tmp.objbox.top));
		tmp.objbox.right =MIN(m_pWorkImg->width-1,MAX(tmp.objbox.left,tmp.objbox.right));
		tmp.objbox.bottom =MIN(m_pWorkImg->height-1,MAX(tmp.objbox.top,tmp.objbox.bottom));
		tmp.enable = TRUE;
		tmp.ischange = m_ischange;
		m_ObjList[m_rect_i] = (tmp);
		curpart = m_ObjList[m_rect_i].partlist.size() - 1;
		curobj = m_ObjList.size() - 1;
		
	}
	//部件
	else
	{
		//取得当前目标
		int i = dlgSetSelect.m_iPartObj;
		_Part tmppart;
		tmppart.ipartclass = dlgSetSelect.m_iPartCls;
		tmppart.partrect = pView->m_rectTracker;
		tmppart.ablerect = TRUE;
		CWnd* pWnd = CWnd::FromHandle(dlgSetSelect.m_hWnd);
		pWnd->UpdateData();
		char color[100] = { 0 };
		//::GetDlgItemTextA(dlgSetSelect.m_hWnd, IDC_EDIT1, color,100);
		//dlgSetSelect.m_Ecolor.GetWindowTextA(color,100);

		strcpy(tmppart.color, color);
		pWnd->UpdateData(FALSE);
		tmppart.partrect.left =MIN(m_ObjList[i].objbox.right,MAX(m_ObjList[i].objbox.left,tmppart.partrect.left));
		tmppart.partrect.top =MIN(m_ObjList[i].objbox.bottom,MAX(m_ObjList[i].objbox.top,tmppart.partrect.top));
		tmppart.partrect.right =MIN(m_ObjList[i].objbox.right,MAX(tmppart.partrect.left,tmppart.partrect.right));
		tmppart.partrect.bottom =MIN(m_ObjList[i].objbox.bottom,MAX(tmppart.partrect.top,tmppart.partrect.bottom));


		tmppart.enable = TRUE;
		m_ObjList[i].partlist.push_back(tmppart);
		curobj = i;
		curpart = m_ObjList[i].partlist.size() - 1;
	}
	
	pView->m_rectTracker.SetRect(0, 0, 0, 0);

	SetModifiedFlag();
	//UpdateAllViews(NULL);
	pFrame->m_wndProperties.UpdataAllData(m_ObjList, curobj, curpart);
	
	return;
}   

//设置polygon
void CImageSnapDoc::SetSelectPoly(int sign)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *)pFrame->GetActiveView();
	CSetproperty pSet;
	if (pView->pp.empty())
	{
		return;
	}
	int curobj = -1;
	int curpart = -1;

	CDlgSetSelect dlgSetSelect(this);

	if (sign == 1)
		dlgSetSelect.m_iOOP = 1;
	dlgSetSelect.m_iOOP = dlgSetSelect.m_iOOP || dlgSetSelect.m_bPart;

	if (IDOK == dlgSetSelect.DoModal())
	{
		//目标


		if (0 == dlgSetSelect.m_iOOP)
		{
			_Polygon tmp;
			tmp.iobjclass = dlgSetSelect.m_iObjCls;
			pSet.m_classlabel = dlgSetSelect.m_iObjCls;
			pSet.m_classname = objclass[dlgSetSelect.m_iObjCls];
			tmp.classname = objclass[dlgSetSelect.m_iObjCls];
			tmp.pose = dlgSetSelect.m_iObjPos;
			tmp.truncated = (BOOL)(abs(dlgSetSelect.m_iObjTru - 1));
			tmp.occluded = (BOOL)(abs(dlgSetSelect.m_iObjOcc - 1));
			tmp.difficult = dlgSetSelect.m_iObjDif;
			tmp.objpolygon = pView->pp;
			pView->pp.clear();
			if (IDOK == pSet.DoModal())
			{
				for (unsigned int i = 0; i < pSet.m_property.size(); i++)
				{
					ss_property Set;
					Set.property_name = pSet.m_property[i].m_property;
					Set.property_options = pSet.m_property[i].m_ablexuanxiang;
					Set.property_id = pSet.m_property[i].m_propertyid;
					tmp.o_property.push_back(Set);
				}

			}
			tmp.enable = TRUE;
			m_Polygon.push_back(tmp);
			curobj = m_Polygon.size() - 1;
		}
		//部件
		else
		{
			//取得当前目标
			int i = dlgSetSelect.m_iPartObj;
			_Part tmppart;
			tmppart.ipartclass = dlgSetSelect.m_iPartCls;
			tmppart.polygon = pView->pp;
			tmppart.ablerect = FALSE;
			pView->pp.clear();
			if (i < m_ObjList.size())
			{
				m_ObjList[i].partlist.push_back(tmppart);
				curobj = i;
				curpart = m_ObjList[i].partlist.size() - 1;
			}
			else if (i - m_ObjList.size()<m_Polygon.size() && i - m_ObjList.size() >= 0)
			{
				int j = i - m_ObjList.size();
				m_Polygon[j].partlist.push_back(tmppart);
				curobj = j;
				curpart = m_Polygon[j].partlist.size() - 1;
			}
			else if (i - m_ObjList.size() - m_Polygon.size()<m_Curve.size() && i - m_ObjList.size() - m_Polygon.size() >= 0)
			{
				int j = i - m_ObjList.size() - m_Polygon.size();
				m_Curve[j].partlist.push_back(tmppart);
				curobj = j;
				curpart = m_Curve[j].partlist.size() - 1;
			}
			//if (i < m_ObjList.size())
			//{
			//	_Part tmppart;
			//	tmppart.ipartclass = dlgSetSelect.m_iPartCls;
			//	tmppart.polygon = pView->pp;
			//	tmppart.ablerect = FALSE;
			//	pView->pp.clear();

			//	/*tmppart.partrect.left =MIN(m_ObjList[i].objbox.right,MAX(m_ObjList[i].objbox.left,tmppart.partrect.left));
			//	tmppart.partrect.top =MIN(m_ObjList[i].objbox.bottom,MAX(m_ObjList[i].objbox.top,tmppart.partrect.top));
			//	tmppart.partrect.right =MIN(m_ObjList[i].objbox.right,MAX(tmppart.partrect.left,tmppart.partrect.right));
			//	tmppart.partrect.bottom =MIN(m_ObjList[i].objbox.bottom,MAX(tmppart.partrect.top,tmppart.partrect.bottom));*/


			//	tmppart.enable = TRUE;
			//	m_ObjList[i].partlist.push_back(tmppart);
			//	curobj = i;
			//	curpart = m_ObjList[i].partlist.size() - 1;
			//}
			//else
			//{
			//	i = i - m_ObjList.size();
			//}
			
		}
		//pView->m_rectTracker.SetRect(0, 0, 0, 0);

		SetModifiedFlag();

	}
	else
	{
		pView->pp.clear();
	}
	UpdateAllViews(NULL);
	pFrame->m_wndProperties.UpdataAllData(m_Polygon, curobj, curpart);
	
	return;
}

void CImageSnapDoc::SetSelect3(int sign)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *)pFrame->GetActiveView();

	int curobj = -1;
	int curpart = -1;

	CDlgSetSelect dlgSetSelect(this);

	if (sign == 1)
		dlgSetSelect.m_iOOP = 1;
	dlgSetSelect.m_iOOP = dlgSetSelect.m_iOOP || dlgSetSelect.m_bPart;
	if (0 == dlgSetSelect.m_iOOP)
	{
		_Polygon tmp;
		tmp.classname = m_Polygon[m_polygon_i].classname;
		tmp.iobjclass = m_Polygon[m_polygon_i].iobjclass;
		tmp.pose = m_Polygon[m_polygon_i].pose;
		tmp.truncated = m_Polygon[m_polygon_i].truncated;
		tmp.occluded = m_Polygon[m_polygon_i].occluded;
		tmp.difficult = m_Polygon[m_polygon_i].difficult;
		tmp.objpolygon = m_Polygon[m_polygon_i].objpolygon;
		tmp.partlist = m_Polygon[m_polygon_i].partlist;
		tmp.enable = TRUE;
		tmp.o_property = m_Polygon[m_polygon_i].o_property;
		m_Polygon[m_polygon_i] = (tmp);
		curpart = m_Polygon[m_polygon_i].partlist.size() - 1;
		curobj = m_Polygon.size() - 1;

	}
	//部件
	else
	{
		//取得当前目标
		int i = dlgSetSelect.m_iPartObj;
		_Part tmppart;
		tmppart.ipartclass = dlgSetSelect.m_iPartCls;
		tmppart.partrect = pView->m_rectTracker;
		tmppart.ablerect = TRUE;
		//CWnd* pWnd = CWnd::FromHandle(dlgSetSelect.m_hWnd);
		//pWnd->UpdateData();
		//char color[100] = { 0 };
		//::GetDlgItemTextA(dlgSetSelect.m_hWnd, IDC_EDIT1, color,100);
		//dlgSetSelect.m_Ecolor.GetWindowTextA(color,100);

		//strcpy(tmppart.color, color);
		//pWnd->UpdateData(FALSE);
		/*tmppart.partrect.left = MIN(m_Polygon[i].objbox.right, MAX(m_Polygon[i].objbox.left, tmppart.partrect.left));
		tmppart.partrect.top = MIN(m_Polygon[i].objbox.bottom, MAX(m_Polygon[i].objbox.top, tmppart.partrect.top));
		tmppart.partrect.right = MIN(m_Polygon[i].objbox.right, MAX(tmppart.partrect.left, tmppart.partrect.right));
		tmppart.partrect.bottom = MIN(m_Polygon[i].objbox.bottom, MAX(tmppart.partrect.top, tmppart.partrect.bottom));*/


		tmppart.enable = TRUE;
		m_Polygon[i].partlist.push_back(tmppart);
		curobj = i;
		curpart = m_Polygon[i].partlist.size() - 1;
	}
	pView->m_rectTracker.SetRect(0, 0, 0, 0);

	SetModifiedFlag();
	//UpdateAllViews(NULL);
	pFrame->m_wndProperties.UpdataAllData(m_Polygon, curobj, curpart);

	return;
}

void CImageSnapDoc::SetSelectCurve(int sign)
{
	printf("SetSelectCurve开始\n");
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *)pFrame->GetActiveView();
	CSetproperty pSet;
	if (pView->cp.empty())
	{
		return;
	}
	int curobj = -1;
	int curpart = -1;

	CDlgSetSelect dlgSetSelect(this);

	if (sign == 1)
		dlgSetSelect.m_iOOP = 1;
	dlgSetSelect.m_iOOP = dlgSetSelect.m_iOOP || dlgSetSelect.m_bPart;

	if (IDOK == dlgSetSelect.DoModal())
	{
		//目标


		if (0 == dlgSetSelect.m_iOOP)
		{
			_Curve tmp;
			tmp.classname = objclass[dlgSetSelect.m_iObjCls];
			tmp.iobjclass = dlgSetSelect.m_iObjCls;
			tmp.pose = dlgSetSelect.m_iObjPos;
			pSet.m_classlabel = dlgSetSelect.m_iObjCls;
			pSet.m_classname = objclass[dlgSetSelect.m_iObjCls];
			tmp.truncated = (BOOL)(abs(dlgSetSelect.m_iObjTru - 1));
			tmp.occluded = (BOOL)(abs(dlgSetSelect.m_iObjOcc - 1));
			tmp.difficult = dlgSetSelect.m_iObjDif;
			tmp.objcurve = pView->cp;
			
			pView->cp.clear();
			if (IDOK == pSet.DoModal())
			{
				for (unsigned int i = 0; i < pSet.m_property.size(); i++)
				{
					ss_property Set;
					Set.property_name = pSet.m_property[i].m_property;
					Set.property_options = pSet.m_property[i].m_ablexuanxiang;
					Set.property_id = pSet.m_property[i].m_propertyid;
					tmp.o_property.push_back(Set);
				}

				
			}
			tmp.enable = TRUE;
			m_Curve.push_back(tmp);
			curobj = m_Curve.size() - 1;
		}
		//部件
		else
		{
		}
		//pView->m_rectTracker.SetRect(0, 0, 0, 0);

		SetModifiedFlag();

	}
	else
	{
		pView->cp.clear();
	}
	UpdateAllViews(NULL);
	pFrame->m_wndProperties.UpdataAllData(m_Curve, curobj, curpart);
	printf("SetSelectCurve结束\n");
	return;
}

void CImageSnapDoc::OnSaveJson()
{
	printf("OnSaveJson()\n");
	using namespace Json;
	if (m_pWorkImg == NULL)
	{
		return;
	}
	Value root;
	Value imagelabel;
	imagelabel["width"] = m_pWorkImg->width;
	imagelabel["height"] = m_pWorkImg->height;
	imagelabel["depth"] = m_pWorkImg->depth;
	//imagelabel["nChannels"] = m_pWorkImg->nChannels;
	Value imageproperty;
	if (m_Image.enable)
	{
		for (unsigned int i = 0; i < m_Image.o_property.size(); i++)
		{
			/*std::string en_name;
			int en_pos = m_Image.o_property[i].property_name.find_first_of("_");
			if (en_pos != -1)
			{
				int zh_len = m_Image.o_property[i].property_name.size() - en_pos;

				en_name.assign(m_Image.o_property[i].property_name, 0, en_pos);
			}
			else
			{
				en_name = m_Image.o_property[i].property_name;
			}*/
			imageproperty[m_Image.o_property[i].property_name] = m_Image.o_property[i].property_id;
		}
	}
	imagelabel["property"] = imageproperty;
	/*imagelabel["street"] = "road";
	imagelabel["weather"] = "clear";
	imagelabel["time"] = "morning";*/
	for (unsigned int i = 0; i < m_ObjList.size(); i++)
	{
		if (m_ObjList[i].enable == FALSE)
		{
			continue;
		}
		Value v_rect;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_ObjList[i].iobjclass];
		for (unsigned int j = 0; j < m_ObjList[i].o_property.size(); j++)
		{
			//std::string en_name;
			//int en_pos = m_ObjList[i].o_property[j].property_name.find_first_of("_");
			//if (en_pos != -1)
			//{
			//	int zh_len = m_ObjList[i].o_property[j].property_name.size() - en_pos;

			//	en_name.assign(m_ObjList[i].o_property[j].property_name, 0, en_pos);
			//}
			//else
			//{
			//	en_name = m_ObjList[i].o_property[j].property_name;
			//}
			v_property[m_ObjList[i].o_property[j].property_name] = m_ObjList[i].o_property[j].property_id;  //int 型 id和value相同
			 
		}
		/*v_property["truncated"] = m_ObjList[i].truncated;
		v_property["occluded"] = m_ObjList[i].occluded;
		v_property["difficult"] = m_ObjList[i].difficult;
		v_property["pose"] = m_ObjList[i].pose;*/
		v_point["p1"].append(m_ObjList[i].objbox.left);
		v_point["p1"].append(m_ObjList[i].objbox.top);
		v_point["p2"].append(m_ObjList[i].objbox.right);
		v_point["p2"].append(m_ObjList[i].objbox.bottom);
		v_rect["rect"] = v_point;
		for (unsigned int j = 0; j < m_ObjList[i].partlist.size(); j++)
		{
			if (FALSE == m_ObjList[i].partlist[j].enable)
			{
				continue;
			}
			Value v_part;
			Value v_p_point;
			Value v_p_property;
			string Pt_name = g_part[m_ObjList[i].partlist[j].ipartclass];
			v_part["name"] = Pt_name;
			if (m_ObjList[i].partlist[j].ablerect == TRUE)
			{
				v_part["class"] = "bndbox";
				v_p_point["p1"].append(m_ObjList[i].partlist[j].partrect.left);
				v_p_point["p1"].append(m_ObjList[i].partlist[j].partrect.top);
				v_p_point["p2"].append(m_ObjList[i].partlist[j].partrect.right);
				v_p_point["p2"].append(m_ObjList[i].partlist[j].partrect.bottom);
				/*v_p_property["truncated"] = FALSE;
				v_p_property["occluded"] = FALSE;
				v_p_property["difficult"] = FALSE;
				v_p_property["pose"] = m_ObjList[i].pose;*/
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_rect["part"].append(v_part);
			}
			else
			{
				v_part["class"] = "polygon";
				for (unsigned int k = 0; k < m_ObjList[i].partlist[j].polygon.size(); k++)
				{
					v_p_point["x"] = m_ObjList[i].partlist[j].polygon[k].x;
					v_p_point["y"] = m_ObjList[i].partlist[j].polygon[k].y;
					v_part["polygon"].append(v_p_point);
				}
				/*v_p_property["truncated"] = FALSE;
				v_p_property["occluded"] = FALSE;
				v_p_property["difficult"] = FALSE;
				v_p_property["pose"] = m_ObjList[i].pose;*/
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_rect["part"].append(v_part);
			}
		}
		v_rect["property"] = v_property;
		v_rect["class"] = "bndbox";
		v_rect["name"] = Ob_name;
		v_rect["ischange"] = m_ObjList[i].ischange;
		imagelabel["Object"].append(v_rect);
	}
	for (unsigned int i = 0; i < m_Polygon.size(); i++)
	{
		if (m_Polygon[i].enable == FALSE)
		{
			continue;
		}
		Value v_polygon;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_Polygon[i].iobjclass];

		for (unsigned int j = 0; j < m_Polygon[i].o_property.size(); j++)
		{
		/*	std::string en_name;
			int en_pos = m_Polygon[i].o_property[j].property_name.find_first_of("_");
			if (en_pos != -1)
			{
				int zh_len = m_Polygon[i].o_property[j].property_name.size() - en_pos;

				en_name.assign(m_Polygon[i].o_property[j].property_name, 0, en_pos);
			}
			else
			{
				en_name = m_Polygon[i].o_property[j].property_name;
			}*/
			v_property[m_Polygon[i].o_property[j].property_name] = m_Polygon[i].o_property[j].property_id;

		}

		for (unsigned int k = 0; k < m_Polygon[i].objpolygon.size(); k++)
		{
			v_point["x"] = m_Polygon[i].objpolygon[k].x;
			v_point["y"] = m_Polygon[i].objpolygon[k].y;
			v_polygon["polygon"].append(v_point);
		}
		for (unsigned int j = 0; j < m_Polygon[i].partlist.size(); j++)
		{
			if (FALSE == m_Polygon[i].partlist[j].enable)
			{
				continue;
			}
			Value v_part;
			Value v_p_point;
			Value v_p_property;
			string Pt_name = g_part[m_Polygon[i].partlist[j].ipartclass];
			v_part["name"] = Pt_name;
			if (m_Polygon[i].partlist[j].ablerect == TRUE)
			{
				v_part["class"] = "bndbox";
				v_p_point["p1"].append(m_Polygon[i].partlist[j].partrect.left);
				v_p_point["p1"].append(m_Polygon[i].partlist[j].partrect.top);
				v_p_point["p2"].append(m_Polygon[i].partlist[j].partrect.right);
				v_p_point["p2"].append(m_Polygon[i].partlist[j].partrect.bottom);
				//v_p_property["truncated"] = FALSE;
				//v_p_property["occluded"] = FALSE;
				//v_p_property["difficult"] = FALSE;
				//v_p_property["pose"] = 0;
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_polygon["part"].append(v_part);
			}
			else
			{
				v_part["class"] = "polygon";
				for (unsigned int k = 0; k < m_Polygon[i].partlist[j].polygon.size(); k++)
				{
					v_p_point["x"] = m_Polygon[i].partlist[j].polygon[k].x;
					v_p_point["y"] = m_Polygon[i].partlist[j].polygon[k].y;
					v_part["polygon"].append(v_p_point);
				}
				//v_p_property["truncated"] = FALSE;
				//v_p_property["occluded"] = FALSE;
				//v_p_property["difficult"] = FALSE;
				//v_p_property["pose"] = 0;
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_polygon["part"].append(v_part);
			}
		}
		v_polygon["property"] = v_property;
		v_polygon["class"] = "polygon";
		v_polygon["name"] = Ob_name;
		imagelabel["Object"].append(v_polygon);
	}
	for (unsigned int i = 0; i < m_Curve.size(); i++)
	{
		if (m_Curve[i].enable == FALSE)
		{
			continue;
		}
		Value v_curve;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_Curve[i].iobjclass];

		for (unsigned int j = 0; j < m_Curve[i].o_property.size(); j++)
		{
			//std::string en_name;
			//int en_pos = m_Curve[i].o_property[j].property_name.find_first_of("_");
			//if (en_pos != -1)
			//{
			//	int zh_len = m_Curve[i].o_property[j].property_name.size() - en_pos;

			//	en_name.assign(m_Curve[i].o_property[j].property_name, 0, en_pos);
			//}
			//else
			//{
			//	en_name = m_Curve[i].o_property[j].property_name;
			//}
			v_property[m_Curve[i].o_property[j].property_name] = m_Curve[i].o_property[j].property_id;

		}

		for (unsigned int k = 0; k < m_Curve[i].objcurve.size(); k++)
		{
			string num = to_string(k+1);
			string px, py, c1x, c1y, c2x, c2y;
			px = "x" + num;
			py = "y" + num;
			c1x = px + "_c1";
			c1y = py + "_c1";
			c2x = px + "_c2";
			c2y = py + "_c2";
			v_point[px] = m_Curve[i].objcurve[k].p.x;
			v_point[py] = m_Curve[i].objcurve[k].p.y;
			v_point[c1x] = m_Curve[i].objcurve[k].c1.x;
			v_point[c1y] = m_Curve[i].objcurve[k].c1.y;
			v_point[c2x] = m_Curve[i].objcurve[k].c2.x;
			v_point[c2y] = m_Curve[i].objcurve[k].c2.y;
			
		}
		v_curve["point"] = v_point;
		v_curve["property"] = v_property;
		v_curve["class"] = "curve";
		v_curve["name"] = Ob_name;
		imagelabel["Object"].append(v_curve);
	}
	root["name"] = (string)m_filename;
	root["imagelabel"] = imagelabel;
	ofstream ofs;
	string save_path = "./data/" + (string)m_filename + ".json";

	ofs.open(save_path, ios::trunc);
	ofs << root.toStyledString();

	printf("OnSaveJson()完成\n");
}
void CImageSnapDoc::OnSaveJson2()
{
	printf("OnSaveJson()\n");
	using namespace Json;
	Value root;
	Value imagelabel;
	imagelabel["width"] = m_width;
	imagelabel["height"] = m_height;
	imagelabel["depth"] = m_depth;
	//imagelabel["nChannels"] = m_pWorkImg->nChannels;
	Value imageproperty;
	if (m_Image.enable)
	{
		for (unsigned int i = 0; i < m_Image.o_property.size(); i++)
		{
			/*std::string en_name;
			int en_pos = m_Image.o_property[i].property_name.find_first_of("_");
			if (en_pos != -1)
			{
				int zh_len = m_Image.o_property[i].property_name.size() - en_pos;

				en_name.assign(m_Image.o_property[i].property_name, 0, en_pos);
			}
			else
			{
				en_name = m_Image.o_property[i].property_name;
			}*/
			imageproperty[m_Image.o_property[i].property_name] = m_Image.o_property[i].property_id;
		}
	}
	imagelabel["property"] = imageproperty;
	/*imagelabel["street"] = "road";
	imagelabel["weather"] = "clear";
	imagelabel["time"] = "morning";*/
	for (unsigned int i = 0; i < m_ObjList.size(); i++)
	{
		//if (m_ObjList[i].enable == FALSE)
		//{
		//	continue;
		//}
		Value v_rect;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_ObjList[i].iobjclass];
		for (unsigned int j = 0; j < m_ObjList[i].o_property.size(); j++)
		{
			//std::string en_name;
			//int en_pos = m_ObjList[i].o_property[j].property_name.find_first_of("_");
			//if (en_pos != -1)
			//{
			//	int zh_len = m_ObjList[i].o_property[j].property_name.size() - en_pos;

			//	en_name.assign(m_ObjList[i].o_property[j].property_name, 0, en_pos);
			//}
			//else
			//{
			//	en_name = m_ObjList[i].o_property[j].property_name;
			//}
			v_property[m_ObjList[i].o_property[j].property_name] = m_ObjList[i].o_property[j].property_id;  //int 型 id和value相同

		}
		/*v_property["truncated"] = m_ObjList[i].truncated;
		v_property["occluded"] = m_ObjList[i].occluded;
		v_property["difficult"] = m_ObjList[i].difficult;
		v_property["pose"] = m_ObjList[i].pose;*/
		v_point["p1"].append(m_ObjList[i].objbox.left);
		v_point["p1"].append(m_ObjList[i].objbox.top);
		v_point["p2"].append(m_ObjList[i].objbox.right);
		v_point["p2"].append(m_ObjList[i].objbox.bottom);
		v_rect["rect"] = v_point;
		for (unsigned int j = 0; j < m_ObjList[i].partlist.size(); j++)
		{
			if (FALSE == m_ObjList[i].partlist[j].enable)
			{
				continue;
			}
			Value v_part;
			Value v_p_point;
			Value v_p_property;
			string Pt_name = g_part[m_ObjList[i].partlist[j].ipartclass];
			v_part["name"] = Pt_name;
			if (m_ObjList[i].partlist[j].ablerect == TRUE)
			{
				v_part["class"] = "bndbox";
				v_p_point["p1"].append(m_ObjList[i].partlist[j].partrect.left);
				v_p_point["p1"].append(m_ObjList[i].partlist[j].partrect.top);
				v_p_point["p2"].append(m_ObjList[i].partlist[j].partrect.right);
				v_p_point["p2"].append(m_ObjList[i].partlist[j].partrect.bottom);
				/*v_p_property["truncated"] = FALSE;
				v_p_property["occluded"] = FALSE;
				v_p_property["difficult"] = FALSE;
				v_p_property["pose"] = m_ObjList[i].pose;*/
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_rect["part"].append(v_part);
			}
			else
			{
				v_part["class"] = "polygon";
				for (unsigned int k = 0; k < m_ObjList[i].partlist[j].polygon.size(); k++)
				{
					v_p_point["x"] = m_ObjList[i].partlist[j].polygon[k].x;
					v_p_point["y"] = m_ObjList[i].partlist[j].polygon[k].y;
					v_part["polygon"].append(v_p_point);
				}
				/*v_p_property["truncated"] = FALSE;
				v_p_property["occluded"] = FALSE;
				v_p_property["difficult"] = FALSE;
				v_p_property["pose"] = m_ObjList[i].pose;*/
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_rect["part"].append(v_part);
			}
		}
		v_rect["property"] = v_property;
		v_rect["class"] = "bndbox";
		v_rect["name"] = Ob_name;
		v_rect["ischange"] = m_ObjList[i].ischange;
		imagelabel["Object"].append(v_rect);
	}
	for (unsigned int i = 0; i < m_Polygon.size(); i++)
	{
		if (m_Polygon[i].enable == FALSE)
		{
			continue;
		}
		Value v_polygon;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_Polygon[i].iobjclass];

		for (unsigned int j = 0; j < m_Polygon[i].o_property.size(); j++)
		{
			/*	std::string en_name;
				int en_pos = m_Polygon[i].o_property[j].property_name.find_first_of("_");
				if (en_pos != -1)
				{
					int zh_len = m_Polygon[i].o_property[j].property_name.size() - en_pos;

					en_name.assign(m_Polygon[i].o_property[j].property_name, 0, en_pos);
				}
				else
				{
					en_name = m_Polygon[i].o_property[j].property_name;
				}*/
			v_property[m_Polygon[i].o_property[j].property_name] = m_Polygon[i].o_property[j].property_id;

		}

		for (unsigned int k = 0; k < m_Polygon[i].objpolygon.size(); k++)
		{
			v_point["x"] = m_Polygon[i].objpolygon[k].x;
			v_point["y"] = m_Polygon[i].objpolygon[k].y;
			v_polygon["polygon"].append(v_point);
		}
		for (unsigned int j = 0; j < m_Polygon[i].partlist.size(); j++)
		{
			if (FALSE == m_Polygon[i].partlist[j].enable)
			{
				continue;
			}
			Value v_part;
			Value v_p_point;
			Value v_p_property;
			string Pt_name = g_part[m_Polygon[i].partlist[j].ipartclass];
			v_part["name"] = Pt_name;
			if (m_Polygon[i].partlist[j].ablerect == TRUE)
			{
				v_part["class"] = "bndbox";
				v_p_point["p1"].append(m_Polygon[i].partlist[j].partrect.left);
				v_p_point["p1"].append(m_Polygon[i].partlist[j].partrect.top);
				v_p_point["p2"].append(m_Polygon[i].partlist[j].partrect.right);
				v_p_point["p2"].append(m_Polygon[i].partlist[j].partrect.bottom);
				//v_p_property["truncated"] = FALSE;
				//v_p_property["occluded"] = FALSE;
				//v_p_property["difficult"] = FALSE;
				//v_p_property["pose"] = 0;
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_polygon["part"].append(v_part);
			}
			else
			{
				v_part["class"] = "polygon";
				for (unsigned int k = 0; k < m_Polygon[i].partlist[j].polygon.size(); k++)
				{
					v_p_point["x"] = m_Polygon[i].partlist[j].polygon[k].x;
					v_p_point["y"] = m_Polygon[i].partlist[j].polygon[k].y;
					v_part["polygon"].append(v_p_point);
				}
				//v_p_property["truncated"] = FALSE;
				//v_p_property["occluded"] = FALSE;
				//v_p_property["difficult"] = FALSE;
				//v_p_property["pose"] = 0;
				v_part["rect"] = v_p_point;
				v_part["property"] = v_p_property;
				v_polygon["part"].append(v_part);
			}
		}
		v_polygon["property"] = v_property;
		v_polygon["class"] = "polygon";
		v_polygon["name"] = Ob_name;
		imagelabel["Object"].append(v_polygon);
	}
	for (unsigned int i = 0; i < m_Curve.size(); i++)
	{
		if (m_Curve[i].enable == FALSE)
		{
			continue;
		}
		Value v_curve;
		Value v_property;
		Value v_point;
		string Ob_name = objclass[m_Curve[i].iobjclass];

		for (unsigned int j = 0; j < m_Curve[i].o_property.size(); j++)
		{
			//std::string en_name;
			//int en_pos = m_Curve[i].o_property[j].property_name.find_first_of("_");
			//if (en_pos != -1)
			//{
			//	int zh_len = m_Curve[i].o_property[j].property_name.size() - en_pos;

			//	en_name.assign(m_Curve[i].o_property[j].property_name, 0, en_pos);
			//}
			//else
			//{
			//	en_name = m_Curve[i].o_property[j].property_name;
			//}
			v_property[m_Curve[i].o_property[j].property_name] = m_Curve[i].o_property[j].property_id;

		}

		for (unsigned int k = 0; k < m_Curve[i].objcurve.size(); k++)
		{
			string num = to_string(k + 1);
			string px, py, c1x, c1y, c2x, c2y;
			px = "x" + num;
			py = "y" + num;
			c1x = px + "_c1";
			c1y = py + "_c1";
			c2x = px + "_c2";
			c2y = py + "_c2";
			v_point[px] = m_Curve[i].objcurve[k].p.x;
			v_point[py] = m_Curve[i].objcurve[k].p.y;
			v_point[c1x] = m_Curve[i].objcurve[k].c1.x;
			v_point[c1y] = m_Curve[i].objcurve[k].c1.y;
			v_point[c2x] = m_Curve[i].objcurve[k].c2.x;
			v_point[c2y] = m_Curve[i].objcurve[k].c2.y;

		}
		v_curve["point"] = v_point;
		v_curve["property"] = v_property;
		v_curve["class"] = "curve";
		v_curve["name"] = Ob_name;
		imagelabel["Object"].append(v_curve);
	}
	root["name"] = (string)m_filename;
	root["imagelabel"] = imagelabel;
	ofstream ofs;
	string save_path = "./data/" + (string)m_filename + ".json";

	ofs.open(save_path, ios::trunc);
	ofs << root.toStyledString();

	printf("OnSaveJson()完成\n");
}
void CImageSnapDoc::OnSetSave()
{
	// TODO: 在此添加命令处理程序代码
	printf("OnSetSave()\n");
    if (m_pWorkImg==NULL)
    {
		return;
    }
	CString strTmp;

	CMarkupMSXML xml;
	xml.SetDoc("<annotation>\r\n</annotation>");

	//头信息
	xml.FindChildElem("annotation");
	xml.AddChildElem("folder","VOC2007");
	xml.AddChildElem("filename",m_filename+"."+m_fileext);
	
	xml.AddChildElem("source");
	xml.IntoElem();
	xml.AddChildElem("database","The VOC2007 Database");
	xml.AddChildElem("annotation","PASCAL VOC2007");
	xml.AddChildElem("image","flickr");
	xml.OutOfElem();

	//图像信息
	xml.AddChildElem("size");
	xml.IntoElem();
    strTmp.Format("%d",m_pWorkImg->width);
	xml.AddChildElem("width",strTmp);
	strTmp.Format("%d",m_pWorkImg->height);
	xml.AddChildElem("height",strTmp);
	strTmp.Format("%d",m_pWorkImg->nChannels);
	xml.AddChildElem("depth",strTmp);
	xml.OutOfElem();

	//segmented字段
	xml.AddChildElem("segmented","0");

    //目标
	for (unsigned int i=0;i<m_ObjList.size();i++)
	{
		if (FALSE == m_ObjList[i].enable)
		{
			continue;
		}
		xml.AddChildElem("object");
		xml.IntoElem();
		//name
		xml.AddChildElem("name",objclass[m_ObjList[i].iobjclass]);
		//pose
		xml.AddChildElem("pose",g_pose[m_ObjList[i].pose]);
		//遮挡
		strTmp.Format("%d",(int)m_ObjList[i].truncated);
		xml.AddChildElem("truncated",strTmp);
		//遮挡
		strTmp.Format("%d", (int)m_ObjList[i].occluded);
		xml.AddChildElem("occluded",strTmp);
		//dif
		xml.AddChildElem("difficult","0");
		//Rect
		xml.AddChildElem("ischange", (int)m_ObjList[i].ischange);
		xml.AddChildElem("bndbox");
		
		xml.IntoElem();

		m_ObjList[i].objbox.left =MIN(m_pWorkImg->width-1,MAX(0,m_ObjList[i].objbox.left));
		m_ObjList[i].objbox.top =MIN(m_pWorkImg->height-1,MAX(0,m_ObjList[i].objbox.top));
		m_ObjList[i].objbox.right =MIN(m_pWorkImg->width-1,MAX(m_ObjList[i].objbox.left,m_ObjList[i].objbox.right));
		m_ObjList[i].objbox.bottom =MIN(m_pWorkImg->height-1,MAX(m_ObjList[i].objbox.top,m_ObjList[i].objbox.bottom));

		strTmp.Format("%d",m_ObjList[i].objbox.left);
		xml.AddChildElem("xmin",strTmp);
		strTmp.Format("%d",m_ObjList[i].objbox.top);
		xml.AddChildElem("ymin",strTmp);
		strTmp.Format("%d",m_ObjList[i].objbox.right);
		xml.AddChildElem("xmax",strTmp);
		strTmp.Format("%d",m_ObjList[i].objbox.bottom);
		xml.AddChildElem("ymax",strTmp);
		xml.OutOfElem();
		
		//part
		for (unsigned int j=0;j<m_ObjList[i].partlist.size() ;j++)
		{
			if (FALSE == m_ObjList[i].partlist[j].enable)
			{
				continue;
			}
			xml.AddChildElem("part");
			xml.IntoElem();
			xml.AddChildElem("name",g_part[m_ObjList[i].partlist[j].ipartclass]);
			xml.AddChildElem("bndbox");
			
			xml.IntoElem();

			m_ObjList[i].partlist[j].partrect.left =MIN(m_ObjList[i].objbox.right,MAX(m_ObjList[i].objbox.left,m_ObjList[i].partlist[j].partrect.left));
			m_ObjList[i].partlist[j].partrect.top =MIN(m_ObjList[i].objbox.bottom,MAX(m_ObjList[i].objbox.top,m_ObjList[i].partlist[j].partrect.top));
			m_ObjList[i].partlist[j].partrect.right =MIN(m_ObjList[i].objbox.right,MAX(m_ObjList[i].partlist[j].partrect.left,m_ObjList[i].partlist[j].partrect.right));
			m_ObjList[i].partlist[j].partrect.bottom =MIN(m_ObjList[i].objbox.bottom,MAX(m_ObjList[i].partlist[j].partrect.top,m_ObjList[i].partlist[j].partrect.bottom));

			strTmp.Format("%d",m_ObjList[i].partlist[j].partrect.left);
			xml.AddChildElem("xmin",strTmp);
			strTmp.Format("%d",m_ObjList[i].partlist[j].partrect.top);
			xml.AddChildElem("ymin",strTmp);
			strTmp.Format("%d",m_ObjList[i].partlist[j].partrect.right);
			xml.AddChildElem("xmax",strTmp);
			strTmp.Format("%d",m_ObjList[i].partlist[j].partrect.bottom);
			xml.AddChildElem("ymax",strTmp);
			//xml.AddChildElem("color", m_ObjList[i].partlist[j].color);
			
			xml.OutOfElem();
			xml.OutOfElem();

		}
		xml.OutOfElem();
	}
	//for (unsigned int i = 0; i<m_Polygon.size(); i++)
	//{
	//	if (FALSE == m_Polygon[i].enable)
	//	{
	//		continue;
	//	}
	//	xml.AddChildElem("object");
	//	xml.IntoElem();
	//	//name
	//	xml.AddChildElem("name", objclass[m_Polygon[i].iobjclass]);
	//	//pose
	//	xml.AddChildElem("pose", g_pose[m_Polygon[i].pose]);
	//	//遮挡
	//	strTmp.Format("%d", m_Polygon[i].truncated);
	//	xml.AddChildElem("truncated", strTmp);
	//	//遮挡
	//	strTmp.Format("%d", m_Polygon[i].occluded);
	//	xml.AddChildElem("occluded", strTmp);
	//	//dif
	//	xml.AddChildElem("difficult", "0");
	//	//Rect
	//	xml.AddChildElem("polygon");
	//	xml.IntoElem();

	//	for (unsigned int j = 0; j < m_Polygon[i].objpolygon.size(); j++)
	//	{
	//		strTmp.Format("%d,%d", m_Polygon[i].objpolygon[j].x, m_Polygon[i].objpolygon[j].y);
	//		xml.AddChildElem("corner", strTmp);
	//	}
	//	xml.OutOfElem();

	//	//part
	//	/*for (unsigned int j = 0; j<m_Polygon[i].partlist.size(); j++)
	//	{
	//		if (FALSE == m_Polygon[i].partlist[j].enable)
	//		{
	//			continue;
	//		}
	//		xml.AddChildElem("part");
	//		xml.IntoElem();
	//		xml.AddChildElem("name", g_part[m_Polygon[i].partlist[j].ipartclass]);
	//		xml.AddChildElem("bndbox");

	//		xml.IntoElem();

	//		m_Polygon[i].partlist[j].partrect.left = MIN(m_Polygon[i].objbox.right, MAX(m_Polygon[i].objbox.left, m_Polygon[i].partlist[j].partrect.left));
	//		m_Polygon[i].partlist[j].partrect.top = MIN(m_Polygon[i].objbox.bottom, MAX(m_Polygon[i].objbox.top, m_Polygon[i].partlist[j].partrect.top));
	//		m_Polygon[i].partlist[j].partrect.right = MIN(m_Polygon[i].objbox.right, MAX(m_Polygon[i].partlist[j].partrect.left, m_Polygon[i].partlist[j].partrect.right));
	//		m_Polygon[i].partlist[j].partrect.bottom = MIN(m_Polygon[i].objbox.bottom, MAX(m_Polygon[i].partlist[j].partrect.top, m_Polygon[i].partlist[j].partrect.bottom));

	//		strTmp.Format("%d", m_Polygon[i].partlist[j].partrect.left);
	//		xml.AddChildElem("xmin", strTmp);
	//		strTmp.Format("%d", m_Polygon[i].partlist[j].partrect.top);
	//		xml.AddChildElem("ymin", strTmp);
	//		strTmp.Format("%d", m_Polygon[i].partlist[j].partrect.right);
	//		xml.AddChildElem("xmax", strTmp);
	//		strTmp.Format("%d", m_Polygon[i].partlist[j].partrect.bottom);
	//		xml.AddChildElem("ymax", strTmp);
	//		xml.AddChildElem("color", m_Polygon[i].partlist[j].color);

	//		xml.OutOfElem();
	//		xml.OutOfElem();

	//	}*/
	//	xml.OutOfElem();
	//}
	CString strCurpath = GetCurPath();
	SetSysPath();
	CString savefile;
	savefile=".\\data\\"+m_filename+".xml";
	//remove(savefile);
	xml.Save(savefile);
	SetCurrentDirectory(strCurpath);
	printf("OnSetSave()完成\n");
	return;

}
void CImageSnapDoc::OnSetSave2()
{
	// TODO: 在此添加命令处理程序代码

	CString strTmp;

	CMarkupMSXML xml;
	xml.SetDoc("<annotation>\r\n</annotation>");
	//头信息
	xml.FindChildElem("annotation");
	xml.AddChildElem("folder", "VOC2007");
	xml.AddChildElem("filename", m_filename + "." + "jpg");

	xml.AddChildElem("source");
	xml.IntoElem();
	xml.AddChildElem("database", "The VOC2007 Database");
	xml.AddChildElem("annotation", "PASCAL VOC2007");
	xml.AddChildElem("image", "flickr");
	xml.OutOfElem();

	//图像信息
	xml.AddChildElem("size");
	xml.IntoElem();
	strTmp.Format("%d", m_width);
	xml.AddChildElem("width", strTmp);
	strTmp.Format("%d", m_height);
	xml.AddChildElem("height", strTmp);
	strTmp.Format("%d", m_depth);
	xml.AddChildElem("depth", strTmp);
	xml.OutOfElem();

	//segmented字段
	xml.AddChildElem("segmented", "0");

	//目标
	for (unsigned int i = 0; i < m_ObjList.size(); i++)
	{
		if (FALSE == m_ObjList[i].enable)
		{
			continue;
		}
		xml.AddChildElem("object");
		xml.IntoElem();
		//name
		xml.AddChildElem("name", objclass[m_ObjList[i].iobjclass]);
		//pose
		xml.AddChildElem("pose", g_pose[m_ObjList[i].pose]);
		//遮挡
		strTmp.Format("%d", (int)m_ObjList[i].truncated);
		xml.AddChildElem("truncated", strTmp);
		//遮挡
		strTmp.Format("%d", (int)m_ObjList[i].occluded);
		xml.AddChildElem("occluded", strTmp);
		//dif
		xml.AddChildElem("difficult", "0");
		//Rect
		xml.AddChildElem("ischange", (int)m_ObjList[i].ischange);
		xml.AddChildElem("bndbox");
		xml.IntoElem();

		m_ObjList[i].objbox.left = MIN(m_width - 1, MAX(0, m_ObjList[i].objbox.left));
		m_ObjList[i].objbox.top = MIN(m_height - 1, MAX(0, m_ObjList[i].objbox.top));
		m_ObjList[i].objbox.right = MIN(m_width - 1, MAX(m_ObjList[i].objbox.left, m_ObjList[i].objbox.right));
		m_ObjList[i].objbox.bottom = MIN(m_height - 1, MAX(m_ObjList[i].objbox.top, m_ObjList[i].objbox.bottom));

		strTmp.Format("%d", m_ObjList[i].objbox.left);
		xml.AddChildElem("xmin", strTmp);
		strTmp.Format("%d", m_ObjList[i].objbox.top);
		xml.AddChildElem("ymin", strTmp);
		strTmp.Format("%d", m_ObjList[i].objbox.right);
		xml.AddChildElem("xmax", strTmp);
		strTmp.Format("%d", m_ObjList[i].objbox.bottom);
		xml.AddChildElem("ymax", strTmp);
		xml.OutOfElem();

		//part
		for (unsigned int j = 0; j < m_ObjList[i].partlist.size(); j++)
		{
			if (FALSE == m_ObjList[i].partlist[j].enable)
			{
				continue;
			}
			xml.AddChildElem("part");
			xml.IntoElem();
			xml.AddChildElem("name", g_part[m_ObjList[i].partlist[j].ipartclass]);
			xml.AddChildElem("bndbox");

			xml.IntoElem();

			m_ObjList[i].partlist[j].partrect.left = MIN(m_ObjList[i].objbox.right, MAX(m_ObjList[i].objbox.left, m_ObjList[i].partlist[j].partrect.left));
			m_ObjList[i].partlist[j].partrect.top = MIN(m_ObjList[i].objbox.bottom, MAX(m_ObjList[i].objbox.top, m_ObjList[i].partlist[j].partrect.top));
			m_ObjList[i].partlist[j].partrect.right = MIN(m_ObjList[i].objbox.right, MAX(m_ObjList[i].partlist[j].partrect.left, m_ObjList[i].partlist[j].partrect.right));
			m_ObjList[i].partlist[j].partrect.bottom = MIN(m_ObjList[i].objbox.bottom, MAX(m_ObjList[i].partlist[j].partrect.top, m_ObjList[i].partlist[j].partrect.bottom));

			strTmp.Format("%d", m_ObjList[i].partlist[j].partrect.left);
			xml.AddChildElem("xmin", strTmp);
			strTmp.Format("%d", m_ObjList[i].partlist[j].partrect.top);
			xml.AddChildElem("ymin", strTmp);
			strTmp.Format("%d", m_ObjList[i].partlist[j].partrect.right);
			xml.AddChildElem("xmax", strTmp);
			strTmp.Format("%d", m_ObjList[i].partlist[j].partrect.bottom);
			xml.AddChildElem("ymax", strTmp);
			//xml.AddChildElem("color", m_ObjList[i].partlist[j].color);

			xml.OutOfElem();
			xml.OutOfElem();

		}
		xml.OutOfElem();
	}
	CString strCurpath = GetCurPath();
	SetSysPath();
	CString savefile;
	savefile = ".\\data\\" + m_filename + ".xml";
	//remove(savefile);
	xml.Save(savefile);
	SetCurrentDirectory(strCurpath);
	printf("OnSetSave()完成\n");
	return;

}
BOOL CImageSnapDoc::ReadJsonInfo(CString filename)
{
	using namespace Json;
	if (filename.IsEmpty())
	{
		return FALSE;
	}
	CString strCurPath = GetCurPath();


	SetSysPath();
	if (-1 == filename.Find("json",0))
	{
		strXmlFile = ".\\data\\" + filename + ".json";
	}
	else
	{
		strXmlFile = filename;
	}

	CFileFind   find;
	if (!find.FindFile(strXmlFile))
		return FALSE;
	

	ifstream ifs;
	ifs.open(strXmlFile); 
	Value root;
	CharReaderBuilder fh;
	JSONCPP_STRING errs;
	parseFromStream(fh, ifs, &root, &errs);


	ifstream ifs2;
	string setpath = "./set.json";
	ifs2.open(setpath);
	Value setroot;
	CharReaderBuilder fh2;
	JSONCPP_STRING errs2;
	parseFromStream(fh2, ifs2, &setroot, &errs2);

	if (!root.isMember("name"))
	{
		AfxMessageBox("文件名不符合");
		return FALSE;
	}
	if (root.isMember("imagelabel"))
	{
		if (root["imagelabel"].isObject())
		{
			Value imagelabel = root["imagelabel"];
			if (imagelabel.isMember("width") && imagelabel["width"].isInt()&&imagelabel.isMember("height") && imagelabel["height"].isInt()&&imagelabel.isMember("depth") && imagelabel["depth"].isInt())
			{
				int width = imagelabel["width"].asInt();
				int height = imagelabel["height"].asInt();
				int depth = imagelabel["depth"].asInt();
				if ((width != m_pWorkImg->width )|( height != m_pWorkImg->height )|( depth != m_pWorkImg->depth))
				{
					AfxMessageBox("图像尺寸不符合");
					return FALSE;
				}
			}
			if (imagelabel.isMember("property") && imagelabel["property"].isObject())
			{
				Value img_property = imagelabel["property"];
				Value::Members propertyMember = img_property.getMemberNames();
				for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
				{
					ss_property Set;
					Set.property_id = img_property[*it].asInt();
					Set.property_name = *it;
					if (setroot.isMember("image"))
					{
						if (setroot["image"][*it].isArray())
						{
							Set.property_options = setroot["image"][*it][Set.property_id].asString();
						}
						else if (setroot["image"][*it].isInt())
						{
							Set.property_value = setroot["image"][*it].asInt();
						}
					}

					m_Image.o_property.push_back(Set);
				}
			}
			if (imagelabel.isMember("Object") && imagelabel["Object"].isArray())
			{
				Value Object = imagelabel["Object"];
				int ob_size = Object.size();
				for (int i = 0; i < ob_size; i++)
				{
					if (Object[i].isObject())
					{
						if (Object[i].isMember("class") && Object[i]["class"].isString())
						{
							string classname = Object[i]["class"].asString();
							if (classname == "bndbox")
							{
								_Object objTmp;
								if (Object[i].isMember("ischange") && Object[i]["ischange"].isInt())
								{
									objTmp.ischange = Object[i]["ischange"].asInt();
								}
								if (Object[i].isMember("name") && Object[i]["name"].isString())
								{
									CString name = Object[i]["name"].asCString();
									objTmp.classname = Object[i]["name"].asString();
									objTmp.enable = TRUE;
									for (unsigned int j = 0; j < lines_en.size(); j++)
									{
										CString Comparename = lines_en[j].second.c_str();
										if (0 == name.CompareNoCase(Comparename))
										{
											objTmp.iobjclass = j;
											break;
										}
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];
									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}
											
										objTmp.o_property.push_back(Set);
									}
									
								}
								objTmp.difficult = TRUE;
								objTmp.occluded = FALSE;
								objTmp.truncated = FALSE;
								objTmp.pose = 0;
								if (Object[i].isMember("rect") && Object[i]["rect"].isObject())
								{
									Value v_rect = Object[i]["rect"];
									if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
									{
										objTmp.objbox.left = v_rect["p1"][0].asInt();
										objTmp.objbox.top = v_rect["p1"][1].asInt();
										objTmp.objbox.right = v_rect["p2"][0].asInt();
										objTmp.objbox.bottom = v_rect["p2"][1].asInt();
									}
								}
								if (Object[i].isMember("part") && Object[i]["part"].isArray())
								{
									_Part partTmp;
									partTmp.enable = TRUE;
									partTmp.ipartclass = 0;
									partTmp.partrect.SetRectEmpty();
									Value v_part = Object[i]["part"];
									for (unsigned int j = 0; j < v_part.size(); j++)
									{
										if (v_part[j].isObject())
										{
											if (v_part[j].isMember("class") && v_part[j]["class"].isString())
											{
												string part_class = v_part[j]["class"].asString();
												if (part_class == "bndbox")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("rect") && v_part[j]["rect"].isObject())
													{
														Value v_rect = v_part[j]["rect"];
														if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
														{
															partTmp.partrect.left = v_rect["p1"][0].asInt64();
															partTmp.partrect.top = v_rect["p1"][1].asInt64();
															partTmp.partrect.right = v_rect["p2"][0].asInt64();
															partTmp.partrect.bottom = v_rect["p2"][1].asInt64();
															partTmp.ablerect = TRUE;
														}
													}
												}
												else if (part_class == "polygon")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("polygon") && v_part[j]["polygon"].isArray())
													{
														Value v_polygon = v_part[j]["polygon"];
														for (unsigned int k = 0; k < v_polygon.size(); k++)
														{
															Point v_point(v_polygon[k]["x"].asInt(), v_polygon[k]["y"].asInt());
															partTmp.polygon.push_back(v_point);
															partTmp.ablerect = FALSE;
														}
													}
												}
											}
										}
										objTmp.partlist.push_back(partTmp);
									}
								}
								
								m_ObjList.push_back(objTmp);
							}
							else if (classname == "polygon")
							{
								_Polygon obpoly;
								CString name = Object[i]["name"].asCString();
								obpoly.classname = Object[i]["name"].asString();
								obpoly.enable = TRUE;
								for (unsigned int j = 0; j < lines_en.size(); j++)
								{
									CString Comparename = lines_en[j].second.c_str();
									if (0 == name.CompareNoCase(Comparename))
									{
										obpoly.iobjclass = j;
										break;
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];
									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}
										obpoly.o_property.push_back(Set);
									}
									
								}
								obpoly.difficult = TRUE;
								obpoly.occluded = FALSE;
								obpoly.truncated = FALSE;
								obpoly.pose = 0;
								if (Object[i].isMember("polygon") && Object[i]["polygon"].isArray())
								{
									Value polygon = Object[i]["polygon"];
									for (unsigned int j = 0; j < polygon.size(); j++)
									{
										Point v_point(polygon[j]["x"].asInt(), polygon[j]["y"].asInt());
										obpoly.objpolygon.push_back(v_point);
									}
								}
								if (Object[i].isMember("part") && Object[i]["part"].isArray())
								{
									_Part partTmp;
									partTmp.enable = 1;
									partTmp.ipartclass = 0;
									partTmp.partrect.SetRectEmpty();
									Value v_part = Object[i]["part"];
									for (unsigned int j = 0; j < v_part.size(); j++)
									{
										if (v_part[j].isObject())
										{
											if (v_part[j].isMember("class") && v_part[j]["class"].isString())
											{
												string part_class = v_part[j]["class"].asString();
												if (part_class == "bndbox")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("rect") && v_part[j]["rect"].isObject())
													{
														Value v_rect = v_part[j]["rect"];
														if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
														{
															partTmp.partrect.left = v_rect["p1"][0].asInt64();
															partTmp.partrect.top = v_rect["p1"][1].asInt64();
															partTmp.partrect.right = v_rect["p2"][0].asInt64();
															partTmp.partrect.bottom = v_rect["p2"][1].asInt64();
															partTmp.ablerect = TRUE;
														}
													}
												}
												else if (part_class == "polygon")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("polygon") && v_part[j]["polygon"].isArray())
													{
														Value v_polygon = v_part[j]["polygon"];
														for (unsigned int k = 0; k < v_polygon.size(); k++)
														{
															Point v_point(v_polygon[k]["x"].asInt(), v_polygon[k]["y"].asInt());
															partTmp.polygon.push_back(v_point);
															partTmp.ablerect = FALSE;
														}
													}
												}
											}
										}
										obpoly.partlist.push_back(partTmp);
									}
								}

								m_Polygon.push_back(obpoly);

							}
							else if (classname == "curve")
							{
								_Curve obcurve;
								CString name = Object[i]["name"].asCString();
								obcurve.classname = Object[i]["name"].asString();
								obcurve.enable = TRUE;
								for (unsigned int j = 0; j < lines_en.size(); j++)
								{
									CString Comparename = lines_en[j].second.c_str();
									if (0 == name.CompareNoCase(Comparename))
									{
										obcurve.iobjclass = j;
										break;
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];

									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}
										obcurve.o_property.push_back(Set);
									}
									
								}
								obcurve.difficult = TRUE;
								obcurve.occluded = FALSE;
								obcurve.truncated = FALSE;
								obcurve.pose = 0;
								if (Object[i].isMember("point") && Object[i]["point"].isObject())
								{
									Value v_curve = Object[i]["point"];
									int curve_point_size = Object[i]["point"].size();
									int kmax = curve_point_size / 6;
									for (int k = 1; k < kmax + 1; k++)
									{
										CurvePoint cp;
										string num = to_string(k);
										string pxname, pyname, c1xname, c1yname, c2xname, c2yname;
										pxname = "x" + num;
										pyname = "y" + num;
										c1xname = pxname + "_c1";
										c1yname = pyname + "_c1";
										c2xname = pxname + "_c2";
										c2yname = pyname + "_c2";
										cp.p.x = v_curve[pxname].asInt();
										cp.p.y = v_curve[pyname].asInt();
										cp.c1.x = v_curve[c1xname].asInt();
										cp.c1.y = v_curve[c1yname].asInt();
										cp.c2.x = v_curve[c2xname].asInt();
										cp.c2.y = v_curve[c2yname].asInt();
										obcurve.objcurve.push_back(cp);
									}
									m_Curve.push_back(obcurve);
								}
								
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CImageSnapDoc::ReadJsonInfo2(CString filename)
{
	using namespace Json;
	if (filename.IsEmpty())
	{
		return FALSE;
	}
	CString strCurPath = GetCurPath();
	m_ObjList.clear();
	m_Curve.clear();
	m_Polygon.clear();
	m_Image.o_property.clear();
	SetSysPath();
	strXmlFile = ".\\data\\" + filename;
	int m = filename.ReverseFind('.');
	if (m != -1)
	{
		m_filename = filename.Left(m);
	}
	CFileFind   find;
	if (!find.FindFile(strXmlFile))
		return FALSE;


	ifstream ifs;
	ifs.open(strXmlFile);
	Value root;
	CharReaderBuilder fh;
	JSONCPP_STRING errs;
	parseFromStream(fh, ifs, &root, &errs);


	ifstream ifs2;
	string setpath = "./set.json";
	ifs2.open(setpath);
	Value setroot;
	CharReaderBuilder fh2;
	JSONCPP_STRING errs2;
	parseFromStream(fh2, ifs2, &setroot, &errs2);

	if (!root.isMember("name"))
	{
		AfxMessageBox("文件名不符合");
		return FALSE;
	}
	if (root.isMember("imagelabel"))
	{
		if (root["imagelabel"].isObject())
		{
			Value imagelabel = root["imagelabel"];
			if (imagelabel.isMember("width") && imagelabel["width"].isInt() && imagelabel.isMember("height") && imagelabel["height"].isInt() && imagelabel.isMember("depth") && imagelabel["depth"].isInt())
			{
				m_width = imagelabel["width"].asInt();
				m_height = imagelabel["height"].asInt();
				m_depth = imagelabel["depth"].asInt();
			}
			if (imagelabel.isMember("property") && imagelabel["property"].isObject())
			{
				Value img_property = imagelabel["property"];
				Value::Members propertyMember = img_property.getMemberNames();
				for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
				{
					ss_property Set;
					Set.property_id = img_property[*it].asInt();
					Set.property_name = *it;
					if (setroot.isMember("image"))
					{
						if (setroot["image"][*it].isArray())
						{
							Set.property_options = setroot["image"][*it][Set.property_id].asString();
						}
						else if (setroot["image"][*it].isInt())
						{
							Set.property_value = setroot["image"][*it].asInt();
						}
					}

					m_Image.o_property.push_back(Set);
				}
			}
			if (imagelabel.isMember("Object") && imagelabel["Object"].isArray())
			{
				Value Object = imagelabel["Object"];
				int ob_size = Object.size();
				for (int i = 0; i < ob_size; i++)
				{
					if (Object[i].isObject())
					{
						if (Object[i].isMember("class") && Object[i]["class"].isString())
						{
							string classname = Object[i]["class"].asString();
							if (classname == "bndbox")
							{
								_Object objTmp;
								if (Object[i].isMember("ischange") && Object[i]["ischange"].isInt())
								{
									objTmp.ischange = Object[i]["ischange"].asInt();
								}
								if (Object[i].isMember("name") && Object[i]["name"].isString())
								{
									CString name = Object[i]["name"].asCString();
									objTmp.classname = Object[i]["name"].asString();
									objTmp.enable = TRUE;
									for (unsigned int j = 0; j < lines_en.size(); j++)
									{
										CString Comparename = lines_en[j].second.c_str();
										if (0 == name.CompareNoCase(Comparename))
										{
											objTmp.iobjclass = j;
											break;
										}
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];
									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}

										objTmp.o_property.push_back(Set);
									}

								}
								objTmp.difficult = TRUE;
								objTmp.occluded = FALSE;
								objTmp.truncated = FALSE;
								objTmp.pose = 0;
								if (Object[i].isMember("rect") && Object[i]["rect"].isObject())
								{
									Value v_rect = Object[i]["rect"];
									if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
									{
										objTmp.objbox.left = v_rect["p1"][0].asInt();
										objTmp.objbox.top = v_rect["p1"][1].asInt();
										objTmp.objbox.right = v_rect["p2"][0].asInt();
										objTmp.objbox.bottom = v_rect["p2"][1].asInt();
									}
								}
								if (Object[i].isMember("part") && Object[i]["part"].isArray())
								{
									_Part partTmp;
									partTmp.enable = TRUE;
									partTmp.ipartclass = 0;
									partTmp.partrect.SetRectEmpty();
									Value v_part = Object[i]["part"];
									for (unsigned int j = 0; j < v_part.size(); j++)
									{
										if (v_part[j].isObject())
										{
											if (v_part[j].isMember("class") && v_part[j]["class"].isString())
											{
												string part_class = v_part[j]["class"].asString();
												if (part_class == "bndbox")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];

														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("rect") && v_part[j]["rect"].isObject())
													{
														Value v_rect = v_part[j]["rect"];
														if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
														{
															partTmp.partrect.left = v_rect["p1"][0].asInt64();
															partTmp.partrect.top = v_rect["p1"][1].asInt64();
															partTmp.partrect.right = v_rect["p2"][0].asInt64();
															partTmp.partrect.bottom = v_rect["p2"][1].asInt64();
															partTmp.ablerect = TRUE;
														}
													}
												}
												else if (part_class == "polygon")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("polygon") && v_part[j]["polygon"].isArray())
													{
														Value v_polygon = v_part[j]["polygon"];
														for (unsigned int k = 0; k < v_polygon.size(); k++)
														{
															Point v_point(v_polygon[k]["x"].asInt(), v_polygon[k]["y"].asInt());
															partTmp.polygon.push_back(v_point);
															partTmp.ablerect = FALSE;
														}
													}
												}
											}
										}
										objTmp.partlist.push_back(partTmp);
									}
								}

								m_ObjList.push_back(objTmp);
							}
							else if (classname == "polygon")
							{
								_Polygon obpoly;
								CString name = Object[i]["name"].asCString();
								obpoly.classname = Object[i]["name"].asString();
								obpoly.enable = TRUE;
								for (unsigned int j = 0; j < lines_en.size(); j++)
								{
									CString Comparename = lines_en[j].second.c_str();
									if (0 == name.CompareNoCase(Comparename))
									{
										obpoly.iobjclass = j;
										break;
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];
									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}
										obpoly.o_property.push_back(Set);
									}

								}
								obpoly.difficult = TRUE;
								obpoly.occluded = FALSE;
								obpoly.truncated = FALSE;
								obpoly.pose = 0;
								if (Object[i].isMember("polygon") && Object[i]["polygon"].isArray())
								{
									Value polygon = Object[i]["polygon"];
									for (unsigned int j = 0; j < polygon.size(); j++)
									{
										Point v_point(polygon[j]["x"].asInt(), polygon[j]["y"].asInt());
										obpoly.objpolygon.push_back(v_point);
									}
								}
								if (Object[i].isMember("part") && Object[i]["part"].isArray())
								{
									_Part partTmp;
									partTmp.enable = 1;
									partTmp.ipartclass = 0;
									partTmp.partrect.SetRectEmpty();
									Value v_part = Object[i]["part"];
									for (unsigned int j = 0; j < v_part.size(); j++)
									{
										if (v_part[j].isObject())
										{
											if (v_part[j].isMember("class") && v_part[j]["class"].isString())
											{
												string part_class = v_part[j]["class"].asString();
												if (part_class == "bndbox")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("rect") && v_part[j]["rect"].isObject())
													{
														Value v_rect = v_part[j]["rect"];
														if (v_rect.isMember("p1") && v_rect["p1"].isArray() && v_rect.isMember("p2") && v_rect["p2"].isArray())
														{
															partTmp.partrect.left = v_rect["p1"][0].asInt64();
															partTmp.partrect.top = v_rect["p1"][1].asInt64();
															partTmp.partrect.right = v_rect["p2"][0].asInt64();
															partTmp.partrect.bottom = v_rect["p2"][1].asInt64();
															partTmp.ablerect = TRUE;
														}
													}
												}
												else if (part_class == "polygon")
												{
													if (v_part[j].isMember("name") && v_part[j]["name"].isString())
													{
														CString name = v_part[j]["name"].asCString();
														for (unsigned int i = 0; i < NUM_PART; i++)
														{
															if (0 == name.CompareNoCase(g_part[i]))
															{
																partTmp.ipartclass = i;
																break;
															}
														}
													}
													if (v_part[j].isMember("property") && v_part[j]["property"].isObject())
													{
														Value v_p_property = v_part[j]["property"];
														/*partTmp.truncated = v_p_property["truncated"];
														partTmp.occluded = v_p_property["occluded"];
														partTmp.difficult = v_p_property["difficult"];
														partTmp.pose = v_p_property["pose"];*/
													}
													if (v_part[j].isMember("polygon") && v_part[j]["polygon"].isArray())
													{
														Value v_polygon = v_part[j]["polygon"];
														for (unsigned int k = 0; k < v_polygon.size(); k++)
														{
															Point v_point(v_polygon[k]["x"].asInt(), v_polygon[k]["y"].asInt());
															partTmp.polygon.push_back(v_point);
															partTmp.ablerect = FALSE;
														}
													}
												}
											}
										}
										obpoly.partlist.push_back(partTmp);
									}
								}

								m_Polygon.push_back(obpoly);

							}
							else if (classname == "curve")
							{
								_Curve obcurve;
								CString name = Object[i]["name"].asCString();
								obcurve.classname = Object[i]["name"].asString();
								obcurve.enable = TRUE;
								for (unsigned int j = 0; j < lines_en.size(); j++)
								{
									CString Comparename = lines_en[j].second.c_str();
									if (0 == name.CompareNoCase(Comparename))
									{
										obcurve.iobjclass = j;
										break;
									}
								}
								if (Object[i].isMember("property") && Object[i]["property"].isObject())
								{
									Value v_property = Object[i]["property"];

									Value::Members propertyMember = v_property.getMemberNames();
									for (Value::Members::iterator it = propertyMember.begin(); it != propertyMember.end(); ++it)
									{
										ss_property Set;
										string name = Object[i]["name"].asString();
										Set.property_id = v_property[*it].asInt();
										Set.property_name = *it;
										if (setroot.isMember(name))
										{
											if (setroot[name][*it].isArray())
											{
												Set.property_options = setroot[name][*it][Set.property_id].asString();
											}
											else if (setroot[name][*it].isInt())
											{
												Set.property_value = v_property[*it].asInt();
											}
										}
										obcurve.o_property.push_back(Set);
									}

								}
								obcurve.difficult = TRUE;
								obcurve.occluded = FALSE;
								obcurve.truncated = FALSE;
								obcurve.pose = 0;
								if (Object[i].isMember("point") && Object[i]["point"].isObject())
								{
									Value v_curve = Object[i]["point"];
									int curve_point_size = Object[i]["point"].size();
									int kmax = curve_point_size / 6;
									for (int k = 1; k < kmax + 1; k++)
									{
										CurvePoint cp;
										string num = to_string(k);
										string pxname, pyname, c1xname, c1yname, c2xname, c2yname;
										pxname = "x" + num;
										pyname = "y" + num;
										c1xname = pxname + "_c1";
										c1yname = pyname + "_c1";
										c2xname = pxname + "_c2";
										c2yname = pyname + "_c2";
										cp.p.x = v_curve[pxname].asInt();
										cp.p.y = v_curve[pyname].asInt();
										cp.c1.x = v_curve[c1xname].asInt();
										cp.c1.y = v_curve[c1yname].asInt();
										cp.c2.x = v_curve[c2xname].asInt();
										cp.c2.y = v_curve[c2yname].asInt();
										obcurve.objcurve.push_back(cp);
									}
									m_Curve.push_back(obcurve);
								}

							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}
//filename不包括扩展名
void CImageSnapDoc::ReadXmlInfo(CString filename)
{
	if (filename.IsEmpty())
	{
		return;
	}
	//CString strXmlFile;
	CString strCurPath = GetCurPath();
	m_ObjList.clear();
	m_Curve.clear();
	m_Polygon.clear();
	m_Image.o_property.clear();

	SetSysPath();
	strXmlFile = ".\\data\\"+filename;
	int m = filename.ReverseFind('.');
	if (m != -1)
	{
		m_filename = filename.Left(m);
	}
	CFileFind   find; 
	if   (!find.FindFile( strXmlFile))
		return;

	//加载xml文件
	CMarkupMSXML xml;
	BOOL b = xml.Load(strXmlFile);

	SetCurrentDirectory(strCurPath);

	int isfind = 0;
	//检查关键字段,文件名
	if(!xml.FindChildElem("filename"))
	{
		AfxMessageBox("文件名不符合");
		return;
	}

	if(xml.FindChildElem("size"))
	{
		xml.IntoElem();
		xml.FindChildElem("width");
		m_width = atoi(xml.GetChildData());
		xml.FindChildElem("height");
		m_height = atoi(xml.GetChildData());
		xml.FindChildElem("depth");
		m_depth = atoi(xml.GetChildData());
		//if (width!=m_pWorkImg->width || height!=m_pWorkImg->height)
		//{
		//	AfxMessageBox("尺寸错误");
		//	AfxMessageBox("00");
		//	return;
		//}
		xml.OutOfElem();
	}
	else
	{
		AfxMessageBox("尺寸错误");
		return;
	}
	
	while (xml.FindChildElem("object"))
	{
		_Object objTmp;
		_Polygon obpoly;

		objTmp.iobjclass=0;
		objTmp.pose=0;
		objTmp.truncated=0;
		objTmp.occluded=0;
		objTmp.difficult=0;
		objTmp.objbox.SetRectEmpty();
		objTmp.enable=1;

		obpoly.difficult = 0;
		obpoly.enable = 1;
		obpoly.iobjclass = 0;
		obpoly.pose = 0;
		obpoly.truncated = 0;
		obpoly.occluded = 0;
		obpoly.objpolygon.clear();


		xml.IntoElem();
		CString name;
		CString pose;
		if (xml.FindChildElem("name"))
		{
			name = xml.GetChildData();
			objTmp.classname = name;
			for (unsigned int i = 0;i<NUM_OBJ;i++)
			{
				if (0 == name.CompareNoCase(objclass[i]))
				{
					objTmp.iobjclass = i;
					break;
				}

			}
		}
	
		
		if (xml.FindChildElem("pose"))
		{
			pose = xml.GetChildData();
			for (unsigned int i = 0;i<NUM_POSE;i++)
			{
				if (0 == pose.CompareNoCase(g_pose[i]))
				{
					objTmp.pose = i;
					break;
				}

			}
		}

		if (xml.FindChildElem("ischange"))
		{
			objTmp.ischange = atoi(xml.GetChildData());

		}
		if (xml.FindChildElem("truncated"))
		{
			objTmp.truncated = atoi(xml.GetChildData());
		}

		if (xml.FindChildElem("occluded"))
		{
			objTmp.occluded = atoi(xml.GetChildData());
		}

		if (xml.FindChildElem("difficult"))
		{
			objTmp.difficult = atoi(xml.GetChildData());
		}

		if (xml.FindChildElem("bndbox"))
		{
			xml.IntoElem();

			if (xml.FindChildElem("xmin"))
			{
				objTmp.objbox.left = atoi(xml.GetChildData());
			}

			if (xml.FindChildElem("ymin"))
			{
				objTmp.objbox.top = atoi(xml.GetChildData());
			}

			if (xml.FindChildElem("xmax"))
			{
				objTmp.objbox.right = atoi(xml.GetChildData());
			}

			if (xml.FindChildElem("ymax"))
			{
				objTmp.objbox.bottom = atoi(xml.GetChildData());
			}

			xml.OutOfElem();
			
		}


		while (xml.FindChildElem("part"))
		{
			xml.IntoElem();
			_Part partTmp;
			partTmp.enable =1;
			partTmp.ipartclass = 0;
			partTmp.partrect.SetRectEmpty();

			CString name;
			if (xml.FindChildElem("name"))
			{
				name = xml.GetChildData();
				for (unsigned int i = 0;i<NUM_PART;i++)
				{
					if (0 == name.CompareNoCase(g_part[i]))
					{
						partTmp.ipartclass = i;
						break;
					}

				}
			}

			if (xml.FindChildElem("bndbox"))
			{
				xml.IntoElem();

				if (xml.FindChildElem("xmin"))
				{
					partTmp.partrect.left = atoi(xml.GetChildData());
				}

				if (xml.FindChildElem("ymin"))
				{
					partTmp.partrect.top = atoi(xml.GetChildData());
				}

				if (xml.FindChildElem("xmax"))
				{
					partTmp.partrect.right = atoi(xml.GetChildData());
				}

				if (xml.FindChildElem("ymax"))
				{
					partTmp.partrect.bottom = atoi(xml.GetChildData());
				}
				xml.OutOfElem();
			}

			if (xml.FindChildElem("color"))
			{
				strcpy(partTmp.color, xml.GetChildData());
				
			}

			xml.OutOfElem();
			objTmp.partlist.push_back(partTmp);
		}
		xml.OutOfElem();
		m_ObjList.push_back(objTmp);
	}

}

void CImageSnapDoc::SetSysPath()
{
	//得到系统路径
	CHAR buf[120];
	GetModuleFileName(NULL,buf,sizeof(buf));
	CString strTemp,strAppPath;
	strTemp=buf;
	int m=strTemp.ReverseFind('\\');
	if (m != -1) 
	{
		strAppPath=strTemp.Left(m);
	}
	SetCurrentDirectory(strAppPath);
}

CString CImageSnapDoc::GetCurPath()
{
	//得到当前路径
	char pBuf[MAX_PATH];                                 //存放路径的变量      
	GetCurrentDirectory(MAX_PATH,pBuf);                   //获取程序的当前目录
	strcat(pBuf,"\\");
	CString strtemp=pBuf;
	return strtemp;
}

void CImageSnapDoc::SetModifiedFlag(BOOL bModified)
{
	m_bModified = bModified;

}
BOOL CImageSnapDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (FALSE == m_bModified)
	{
		return TRUE;
	}
	if ((m_ObjList.size() + m_Polygon.size() +m_Curve.size()+m_Image.o_property.size())!= 0)
	{

		if (IDOK == MessageBox(NULL,"是否保存当前文档","标题", MB_ICONEXCLAMATION|MB_OKCANCEL ))
		{
			_mkdir("./data/");
			OnSetSave();
			OnSaveJson();
		}

	}
	return TRUE;

	//return CDocument::SaveModified();
}


void CImageSnapDoc::GetAllFileNames(const CString& path, CStringArray& ary)
{
	ary.RemoveAll();

	CFileFind fileFind;
	CString filesPath = path + L"\\*.*";

	if (fileFind.FindFile(filesPath))
	{

		CString strPicPath;
		CString strExt;
		// 如果该目录还有文件
		while (fileFind.FindNextFile())
		{
			strPicPath = fileFind.GetFileName();
			if (fileFind.IsDirectory())
			{
				continue;
			}
			// 获取后缀名
			int m=strPicPath.ReverseFind('.');
			if (m == -1) 
			{
				continue;
		
			}
			strExt = strPicPath.Right(strPicPath.GetLength() - m -1);
			strExt.MakeLower();

			// 如果后缀名为bmp,jpg则判断是图片文件
			// 此处可以扩充功能
			if (strExt == "bmp" || strExt == "jpg" || strExt == "jpeg")
				ary.Add(strPicPath);
		}

		strPicPath = fileFind.GetFileName();
		// 获取后缀名
		// 获取后缀名
		int m=strPicPath.ReverseFind('.');
		if (m == -1) 
		{
			fileFind.Close();
			return;
			

		}
		strExt = strPicPath.Right(strPicPath.GetLength() - m - 1);
		strExt.MakeLower();

		// 如果后缀名为bmp,jpg则判断是图片文件
		// 此处可以扩充功能
		if (strExt == "bmp" || strExt == "jpg" || strExt == "jpeg")
			ary.Add(strPicPath);
	}

	fileFind.Close();
}
void CImageSnapDoc::AllJson2xml()
{
	std::vector<path_save> jsonlist;
	find_all_files("./data/", jsonlist,"json");
	for (size_t i = 0; i < jsonlist.size(); i++)
	{
		ReadJsonInfo2(jsonlist[i].file_name.c_str());
		OnSetSave2();
	}
	string message = "已经完成" + std::to_string(jsonlist.size()) + "个Json文件的转换";
	AfxMessageBox(message.c_str());
}
void CImageSnapDoc::OpenFile()
{
	printf("OpenFile()\n");
	if (1)
	{
		//printf("存储xml和json\n");
		_mkdir("./data/");
		OnSaveJson();
		OnSetSave();
		m_bModified = FALSE;
	}

	CString filename = GetFilePath(m_nPos);



	InitData();

	IplImage* pImg=NULL;
	m_ischange = 0;
	pImg = cvLoadImage(filename,1);      //  读图像文件(DSCV)
	if (!pImg) 
	{
		printf("openfile中图片读取失败\n");
		return;
	}

	if (m_pSrcImg) 
	{
		cvReleaseImage(&m_pSrcImg);
	}
	if (m_pWorkImg) 
	{
		cvReleaseImage(&m_pWorkImg);
	}
	m_pWorkImg = cvCloneImage(pImg);
	m_pSrcImg = cvCloneImage(pImg);
	cvReleaseImage(&pImg);

	//读入对应的XML文件
	int m = m_FilesNameAry[m_nPos].ReverseFind('.');
	if (m != -1)
	{
		m_filename = m_FilesNameAry[m_nPos].Left(m);
	}
	//ReadXmlInfo(m_filename);
	
	//继承上一张图rect信息
	if (m_inherit)
	{
		if (m_nPos > 0)
		{
			m = m_FilesNameAry[m_nPos - 1].ReverseFind('.');
			ReadJsonInfo(m_FilesNameAry[m_nPos - 1].Left(m));
			m_bModified = TRUE;
		}
		//m_Polygon.clear();
		//m_Curve.clear();

	}
	else
	{
		isFile = ReadJsonInfo(m_filename);
	}
	m_Display = 0;

	UpdateAllViews(NULL);
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *)pFrame->GetActiveView();
	pFrame->SetWindowText ("ImageSnap_"+ m_FilesNameAry[m_nPos]);
	if (pView->m_rect)
	{
		int curobj = m_ObjList.size() - 1;
		pFrame->m_wndProperties.UpdataAllData(m_ObjList, curobj, -1);
	}
	else if (pView->m_polygon)
	{
		int curobj = m_Polygon.size() - 1;
		pFrame->m_wndProperties.UpdataAllData(m_Polygon, curobj, -1);
	}
	else if (pView->m_curve)
	{
		int curobj = m_Curve.size() - 1;
		pFrame->m_wndProperties.UpdataAllData(m_Curve, curobj, -1);
	}


	return;

}

CString CImageSnapDoc::GetFilePath(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nPicNum);
	return m_strPath + L"\\" + m_FilesNameAry[nIndex];
}

void CImageSnapDoc::OnReMarked()
{
	// TODO: 在此添加命令处理程序代码
	/*SetCurrentDirectory(GetCurPath());
	AfxMessageBox(GetCurPath());*/
	char szAppName[MAX_PATH];
	:: GetModuleFileName(AfxGetInstanceHandle(), szAppName, MAX_PATH);
	char *p=strrchr(szAppName,'\\');
	//p++;
	*p='\0';

	char buffer[100];
	sprintf_s(buffer,100,"%s\\data\\%s.xml",szAppName,m_filename);
	//AfxMessageBox(buffer);
	CFileFind   find; 
	if (!find.FindFile( buffer))
	{
		//AfxMessageBox(GetCurPath());
		//AfxMessageBox(strXmlFile);
		return;
	}

	DeleteFile(buffer);
	OpenFile();
}

CSize CImageSnapDoc::GetDocSize() const
{
	return m_sizeDoc;
}


void CImageSnapDoc::OnInherit()
{
	// TODO:  在此添加命令处理程序代码

	m_inherit = !m_inherit;

}


void CImageSnapDoc::OnUpdateInherit(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_inherit);
}


void CImageSnapDoc::OnJson2Xml()
{
	// TODO: 在此添加命令处理程序代码
	AllJson2xml();
}


void CImageSnapDoc::OnFindchange()
{
	// TODO: 在此添加命令处理程序代码
	using namespace Json;
	std::vector<path_save> jsonlist;
	find_all_files("./data/", jsonlist,"json");
	float change = 0;
	float nochange = 0;
	for (int i = 0; i < jsonlist.size(); i++)
	{
		std::string strXmlFile =  jsonlist[i].file_path;
		ifstream ifs;
		ifs.open(strXmlFile);
		Value root;
		CharReaderBuilder fh;
		JSONCPP_STRING errs;
		parseFromStream(fh, ifs, &root, &errs);
		Value imagelabel = root["imagelabel"];
		Value Object = imagelabel["Object"];
		for (int j = 0; j < Object.size(); j++)
		{
			if (Object[j].isMember("ischange"))
			{
				if (Object[j]["ischange"].asInt() == 0)
				{
					nochange++;
				}
				else if (Object[j]["ischange"].asInt() == 1)
				{
					change++;
				}

			}
		}
	}
	float rate = change / (change + nochange);
	char message[50];
	sprintf_s(message, "%s%f", "修改率为", rate);
	AfxMessageBox(message);
}


void CImageSnapDoc::OnXml2Json()
{
	// TODO: 在此添加命令处理程序代码
	std::vector<path_save> xmllist;
	find_all_files("./data/", xmllist, "xml");
	for (size_t i = 0; i < xmllist.size(); i++)
	{
		ReadXmlInfo(xmllist[i].file_name.c_str());
		OnSaveJson2();
	}
	string message = "已经完成" + std::to_string(xmllist.size()) + "个Xml文件的转换";
	AfxMessageBox(message.c_str());
}


void CImageSnapDoc::OnAnalyse()
{
	// TODO: 在此添加命令处理程序代码
	using namespace Json;
	std::vector<path_save> jsonlist;
	find_all_files("./data/", jsonlist, "json");
	int* count = new int[lines_en.size()];
	std::string message;
	for (size_t i = 0; i < lines_en.size(); i++)
	{
		count[i] = 0;
	}
	for (int i = 0; i < jsonlist.size(); i++)
	{
		ifstream ifs;
		ifs.open(jsonlist[i].file_path);
		Value root;
		CharReaderBuilder fh;
		JSONCPP_STRING errs;
		parseFromStream(fh, ifs, &root, &errs);
		Value Object = root["imagelabel"]["Object"];
		for (int j = 0; j < Object.size(); j++)
		{
			for (int k = 0; k < lines_en.size(); k++)
			{
				if (Object[j]["name"].asString() == lines_en[k].second)
				{
					count[k]++;
				}
			}
		}
	}
	if (lines_en.size()<20)
	{
		for (size_t i = 0; i < lines_en.size(); i++)
		{
			char s[50];
			sprintf_s(s, "%s%s%d\n", lines_zh[i].second.c_str(), " 数量：", count[i]);
			message = message + s;
		}
	}
	else
	{
		ofstream file("Analyse.txt");
		for (size_t i = 0; i < lines_en.size(); i++)
		{
			char s[50];
			
			sprintf_s(s, "%s%s%d\n", lines_zh[i].second.c_str(), " 数量：", count[i]);
			file << s;
		}
		
		file.close();
		message = "Analyse.txt导出完毕！";
	}
	
	AfxMessageBox(message.c_str());
}

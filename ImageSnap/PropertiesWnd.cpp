
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ImageSnap.h"
#include "ImageSnapDoc.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	std::ifstream infile("21cls.txt");
	int nId;
	string clsname;
	//while (infile >> nId >> clsname)
	//{	

	//	lines_.push_back(std::make_pair(nId, clsname));

	//}
	int i = 0;
	while (getline(infile, clsname))//(infile >> nId >> clsname)
	{
		//std::string GB_str;
		//GB_str = UTF8ToGB(clsname.c_str());
		int space_pos = clsname.find_first_of(' ');
		std::string temp_name = clsname.substr(space_pos + 1);
		lines_.push_back(std::make_pair(i, temp_name));
		i++;

	}
	infile.close();

	curexpendobj = -1;
	curexpendobj = -1;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_CBN_SELCHANGE(ID_COMBOBOX_SHOWTYPE, OnSelchangeComboShowType)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, ID_COMBOBOX_SHOWTYPE))
	{
		TRACE0("未能创建属性组合 \n");
		return -1;      // 未能创建
	}

	m_wndObjectCombo.AddString(_T("全部显示"));
	m_wndObjectCombo.AddString(_T("显示有效"));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel(1);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}
void CPropertiesWnd::OnSelchangeComboShowType()
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
	CMainFrame*       pMain=(CMainFrame   *)AfxGetApp()->m_pMainWnd; 
	CImageSnapDoc *   pCurrentDoc   =(CImageSnapDoc *)pMain->GetActiveDocument(); 
	if (NULL == pCurrentDoc)
	{
		return;
	}
	UpdataAllData(pCurrentDoc->m_ObjList,curexpendobj,curexpendpart);
	return;
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("外观"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("三维外观"), (_variant_t) FALSE, _T("指定窗口的字体不使用粗体，并且控件将使用三维边框")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("边框"), _T("Dialog Frame"), _T("其中之一: 无(None)、细(Thin)、可调整大小(Resizable)、对话框外框(Dialog Frame)"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);


	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("标题"), (_variant_t) _T("关于"), _T("指定窗口标题栏中显示的文本")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("窗口大小"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("高度"), (_variant_t) 250l, _T("指定窗口的高度"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("宽度"), (_variant_t) 150l, _T("指定窗口的宽度"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("字体"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("宋体, Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("字体"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("指定窗口的默认字体")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("使用系统字体"), (_variant_t) TRUE, _T("指定窗口使用“MS Shell Dlg”字体")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("杂项"));
	pProp = new CMFCPropertyGridProperty(_T("(名称)"), _T("应用程序"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("窗口颜色"), RGB(210, 192, 254), NULL, _T("指定默认的窗口颜色"));
	pColorProp->EnableOtherButton(_T("其他..."));
	pColorProp->EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static TCHAR BASED_CODE szFilter[] = _T("图标文件(*.ico)|*.ico|所有文件(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("图标"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("指定窗口图标")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("文件夹"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("层次结构"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("第一个子级"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("第二个子级"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 1"), (_variant_t) _T("值 1"), _T("此为说明")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 2"), (_variant_t) _T("值 2"), _T("此为说明")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 3"), (_variant_t) _T("值 3"), _T("此为说明")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
LRESULT CPropertiesWnd::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;
	CMFCPropertyGridProperty* parent = pProp->GetParent();
	if (NULL == parent)
	{
		return 0;
	}
	CMainFrame*       pMain=(CMainFrame   *)AfxGetApp()->m_pMainWnd; 
	CImageSnapDoc *   pCurrentDoc   =(CImageSnapDoc *)pMain->GetActiveDocument(); 
	COleVariant provalue = pProp->GetValue();


	int data = parent->GetData();
	if (pCurrentDoc->m_rect)
	{
		if (PRO_OBJ == data)
		{
			CMFCPropertyGridProperty*  pObjIndex = parent->GetSubItem(0);
			int dataindex = pObjIndex->GetData();
			if (PRO_OBJ_INDEX == dataindex)
			{
				//得到目标索引
				unsigned int objindex = pObjIndex->GetValue().intVal - 1;
				if (objindex<0 || objindex>pCurrentDoc->m_ObjList.size())
				{
					pCurrentDoc->UpdateAllViews(NULL);
					UpdataAllData(pCurrentDoc->m_ObjList, curexpendobj, curexpendpart);
					return 0;
				}
				curexpendobj = objindex;
				curexpendpart = -1;
				switch (pProp->GetData())
				{
				case PRO_OBJ_INDEX:
				{
					break;
				}
				case PRO_OBJ_CLASS:
				{
					CString newValue;
					newValue = provalue.bstrVal;

					for (unsigned int i = 0; i < lines_.size(); i++)
					{
						if (0 == newValue.Compare(lines_[i].second.c_str()))
						{
							pCurrentDoc->m_ObjList[objindex].iobjclass = i; 
						}
					}
					/*if (0 == newValue.Compare("铁路"))
					{
					pCurrentDoc->m_ObjList[objindex].iobjclass = PERSON;
					}
					if (0 == newValue.Compare("汽车"))
					{
					pCurrentDoc->m_ObjList[objindex].iobjclass = CAR;
					}*/

					break;
				}
				case PRO_OBJ_POSE:
				{
					CString newValue;
					newValue = provalue.bstrVal;
					if (0 == newValue.Compare("正面"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = FRONTAL;
					}
					else if (0 == newValue.Compare("左面"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = LEFT;
					}
					else if (0 == newValue.Compare("右面"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = RIGHT;
					}
					else if (0 == newValue.Compare("背面"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = BACK;
					}
					else
					{
						pCurrentDoc->m_ObjList[objindex].pose = UNKNOWN;
					}
					break;
				}
				case PRO_OBJ_TRU:
				{
					pCurrentDoc->m_ObjList[objindex].truncated = provalue.boolVal;
					break;
				}
				case PRO_OBJ_OCC:
				{
					pCurrentDoc->m_ObjList[objindex].occluded = provalue.boolVal;
					break;
				}
				case PRO_OBJ_ENABLE:
				{
					pCurrentDoc->m_ObjList[objindex].enable = provalue.boolVal;
					break;
				}
				case PRO_OBJ_LEFT:
				{

					pCurrentDoc->m_ObjList[objindex].objbox.left = MIN(pCurrentDoc->m_pWorkImg->width - 1, MAX(0, provalue.intVal));
					break;
				}
				case PRO_OBJ_TOP:
				{
					pCurrentDoc->m_ObjList[objindex].objbox.top = MIN(pCurrentDoc->m_pWorkImg->height - 1, MAX(0, provalue.intVal));
					break;
				}
				case PRO_OBJ_RIGHT:
				{
					pCurrentDoc->m_ObjList[objindex].objbox.right = MIN(pCurrentDoc->m_pWorkImg->width - 1, MAX(pCurrentDoc->m_ObjList[objindex].objbox.left, provalue.intVal));
					break;
				}
				case PRO_OBJ_BUTTOM:
				{
					pCurrentDoc->m_ObjList[objindex].objbox.bottom = MIN(pCurrentDoc->m_pWorkImg->height - 1, MAX(pCurrentDoc->m_ObjList[objindex].objbox.top, provalue.intVal));
					break;
				}
				default:
				{
					break;
				}
				}

			}

			/*if (pCurrentDoc->m_ObjList.size()==0)
			{
			pCurrentDoc->SetModifiedFlag(FALSE);
			}
			else
			{
			pCurrentDoc->SetModifiedFlag(TRUE);

			}*/



		}
		if (PRO_PART == data)
		{
			CMFCPropertyGridProperty*  pPartIndex = parent->GetSubItem(0);
			CMFCPropertyGridProperty*  pPartLinkObjIndex = parent->GetSubItem(1);
			int dataindex = pPartIndex->GetData();
			int dataLinkObjIndex = pPartLinkObjIndex->GetData();
			unsigned int LinkObjIndex = 0;
			if (PRO_PART_OBJ == dataLinkObjIndex)
			{
				LinkObjIndex = (pPartLinkObjIndex->GetOriginalValue().intVal) - 1;
			}
			else
			{
				pCurrentDoc->UpdateAllViews(NULL);
				UpdataAllData(pCurrentDoc->m_ObjList, curexpendobj, curexpendpart);
				return 0;
			}
			curexpendobj = LinkObjIndex;



			if (PRO_PART_INDEX == dataindex)
			{
				//得到部件索引
				int partindex = (pPartIndex->GetValue().intVal - 1);
				curexpendpart = partindex;
				switch (pProp->GetData())
				{
				case PRO_PART_INDEX:
				{
					break;
				}
				case PRO_PART_OBJ:
				{
					unsigned int newObjIndex = (provalue.intVal) - 1;
					if (newObjIndex<0 || newObjIndex>pCurrentDoc->m_ObjList.size())
					{
						break;
					}
					if (LinkObjIndex<0 || LinkObjIndex>pCurrentDoc->m_ObjList.size())
					{
						break;
					}
					//添加到新节点下
					int t = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.size();
					pCurrentDoc->m_ObjList[newObjIndex].partlist.push_back(pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex]);
					//从老节点删除
					vector<_Part>::iterator it = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.begin() + partindex;
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist.erase(it);
					t = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.size();
					break;
				}
				case PRO_PART_NAME:
				{
					CString newValue;
					newValue = provalue.bstrVal;
					/*if (0 == newValue.Compare("上瓷瓶"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = head;
					}
					if (0 == newValue.Compare("下瓷瓶"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = shoulder;
					}
					if (0 == newValue.Compare("手"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = hand;
					}
					if (0 == newValue.Compare("足"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = foot;
					}*/

					if (0 == newValue.Compare("左红"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lred;
					}
					if (0 == newValue.Compare("左绿"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lgreen;
					}
					if (0 == newValue.Compare("中红"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Mred;
					}
					if (0 == newValue.Compare("中绿"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Mgreen;
					}
					if (0 == newValue.Compare("右红"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Rred;
					}
					if (0 == newValue.Compare("右绿"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Rgreen;
					}
					if (0 == newValue.Compare("左黄"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lyellow;
					}
					if (0 == newValue.Compare("中黄"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Myellow;
					}
					if (0 == newValue.Compare("右黄"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Ryellow;
					}
					if (0 == newValue.Compare("未知"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Unkown;
					}
					break;
				}
				case PRO_PART_ENABLE:
				{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].enable = provalue.boolVal;
					break;
				}
				case PRO_PART_LEFT:
				{

					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].partrect.left = MIN(pCurrentDoc->m_pWorkImg->width - 1, MAX(0, provalue.intVal));
					break;
				}
				case PRO_PART_TOP:
				{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].partrect.top = MIN(pCurrentDoc->m_pWorkImg->height - 1, MAX(0, provalue.intVal));
					break;
				}
				case PRO_PART_RIGHT:
				{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].partrect.right = MIN(pCurrentDoc->m_pWorkImg->width - 1, MAX(pCurrentDoc->m_ObjList[LinkObjIndex].objbox.left, provalue.intVal));
					break;
				}
				case PRO_PART_BUTTOM:
				{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].partrect.bottom = MIN(pCurrentDoc->m_pWorkImg->height - 1, MAX(pCurrentDoc->m_ObjList[LinkObjIndex].objbox.top, provalue.intVal));
					break;
				}
				default:
				{
					break;
				}
				}
			}

		}

		if (pCurrentDoc->m_ObjList.size() == 0)
		{
			pCurrentDoc->SetModifiedFlag(FALSE);
		}
		else
		{
			pCurrentDoc->SetModifiedFlag(TRUE);

		}
		
		UpdataAllData(pCurrentDoc->m_ObjList, curexpendobj, curexpendpart);
		pCurrentDoc->UpdateAllViews(NULL);
	}
	else if (pCurrentDoc->m_curve)
	{
		if (PRO_OBJ == data)
		{
			CMFCPropertyGridProperty*  pObjIndex = parent->GetSubItem(0);
			int dataindex = pObjIndex->GetData();
			if (PRO_OBJ_INDEX == dataindex)
			{
				//得到目标索引
				unsigned int objindex = pObjIndex->GetValue().intVal - 1;
				if (objindex<0 || objindex>pCurrentDoc->m_Curve.size())
				{
					
					UpdataAllData(pCurrentDoc->m_Curve, curexpendobj, curexpendpart);
					pCurrentDoc->UpdateAllViews(NULL);
					return 0;
				}
				curexpendobj = objindex;
				curexpendpart = -1;
				switch (pProp->GetData())
				{
				case PRO_OBJ_INDEX:
				{
					break;
				}
				case PRO_OBJ_CLASS:
				{
					CString newValue;
					newValue = provalue.bstrVal;




					for (unsigned int i = 0; i < lines_.size(); i++)
					{
						if (0 == newValue.Compare(lines_[i].second.c_str()))
						{
							pCurrentDoc->m_Curve[objindex].iobjclass = i;
						}
					}
					/*if (0 == newValue.Compare("铁路"))
					{
					pCurrentDoc->m_Curve[objindex].iobjclass = PERSON;
					}
					if (0 == newValue.Compare("汽车"))
					{
					pCurrentDoc->m_Curve[objindex].iobjclass = CAR;
					}*/

					break;
				}
				case PRO_OBJ_TRU:
				{
					pCurrentDoc->m_Curve[objindex].truncated = provalue.boolVal;
					break;
				}
				case PRO_OBJ_OCC:
				{
					pCurrentDoc->m_Curve[objindex].occluded = provalue.boolVal;
					break;
				}
				case PRO_OBJ_ENABLE:
				{
					pCurrentDoc->m_Curve[objindex].enable = provalue.boolVal;
					break;
				}
				default:
				{
					break;
				}
				}

			}

			/*if (pCurrentDoc->m_Curve.size()==0)
			{
			pCurrentDoc->SetModifiedFlag(FALSE);
			}
			else
			{
			pCurrentDoc->SetModifiedFlag(TRUE);

			}*/



		}
		if (pCurrentDoc->m_Curve.size() == 0)
		{
			pCurrentDoc->SetModifiedFlag(FALSE);
		}
		else
		{
			pCurrentDoc->SetModifiedFlag(TRUE);

		}
		pCurrentDoc->UpdateAllViews(NULL);
		UpdataAllData(pCurrentDoc->m_Curve, curexpendobj, curexpendpart);
	}
	

	return 0;
}
void CPropertiesWnd::UpdataAllData(vector <_Object> Objlist, int expendobj, int expendpart)
{
	CImageSnapDoc *   pCurrentDoc   =(CImageSnapDoc *)((CMainFrame   *)AfxGetApp()->m_pMainWnd)->GetActiveDocument();
	m_wndPropList.RemoveAll();

	int NoShowAll = m_wndObjectCombo.GetCurSel();
	curexpendobj = expendobj;
	curexpendpart = expendpart;

	for (unsigned int i = 0;i<Objlist.size();i++)
	{
		if (NoShowAll && FALSE == Objlist[i].enable)
		{
			continue;
		}
		CString strTmpObj;
		
		char buffer[150];

		
		sprintf_s(buffer,"目标%d_%s",i+1,lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj=buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("目标%d_铁路",i+1);
		}

		else
		{
		strTmpObj.Format("目标%d_汽车",i+1);
		}*/

		/*for ()
		{
		}*/
		
		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("目标索引"), (_variant_t) (i+1), _T("指定目标序号，用于部件指定"));
        pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
//(CMainFrame   *)
		
		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = {0};
		char currType[40]={30};
		sprintf_s(currType,40,"%d",tmp_class);
		sprintf_s(wholeszType,500,"%s ","其中之一:");
		//for (unsigned int k=0;k<pCurrentDoc->lines_2.size();k++)
		//{
		//	sprintf_s(wholeszType, 100000,"%s %s.",wholeszType,pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("目标类别"), currType,wholeszType /*_T("其中之一: 铁路，汽车")*/);
		
		//for (unsigned int k=0;k<pCurrentDoc->lines_2.size();k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}
		
		//pPropClass->AddOption(_T("汽车"));

		
		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("汽车"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("目标姿势"), _T("正面"), _T("其中之一: 正面，侧面，背面"));
		pPropPose->AddOption(_T("正面"));
		pPropPose->AddOption(_T("左面"));
		pPropPose->AddOption(_T("右面"));
		pPropPose->AddOption(_T("背面"));
		pPropPose->AddOption(_T("未知"));
		if (FRONTAL== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("正面"));
		}
		if (LEFT== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("左面"));
		}
		if (RIGHT== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("右面"));
		}
		if (BACK== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("背面"));
		}
		if (UNKNOWN== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("未知"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("是否遮挡"), (_variant_t) Objlist[i].occluded, _T("目标是否遮挡"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("非全选择"), (_variant_t) Objlist[i].truncated, _T("目标是否完全选择"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t) Objlist[i].enable, _T("是否有效目标"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
	    pGroupObj->AddSubItem(pPropENABLE);

		CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t) Objlist[i].objbox.left, _T("目标区域左上点"));
		pPropLeft->SetData(PRO_OBJ_LEFT);
		pGroupObj->AddSubItem(pPropLeft);

		CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t) Objlist[i].objbox.top, _T("目标区域左上点"));
		pPropTop->SetData(PRO_OBJ_TOP);
		pGroupObj->AddSubItem(pPropTop);

		CMFCPropertyGridProperty* pPropRight =  new CMFCPropertyGridProperty(_T("Right"), (_variant_t) Objlist[i].objbox.right, _T("目标区域右下点"));
		pPropRight->SetData(PRO_OBJ_RIGHT);
		pGroupObj->AddSubItem(pPropRight);

		CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t) Objlist[i].objbox.bottom, _T("目标区域右下点"));
		pPropBut->SetData(PRO_OBJ_BUTTOM);
		pGroupObj->AddSubItem(pPropBut);

		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("属性"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("属性"));
				pGroupObj->AddSubItem(p);
			}
			
		}

		for (unsigned int j = 0;j<Objlist[i].partlist.size();j++)
		{
			if (NoShowAll && FALSE == Objlist[i].partlist[j].enable)
			{
				continue;
			}
			CString strTmpPart;
			if (Lred== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左红",j+1);
			}
			if (Lgreen== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左绿",j+1);
			}
			if (Mred== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中红",j+1);
			}
			if (Mgreen== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中绿",j+1);
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右红", j + 1);
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右绿", j + 1);
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左黄", j + 1);
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中黄", j + 1);
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右黄", j + 1);
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_未知", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("部件索引"), (_variant_t) (j+1), _T("指定该目标下部件序号"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("所属目标"), (_variant_t) (i+1), _T("指定该部件所属目标"));
			for (unsigned int n = 0;n<Objlist.size();n++)
			{
				if (TRUE == Objlist[n].enable)
				{
					CString strTmp;
					strTmp.Format("%d",n+1);
					pPropObjIndex->AddOption(strTmp);
				}
			}
			pPropObjIndex->SetData(PRO_PART_OBJ);
			pGroupPart->AddSubItem(pPropObjIndex);

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("部件名称"), _T("左红"), _T("指定该部件所属目标"));
			pPropPartName->AddOption(_T("左红"));
			pPropPartName->AddOption(_T("左绿"));
			pPropPartName->AddOption(_T("中红"));
			pPropPartName->AddOption(_T("中绿"));
			pPropPartName->AddOption(_T("右红"));
			pPropPartName->AddOption(_T("右绿"));
			pPropPartName->AddOption(_T("左黄"));
			pPropPartName->AddOption(_T("中黄"));
			pPropPartName->AddOption(_T("右黄"));
			pPropPartName->AddOption(_T("未知"));



			if (Lred== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左红"));
			}
			if (Lgreen== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左绿"));
			}
			if (Mred== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中红"));
			}
			if (Mgreen== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中绿"));
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右红"));
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右绿"));
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左黄"));
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中黄"));
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右黄"));
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("未知"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);

			
			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t) Objlist[i].partlist[j].enable, _T("是否有效目标"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t) Objlist[i].partlist[j].partrect.left, _T("部件区域左上点"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t) Objlist[i].partlist[j].partrect.top, _T("部件区域左上点"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t) Objlist[i].partlist[j].partrect.right, _T("部件区域右下点"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("部件区域右下点"));
			pPropPartBut->SetData(PRO_PART_BUTTOM);
			pGroupPart->AddSubItem(pPropPartBut);

			if (j==expendpart && i==expendobj)
			{
				pGroupPart->Expand(TRUE);
			}
			else
			{
				pGroupPart->Expand(FALSE);
			}

			pGroupObj->AddSubItem(pGroupPart);
            
		}
		if (i==expendobj)
		{
			pGroupObj->Expand(TRUE);
		}
		else
		{
			pGroupObj->Expand(FALSE);
		}
		m_wndPropList.AddProperty(pGroupObj);

	}
	return;
}
void CPropertiesWnd::UpdataAllData(vector <_Polygon> Objlist, int expendobj, int expendpart)
{
	CImageSnapDoc *   pCurrentDoc = (CImageSnapDoc *)((CMainFrame   *)AfxGetApp()->m_pMainWnd)->GetActiveDocument();
	m_wndPropList.RemoveAll();

	int NoShowAll = m_wndObjectCombo.GetCurSel();
	curexpendobj = expendobj;
	curexpendpart = expendpart;

	for (unsigned int i = 0; i<Objlist.size(); i++)
	{
		if (NoShowAll && FALSE == Objlist[i].enable)
		{
			continue;
		}
		CString strTmpObj;

		char buffer[150];


		sprintf_s(buffer, "目标%d_%s", i + 1 + pCurrentDoc->m_ObjList.size(), lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj = buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("目标%d_铁路",i+1);
		}

		else
		{
		strTmpObj.Format("目标%d_汽车",i+1);
		}*/

		/*for ()
		{
		}*/

		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("目标索引"), (_variant_t)(i + 1), _T("指定目标序号，用于部件指定"));
		pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
		//(CMainFrame   *)

		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = { 0 };
		char currType[30] = { 30 };
		sprintf_s(currType, 30, "%d", tmp_class);
		sprintf_s(wholeszType, 500, "%s ", "其中之一:");
		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	sprintf_s(wholeszType, 100000, "%s %s.", wholeszType, pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("目标类别"), currType, wholeszType /*_T("其中之一: 铁路，汽车")*/);

		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}

		//pPropClass->AddOption(_T("汽车"));


		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("汽车"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("目标姿势"), _T("正面"), _T("其中之一: 正面，侧面，背面"));
		pPropPose->AddOption(_T("正面"));
		pPropPose->AddOption(_T("左面"));
		pPropPose->AddOption(_T("右面"));
		pPropPose->AddOption(_T("背面"));
		pPropPose->AddOption(_T("未知"));
		if (FRONTAL == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("正面"));
		}
		if (LEFT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("左面"));
		}
		if (RIGHT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("右面"));
		}
		if (BACK == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("背面"));
		}
		if (UNKNOWN == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("未知"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("是否遮挡"), (_variant_t)Objlist[i].occluded, _T("目标是否遮挡"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("非全选择"), (_variant_t)Objlist[i].truncated, _T("目标是否完全选择"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t)Objlist[i].enable, _T("是否有效目标"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
		pGroupObj->AddSubItem(pPropENABLE);

		//CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].objbox.left, _T("目标区域左上点"));
		//pPropLeft->SetData(PRO_OBJ_LEFT);
		//pGroupObj->AddSubItem(pPropLeft);

		//CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].objbox.top, _T("目标区域左上点"));
		//pPropTop->SetData(PRO_OBJ_TOP);
		//pGroupObj->AddSubItem(pPropTop);

		//CMFCPropertyGridProperty* pPropRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].objbox.right, _T("目标区域右下点"));
		//pPropRight->SetData(PRO_OBJ_RIGHT);
		//pGroupObj->AddSubItem(pPropRight);

		//CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t)Objlist[i].objbox.bottom, _T("目标区域右下点"));
		/*pPropBut->SetData(PRO_OBJ_BUTTOM);
		pGroupObj->AddSubItem(pPropBut);*/
		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("属性"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("属性"));
				pGroupObj->AddSubItem(p);
			}

		}
		for (unsigned int j = 0; j<Objlist[i].partlist.size(); j++)
		{
			if (NoShowAll && FALSE == Objlist[i].partlist[j].enable)
			{
				continue;
			}
			CString strTmpPart;
			if (Lred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左红", j + 1);
			}
			if (Lgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左绿", j + 1);
			}
			if (Mred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中红", j + 1);
			}
			if (Mgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中绿", j + 1);
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右红", j + 1);
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右绿", j + 1);
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左黄", j + 1);
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_中黄", j + 1);
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右黄", j + 1);
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_未知", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("部件索引"), (_variant_t)(j + 1), _T("指定该目标下部件序号"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("所属目标"), (_variant_t)(i + 1), _T("指定该部件所属目标"));
			for (unsigned int n = 0; n<Objlist.size(); n++)
			{
				if (TRUE == Objlist[n].enable)
				{
					CString strTmp;
					strTmp.Format("%d", n + 1);
					pPropObjIndex->AddOption(strTmp);
				}
			}
			pPropObjIndex->SetData(PRO_PART_OBJ);
			pGroupPart->AddSubItem(pPropObjIndex);

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("部件名称"), _T("头"), _T("指定该部件所属目标"));
			pPropPartName->AddOption(_T("头"));
			pPropPartName->AddOption(_T("帽子"));
			pPropPartName->AddOption(_T("上身"));
			pPropPartName->AddOption(_T("背包"));
			pPropPartName->AddOption(_T("下身"));
			pPropPartName->AddOption(_T("左脚"));
			pPropPartName->AddOption(_T("右脚"));



			if (Lred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左红"));
			}
			if (Lgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左绿"));
			}
			if (Mred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中红"));
			}
			if (Mgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中绿"));
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右红"));
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右绿"));
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左黄"));
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("中黄"));
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右黄"));
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("未知"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);


			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t)Objlist[i].partlist[j].enable, _T("是否有效目标"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].partlist[j].partrect.left, _T("部件区域左上点"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].partlist[j].partrect.top, _T("部件区域左上点"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].partlist[j].partrect.right, _T("部件区域右下点"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("部件区域右下点"));
			pPropPartBut->SetData(PRO_PART_BUTTOM);
			pGroupPart->AddSubItem(pPropPartBut);
			
			if (j == expendpart && i == expendobj)
			{
				pGroupPart->Expand(TRUE);
			}
			else
			{
				pGroupPart->Expand(FALSE);
			}

			pGroupObj->AddSubItem(pGroupPart);

		}
		if (i == expendobj)
		{
			pGroupObj->Expand(TRUE);
		}
		else
		{
			pGroupObj->Expand(FALSE);
		}
		m_wndPropList.AddProperty(pGroupObj);

	}
	return;
}

void CPropertiesWnd::UpdataAllData(vector <_Curve> Objlist, int expendobj, int expendpart)
{
	CImageSnapDoc *   pCurrentDoc = (CImageSnapDoc *)((CMainFrame   *)AfxGetApp()->m_pMainWnd)->GetActiveDocument();
	m_wndPropList.RemoveAll();

	int NoShowAll = m_wndObjectCombo.GetCurSel();
	curexpendobj = expendobj;
	curexpendpart = expendpart;

	for (unsigned int i = 0; i<Objlist.size(); i++)
	{
		if (NoShowAll && FALSE == Objlist[i].enable)
		{
			continue;
		}
		CString strTmpObj;

		char buffer[150];


		sprintf_s(buffer, "目标%d_%s", i + 1 + pCurrentDoc->m_ObjList.size() + pCurrentDoc->m_Polygon.size(), lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj = buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("目标%d_铁路",i+1);
		}

		else
		{
		strTmpObj.Format("目标%d_汽车",i+1);
		}*/

		/*for ()
		{
		}*/

		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("目标索引"), (_variant_t)(i + 1), _T("指定目标序号，用于部件指定"));
		pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
		//(CMainFrame   *)

		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = { 0 };
		char currType[30] = { 30 };
		sprintf_s(currType, 30, "%d", tmp_class);
		sprintf_s(wholeszType, 500, "%s ", "其中之一:");
		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	sprintf_s(wholeszType, 100000, "%s %s.", wholeszType, pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("目标类别"), currType, wholeszType /*_T("其中之一: 铁路，汽车")*/);

		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}

		//pPropClass->AddOption(_T("汽车"));


		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("汽车"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("目标姿势"), _T("正面"), _T("其中之一: 正面，侧面，背面"));
		pPropPose->AddOption(_T("正面"));
		pPropPose->AddOption(_T("左面"));
		pPropPose->AddOption(_T("右面"));
		pPropPose->AddOption(_T("背面"));
		pPropPose->AddOption(_T("未知"));
		if (FRONTAL == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("正面"));
		}
		if (LEFT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("左面"));
		}
		if (RIGHT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("右面"));
		}
		if (BACK == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("背面"));
		}
		if (UNKNOWN == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("未知"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("是否遮挡"), (_variant_t)Objlist[i].occluded, _T("目标是否遮挡"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("非全选择"), (_variant_t)Objlist[i].truncated, _T("目标是否完全选择"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t)Objlist[i].enable, _T("是否有效目标"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
		pGroupObj->AddSubItem(pPropENABLE);

		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("属性"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("属性"));
				pGroupObj->AddSubItem(p);
			}

		}
		//CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].objbox.left, _T("目标区域左上点"));
		//pPropLeft->SetData(PRO_OBJ_LEFT);
		//pGroupObj->AddSubItem(pPropLeft);

		//CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].objbox.top, _T("目标区域左上点"));
		//pPropTop->SetData(PRO_OBJ_TOP);
		//pGroupObj->AddSubItem(pPropTop);

		//CMFCPropertyGridProperty* pPropRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].objbox.right, _T("目标区域右下点"));
		//pPropRight->SetData(PRO_OBJ_RIGHT);
		//pGroupObj->AddSubItem(pPropRight);

		//CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t)Objlist[i].objbox.bottom, _T("目标区域右下点"));
		/*pPropBut->SetData(PRO_OBJ_BUTTOM);
		pGroupObj->AddSubItem(pPropBut);*/

		/*for (unsigned int j = 0; j<Objlist[i].partlist.size(); j++)
		{
			if (NoShowAll && FALSE == Objlist[i].partlist[j].enable)
			{
				continue;
			}
			CString strTmpPart;
			if (head == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_头", j + 1);
			}
			if (hat == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_帽子", j + 1);
			}
			if (backpack == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_背包", j + 1);
			}
			if (Upperbody == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_上身", j + 1);
			}
			if (Lowerbody == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_下身", j + 1);
			}
			if (Leftfoot == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_左脚", j + 1);
			}
			if (Rightfoot == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("部件%d_右脚", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("部件索引"), (_variant_t)(j + 1), _T("指定该目标下部件序号"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("所属目标"), (_variant_t)(i + 1), _T("指定该部件所属目标"));
			for (unsigned int n = 0; n<Objlist.size(); n++)
			{
				if (TRUE == Objlist[n].enable)
				{
					CString strTmp;
					strTmp.Format("%d", n + 1);
					pPropObjIndex->AddOption(strTmp);
				}
			}
			pPropObjIndex->SetData(PRO_PART_OBJ);
			pGroupPart->AddSubItem(pPropObjIndex);

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("部件名称"), _T("头"), _T("指定该部件所属目标"));
			pPropPartName->AddOption(_T("头"));
			pPropPartName->AddOption(_T("帽子"));
			pPropPartName->AddOption(_T("上身"));
			pPropPartName->AddOption(_T("背包"));
			pPropPartName->AddOption(_T("下身"));
			pPropPartName->AddOption(_T("左脚"));
			pPropPartName->AddOption(_T("右脚"));



			if (head == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("头"));
			}
			if (hat == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("帽子"));
			}
			if (Upperbody == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("上身"));
			}
			if (backpack == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("背包"));
			}
			if (Lowerbody == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("下身"));
			}
			if (Leftfoot == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("左脚"));
			}
			if (Rightfoot == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("右脚"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);


			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("是否有效"), (_variant_t)Objlist[i].partlist[j].enable, _T("是否有效目标"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].partlist[j].partrect.left, _T("部件区域左上点"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].partlist[j].partrect.top, _T("部件区域左上点"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].partlist[j].partrect.right, _T("部件区域右下点"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("部件区域右下点"));
			pPropPartBut->SetData(PRO_PART_BUTTOM);
			pGroupPart->AddSubItem(pPropPartBut);

			if (j == expendpart && i == expendobj)
			{
				pGroupPart->Expand(TRUE);
			}
			else
			{
				pGroupPart->Expand(FALSE);
			}

			pGroupObj->AddSubItem(pGroupPart);

		}*/
		if (i == expendobj)
		{
			pGroupObj->Expand(TRUE);
		}
		else
		{
			pGroupObj->Expand(FALSE);
		}
		m_wndPropList.AddProperty(pGroupObj);

	}
	return;
}


void CPropertiesWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDockablePane::OnMouseMove(nFlags, point);
}

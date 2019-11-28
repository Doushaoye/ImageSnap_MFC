
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
// CResourceViewBar ��Ϣ�������

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

	// �������:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, ID_COMBOBOX_SHOWTYPE))
	{
		TRACE0("δ�ܴ���������� \n");
		return -1;      // δ�ܴ���
	}

	m_wndObjectCombo.AddString(_T("ȫ����ʾ"));
	m_wndObjectCombo.AddString(_T("��ʾ��Ч"));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel(1);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("δ�ܴ�����������\n");
		return -1;      // δ�ܴ���
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ������*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ����*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// �������ͨ���˿ؼ�·�ɣ�������ͨ�������·��:
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
	// TODO: �ڴ˴���������������
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: �ڴ˴����������� UI ����������
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: �ڴ˴���������������
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: �ڴ˴����������� UI ����������
}
void CPropertiesWnd::OnSelchangeComboShowType()
{
	// TODO: �ڴ˴����������� UI ����������
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

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("���"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("��ά���"), (_variant_t) FALSE, _T("ָ�����ڵ����岻ʹ�ô��壬���ҿؼ���ʹ����ά�߿�")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("�߿�"), _T("Dialog Frame"), _T("����֮һ: ��(None)��ϸ(Thin)���ɵ�����С(Resizable)���Ի������(Dialog Frame)"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);


	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("����"), (_variant_t) _T("����"), _T("ָ�����ڱ���������ʾ���ı�")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("���ڴ�С"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("�߶�"), (_variant_t) 250l, _T("ָ�����ڵĸ߶�"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("���"), (_variant_t) 150l, _T("ָ�����ڵĿ��"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("����"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("����, Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("����"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("ָ�����ڵ�Ĭ������")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("ʹ��ϵͳ����"), (_variant_t) TRUE, _T("ָ������ʹ�á�MS Shell Dlg������")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("����"));
	pProp = new CMFCPropertyGridProperty(_T("(����)"), _T("Ӧ�ó���"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("������ɫ"), RGB(210, 192, 254), NULL, _T("ָ��Ĭ�ϵĴ�����ɫ"));
	pColorProp->EnableOtherButton(_T("����..."));
	pColorProp->EnableAutomaticButton(_T("Ĭ��"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static TCHAR BASED_CODE szFilter[] = _T("ͼ���ļ�(*.ico)|*.ico|�����ļ�(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("ͼ��"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("ָ������ͼ��")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�ļ���"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("��νṹ"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("��һ���Ӽ�"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("�ڶ����Ӽ�"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�� 1"), (_variant_t) _T("ֵ 1"), _T("��Ϊ˵��")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�� 2"), (_variant_t) _T("ֵ 2"), _T("��Ϊ˵��")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�� 3"), (_variant_t) _T("ֵ 3"), _T("��Ϊ˵��")));

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
				//�õ�Ŀ������
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
					/*if (0 == newValue.Compare("��·"))
					{
					pCurrentDoc->m_ObjList[objindex].iobjclass = PERSON;
					}
					if (0 == newValue.Compare("����"))
					{
					pCurrentDoc->m_ObjList[objindex].iobjclass = CAR;
					}*/

					break;
				}
				case PRO_OBJ_POSE:
				{
					CString newValue;
					newValue = provalue.bstrVal;
					if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = FRONTAL;
					}
					else if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = LEFT;
					}
					else if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[objindex].pose = RIGHT;
					}
					else if (0 == newValue.Compare("����"))
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
				//�õ���������
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
					//��ӵ��½ڵ���
					int t = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.size();
					pCurrentDoc->m_ObjList[newObjIndex].partlist.push_back(pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex]);
					//���Ͻڵ�ɾ��
					vector<_Part>::iterator it = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.begin() + partindex;
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist.erase(it);
					t = pCurrentDoc->m_ObjList[LinkObjIndex].partlist.size();
					break;
				}
				case PRO_PART_NAME:
				{
					CString newValue;
					newValue = provalue.bstrVal;
					/*if (0 == newValue.Compare("�ϴ�ƿ"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = head;
					}
					if (0 == newValue.Compare("�´�ƿ"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = shoulder;
					}
					if (0 == newValue.Compare("��"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = hand;
					}
					if (0 == newValue.Compare("��"))
					{
					pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = foot;
					}*/

					if (0 == newValue.Compare("���"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lred;
					}
					if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lgreen;
					}
					if (0 == newValue.Compare("�к�"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Mred;
					}
					if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Mgreen;
					}
					if (0 == newValue.Compare("�Һ�"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Rred;
					}
					if (0 == newValue.Compare("����"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Rgreen;
					}
					if (0 == newValue.Compare("���"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Lyellow;
					}
					if (0 == newValue.Compare("�л�"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Myellow;
					}
					if (0 == newValue.Compare("�һ�"))
					{
						pCurrentDoc->m_ObjList[LinkObjIndex].partlist[partindex].ipartclass = Ryellow;
					}
					if (0 == newValue.Compare("δ֪"))
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
				//�õ�Ŀ������
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
					/*if (0 == newValue.Compare("��·"))
					{
					pCurrentDoc->m_Curve[objindex].iobjclass = PERSON;
					}
					if (0 == newValue.Compare("����"))
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

		
		sprintf_s(buffer,"Ŀ��%d_%s",i+1,lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj=buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("Ŀ��%d_��·",i+1);
		}

		else
		{
		strTmpObj.Format("Ŀ��%d_����",i+1);
		}*/

		/*for ()
		{
		}*/
		
		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("Ŀ������"), (_variant_t) (i+1), _T("ָ��Ŀ����ţ����ڲ���ָ��"));
        pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
//(CMainFrame   *)
		
		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = {0};
		char currType[40]={30};
		sprintf_s(currType,40,"%d",tmp_class);
		sprintf_s(wholeszType,500,"%s ","����֮һ:");
		//for (unsigned int k=0;k<pCurrentDoc->lines_2.size();k++)
		//{
		//	sprintf_s(wholeszType, 100000,"%s %s.",wholeszType,pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("Ŀ�����"), currType,wholeszType /*_T("����֮һ: ��·������")*/);
		
		//for (unsigned int k=0;k<pCurrentDoc->lines_2.size();k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}
		
		//pPropClass->AddOption(_T("����"));

		
		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("����"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("Ŀ������"), _T("����"), _T("����֮һ: ���棬���棬����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("δ֪"));
		if (FRONTAL== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (LEFT== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (RIGHT== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (BACK== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (UNKNOWN== Objlist[i].pose)
		{
			pPropPose->SetValue(_T("δ֪"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("�Ƿ��ڵ�"), (_variant_t) Objlist[i].occluded, _T("Ŀ���Ƿ��ڵ�"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("��ȫѡ��"), (_variant_t) Objlist[i].truncated, _T("Ŀ���Ƿ���ȫѡ��"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t) Objlist[i].enable, _T("�Ƿ���ЧĿ��"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
	    pGroupObj->AddSubItem(pPropENABLE);

		CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t) Objlist[i].objbox.left, _T("Ŀ���������ϵ�"));
		pPropLeft->SetData(PRO_OBJ_LEFT);
		pGroupObj->AddSubItem(pPropLeft);

		CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t) Objlist[i].objbox.top, _T("Ŀ���������ϵ�"));
		pPropTop->SetData(PRO_OBJ_TOP);
		pGroupObj->AddSubItem(pPropTop);

		CMFCPropertyGridProperty* pPropRight =  new CMFCPropertyGridProperty(_T("Right"), (_variant_t) Objlist[i].objbox.right, _T("Ŀ���������µ�"));
		pPropRight->SetData(PRO_OBJ_RIGHT);
		pGroupObj->AddSubItem(pPropRight);

		CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t) Objlist[i].objbox.bottom, _T("Ŀ���������µ�"));
		pPropBut->SetData(PRO_OBJ_BUTTOM);
		pGroupObj->AddSubItem(pPropBut);

		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("����"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("����"));
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
				strTmpPart.Format("����%d_���",j+1);
			}
			if (Lgreen== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����",j+1);
			}
			if (Mred== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�к�",j+1);
			}
			if (Mgreen== Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����",j+1);
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�Һ�", j + 1);
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_���", j + 1);
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�л�", j + 1);
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�һ�", j + 1);
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_δ֪", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("��������"), (_variant_t) (j+1), _T("ָ����Ŀ���²������"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("����Ŀ��"), (_variant_t) (i+1), _T("ָ���ò�������Ŀ��"));
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

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("��������"), _T("���"), _T("ָ���ò�������Ŀ��"));
			pPropPartName->AddOption(_T("���"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("�к�"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("�Һ�"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("���"));
			pPropPartName->AddOption(_T("�л�"));
			pPropPartName->AddOption(_T("�һ�"));
			pPropPartName->AddOption(_T("δ֪"));



			if (Lred== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("���"));
			}
			if (Lgreen== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Mred== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�к�"));
			}
			if (Mgreen== Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�Һ�"));
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("���"));
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�л�"));
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�һ�"));
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("δ֪"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);

			
			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t) Objlist[i].partlist[j].enable, _T("�Ƿ���ЧĿ��"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t) Objlist[i].partlist[j].partrect.left, _T("�����������ϵ�"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t) Objlist[i].partlist[j].partrect.top, _T("�����������ϵ�"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t) Objlist[i].partlist[j].partrect.right, _T("�����������µ�"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("�����������µ�"));
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


		sprintf_s(buffer, "Ŀ��%d_%s", i + 1 + pCurrentDoc->m_ObjList.size(), lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj = buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("Ŀ��%d_��·",i+1);
		}

		else
		{
		strTmpObj.Format("Ŀ��%d_����",i+1);
		}*/

		/*for ()
		{
		}*/

		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("Ŀ������"), (_variant_t)(i + 1), _T("ָ��Ŀ����ţ����ڲ���ָ��"));
		pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
		//(CMainFrame   *)

		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = { 0 };
		char currType[30] = { 30 };
		sprintf_s(currType, 30, "%d", tmp_class);
		sprintf_s(wholeszType, 500, "%s ", "����֮һ:");
		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	sprintf_s(wholeszType, 100000, "%s %s.", wholeszType, pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("Ŀ�����"), currType, wholeszType /*_T("����֮һ: ��·������")*/);

		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}

		//pPropClass->AddOption(_T("����"));


		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("����"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("Ŀ������"), _T("����"), _T("����֮һ: ���棬���棬����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("δ֪"));
		if (FRONTAL == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (LEFT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (RIGHT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (BACK == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (UNKNOWN == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("δ֪"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("�Ƿ��ڵ�"), (_variant_t)Objlist[i].occluded, _T("Ŀ���Ƿ��ڵ�"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("��ȫѡ��"), (_variant_t)Objlist[i].truncated, _T("Ŀ���Ƿ���ȫѡ��"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t)Objlist[i].enable, _T("�Ƿ���ЧĿ��"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
		pGroupObj->AddSubItem(pPropENABLE);

		//CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].objbox.left, _T("Ŀ���������ϵ�"));
		//pPropLeft->SetData(PRO_OBJ_LEFT);
		//pGroupObj->AddSubItem(pPropLeft);

		//CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].objbox.top, _T("Ŀ���������ϵ�"));
		//pPropTop->SetData(PRO_OBJ_TOP);
		//pGroupObj->AddSubItem(pPropTop);

		//CMFCPropertyGridProperty* pPropRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].objbox.right, _T("Ŀ���������µ�"));
		//pPropRight->SetData(PRO_OBJ_RIGHT);
		//pGroupObj->AddSubItem(pPropRight);

		//CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t)Objlist[i].objbox.bottom, _T("Ŀ���������µ�"));
		/*pPropBut->SetData(PRO_OBJ_BUTTOM);
		pGroupObj->AddSubItem(pPropBut);*/
		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("����"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("����"));
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
				strTmpPart.Format("����%d_���", j + 1);
			}
			if (Lgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Mred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�к�", j + 1);
			}
			if (Mgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�Һ�", j + 1);
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_���", j + 1);
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�л�", j + 1);
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�һ�", j + 1);
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_δ֪", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("��������"), (_variant_t)(j + 1), _T("ָ����Ŀ���²������"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("����Ŀ��"), (_variant_t)(i + 1), _T("ָ���ò�������Ŀ��"));
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

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("��������"), _T("ͷ"), _T("ָ���ò�������Ŀ��"));
			pPropPartName->AddOption(_T("ͷ"));
			pPropPartName->AddOption(_T("ñ��"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("���"));
			pPropPartName->AddOption(_T("�ҽ�"));



			if (Lred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("���"));
			}
			if (Lgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Mred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�к�"));
			}
			if (Mgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Rred == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�Һ�"));
			}
			if (Rgreen == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Lyellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("���"));
			}
			if (Myellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�л�"));
			}
			if (Ryellow == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�һ�"));
			}
			if (Unkown == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("δ֪"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);


			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t)Objlist[i].partlist[j].enable, _T("�Ƿ���ЧĿ��"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].partlist[j].partrect.left, _T("�����������ϵ�"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].partlist[j].partrect.top, _T("�����������ϵ�"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].partlist[j].partrect.right, _T("�����������µ�"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("�����������µ�"));
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


		sprintf_s(buffer, "Ŀ��%d_%s", i + 1 + pCurrentDoc->m_ObjList.size() + pCurrentDoc->m_Polygon.size(), lines_[Objlist[i].iobjclass].second.c_str());
		strTmpObj = buffer;

		//AfxMessageBox(strTmpObj);

		/*if (PERSON== Objlist[i].iobjclass)
		{
		strTmpObj.Format("Ŀ��%d_��·",i+1);
		}

		else
		{
		strTmpObj.Format("Ŀ��%d_����",i+1);
		}*/

		/*for ()
		{
		}*/

		CMFCPropertyGridProperty* pGroupObj = new CMFCPropertyGridProperty(strTmpObj);
		pGroupObj->SetData(PRO_OBJ);


		CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("Ŀ������"), (_variant_t)(i + 1), _T("ָ��Ŀ����ţ����ڲ���ָ��"));
		pPropIndex->AllowEdit(FALSE);
		pPropIndex->SetData(PRO_OBJ_INDEX);
		pGroupObj->AddSubItem(pPropIndex);
		//(CMainFrame   *)

		//MessageBox(pCurrentDoc->lines_2[1].second.c_str());
		//printf("%s\n",pCurrentDoc->lines_2[1].second.c_str());

		char wholeszType[500] = { 0 };
		char currType[30] = { 30 };
		sprintf_s(currType, 30, "%d", tmp_class);
		sprintf_s(wholeszType, 500, "%s ", "����֮һ:");
		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	sprintf_s(wholeszType, 100000, "%s %s.", wholeszType, pCurrentDoc->lines_2[k].second.c_str());
		//}
		/////////////////////////////
		CMFCPropertyGridProperty* pPropClass = new CMFCPropertyGridProperty(_T("Ŀ�����"), currType, wholeszType /*_T("����֮һ: ��·������")*/);

		//for (unsigned int k = 0; k<pCurrentDoc->lines_2.size(); k++)
		//{
		//	pPropClass->AddOption(pCurrentDoc->lines_2[k].second.c_str());
		//}

		//pPropClass->AddOption(_T("����"));


		pPropClass->SetValue(lines_[Objlist[i].iobjclass].second.c_str());
		//if (CAR== Objlist[i].iobjclass)
		//{
		//	pPropClass->SetValue(_T("����"));
		//}
		pPropClass->AllowEdit(FALSE);
		pPropClass->SetData(PRO_OBJ_CLASS);
		pGroupObj->AddSubItem(pPropClass);

		CMFCPropertyGridProperty* pPropPose = new CMFCPropertyGridProperty(_T("Ŀ������"), _T("����"), _T("����֮һ: ���棬���棬����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("����"));
		pPropPose->AddOption(_T("δ֪"));
		if (FRONTAL == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (LEFT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (RIGHT == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (BACK == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("����"));
		}
		if (UNKNOWN == Objlist[i].pose)
		{
			pPropPose->SetValue(_T("δ֪"));
		}
		pPropPose->AllowEdit(FALSE);
		pPropPose->SetData(PRO_OBJ_POSE);
		pGroupObj->AddSubItem(pPropPose);

		CMFCPropertyGridProperty* pPropOCC = new CMFCPropertyGridProperty(_T("�Ƿ��ڵ�"), (_variant_t)Objlist[i].occluded, _T("Ŀ���Ƿ��ڵ�"));
		pPropOCC->SetData(PRO_OBJ_OCC);
		pGroupObj->AddSubItem(pPropOCC);

		CMFCPropertyGridProperty* pPropTRU = new CMFCPropertyGridProperty(_T("��ȫѡ��"), (_variant_t)Objlist[i].truncated, _T("Ŀ���Ƿ���ȫѡ��"));
		pPropTRU->SetData(PRO_OBJ_TRU);
		pGroupObj->AddSubItem(pPropTRU);

		CMFCPropertyGridProperty* pPropENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t)Objlist[i].enable, _T("�Ƿ���ЧĿ��"));
		pPropENABLE->SetData(PRO_OBJ_ENABLE);
		pGroupObj->AddSubItem(pPropENABLE);

		for (unsigned int j = 0; j < Objlist[i].o_property.size(); j++)
		{
			if (Objlist[i].o_property[j].property_options.length() != 0)
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), _T(Objlist[i].o_property[j].property_options.c_str()), _T("����"));
				pGroupObj->AddSubItem(p);
			}
			else
			{
				CMFCPropertyGridProperty* p = new CMFCPropertyGridProperty(_T(Objlist[i].o_property[j].property_name.c_str()), (_variant_t)Objlist[i].o_property[j].property_id, _T("����"));
				pGroupObj->AddSubItem(p);
			}

		}
		//CMFCPropertyGridProperty* pPropLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].objbox.left, _T("Ŀ���������ϵ�"));
		//pPropLeft->SetData(PRO_OBJ_LEFT);
		//pGroupObj->AddSubItem(pPropLeft);

		//CMFCPropertyGridProperty* pPropTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].objbox.top, _T("Ŀ���������ϵ�"));
		//pPropTop->SetData(PRO_OBJ_TOP);
		//pGroupObj->AddSubItem(pPropTop);

		//CMFCPropertyGridProperty* pPropRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].objbox.right, _T("Ŀ���������µ�"));
		//pPropRight->SetData(PRO_OBJ_RIGHT);
		//pGroupObj->AddSubItem(pPropRight);

		//CMFCPropertyGridProperty* pPropBut = new CMFCPropertyGridProperty(_T("Buttom"), (_variant_t)Objlist[i].objbox.bottom, _T("Ŀ���������µ�"));
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
				strTmpPart.Format("����%d_ͷ", j + 1);
			}
			if (hat == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_ñ��", j + 1);
			}
			if (backpack == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Upperbody == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Lowerbody == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_����", j + 1);
			}
			if (Leftfoot == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_���", j + 1);
			}
			if (Rightfoot == Objlist[i].partlist[j].ipartclass)
			{
				strTmpPart.Format("����%d_�ҽ�", j + 1);
			}

			CMFCPropertyGridProperty* pGroupPart = new CMFCPropertyGridProperty(strTmpPart);
			pGroupPart->SetData(PRO_PART);

			CMFCPropertyGridProperty* pPropIndex = new CMFCPropertyGridProperty(_T("��������"), (_variant_t)(j + 1), _T("ָ����Ŀ���²������"));
			pPropIndex->AllowEdit(FALSE);
			pPropIndex->SetData(PRO_PART_INDEX);
			pGroupPart->AddSubItem(pPropIndex);

			CMFCPropertyGridProperty* pPropObjIndex = new CMFCPropertyGridProperty(_T("����Ŀ��"), (_variant_t)(i + 1), _T("ָ���ò�������Ŀ��"));
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

			CMFCPropertyGridProperty* pPropPartName = new CMFCPropertyGridProperty(_T("��������"), _T("ͷ"), _T("ָ���ò�������Ŀ��"));
			pPropPartName->AddOption(_T("ͷ"));
			pPropPartName->AddOption(_T("ñ��"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("����"));
			pPropPartName->AddOption(_T("���"));
			pPropPartName->AddOption(_T("�ҽ�"));



			if (head == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("ͷ"));
			}
			if (hat == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("ñ��"));
			}
			if (Upperbody == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (backpack == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Lowerbody == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("����"));
			}
			if (Leftfoot == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("���"));
			}
			if (Rightfoot == Objlist[i].partlist[j].ipartclass)
			{
				pPropPartName->SetValue(_T("�ҽ�"));
			}
			pPropPartName->AllowEdit(FALSE);
			pPropPartName->SetData(PRO_PART_NAME);
			pGroupPart->AddSubItem(pPropPartName);


			CMFCPropertyGridProperty* pPropPartENABLE = new CMFCPropertyGridProperty(_T("�Ƿ���Ч"), (_variant_t)Objlist[i].partlist[j].enable, _T("�Ƿ���ЧĿ��"));
			pPropPartENABLE->SetData(PRO_PART_ENABLE);
			pGroupPart->AddSubItem(pPropPartENABLE);
			CMFCPropertyGridProperty* pPropPartLeft = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)Objlist[i].partlist[j].partrect.left, _T("�����������ϵ�"));
			pPropPartLeft->SetData(PRO_PART_LEFT);
			pGroupPart->AddSubItem(pPropPartLeft);
			CMFCPropertyGridProperty* pPropPartTop = new CMFCPropertyGridProperty(_T("Top"), (_variant_t)Objlist[i].partlist[j].partrect.top, _T("�����������ϵ�"));
			pPropPartTop->SetData(PRO_PART_TOP);
			pGroupPart->AddSubItem(pPropPartTop);
			CMFCPropertyGridProperty* pPropPartRight = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)Objlist[i].partlist[j].partrect.right, _T("�����������µ�"));
			pPropPartRight->SetData(PRO_PART_RIGHT);
			pGroupPart->AddSubItem(pPropPartRight);
			CMFCPropertyGridProperty* pPropPartBut = new CMFCPropertyGridProperty(_T("Button"), (_variant_t)Objlist[i].partlist[j].partrect.bottom, _T("�����������µ�"));
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDockablePane::OnMouseMove(nFlags, point);
}

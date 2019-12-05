// DlgSetSelect.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageSnap.h"
#include "DlgSetSelect.h"


// CDlgSetSelect 对话框

IMPLEMENT_DYNAMIC(CDlgSetSelect, CDialog)

CDlgSetSelect::CDlgSetSelect(CImageSnapDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetSelect::IDD, pParent)
	, m_bPart(FALSE)
{
	m_iOOP = 0;
	m_pDoc = pDoc;
}


CDlgSetSelect::~CDlgSetSelect()
{
}

void CDlgSetSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SELECT_OOP, m_CBmodelOOP);
	DDX_Control(pDX, IDC_SELECT_OBJ_CLASS, m_CBmodelObjCls);
	DDX_Control(pDX, IDC_SELECT_OBJ_POSE, m_CBmodelObjPos);
	DDX_Control(pDX, IDC_SELECT_OBJ_OCC, m_CBmodelObjOcc);
	DDX_Control(pDX, IDC_SELECT_OBJ_TRUN, m_CBmodelObjTru);
	DDX_Control(pDX, IDC_SELECT_OBJ_DIFF, m_CBmodelObjDif);
	DDX_Control(pDX, IDC_SELECT_PART_OBJ, m_CBmodelPartObj);
	DDX_Control(pDX, IDC_SELECT_PART_CLASS, m_CBmodelPartCls);
	DDX_Check(pDX, IDC_CHECK1, m_bPart);
	DDX_Control(pDX, IDC_EDIT_COLOR, m_Ecolor);
}


BEGIN_MESSAGE_MAP(CDlgSetSelect, CDialog)
	ON_CBN_SELENDOK(IDC_SELECT_OOP, &CDlgSetSelect::OnCbnSelendokSelectOop)
	ON_CBN_SELENDOK(IDC_SELECT_OBJ_CLASS, &CDlgSetSelect::OnCbnSelendokSelectObjClass)
	ON_CBN_SELENDOK(IDC_SELECT_OBJ_POSE, &CDlgSetSelect::OnCbnSelendokSelectObjPose)
	ON_CBN_SELENDOK(IDC_SELECT_OBJ_OCC, &CDlgSetSelect::OnCbnSelendokSelectObjOcc)
	ON_CBN_SELENDOK(IDC_SELECT_OBJ_TRUN, &CDlgSetSelect::OnCbnSelendokSelectObjTrun)
	ON_CBN_SELENDOK(IDC_SELECT_OBJ_DIFF, &CDlgSetSelect::OnCbnSelendokSelectObjDiff)
	ON_CBN_SELENDOK(IDC_SELECT_PART_OBJ, &CDlgSetSelect::OnCbnSelendokSelectPartObj)
	ON_CBN_SELENDOK(IDC_SELECT_PART_CLASS, &CDlgSetSelect::OnCbnSelendokSelectPartClass)
	ON_BN_CLICKED(IDOK, &CDlgSetSelect::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgSetSelect::OnBnClickedCheck1)
	ON_BN_CLICKED(IDCANCEL, &CDlgSetSelect::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSetSelect::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgSetSelect 消息处理程序

BOOL CDlgSetSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CBmodelPartCls.ResetContent();
	// TODO:  在此添加额外的初始化
	int i = 0;
	if (ableobject)
	{
		m_CBmodelOOP.InsertString(i, "部件");
		m_CBmodelOOP.InsertString(i, "目标");
	}
	m_CBmodelOOP.SetCurSel(m_iOOP);
	m_bPart = FALSE;
	//((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(1);
	//std::vector<std::pair<int, std::string> > lines_zh;
	//std::ifstream infile("21cls.txt");
	//int nId;
	//string clsname;
	//while (infile >> nId >> clsname)
	//{	

	//	lines_zh.push_back(std::make_pair(nId, clsname));

	//}
	//infile.close();

	/*i = 0;
	m_CBmodelObjCls.InsertString(i, "汽车");
	m_CBmodelObjCls.InsertString(i, "铁路");*/

	std::vector<std::pair<int, std::string> > lines_zh = m_pDoc->lines_zh;
	std::vector<CString> data_list;
	for (unsigned int i=0;i<lines_zh.size();i++)
	{
		data_list.push_back(lines_zh[i].second.c_str());
		m_CBmodelObjCls.InsertString(0,lines_zh[lines_zh.size()-i-1].second.c_str());
		vec_zhcls.push_back(lines_zh[i].second.c_str());
		//printf("%s\n",lines_zh[i].second.c_str());
	}
	//int tmp_class=0;
	m_CMemoryEdit.Create(data_list,&m_iObjCls,WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT, CRect(30, 70, 200, 300), this, 1001);
	m_iObjCls=tmp_class;
	m_CBmodelObjCls.SetCurSel(m_iObjCls);

	
	i = 0;
	m_CBmodelObjPos.InsertString(i, "未知");
	m_CBmodelObjPos.InsertString(i, "背面");
	m_CBmodelObjPos.InsertString(i, "右面");
	m_CBmodelObjPos.InsertString(i, "左面");
	m_CBmodelObjPos.InsertString(i, "正面");
	m_iObjPos   = 0;
	m_CBmodelObjPos.SetCurSel(m_iObjPos);



	i = 0;
	m_CBmodelObjOcc.InsertString(i, "否");
	m_CBmodelObjOcc.InsertString(i, "是");
	m_iObjOcc   = 1;
	m_CBmodelObjOcc.SetCurSel(1);

	i = 0;
	m_CBmodelObjTru.InsertString(i, "否");
	m_CBmodelObjTru.InsertString(i, "是");
	m_iObjTru   = 1;
	m_CBmodelObjTru.SetCurSel(1);

	i = 0;
	m_CBmodelObjDif.InsertString(i, "否");
	m_CBmodelObjDif.InsertString(i, "是");
	m_iObjDif   = 1;
	m_CBmodelObjDif.SetCurSel(m_iObjDif);

	i = 0;
	for (unsigned int j = m_pDoc->m_ObjList.size() + m_pDoc->m_Polygon.size() + m_pDoc->m_Curve.size(); j>0; --j)
	{
		if (ablepart)
		{
			ablepart = !ablepart;
			break;
		}
			
		CString strTmp;
		strTmp.Format("%d",j);
		m_CBmodelPartObj.InsertString(i, strTmp);
	}
	m_iPartObj  = 0;
	m_CBmodelPartObj.SetCurSel(0);

	i = 0;
	//m_CBmodelPartCls.InsertString(i, "足");
	//m_CBmodelPartCls.InsertString(i, "手");
	//m_CBmodelPartCls.InsertString(i, "下瓷瓶");
	//m_CBmodelPartCls.InsertString(i, "上瓷瓶");
	m_CBmodelPartCls.InsertString(i, "未知");
	m_CBmodelPartCls.InsertString(i, "右黄");
	m_CBmodelPartCls.InsertString(i, "中黄");
	m_CBmodelPartCls.InsertString(i, "左黄");
	m_CBmodelPartCls.InsertString(i, "右绿");
	m_CBmodelPartCls.InsertString(i, "右红");
	m_CBmodelPartCls.InsertString(i, "中绿");
	m_CBmodelPartCls.InsertString(i, "中红");
	m_CBmodelPartCls.InsertString(i, "左绿");
	m_CBmodelPartCls.InsertString(i, "左红");



	m_iPartCls  = 0;
	m_CBmodelPartCls.SetCurSel(m_iPartCls);



	if (0 != m_iOOP)
	{
		//CWnd* pWnd = CWnd::FromHandle(dlgSetSelect.m_hWnd);

		GetDlgItem(IDC_SELECT_OBJ_CLASS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_OBJ_POSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_OBJ_OCC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_OBJ_TRUN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_OBJ_DIFF)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_SELECT_PART_OBJ)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SELECT_PART_CLASS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_COLOR)->ShowWindow(SW_SHOW);
		//pWnd->UpdateWindow();
	}
	else
	{
		//	CWnd* pWnd = CWnd::FromHandle(dlgSetSelect.m_hWnd);
		GetDlgItem(IDC_SELECT_OBJ_CLASS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SELECT_OBJ_POSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SELECT_OBJ_OCC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SELECT_OBJ_TRUN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SELECT_OBJ_DIFF)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_SELECT_PART_OBJ)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_PART_CLASS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COLOR)->ShowWindow(SW_HIDE);
		//pWnd->UpdateWindow();
	}
	GetDlgItem(IDOK)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSetSelect::OnCbnSelendokSelectOop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iOOP = m_CBmodelOOP.GetCurSel();
	ablepart = TRUE;
	ableobject = FALSE;
	OnInitDialog();
}

void CDlgSetSelect::OnCbnSelendokSelectObjClass()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iObjCls = m_CBmodelObjCls.GetCurSel();
	tmp_class=m_iObjCls;
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectObjPose()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iObjPos = m_CBmodelObjPos.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectObjOcc()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iObjOcc = m_CBmodelOOP.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectObjTrun()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iObjTru = m_CBmodelObjTru.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectObjDiff()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iObjDif = m_CBmodelObjDif.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectPartObj()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPartObj = m_CBmodelPartObj.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnCbnSelendokSelectPartClass()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPartCls = m_CBmodelPartCls.GetCurSel();
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgSetSelect::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString CellID;
	GetDlgItem(1001)->GetWindowText(CellID);
	for (size_t i = 0; i < vec_zhcls.size(); i++)
	{
		if (CellID == vec_zhcls[i])
		{
			m_iObjCls = i;
			tmp_class = i;
		}
	}
	GetDlgItem(IDOK)->SetFocus();
	OnOK();
}


void CDlgSetSelect::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_bPart)
		m_bPart = FALSE;
	else
		m_bPart = TRUE;
}


void CDlgSetSelect::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialog::OnCancel();
}



void CDlgSetSelect::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

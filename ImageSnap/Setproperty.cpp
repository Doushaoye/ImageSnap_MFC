// Setproperty.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageSnap.h"
#include "Setproperty.h"
#include "afxdialogex.h"
#include "Json/json.h"
#include <iostream>
#include <fstream>


// CSetproperty 对话框

IMPLEMENT_DYNAMIC(CSetproperty, CDialogEx)

CSetproperty::CSetproperty(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetproperty::IDD, pParent)
{

}

CSetproperty::~CSetproperty()
{
}

void CSetproperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetproperty, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetproperty::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetproperty::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetproperty 消息处理程序


void CSetproperty::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	for (unsigned int i = 0; i < m_property.size(); i++)
	{
		
		CWnd* wd = GetDlgItem(1500 + i);
		HWND h_item = wd->GetSafeHwnd();
		char item_class[20];
		GetClassName(h_item, item_class,256);
		if (_tcsicmp(item_class, _T("ComboBox"))==0)
		{
			int listID = p_combo[i].GetCurSel();
			
			CString listChar;
			p_combo[i].GetLBText(listID, listChar);
			m_property[i].m_ablexuanxiang = listChar;
			listID = m_property[i].m_xuanxiang.size() - 1 - listID;
			m_property[i].m_propertyid = listID;
		}
		else if (_tcsicmp(item_class, _T("Edit"))==0)
		{
			m_property[i].m_value = GetDlgItemInt(1500 + i);
			m_property[i].m_propertyid = GetDlgItemInt(1500 + i);
		}
	
	}
	
	CDialogEx::OnOK();
}


void CSetproperty::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
void CSetproperty::ReadJson()
{
	using namespace Json;
	using namespace std;
	m_jsonpath = "set.json";
	ifstream ifs;
	ifs.open(m_jsonpath);
	Value root;
	CharReaderBuilder fh;
	JSONCPP_STRING errs;
	parseFromStream(fh, ifs, &root, &errs);

	if (root.isMember(m_classname))
	{
		Value::Members rootMember = root[m_classname].getMemberNames();
		Value v_class = root[m_classname];
		for (Value::Members::iterator it = rootMember.begin(); it != rootMember.end(); ++it)
		{
			if ((*it) != "EnToZh")
			{
				s_property s1;
				s1.m_property = *it;
				s1.m_property_zh = v_class["EnToZh"][*it].asString();
				if (v_class.isMember(*it) && v_class[*it].isArray())
				{
					for (int j = 0; j < v_class[*it].size(); j++)
					{
						s1.m_xuanxiang.push_back(v_class[*it][j].asString());
					}
				}
				if (v_class.isMember(*it) && v_class[*it].isInt())
				{
					s1.m_xuanxiang.push_back("Int");
				}
				m_property.push_back(s1);
			}
			
		}
	}
	else
	{
		OnBnClickedOk();
		return;
	}

}

BOOL CSetproperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ReadJson();

	//NewMyButton(ID_CBUTTON, CRect(10, 20, 60, 50), BS_DEFPUSHBUTTON);
	for (unsigned int i = 0; i < m_property.size(); i++)
	{

		p_text[i].Create(m_property[i].m_property_zh.c_str(), WS_CHILD | WS_VISIBLE, CRect(10, 20 + i * 50, 100, 50 + i * 50), this, 1400 + i); //创建按钮

		if (m_property[i].m_xuanxiang[0] == "Int")
		{
			p_edit[i].Create(WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(150, 20 + i * 50, 300, 50 + i * 50), this, 1500 + i);
		}
		else
		{
			p_combo[i].Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS | WS_VSCROLL, CRect(150, 20 + i * 50, 300, 50 + i * 50), this, 1500 + i); //创建按钮
			for (int j = 0; j <m_property[i].m_xuanxiang.size(); j++)
			{
				p_combo[i].InsertString(0, m_property[i].m_xuanxiang[j].c_str());
			}
			p_combo[i].SetCurSel(0);
			CRect rc;
			p_combo[i].GetWindowRect(&rc);
			ScreenToClient(&rc);
			rc.bottom = rc.top + 400;
			p_combo[i].MoveWindow(&rc);
			p_combo[i].SetItemHeight(1, 20);
		}
		

	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

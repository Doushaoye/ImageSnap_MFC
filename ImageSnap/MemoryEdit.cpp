// MemoryEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "MemoryEdit.h"


//----------CListBoxExt

CListBoxExt::CListBoxExt()
{
}

CListBoxExt::~CListBoxExt()
{
}

BEGIN_MESSAGE_MAP(CListBoxExt, CListBox)
	ON_MESSAGE(LB_FINDSTRING, CListBoxExt::OnLbFindString)
END_MESSAGE_MAP()


LRESULT CListBoxExt::OnLbFindString(WPARAM wParam, LPARAM lParam)
{
	return SendMessage(LB_FINDSTRINGEXACT, wParam, lParam);
}



//---------- CComboboxExt

IMPLEMENT_DYNAMIC(CMemoryEdit, CComboBox)

CMemoryEdit::CMemoryEdit()
{
	GetHistoryData();
}

CMemoryEdit::~CMemoryEdit()
{
}

bool CMemoryEdit::GetHistoryData()
{
     //读出即将用到的数据 
	return true;
}
BEGIN_MESSAGE_MAP(CMemoryEdit, CComboBox)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CMemoryEdit::OnCbnEditchange)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CComboboxExt 消息处理程序




//void CMemoryEdit::PreSubclassWindow()
//{
//	// TODO:  在此添加专用代码和/或调用基类
//	COMBOBOXINFO cbi = { 0 };
//	cbi.cbSize = sizeof(COMBOBOXINFO);
//	BOOL bRet = ::GetComboBoxInfo(m_hWnd, &cbi);
//	if (bRet&&NULL != cbi.hwndList)
//	{
//		if (m_pListBox.SubclassWindow(cbi.hwndList))
//		{
//			m_pListBox.m_pComboBox = this;
//		}
//	}
//
//	CComboBox::PreSubclassWindow();
//}


void CMemoryEdit::OnCbnEditchange()
{
	// TODO:  在此添加控件通知处理程序代码
	SendMessage(CB_GETEDITSEL, (WPARAM)&m_dwStartSel, (LPARAM)&m_dwEndSel);
	CString strEdit;
	GetWindowText(strEdit);
	int nIndex = GetCount();
	while (nIndex)
	{
		DeleteString(0);
		--nIndex;
	}
	ShowDropDown(false);
	if (strEdit.IsEmpty())
		return;

	bool bFind = false;
	std::vector<CString>::const_iterator ite = m_editData.begin();

	for (int i = 0; ite != m_editData.end(); ite++)
	{
		if (!(ite->Find(strEdit) < 0))
		{
			AddString(*ite);
			bFind = true;
		}
	}
	if (bFind)
	{
		ShowDropDown();
		SetWindowText(strEdit);
	}
	SetWindowText(strEdit);
	SetEditSel(m_dwStartSel, m_dwEndSel);
}



BOOL CMemoryEdit::Create(std::vector<CString>& data_list,int* cls,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO:  在此添加专用代码和/或调用基类
	m_editData = data_list;
	p_cls = cls;
	BOOL b = CComboBox::Create(dwStyle, rect, pParentWnd, nID);

	COMBOBOXINFO cbi = { 0 };
	cbi.cbSize = sizeof(COMBOBOXINFO);
	BOOL bRet = ::GetComboBoxInfo(m_hWnd, &cbi);
	if (bRet&&NULL != cbi.hwndList)
	{
		if (m_pListBox.SubclassWindow(cbi.hwndList))
		{
			m_pListBox.m_pComboBox = this;
		}
	}

	CComboBox::PreSubclassWindow();
    //初始化时addstring之前几次的历史数据

	int dataCount = m_editData.size();

	int nIndex = (dataCount > 5) ? 5 : dataCount;

	std::vector<CString>::iterator m_ite = m_editData.end();

	for (int i = 0; i < nIndex; i++)
	{
		AddString(*(m_ite - i - 1));
	}


	return b;
}

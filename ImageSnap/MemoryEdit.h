#pragma once


// CMemoryEdit

#pragma once

class CMemoryEdit;

class CListBoxExt : public CListBox
{
public:
	CListBoxExt();

	virtual ~CListBoxExt();
public:
	CMemoryEdit *m_pComboBox;
	afx_msg LRESULT OnLbFindString(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};


// CComboboxExt

class CMemoryEdit : public CComboBox
{
	DECLARE_DYNAMIC(CMemoryEdit)

public:
	CMemoryEdit();
	virtual ~CMemoryEdit();
protected:
	CListBoxExt m_pListBox;
	std::vector<CString> m_editData;  //�洢���ݿ��е���ʷ����

	CBrush m_brBackGnd;

	DWORD m_dwStartSel, m_dwEndSel; //edit�й���λ��
	int* p_cls;

protected:
	DECLARE_MESSAGE_MAP()

	//virtual void PreSubclassWindow();
public:
	afx_msg void OnCbnEditchange();
	bool GetHistoryData();
	virtual BOOL Create(std::vector<CString>& data_list , int* cls,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

};



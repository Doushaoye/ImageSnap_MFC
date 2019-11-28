
#pragma once
#include "Lable.h"
//#include "DlgSetSelect.h"

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();

// 属性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
	std::vector<std::pair<int, std::string> > lines_;
	
protected:
	int curexpendobj;
	int curexpendpart;
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	
	CComboBox m_propertiesclass;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;


// 实现
public:
	virtual ~CPropertiesWnd();
	void UpdataAllData(vector <_Object> Objlist,int expendboj,int expendpart);
	void UpdataAllData(vector <_Polygon> Polygonlist, int expendobj, int expendpart);
	void UpdataAllData(vector <_Curve> Polygonlist, int expendobj, int expendpart);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelchangeComboShowType();

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
	
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


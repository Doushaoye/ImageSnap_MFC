#pragma once

#include "ImageSnapDoc.h"
#include "MemoryEdit.h"
#include "afxwin.h"
// CDlgSetSelect 对话框

class CDlgSetSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetSelect)

public:
	CDlgSetSelect(CImageSnapDoc *pDoc,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetSelect();

// 对话框数据
	enum { IDD = IDD_SELECTSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CBmodelOOP;
	virtual BOOL OnInitDialog();
	CComboBox m_CBmodelObjCls;
	CComboBox m_CBmodelObjPos;
	CComboBox m_CBmodelObjOcc;
	CComboBox m_CBmodelObjTru;
	CComboBox m_CBmodelObjDif;
	CComboBox m_CBmodelPartObj;
	CComboBox m_CBmodelPartCls;
	CMemoryEdit m_CMemoryEdit;
	BOOL ablepart = FALSE;
	BOOL ableobject = TRUE;
	int m_iOOP;
	int m_iObjCls;
	int m_iObjPos;
	int m_iObjOcc;
	int m_iObjTru;
	int m_iObjDif;
	int m_iPartObj;
	int m_iPartCls;
	std::vector<CString> vec_zhcls;

	CImageSnapDoc *m_pDoc;
	afx_msg void OnCbnSelendokSelectOop();
	afx_msg void OnCbnSelendokSelectObjClass();
	afx_msg void OnCbnSelendokSelectObjPose();
	afx_msg void OnCbnSelendokSelectObjOcc();
	afx_msg void OnCbnSelendokSelectObjTrun();
	afx_msg void OnCbnSelendokSelectObjDiff();
	afx_msg void OnCbnSelendokSelectPartObj();
	afx_msg void OnCbnSelendokSelectPartClass();
	afx_msg void OnBnClickedOk();
	BOOL m_bPart;
	afx_msg void OnBnClickedCheck1();
	CEdit m_Ecolor;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
};

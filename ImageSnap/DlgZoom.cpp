// DlgZoom.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageSnap.h"
#include "DlgZoom.h"
#include "MainFrm.h"
#include "ImageSnapView.h"


// CDlgZoom 对话框

IMPLEMENT_DYNAMIC(CDlgZoom, CDialog)

CDlgZoom::CDlgZoom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgZoom::IDD, pParent)
	, m_zoomfactor(0)
{

}

CDlgZoom::~CDlgZoom()
{
}

void CDlgZoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ZOOM_FAC, m_zoomfactor);
}


BEGIN_MESSAGE_MAP(CDlgZoom, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgZoom::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgZoom 消息处理程序

void CDlgZoom::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

BOOL CDlgZoom::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *) pFrame->GetActiveView();
	m_zoomfactor = pView->m_ZoomFactor;
	UpdateData(FALSE);


	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

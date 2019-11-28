// DlgZoom.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageSnap.h"
#include "DlgZoom.h"
#include "MainFrm.h"
#include "ImageSnapView.h"


// CDlgZoom �Ի���

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


// CDlgZoom ��Ϣ�������

void CDlgZoom::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

BOOL CDlgZoom::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CImageSnapView *pView = (CImageSnapView *) pFrame->GetActiveView();
	m_zoomfactor = pView->m_ZoomFactor;
	UpdateData(FALSE);


	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

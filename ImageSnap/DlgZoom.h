#pragma once


// CDlgZoom �Ի���

class CDlgZoom : public CDialog
{
	DECLARE_DYNAMIC(CDlgZoom)

public:
	CDlgZoom(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgZoom();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
	double m_zoomfactor;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

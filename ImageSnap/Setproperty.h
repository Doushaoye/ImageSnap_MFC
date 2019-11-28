#include <vector>
#include <string>
#pragma once


// CSetproperty �Ի���
struct s_property
{
	std::string m_property_zh;
	std::string m_property;
	std::vector<std::string> m_xuanxiang;
	std::string m_ablexuanxiang;
	int m_value;
	int m_propertyid;
};
class CSetproperty : public CDialogEx
{
	DECLARE_DYNAMIC(CSetproperty)

public:
	CSetproperty(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetproperty();

	int m_classlabel;
	std::string m_jsonpath;
	std::string m_classname;
	std::vector<s_property> m_property;

	CStatic p_text[10];
	CComboBox p_combo[10];
	CEdit p_edit[10];

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	void ReadJson();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};

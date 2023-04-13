#pragma once


// CToolDlg 对话框

class CToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnBnClickedCreatelayout();
	afx_msg void OnBnClickedDellayout();
	afx_msg void OnBnClickedChangecolor();
	afx_msg void OnBnClickedShowlayerstatus();
	afx_msg void OnBnClickedCreatebox();
	afx_msg void OnBnClickedCreatefrustum();
	afx_msg void OnBnClickedCreatespire();
};

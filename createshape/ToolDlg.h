#pragma once


// CToolDlg �Ի���

class CToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

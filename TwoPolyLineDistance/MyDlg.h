#pragma once
#include "resource.h"
#include "Distance.h"
// CMyDlg 对话框

class CMyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDlg)

public:
	CMyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDlg();

// 对话框数据
	enum { IDD = IDD_PlylineDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreatetwopolyline();
	afx_msg void OnBnClickedButton2();


private:
	AcDbObjectId m_polyLineID_1;
	AcDbObjectId m_polyLineID_2;

	CDistance m_dis;

public:
	afx_msg void OnNcDestroy();
};

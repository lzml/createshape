#pragma once
#include "resource.h"
#include "Distance.h"
// CMyDlg �Ի���

class CMyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDlg)

public:
	CMyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyDlg();

// �Ի�������
	enum { IDD = IDD_PlylineDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

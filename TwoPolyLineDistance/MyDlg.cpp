// MyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyDlg.h"
#include "afxdialogex.h"

extern CMyDlg* g_mydlg ;

// CMyDlg 对话框

IMPLEMENT_DYNAMIC(CMyDlg, CDialogEx)

	CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyDlg::IDD, pParent)
{

}

CMyDlg::~CMyDlg()
{
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CreateTwoPolyLine, &CMyDlg::OnBnClickedCreatetwopolyline)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyDlg::OnBnClickedButton2)
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CMyDlg 消息处理程序


void CMyDlg::OnBnClickedCreatetwopolyline()
{
	// TODO: 在此添加控件通知处理程序代码
	
	acDocManager->lockDocument(acDocManager->curDocument());

	AcDbBlockTable *pBlockTable = NULL;
	acDocManager->curDocument()->database()->getBlockTable(pBlockTable, AcDb::kForRead);
	if (pBlockTable)
	{
		AcDbBlockTableRecord *pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		if (pBlockTableRecord)
		{
			//至少应该有2个点，不超过5个点
			srand((unsigned int)time(NULL));
			int pointNum_1 = rand()%4 +1;
			int pointNum_2 = rand()%4 +1;

			//创建两个多线段
			AcDbPolyline *pPolyLine1 = new AcDbPolyline();
			AcDbPolyline *pPolyLine2 = new AcDbPolyline();

			AcGePoint2dArray pointArray_1, pointArray_2;
			for (int i=0;i<pointNum_1; i++)
			{
				pPolyLine1->addVertexAt(i, AcGePoint2d(rand()	%200,rand()	%200), rand()%300/100.0,0,0);
			}
			for (int i=0;i<pointNum_2; i++)
			{
				pPolyLine2->addVertexAt(i, AcGePoint2d(rand()	%200,rand()	%200), rand()%300/100.0,0,0);
			}

			pBlockTableRecord->appendAcDbEntity(m_polyLineID_1, pPolyLine1);
			pBlockTableRecord->appendAcDbEntity(m_polyLineID_2, pPolyLine2);

			pPolyLine1->close();
			pPolyLine2->close();


			pBlockTableRecord->close();
		}

		pBlockTable->close();
	}



	acDocManager->unlockDocument(acDocManager->curDocument());


}


void CMyDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	AcDbObjectId id_1;
	AcDbObjectId id_2;

	{
		ads_name name;
		ads_point ptPick;
		if (acedEntSel(_T("\n选择第一个多线段： "),  name, ptPick) == RTNORM)
		{
			acdbGetObjectId(id_1, name);
		}
		else return;
	}

	{
		ads_name name;
		ads_point ptPick;
		if (acedEntSel(_T("\n选择第二个多线段： "),  name, ptPick) == RTNORM)
		{
			acdbGetObjectId(id_2, name);
		}
		else return;
	}

	if (id_1.isValid()  && id_2.isValid())
	{
		m_dis.setTwoPolyline(id_1, id_2);

		acDocManager->lockDocument(acDocManager->curDocument());
		CDisInfo beelineInfo;
		m_dis.DistanceTwoPolyline(beelineInfo);

		AcDbBlockTable *pBlockTable = NULL;
		acDocManager->curDocument()->database()->getBlockTable(pBlockTable, AcDb::kForRead);
		if (pBlockTable)
		{
			AcDbBlockTableRecord *pBlockTableRecord = NULL;
			pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
			if (pBlockTableRecord)
			{
				AcDbLine* pbeelline = new AcDbLine(AcGePoint3d(beelineInfo.pt1.x, beelineInfo.pt1.y, 0),
					AcGePoint3d(beelineInfo.pt2.x, beelineInfo.pt2.y, 0) );
				AcDbObjectId beellineID;
				pbeelline->setColorIndex(1);
				pBlockTableRecord->appendAcDbEntity(beellineID, pbeelline);

				pbeelline->close();
				pBlockTableRecord->close();
			}

			pBlockTable->close();
		}


		acDocManager->unlockDocument(acDocManager->curDocument());

	}


	

}


void CMyDlg::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (NULL != g_mydlg)
	{
		delete g_mydlg;
		g_mydlg = NULL;
	}


}

kHasBookName
// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ToolDlg.h"
#include "afxdialogex.h"


extern CToolDlg* g_pToolDlg;

// CToolDlg 对话框

IMPLEMENT_DYNAMIC(CToolDlg, CDialogEx)

CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToolDlg::IDD, pParent)
{

}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CToolDlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_CreateLayout, &CToolDlg::OnBnClickedCreatelayout)
	ON_BN_CLICKED(IDC_DelLayout, &CToolDlg::OnBnClickedDellayout)
	ON_BN_CLICKED(IDC_ChangeColor, &CToolDlg::OnBnClickedChangecolor)
	ON_BN_CLICKED(IDC_ShowLayerStatus, &CToolDlg::OnBnClickedShowlayerstatus)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序


void CToolDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(false);
	//CDialogEx::OnClose();

}


void CToolDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	AcDbBlockTable * pBlktbl = NULL;
	acDocManager->lockDocument(acDocManager->curDocument());
	Acad::ErrorStatus acadErr =  acdbHostApplicationServices()->workingDatabase()
		->getBlockTable(pBlktbl, AcDb::kForWrite);



	if (Acad::eOk != acadErr )
	{
			acDocManager->unlockDocument(acDocManager->curDocument());
		return ;
	}

	//创建新的块表记录
	AcDbBlockTableRecord* pBlktblRcd;
	pBlktblRcd = new AcDbBlockTableRecord();

	//根据用户输入设置快表名称
	ACHAR blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入块名称："), blkName) !=RTNORM)
	{
		pBlktbl->close();
		delete pBlktblRcd;
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}
	pBlktblRcd->setName(blkName);

	//将块添加到快表中
	AcDbObjectId blkDefld;
	acadErr = pBlktbl->add(blkDefld, pBlktblRcd);
	
	pBlktbl->close();

	AcGePoint3d ptStart(-10,10,0), ptEnd(10,10,0);
	AcDbLine *pLine1 = new AcDbLine(ptStart,ptEnd);
	ptStart.set(20,-10,0);
	ptEnd.set(20,10,0);
	AcDbLine *pLine2 = new AcDbLine(ptStart, ptEnd);

	AcGeVector3d vecNormal(0,0,1);
	AcDbCircle *pCircle = new AcDbCircle(AcGePoint3d::kOrigin, vecNormal, 20);
	AcDbObjectId entid;
	acadErr = pBlktblRcd->appendAcDbEntity(entid, pLine1);
	acadErr = pBlktblRcd->appendAcDbEntity(entid, pLine2);
	pBlktblRcd->appendAcDbEntity(entid, pCircle);

	//创建一个属性
	AcDbAttributeDefinition *pAttdef = new AcDbAttributeDefinition(ptEnd,
		_T("20"), _T("直径"),_T("输入直径"));
	pBlktblRcd->appendAcDbEntity(blkDefld, pAttdef);

	pLine1->close();
	pLine2->close();
	pCircle->close();
	pAttdef->close();

	pBlktblRcd->close();


	acDocManager->unlockDocument(acDocManager->curDocument());

}


void CToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	AcDbBlockTable* pBlktbl = NULL;
	acDocManager->lockDocument(acDocManager->curDocument());

	acDocManager->curDocument()->database()->getBlockTable(pBlktbl,  AcDb::kForWrite);

	//根据用户输入删除快表记录
	ACHAR blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n输入要删除的块名称："), blkName) !=RTNORM)
	{
		pBlktbl->close();
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}

	AcDbBlockTableRecord* pBlockTableRecord = NULL;
	if (pBlktbl->has(blkName))
	{
		if (pBlktbl->getAt(blkName, pBlockTableRecord, AcDb::kForWrite) == Acad::eOk)
		{
			if (!pBlockTableRecord->isDependent())
			{
				pBlockTableRecord->erase();
			}
		}
	}
	if (NULL !=  pBlockTableRecord)
	{
		pBlockTableRecord->close();
	}

	pBlktbl->close();

	acDocManager->unlockDocument(acDocManager->curDocument());


}


void CToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	
}


void CToolDlg::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();

	delete g_pToolDlg;
	g_pToolDlg = NULL;
	// TODO: 在此处添加消息处理程序代码
}


void CToolDlg::OnBnClickedCreatelayout()
{
	// TODO: 在此添加控件通知处理程序代码

	ACHAR layerName[100];
	if (acedGetString(Adesk::kFalse, _T("\n 输入新图层的名称: "), layerName) !=RTNORM)
	{
		return;
	}

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbLayerTable* pLayerTable = NULL;
	acDocManager->curDocument()->database()
		->getLayerTable(pLayerTable,AcDb::kForWrite);
	if (NULL == pLayerTable)
	{
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}
	if (pLayerTable->has(layerName))
	{
		pLayerTable->close();
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}

	//新建层表记录
	AcDbLayerTableRecord *pLayerRecord = NULL;
	pLayerRecord = new AcDbLayerTableRecord();
	pLayerRecord->setName(layerName);
	
	AcDbObjectId layerId;
	pLayerTable->add(layerId, pLayerRecord);

	acDocManager->curDocument()->database()->setClayer(layerId);

	pLayerRecord->close();
	pLayerTable->close();

	acDocManager->unlockDocument(acDocManager->curDocument());




}


void CToolDlg::OnBnClickedDellayout()
{
	// TODO: 在此添加控件通知处理程序代码

	ACHAR layerName[40] = {};
	if (acedGetString(Adesk::kFalse, _T("\n 输入要删除的图层名称"), layerName) != RTNORM )
	{
		return;
	}

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbLayerTable *pLayerTable= NULL;
	acDocManager->curDocument()->database()->getLayerTable(pLayerTable,  AcDb::kForWrite);
	if (NULL == pLayerTable)
	{
		acDocManager->unlockDocument(acDocManager->curDocument());
	}

	AcDbLayerTableRecord* pLayerRecord = NULL;
	if (pLayerTable->has(layerName))
	{

		if (pLayerTable->getAt(layerName, pLayerRecord, AcDb::kForWrite) == Acad::eOk)
		{
			if (NULL != pLayerRecord)
			{
				pLayerRecord->erase();
			}

		}
	}

	if (pLayerRecord)
	{
		pLayerRecord->close();
	}
	if (pLayerTable)
	{
		pLayerTable->close();
	}

	acDocManager->unlockDocument(acDocManager->curDocument());


}


void CToolDlg::OnBnClickedChangecolor()
{
	// TODO: 在此添加控件通知处理程序代码
	ACHAR layerName[40]= {};
	if (acedGetString(Adesk::kFalse, _T("\n 输入要改变颜色的图层名称： "), layerName) != RTNORM)
	{
		return;
	}

	//获取当前图形的层表

	acDocManager->lockDocument(acDocManager->curDocument());

	AcDbLayerTable *pLayerTable  =   NULL;
	acDocManager->curDocument()->database()->getLayerTable(pLayerTable,AcDb::kForWrite);
	if (NULL == pLayerTable)
	{
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}

	if ( !pLayerTable->has(layerName))
	{
			acDocManager->unlockDocument(acDocManager->curDocument());
			return;
	}

	AcDbLayerTableRecord *pLayerRecord = NULL;
	if (pLayerTable->getAt(layerName, pLayerRecord, AcDb::kForWrite) == Acad::eOk)
	{
		AcCmColor oldColor = pLayerRecord->color();
		int nCurColor = oldColor.colorIndex();
		int nNewColor = oldColor.colorIndex();
		if (acedSetColorDialog(nNewColor, Adesk::kFalse,nCurColor))
		{
			AcCmColor color;
			color.setColorIndex(nNewColor);
			pLayerRecord->setColor(color);
		}
	}

	if (pLayerRecord)
	{
		pLayerRecord->close();
	}
	if (pLayerTable)
	{
		pLayerTable->close();
	}


	acDocManager->unlockDocument(acDocManager->curDocument());


}


void CToolDlg::OnBnClickedShowlayerstatus()
{
	// TODO: 在此添加控件通知处理程序代码
	AcDbLayerTable *pLayerTabel = NULL;
	acDocManager->curDocument()->database()->getLayerTable(pLayerTabel, AcDb::kForRead);

	AcDbLayerTableIterator *pLayerTabelIterator = NULL;
	pLayerTabel->newIterator(pLayerTabelIterator);


	for (pLayerTabelIterator->start(); !pLayerTabelIterator->done(); pLayerTabelIterator->step())
	{
		AcDbLayerTableRecord* pLayerRecord = NULL;
		pLayerTabelIterator->getRecord(pLayerRecord, AcDb::kForRead);
		AcString layerName;
		pLayerRecord->getName(layerName);
		
		acutPrintf(_T("\n图层名字：%s   图层是否隐藏:  %d \n"), layerName.kwszPtr(),!pLayerRecord->isOff());

		pLayerRecord->close();
	}

	pLayerTabel->close();


}

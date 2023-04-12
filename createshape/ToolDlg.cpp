kHasBookName
// ToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ToolDlg.h"
#include "afxdialogex.h"


extern CToolDlg* g_pToolDlg;

// CToolDlg �Ի���

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


// CToolDlg ��Ϣ�������


void CToolDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ShowWindow(false);
	//CDialogEx::OnClose();

}


void CToolDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AcDbBlockTable * pBlktbl = NULL;
	acDocManager->lockDocument(acDocManager->curDocument());
	Acad::ErrorStatus acadErr =  acdbHostApplicationServices()->workingDatabase()
		->getBlockTable(pBlktbl, AcDb::kForWrite);



	if (Acad::eOk != acadErr )
	{
			acDocManager->unlockDocument(acDocManager->curDocument());
		return ;
	}

	//�����µĿ���¼
	AcDbBlockTableRecord* pBlktblRcd;
	pBlktblRcd = new AcDbBlockTableRecord();

	//�����û��������ÿ������
	ACHAR blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n��������ƣ�"), blkName) !=RTNORM)
	{
		pBlktbl->close();
		delete pBlktblRcd;
			acDocManager->unlockDocument(acDocManager->curDocument());
		return;
	}
	pBlktblRcd->setName(blkName);

	//������ӵ������
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

	//����һ������
	AcDbAttributeDefinition *pAttdef = new AcDbAttributeDefinition(ptEnd,
		_T("20"), _T("ֱ��"),_T("����ֱ��"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	AcDbBlockTable* pBlktbl = NULL;
	acDocManager->lockDocument(acDocManager->curDocument());

	acDocManager->curDocument()->database()->getBlockTable(pBlktbl,  AcDb::kForWrite);

	//�����û�����ɾ������¼
	ACHAR blkName[40];
	if (acedGetString(Adesk::kFalse, _T("\n����Ҫɾ���Ŀ����ƣ�"), blkName) !=RTNORM)
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

	// TODO: �ڴ˴������Ϣ����������
	
}


void CToolDlg::OnNcDestroy()
{
	CDialogEx::OnNcDestroy();

	delete g_pToolDlg;
	g_pToolDlg = NULL;
	// TODO: �ڴ˴������Ϣ����������
}


void CToolDlg::OnBnClickedCreatelayout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	ACHAR layerName[100];
	if (acedGetString(Adesk::kFalse, _T("\n ������ͼ�������: "), layerName) !=RTNORM)
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

	//�½�����¼
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	ACHAR layerName[40] = {};
	if (acedGetString(Adesk::kFalse, _T("\n ����Ҫɾ����ͼ������"), layerName) != RTNORM )
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ACHAR layerName[40]= {};
	if (acedGetString(Adesk::kFalse, _T("\n ����Ҫ�ı���ɫ��ͼ�����ƣ� "), layerName) != RTNORM)
	{
		return;
	}

	//��ȡ��ǰͼ�εĲ��

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		
		acutPrintf(_T("\nͼ�����֣�%s   ͼ���Ƿ�����:  %d \n"), layerName.kwszPtr(),!pLayerRecord->isOff());

		pLayerRecord->close();
	}

	pLayerTabel->close();


}

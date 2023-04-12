#include "stdafx.h"
#include "Shape.h"
#include "StdAfx.h"

CShape::CShape(void)
{
}


CShape::~CShape(void)
{
}


void CShape::draw()
{
	//��ȡͼ�����ݿ������Ϣ

	//���ָ�����ָ��
	AcDbBlockTable *pBlockTable = NULL;
	Acad::ErrorStatus acadErr = acdbHostApplicationServices()
		->workingDatabase()->getBlockTable(
		pBlockTable,
		AcDb::kForRead);

	//����¼��ָ��
	AcDbBlockTableRecord *pblockTableRecord = NULL;

	if (Acad::eOk == acadErr )
	{
		//���ָ���ض��Ŀ���¼��ģ�Ϳռ䣩��ָ��		
		acadErr = pBlockTable->getAt(ACDB_MODEL_SPACE, pblockTableRecord, AcDb::kForWrite);
		if (Acad::eOk == acadErr)
		{

			drawItem(pblockTableRecord);
		}
		else{
			acutPrintf(L"\n err: ��ȡ����¼ָ��ʧ��");
		}
	}
	else
	{

		acutPrintf(L"\n err: ��ȡ���ָ��ʧ��");
	}

	if (pblockTableRecord != NULL)
	{
		pblockTableRecord->close();
	}
	
	if (pBlockTable != NULL)
	{
		pBlockTable->close();
	}
}

void CShape::drawItem(AcDbBlockTableRecord*  pRecord)
{



}
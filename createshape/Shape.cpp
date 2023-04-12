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
	//获取图形数据库相关信息

	//获得指向块表的指针
	AcDbBlockTable *pBlockTable = NULL;
	Acad::ErrorStatus acadErr = acdbHostApplicationServices()
		->workingDatabase()->getBlockTable(
		pBlockTable,
		AcDb::kForRead);

	//块表记录的指针
	AcDbBlockTableRecord *pblockTableRecord = NULL;

	if (Acad::eOk == acadErr )
	{
		//获得指向特定的块表记录（模型空间）的指针		
		acadErr = pBlockTable->getAt(ACDB_MODEL_SPACE, pblockTableRecord, AcDb::kForWrite);
		if (Acad::eOk == acadErr)
		{

			drawItem(pblockTableRecord);
		}
		else{
			acutPrintf(L"\n err: 获取块表记录指针失败");
		}
	}
	else
	{

		acutPrintf(L"\n err: 获取块表指针失败");
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
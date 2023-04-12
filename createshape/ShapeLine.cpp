#include "stdafx.h"
#include "ShapeLine.h"


CShapeLine::CShapeLine(void)
{
}


CShapeLine::~CShapeLine(void)
{
}

void CShapeLine::drawItem(AcDbBlockTableRecord* pRecord)
{
	if ( NULL != pRecord)
	{

		//创建一个AcDbline对象
		AcGePoint3d ptStart(0,0,0);
		AcGePoint3d ptEnd(100,100,0);
		AcDbLine *pLine = new AcDbLine(ptStart, ptEnd);

		AcDbObjectId lineId;
		pRecord->appendAcDbEntity(lineId, pLine);

		pLine->close();
		return;
	}

}
#include "stdafx.h"
#include "ShapeArc.h"
#include <math.h>

CShapeArc::CShapeArc(void)
{
}


CShapeArc::~CShapeArc(void)
{
}

void CShapeArc::drawItem(AcDbBlockTableRecord* pRecord)
{
	AcGeVector3d vec(0,0,1);
	AcGePoint3d ptCenter(100,150,0);

	AcDbArc *pArc = new AcDbArc(ptCenter, vec, 100, 0,0.5* 4*std::atan(1.0));
	AcDbObjectId arcId ;
	pRecord->appendAcDbEntity(arcId, pArc);
	pArc->close();
}


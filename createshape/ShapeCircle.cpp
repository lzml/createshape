#include "stdafx.h"
#include "ShapeCircle.h"


CShapeCircle::CShapeCircle(void)
{
}


CShapeCircle::~CShapeCircle(void)
{
}

void CShapeCircle::drawItem(AcDbBlockTableRecord* pRecord)
{
	AcGePoint3d ptCenter(100,100,0);
	AcGeVector3d vec(0,0,1);
	AcDbCircle *pCircle = new AcDbCircle(ptCenter, vec, 50);

	AcDbObjectId circleId;
	pRecord->appendAcDbEntity(circleId, pCircle);
	pCircle->close();

}
#include "stdafx.h"
#include "ShapeFrustum.h"


CShapeFrustum::CShapeFrustum(void)
{
}


CShapeFrustum::~CShapeFrustum(void)
{
}

void CShapeFrustum::drawItem(AcDbBlockTableRecord* pRecord)
{
	AcDb3dSolid* pSolid = new AcDb3dSolid;
	pSolid->createFrustum(30,10,10,0);


	AcGeMatrix3d xform;
	AcGeVector3d vec(100,200,50);
	xform.setTranslation(vec);
	pSolid->transformBy(xform);

	AcDbObjectId id;
	pRecord->appendAcDbEntity(id, pSolid);

	pSolid->close();

}

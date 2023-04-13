#include "stdafx.h"
#include "ShapeBox.h"


CShapeBox::CShapeBox(void)
{
}


CShapeBox::~CShapeBox(void)
{
}

void CShapeBox::drawItem(AcDbBlockTableRecord* pRecord)
{
	
	AcDb3dSolid *pSolid = new AcDb3dSolid;
	Acad::ErrorStatus es = pSolid->createBox(40,50,30);
	if (Acad::eOk != es)
	{

	//	acedAlert(_T("创建长方体失败"));
		
		pSolid->close();
		delete pSolid;
		return;
	}

	//acedAlert(_T("创建长方体成功"));
	AcGeMatrix3d xform;
	AcGeVector3d vec(100,100,100);
	xform.setToTranslation(vec);
	pSolid->transformBy(xform);

	AcDbObjectId objId;
	pRecord->appendAcDbEntity(objId, pSolid);

	pSolid->close();
}


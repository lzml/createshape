#pragma once
#include "shape.h"
class CShapeSpire :
	public CShape
{
public:
	CShapeSpire(void);
	~CShapeSpire(void);

private:
	virtual void drawItem(AcDbBlockTableRecord* pRecord);

	AcDbObjectIdArray CreateRegion(const AcDbObjectIdArray& curveIds, AcDbBlockTableRecord* pRecord);

};


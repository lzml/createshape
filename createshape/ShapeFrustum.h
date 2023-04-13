#pragma once
#include "shape.h"
class CShapeFrustum :
	public CShape
{
public:
	CShapeFrustum(void);
	~CShapeFrustum(void);

private:
	virtual void drawItem(AcDbBlockTableRecord* pRecord);
};


#pragma once
#include "shape.h"
class CShapeBox :
	public CShape
{
public:
	CShapeBox(void);
	~CShapeBox(void);

private:
	virtual void drawItem(AcDbBlockTableRecord* pRecord);
};


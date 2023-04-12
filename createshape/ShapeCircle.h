#pragma once
#include "Shape.h"

class CShapeCircle : 
	public CShape
{
public:
	CShapeCircle(void);
	~CShapeCircle(void);

private:
	virtual void drawItem(AcDbBlockTableRecord* pRecord);
};


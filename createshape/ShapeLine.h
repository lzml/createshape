#pragma once
#include "shape.h"
class CShapeLine :
	public CShape
{
public:
	CShapeLine(void);
	~CShapeLine(void);
	
	virtual void drawItem(AcDbBlockTableRecord* pRecord);
};


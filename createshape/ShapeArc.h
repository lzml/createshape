#pragma once
#include "Shape.h"

class CShapeArc : public CShape
{
public:
	CShapeArc(void);
	~CShapeArc(void);

	virtual void drawItem(AcDbBlockTableRecord* pRecord);
};


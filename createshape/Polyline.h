#pragma once
#include "shape.h"
class CPolyline :
	public CShape
{
public:
	CPolyline(void);
	~CPolyline(void);

	virtual void drawItem(AcDbBlockTableRecord* pRecord);

private:
	
};


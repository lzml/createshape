#pragma once
class CShape
{
public:
	CShape(void);
	virtual	~CShape(void);

public:
	void draw();
private:
	virtual void drawItem(AcDbBlockTableRecord*  pRecord); 
};


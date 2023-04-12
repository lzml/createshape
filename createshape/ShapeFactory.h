#pragma once
#include "Shape.h"


enum ShapeID
{
	LINE,
	CIRCLE,
	ARC,
	POLYLINE,
};



class CShapeFactory
{
private:
	CShapeFactory(void);
	~CShapeFactory(void);
public:
	static CShapeFactory* getInstance();
	static void release();

public:
	CShape* createShape(ShapeID id);
	void releaseShape(CShape *p);

private:
	static CShapeFactory* s_factory;
};


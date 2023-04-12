#include "stdafx.h"
#include "ShapeFactory.h"
#include "ShapeLine.h"
#include "ShapeCircle.h"
#include "ShapeArc.h"
#include "Polyline.h"

CShapeFactory::CShapeFactory(void)
{
}


CShapeFactory::~CShapeFactory(void)
{
}


CShapeFactory* CShapeFactory::getInstance()
{
	if (NULL ==  s_factory)
	{
		return new CShapeFactory();
	}
	return s_factory;
}


void CShapeFactory::release(){
	if (s_factory != NULL)
	{
		delete s_factory;
	}
}

CShape* CShapeFactory::createShape(ShapeID id)
{
	if (ShapeID::LINE == id)
	{
		return new CShapeLine;
	}
	else if (ShapeID::CIRCLE == id)
	{
		return new CShapeCircle;
	}
	else if (ShapeID::ARC == id)
	{
		return new CShapeArc;
	}
	else if (ShapeID::POLYLINE == id)
	{
		return new CPolyline;
	}

	return NULL;
}

void CShapeFactory::releaseShape(CShape *p)
{
	delete p;
}


CShapeFactory* CShapeFactory::s_factory = NULL;
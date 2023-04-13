#include "stdafx.h"
#include "ShapeFactory.h"
#include "ShapeLine.h"
#include "ShapeCircle.h"
#include "ShapeArc.h"
#include "Polyline.h"
#include "ShapeBox.h"
#include "ShapeFrustum.h"
#include "ShapeSpire.h"

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
	else if (ShapeID::BOX == id)
	{
		return new CShapeBox;
	}
	else if (ShapeID::FRUSTUM == id)
	{
		return new CShapeFrustum;
	}
	else if (ShapeID::SPIRE == id)
	{
		return new CShapeSpire;
	}

	return NULL;
}

void CShapeFactory::releaseShape(CShape *p)
{
	delete p;
}


CShapeFactory* CShapeFactory::s_factory = NULL;
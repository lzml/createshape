#include "stdafx.h"
#include "Polyline.h"


CPolyline::CPolyline(void)
{
}


CPolyline::~CPolyline(void)
{
}


void CPolyline::drawItem(AcDbBlockTableRecord* pRecord)
{
	AcGePoint2dArray pointArray;
	pointArray.append(AcGePoint2d(0,0));
	pointArray.append(AcGePoint2d(20,40));
	pointArray.append(AcGePoint2d(20,60));

	AcDbPolyline *pPolyLine = new AcDbPolyline(pointArray.length());
	for (int i=0; i<pointArray.length(); i++)
	{
		pPolyLine->addVertexAt(i,pointArray.at(i),0,0,0);
	}

	//¼ÓÒ»¶Î»¡Ïß
	pPolyLine->addVertexAt(3, AcGePoint2d(30,80),1.0, 0,0);
	pPolyLine->addVertexAt(4, AcGePoint2d(40,100),-0.5, 0,0);
	pPolyLine->addVertexAt(5,AcGePoint2d(50,50),0,0,0);

	AcDbObjectId polylineId;
	pRecord->appendAcDbEntity(polylineId, pPolyLine);

	pPolyLine->close();
}
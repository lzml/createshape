#include "stdafx.h"
#include "ShapeSpire.h"
#include <math.h>


CShapeSpire::CShapeSpire(void)
{
}


CShapeSpire::~CShapeSpire(void)
{
}

void CShapeSpire::drawItem(AcDbBlockTableRecord* pRecord)
{
	//指定螺旋线参数
	double radius(30), deltaVertical(12); //半径和每一周在垂直方向上的增量
	double number(5), segment(30);  //螺旋线的旋转圈数，组成一圈的分段数

	//计算点的个数和角度间隔
	int n = number * segment; //点的个数实际上是n+1
	double angle = 8*std::atan(1.0)/segment; //两点之间的旋转角度

	//计算控制点的坐标
	AcGePoint3dArray points;
	const double PI = 4*atan(	1.0);
	for (int i =0; i<n+1; i++)
	{
		AcGePoint3d pt;
		pt.set(	radius*std::cos(angle * i),
			radius*std::sin(angle*i),
			i * deltaVertical / segment
			);
		points.append(pt);
	}

	//创建螺旋路径
	AcDb3dPolyline * p3dPoly = new AcDb3dPolyline(AcDb::k3dSimplePoly, points);

	//将路径添加到模型空间
	AcDbObjectId spireId;
	pRecord->appendAcDbEntity(spireId, p3dPoly);

	p3dPoly->close();

	//创建一个圆作为拉伸界面
	AcGeVector3d vec (0,1,0); // 圆所在平面法向量
	AcGePoint3d ptCenter(30,0,0); //圆心位置和半径大小
	AcDbCircle* pCircle = new AcDbCircle(ptCenter, vec, 3);
	AcDbObjectId circleId;
	pRecord->appendAcDbEntity(circleId, pCircle);
	pCircle->close();

	//根据圆创建一个面域
	AcDbObjectIdArray boundaryIds, regionIds;
	boundaryIds.append(circleId);
	regionIds =CreateRegion(boundaryIds, pRecord);

	//打开拉伸界面和拉伸路径
	AcDbRegion* pRegion; 
	acdbOpenObject(pRegion, regionIds.at(0), AcDb::kForRead);
	AcDb3dPolyline* pPoly;
	acdbOpenObject(pPoly, spireId, AcDb::kForRead);

	//进行拉伸操作
	AcDb3dSolid * pSolid = new AcDb3dSolid;
	pSolid->extrudeAlongPath(pRegion, pPoly);
	AcDbObjectId solidId;
	pRecord->appendAcDbEntity(solidId, pSolid);

	pCircle->close();
	pPoly->clone();
	pRegion->close();
}



AcDbObjectIdArray CShapeSpire::CreateRegion(const AcDbObjectIdArray& curveIds,
	AcDbBlockTableRecord* pRecord)
{
	AcDbObjectIdArray regionIds; // 生成的面域的id数组
	AcDbVoidPtrArray curves; //指向作为面域边界的曲线的指针的数组
	AcDbVoidPtrArray regions; // 指向创建的面域对象的指针的数组
	AcDbEntity *pEnt; //临时指针，用来关闭边界曲线
	AcDbRegion* pRegion; //临时对象，用来将面域添加到模型空间

	//用curveIds初始化curves
	Acad::ErrorStatus es;
	for (int i=0; i<curveIds.length(); i++)
	{
		es = acdbOpenAcDbEntity(pEnt, curveIds.at(i), AcDb::kForRead);
		if (pEnt->isKindOf(AcDbCurve::desc()))
		{
			curves.append(static_cast<void*>(pEnt));
		}
	}

	 es = AcDbRegion::createFromCurves(curves, regions);
	if (Acad::eOk == es)
	{
		//将生成的面域添加到模型空间
		for (int i= 0; i<regions.length(); i++)
		{
			//将空指针转换成指向面域的对象的指针
			pRegion = static_cast<AcDbRegion*>(regions[i]);
			pRegion->setDatabaseDefaults();
			AcDbObjectId regionId;
			pRecord->appendAcDbEntity(regionId, pRegion);
			pRegion->close();
			regionIds.append(regionId);
		}
	}
	else
	{
		for (int i=0; i<regions.length(); i++)
		{
			delete (AcRxObject*)regions[i];
		}

	}

	for (int i = 0; i<curves.length(); i++)
	{
		pEnt = static_cast<AcDbEntity*>(curves[i]);
		pEnt->close();
	}

	return regionIds;
}

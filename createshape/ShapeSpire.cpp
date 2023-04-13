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
	//ָ�������߲���
	double radius(30), deltaVertical(12); //�뾶��ÿһ���ڴ�ֱ�����ϵ�����
	double number(5), segment(30);  //�����ߵ���תȦ�������һȦ�ķֶ���

	//�����ĸ����ͽǶȼ��
	int n = number * segment; //��ĸ���ʵ������n+1
	double angle = 8*std::atan(1.0)/segment; //����֮�����ת�Ƕ�

	//������Ƶ������
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

	//��������·��
	AcDb3dPolyline * p3dPoly = new AcDb3dPolyline(AcDb::k3dSimplePoly, points);

	//��·����ӵ�ģ�Ϳռ�
	AcDbObjectId spireId;
	pRecord->appendAcDbEntity(spireId, p3dPoly);

	p3dPoly->close();

	//����һ��Բ��Ϊ�������
	AcGeVector3d vec (0,1,0); // Բ����ƽ�淨����
	AcGePoint3d ptCenter(30,0,0); //Բ��λ�úͰ뾶��С
	AcDbCircle* pCircle = new AcDbCircle(ptCenter, vec, 3);
	AcDbObjectId circleId;
	pRecord->appendAcDbEntity(circleId, pCircle);
	pCircle->close();

	//����Բ����һ������
	AcDbObjectIdArray boundaryIds, regionIds;
	boundaryIds.append(circleId);
	regionIds =CreateRegion(boundaryIds, pRecord);

	//��������������·��
	AcDbRegion* pRegion; 
	acdbOpenObject(pRegion, regionIds.at(0), AcDb::kForRead);
	AcDb3dPolyline* pPoly;
	acdbOpenObject(pPoly, spireId, AcDb::kForRead);

	//�����������
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
	AcDbObjectIdArray regionIds; // ���ɵ������id����
	AcDbVoidPtrArray curves; //ָ����Ϊ����߽�����ߵ�ָ�������
	AcDbVoidPtrArray regions; // ָ�򴴽�����������ָ�������
	AcDbEntity *pEnt; //��ʱָ�룬�����رձ߽�����
	AcDbRegion* pRegion; //��ʱ����������������ӵ�ģ�Ϳռ�

	//��curveIds��ʼ��curves
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
		//�����ɵ�������ӵ�ģ�Ϳռ�
		for (int i= 0; i<regions.length(); i++)
		{
			//����ָ��ת����ָ������Ķ����ָ��
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

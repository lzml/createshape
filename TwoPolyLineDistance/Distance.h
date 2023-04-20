#pragma once
#include "DisInfo.h"

/*
���ȷ�3��
1.ֱ�߸�ֱ��
2.ֱ�߸�����
3.���߸�����
---------------------------------
���߸����������
1.Բ���Ƿ��ཻ
2.Բ���Ƿ�



*/
class CDistance
{
public:
	CDistance(void);
	~CDistance(void);

private:
	AcDbObjectId m_polyLineID_1;
	AcDbObjectId m_polyLineID_2;

public:
	void  setTwoPolyline(AcDbObjectId id_1, AcDbObjectId id_2);
		


	void DistanceTwoPolyline(CDisInfo &outInfo);

	//�ж�һ�����ǲ������߶���
	//��Ҫ���� : �����������߶ε�ֱ���ϣ������߶���û�п��ǣ�
	bool IsPtInStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd );

	double DistancePointToPoint(AcGePoint2d pt1, AcGePoint2d pt2);

	//�㵽ֱ�߾���
	double DistancePointToLine(AcGePoint2d pt, AcGePoint2d linePt1, AcGePoint2d linePt2, AcGePoint2d& PerpendicularFootPt);
	//�㵽ֱ���߶εľ���
	double DistancePointToStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd, AcGePoint2d& crossPt);

	double DistanceStraightSegToStraightSeg(AcGePoint2d straightSegStart1, AcGePoint2d straightSegEnd1, AcGePoint2d& crossPt1,
		AcGePoint2d straightSegStart2, AcGePoint2d straightSegEnd2, AcGePoint2d& crossPt2);


	//������x��Ԫ�����нǽǶȣ� ��ʱ�뷽��
	double GetVectorAngle(AcGeVector2d v);

	int getLineLineCrossPoint(AcGePoint2d LineStart1, AcGePoint2d LineEnd1, AcGePoint2d LineStart2, AcGePoint2d LineEnd2, AcGePoint2d &crossPt);

	//������˳ʱ�뻹����ʱ��
	bool IsArcAnticlockwise(AcGeCircArc2d arc);

	//�ж�һ�����ǲ����ڽǶ��ڣ��Ƕȱ�������ʱ���start��end
	bool IsPtInAngle(AcGePoint2d pt, AcGePoint2d arcCenter, AcGePoint2d arcStart, AcGePoint2d arcEnd);

	double DistanceArcToStraightSeg(AcGeCircArc2d arc, AcGePoint2d& crossArc, 
		AcGePoint2d segStart, AcGePoint2d segEnd, AcGePoint2d& crossSeg);

	//���ֱ����Բ����û�н��㣬����з����������ͽ���,����ֵ�ǽ��������0,1,2
	int getCrossLineAndCircle(AcGePoint2d linePt1, AcGePoint2d linePt2, AcGeCircArc2d arc, AcGePoint2d &cross1, AcGePoint2d &cross2);
	int getCrossLineAndCircle(double k, double b, AcGeCircArc2d arc, AcGePoint2d& cross1, AcGePoint2d& cross2);

	//��Բ��
	//
	double DistanceArcToArc(AcGeCircArc2d arc1, AcGeCircArc2d arc2, AcGePoint2d& cross1, AcGePoint2d& cross2);


	//�㵽Բ������̾����£�Բ��������꣬�Ƿ񴩹���Բ��
	double DistancePointToArc(AcGePoint2d pt,  bool& bCross, AcGePoint2d & outMinDisPoint,
		AcGeCircArc2d arc, AcGePoint2d arcStart, AcGePoint2d arcEnd);


	//���ǲ�����Բ�ڲ�
	bool isPtInCircle(AcGePoint2d pt, AcGeCircArc2d arc);

};


#pragma once
#include "DisInfo.h"

/*
首先分3类
1.直线跟直线
2.直线跟弧线
3.弧线跟弧线
---------------------------------
弧线跟弧线情况下
1.圆弧是否相交
2.圆弧是否



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

	//判断一个点是不是在线段上
	//必要条件 : 这个点必须在线段的直线上，不在线段上没有考虑，
	bool IsPtInStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd );

	double DistancePointToPoint(AcGePoint2d pt1, AcGePoint2d pt2);

	//点到直线距离
	double DistancePointToLine(AcGePoint2d pt, AcGePoint2d linePt1, AcGePoint2d linePt2, AcGePoint2d& PerpendicularFootPt);
	//点到直线线段的距离
	double DistancePointToStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd, AcGePoint2d& crossPt);

	double DistanceStraightSegToStraightSeg(AcGePoint2d straightSegStart1, AcGePoint2d straightSegEnd1, AcGePoint2d& crossPt1,
		AcGePoint2d straightSegStart2, AcGePoint2d straightSegEnd2, AcGePoint2d& crossPt2);


	//向量与x轴元向量夹角角度， 逆时针方向
	double GetVectorAngle(AcGeVector2d v);

	int getLineLineCrossPoint(AcGePoint2d LineStart1, AcGePoint2d LineEnd1, AcGePoint2d LineStart2, AcGePoint2d LineEnd2, AcGePoint2d &crossPt);

	//弧线是顺时针还是逆时针
	bool IsArcAnticlockwise(AcGeCircArc2d arc);

	//判断一个点是不是在角度内，角度必须是逆时针从start到end
	bool IsPtInAngle(AcGePoint2d pt, AcGePoint2d arcCenter, AcGePoint2d arcStart, AcGePoint2d arcEnd);

	double DistanceArcToStraightSeg(AcGeCircArc2d arc, AcGePoint2d& crossArc, 
		AcGePoint2d segStart, AcGePoint2d segEnd, AcGePoint2d& crossSeg);

	//求解直线与圆的有没有交点，如果有返回相关坐标和交点,返回值是交点个数，0,1,2
	int getCrossLineAndCircle(AcGePoint2d linePt1, AcGePoint2d linePt2, AcGeCircArc2d arc, AcGePoint2d &cross1, AcGePoint2d &cross2);
	int getCrossLineAndCircle(double k, double b, AcGeCircArc2d arc, AcGePoint2d& cross1, AcGePoint2d& cross2);

	//将圆弧
	//
	double DistanceArcToArc(AcGeCircArc2d arc1, AcGeCircArc2d arc2, AcGePoint2d& cross1, AcGePoint2d& cross2);


	//点到圆弧的最短距离下，圆弧点的坐标，是否穿过了圆弧
	double DistancePointToArc(AcGePoint2d pt,  bool& bCross, AcGePoint2d & outMinDisPoint,
		AcGeCircArc2d arc, AcGePoint2d arcStart, AcGePoint2d arcEnd);


	//点是不是在圆内部
	bool isPtInCircle(AcGePoint2d pt, AcGeCircArc2d arc);

};


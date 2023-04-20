#include "stdafx.h"
#include "Distance.h"
#include <math.h>
#include <vector>
#include <algorithm>

CDistance::CDistance(void)
{
}


CDistance::~CDistance(void)
{
}

void  CDistance::setTwoPolyline(AcDbObjectId id_1, AcDbObjectId id_2)
{
	m_polyLineID_1 = id_1;
	m_polyLineID_2 = id_2;
}

void CDistance::DistanceTwoPolyline(CDisInfo& outInfo)
{
	AcDbEntity* pEntity_1 = NULL;
	acdbOpenObject(pEntity_1, m_polyLineID_1, AcDb::kForRead);

	AcDbEntity* pEntity_2 = NULL;
	acdbOpenObject(pEntity_2, m_polyLineID_2, AcDb::kForRead);

	

	if ((NULL != pEntity_1) && (NULL != pEntity_2))
	{
		AcDbPolyline* pLine1 = AcDbPolyline::cast(pEntity_1);
		AcDbPolyline* pLine2 = AcDbPolyline::cast(pEntity_2);

		if ((pLine1 != NULL) && (pLine2 != NULL))
		{
			/*
			enum SegType {
			kLine,
			kArc,
			kCoincident,
			kPoint,
			kEmpty
			};

			*/

			//遍历第一个多段线
			for (int i = 0; i < pLine1->numVerts() - 1; i++)
			{
				AcDbPolyline::SegType seg_line1_type = pLine1->segType(i);

				if (seg_line1_type == AcDbPolyline::SegType::kCoincident
					|| seg_line1_type == AcDbPolyline::SegType::kPoint
					|| seg_line1_type == AcDbPolyline::SegType::kEmpty)
				{
					continue;
				}

				//遍历第二个多段线
				for (int j = 0; j < pLine2->numVerts() - 1; j++)
				{
					CDisInfo tmpInfo;
					AcDbPolyline::SegType seg_line2_type = pLine2->segType(j);

					if (seg_line2_type == AcDbPolyline::SegType::kCoincident
						|| seg_line2_type == AcDbPolyline::SegType::kPoint
						|| seg_line2_type == AcDbPolyline::SegType::kEmpty)
					{
						continue;
					}


					//进行计算
					//两个段都是直线
					if (seg_line1_type == AcDbPolyline::SegType::kLine
						&& seg_line2_type == AcDbPolyline::SegType::kLine)
					{
						AcGeLineSeg2d seg1;
						pLine1->getLineSegAt(i, seg1);
						AcGeLineSeg2d seg2;
						pLine2->getLineSegAt(j, seg2);

						tmpInfo.m_distance = DistanceStraightSegToStraightSeg(seg1.startPoint(), seg1.endPoint(), tmpInfo.pt1,
							seg2.startPoint(), seg2.endPoint(), tmpInfo.pt2);
					}
					//两个段一个直线一个弧线
					if (seg_line1_type == AcDbPolyline::SegType::kArc
						&& seg_line2_type == AcDbPolyline::SegType::kLine)
					{
						AcGeCircArc2d arc;
						pLine1->getArcSegAt(i, arc);
						AcGeLineSeg2d seg2;
						pLine2->getLineSegAt(j, seg2);

						tmpInfo.m_distance = DistanceArcToStraightSeg(arc, tmpInfo.pt1,
							seg2.startPoint(), seg2.endPoint(), tmpInfo.pt2);
					}
					if (seg_line1_type == AcDbPolyline::SegType::kLine
						&& seg_line2_type == AcDbPolyline::SegType::kArc)
					{
						AcGeLineSeg2d seg1;
						pLine1->getLineSegAt(i, seg1);
						AcGeCircArc2d arc;
						pLine2->getArcSegAt(j, arc);
						tmpInfo.m_distance = DistanceArcToStraightSeg(arc, tmpInfo.pt1,
							seg1.startPoint(), seg1.endPoint(), tmpInfo.pt2);
					}
					//两个段都是弧线
					if (seg_line1_type == AcDbPolyline::SegType::kArc
						&& seg_line2_type == AcDbPolyline::SegType::kArc)
					{
						AcGeCircArc2d arc1;
						pLine1->getArcSegAt(i, arc1);
						AcGeCircArc2d arc2;
						pLine2->getArcSegAt(j, arc2);
						tmpInfo.m_distance = DistanceArcToArc(arc1, arc2, tmpInfo.pt1, tmpInfo.pt2);
					}

					if ((i==0 ) && (j == 0))
					{
						outInfo.m_distance = tmpInfo.m_distance;
						outInfo.pt1 = tmpInfo.pt1;
						outInfo.pt2 = tmpInfo.pt2;
					}
					else
					{
						if (tmpInfo.m_distance < outInfo.m_distance)
						{
							outInfo.m_distance = tmpInfo.m_distance;
							outInfo.pt1 = tmpInfo.pt1;
							outInfo.pt2 = tmpInfo.pt2;
						}
					}
					if (std::abs( outInfo.m_distance) < AcGeContext::gTol.equalPoint())
					{
						//距离为0
						break;
					}
				}
			}


		}

	}
	if (NULL != pEntity_1)
	{
		pEntity_1->close();
	}
	if (NULL != pEntity_2)
	{
		pEntity_2->close();
	}

}


bool CDistance::IsPtInStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd)
{
	//必要条件：  已经在直线上，所以只需要判断距离是不是在线段上
	double segLength = DistancePointToPoint(straightSegStart, straightSegEnd);
	double disToStart = DistancePointToPoint(pt, straightSegStart);
	double disToEnd = DistancePointToPoint(pt, straightSegEnd);
	if ((disToStart > segLength) || (disToEnd > segLength))
	{
		return false;
	}
	return true;
}

double CDistance::DistancePointToPoint(AcGePoint2d pt1, AcGePoint2d pt2)
{
	return std::sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2));
}


double CDistance::DistancePointToLine(AcGePoint2d pt, AcGePoint2d linePt1, AcGePoint2d linePt2, AcGePoint2d& PerpendicularFootPt)
{
	double gtol = AcGeContext::gTol.equalPoint();
	if (std::abs(linePt2.x - linePt1.x) < gtol) 
	{
		PerpendicularFootPt.x = linePt1.x;
		PerpendicularFootPt.y = pt.y;
		return std::abs(pt.x - linePt1.x);
	}
	if (std::abs(linePt2.y - linePt1.y) < gtol)
	{
		PerpendicularFootPt.x = pt.x;
		PerpendicularFootPt.y = linePt1.y;
		return std::abs(pt.y - linePt1.y);
	}



	double A = (linePt2.y - linePt1.y) / (linePt2.x - linePt1.x);
	double B = -1;
	double C = linePt1.y - A * linePt1.x;

	PerpendicularFootPt.x = (B * B * pt.x - A * B * pt.y - A * C) / (A * A + B * B);
	PerpendicularFootPt.y = (A * A * pt.y - A * B * pt.x - B * C) / (A * A + B * B);

	return std::abs(A * pt.x + B * pt.y + C) / (std::sqrt(A * A + B * B));
}


double CDistance::DistancePointToStraightSeg(AcGePoint2d pt, AcGePoint2d straightSegStart, AcGePoint2d straightSegEnd, AcGePoint2d& crossPt)
{
	//double A = (straightSegEnd.y - straightSegStart.y) / (straightSegEnd.x - straightSegStart.y);
	//double B = -1;
	//double C = straightSegStart.y - A * straightSegStart.x;


	//垂点坐标
	//AcGePoint2d footPt;

	//footPt.x = (B * B * pt.x - A * B * pt.y - A * C) / (A * A + B * B);
	//footPt.y = (A * A * pt.y - A * B * pt.x - B * C) / (A * A + B * B);

	//垂点坐标
	AcGePoint2d footPt;
	double ptToFootPt = DistancePointToLine(pt, straightSegStart, straightSegEnd, footPt);



	//计算垂点在不在线段上,已经肯定在直线上，所以算距离就好
	double segLength = DistancePointToPoint(straightSegStart, straightSegEnd);
	double disToStart = DistancePointToPoint(footPt, straightSegStart);
	double disToEnd = DistancePointToPoint(footPt, straightSegEnd);
	if ((disToStart < segLength) && (disToEnd < segLength))
	{
		//垂点在线段上
		crossPt = footPt;
		//return std::abs(A * pt.x + B * pt.y + C) / std::sqrt(A * A + B * B);
		return ptToFootPt;
	}

	crossPt = disToStart < disToEnd ? straightSegStart : straightSegEnd;
	return DistancePointToPoint(pt, crossPt);
}

double CDistance::DistanceStraightSegToStraightSeg(AcGePoint2d straightSegStart1, AcGePoint2d straightSegEnd1, AcGePoint2d& crossPt1,
	AcGePoint2d straightSegStart2, AcGePoint2d straightSegEnd2, AcGePoint2d& crossPt2)
{


	//double k1 = (straightSegEnd1.y - straightSegStart1.y) / (straightSegEnd1.x - straightSegStart1.x);
	//double k2 = (straightSegEnd2.y - straightSegStart2.y) / (straightSegEnd2.x - straightSegStart2.x);

	///****************************************/
	//if (k1 == k2)
	//{
	//	//平行，任取两点
	//	crossPt1 = straightSegStart1;
	//	double len1 = DistancePointToPoint(crossPt1, straightSegStart2);
	//	double len2 = DistancePointToPoint(crossPt1, straightSegEnd2);
	//	crossPt2 = len1 < len2 ? straightSegStart2 : straightSegEnd2;
	//	return len1 < len2 ? len1 : len2;
	//}

	////定义两条直线的交点
	//AcGePoint2d crossCrossPt;
	//crossCrossPt.x = (k1 * straightSegStart1.x - k2 * straightSegStart2.y - straightSegStart1.y + straightSegStart2.y)
	//	/ (k1 - k2);
	//crossCrossPt.y = k1 * (crossCrossPt.x - straightSegStart1.x) + straightSegStart1.y;

	//定义两条直线的交点
	AcGePoint2d crossCrossPt;
	int count = getLineLineCrossPoint(straightSegStart1, straightSegEnd1, straightSegStart2, straightSegEnd2, crossCrossPt);
	if (count != 1)
	{
		//平行没有交点，要看锤点过不过直线
		AcGePoint2d footTmp;
		DistancePointToLine(straightSegStart1, straightSegStart2, straightSegEnd2, footTmp);
		if (IsPtInStraightSeg(footTmp, straightSegStart2, straightSegEnd2))
		{
			crossPt1 = straightSegStart1;
			crossPt2 = footTmp;
			return DistancePointToPoint(crossPt1, crossPt2);
		}
		footTmp.x = 0;
		footTmp.y = 0;
		DistancePointToLine(straightSegEnd1, straightSegStart2, straightSegEnd2, footTmp);
		if (IsPtInStraightSeg(footTmp, straightSegStart2, straightSegEnd2))
		{
			crossPt1 = straightSegEnd1;
			crossPt2 = footTmp;
			return DistancePointToPoint(crossPt1, crossPt2);
		}
		footTmp.x = 0;
		footTmp.y = 0;
		DistancePointToLine(straightSegStart2, straightSegStart1, straightSegEnd1, footTmp);
		if (IsPtInStraightSeg(footTmp, straightSegStart1, straightSegEnd1))
		{
			crossPt1 = footTmp;
			crossPt2 = straightSegStart2;
			return DistancePointToPoint(crossPt1, crossPt2);
		}
		footTmp.x = 0;
		footTmp.y = 0;
		DistancePointToLine(straightSegEnd2, straightSegStart1, straightSegEnd1, footTmp);
		if (IsPtInStraightSeg(footTmp, straightSegStart1, straightSegEnd2))
		{
			crossPt1 = footTmp;
			crossPt2 = straightSegEnd2;
			return DistancePointToPoint(crossPt1, crossPt2);
		}
	}

	//交点同时在两条线段上
	if (IsPtInStraightSeg(crossCrossPt, straightSegStart1, straightSegEnd1)
		&& IsPtInStraightSeg(crossCrossPt, straightSegStart2, straightSegEnd2))
	{
		crossPt1 = crossCrossPt;
		crossPt2 = crossCrossPt;
		return 0;
	}

	//两个线段没有交点,就分别看一条线段的点与另一条线段的距离
	AcGePoint2d cross11, cross12, cross21, cross22;
	double dis11 = DistancePointToStraightSeg(straightSegStart1, straightSegStart2, straightSegEnd2, cross11);
	double dis12 = DistancePointToStraightSeg(straightSegEnd1, straightSegStart2, straightSegEnd2, cross12);
	double dis21 = DistancePointToStraightSeg(straightSegStart2, straightSegStart1, straightSegEnd1, cross21);
	double dis22 = DistancePointToStraightSeg(straightSegEnd2, straightSegStart1, straightSegEnd1, cross22);

	//返回4个距离里面最小的
	double rtn;
	if (dis11 < dis12)
	{
		rtn = dis11;
		crossPt1 = straightSegStart1;
		crossPt2 = cross11;
	}
	else
	{
		rtn = dis12;
		crossPt1 = straightSegEnd1;
		crossPt2 = cross12;
	}

	if (rtn > dis21)
	{
		rtn = dis21;
		crossPt1 = straightSegStart2;
		crossPt2 = cross21;
	}

	if (rtn > dis22)
	{
		rtn = dis22;
		crossPt1 = straightSegEnd2;
		crossPt2 = cross22;
	}
	return DistancePointToPoint(crossPt1, crossPt2);
}

double CDistance::GetVectorAngle(AcGeVector2d v)
{
	double angle = std::atan2(v.y, v.x);
	//在逆时针方向下， 从开始到结束向量的夹角 -π到π
	if (angle < 0)
	{
		angle = angle + 8 * std::atan(1.0);
	}
	//修改到0-2π
	return angle;
}

bool CDistance::IsArcAnticlockwise(AcGeCircArc2d arc)
{
	AcGePoint2d arcCenter = arc.center();
	double reallAngle = arc.endAng() - arc.startAng();


	AcGePoint2d arcStart = arc.startPoint();
	AcGePoint2d arcEnd = arc.endPoint();

	double gtol = AcGeContext::gTol.equalPoint();

	//逆时针下，从开始到结束的夹角角度，如果等于真实角度，那就是正向，如果不是，就是反向弧
	double arcStartAngle = GetVectorAngle(AcGeVector2d(arcStart.x - arcCenter.x, arcStart.y - arcCenter.y));
	double arcEndAngle = GetVectorAngle(AcGeVector2d(arcEnd.x - arcCenter.x, arcEnd.y - arcCenter.y));
	double startToEndAngle = arcEndAngle - arcStartAngle;
	if (startToEndAngle < 0)
	{
		//修正到0-2π
		startToEndAngle = startToEndAngle + 8 * atan(1.0);
	}


	if (std::abs(startToEndAngle - reallAngle) < gtol )
	{
		return true;  //弧线是逆时针的
	}

	return false;

}

bool CDistance::IsPtInAngle(AcGePoint2d pt, AcGePoint2d arcCenter, AcGePoint2d arcStart, AcGePoint2d arcEnd)
{

	double ptAngle = GetVectorAngle(AcGeVector2d(pt.x - arcCenter.x, pt.y - arcCenter.y));
	double arcStartAngle = GetVectorAngle(AcGeVector2d(arcStart.x - arcCenter.x, arcStart.y - arcCenter.y));
	double arcEndAngle = GetVectorAngle(AcGeVector2d(arcEnd.x - arcCenter.x, arcEnd.y - arcCenter.y));
	
	//保证角度符合逆时针规律，一定是start逆时针旋转到end的，所以如果开始角度大于终止角度,要区分对待
	double gtol = AcGeContext::gTol.equalPoint();

	if (arcStartAngle > arcEndAngle)
	{
		// ptAngle <2π ， ptAngle > 0都是肯定的
		if ( (std::abs(ptAngle - arcStartAngle) < gtol) || (std::abs(arcEndAngle - ptAngle) < gtol))
		{
			//基本相等情况
			return true;
		}
		if ( (ptAngle > arcStartAngle )  || ( arcEndAngle > ptAngle )  )
		{
			return true;
		}
		return false;
		
	}
	else 
	{
		//pt要夹在arc的角度之间
		if ((ptAngle < arcStartAngle) || (ptAngle > arcEndAngle))
		{
			return false;

		}

	}



	return true;
}


double CDistance::DistanceArcToStraightSeg(AcGeCircArc2d arc, AcGePoint2d& crossArc,
	AcGePoint2d segStart, AcGePoint2d segEnd, AcGePoint2d& crossSeg)
{
	AcGePoint2d arcStart = arc.startPoint();
	AcGePoint2d arcEnd = arc.endPoint();
	AcGePoint2d arcCenter = arc.center();

	//要从真实角度，推断起始角度和结束角度
	double rellArcAngle = arc.endAng() - arc.startAng(); //这里返回的是按照参考向量方向旋转的真实角度，也就是弧线夹角角度0-2π

	//分别计算圆心到4个端点在x轴上面的夹角
	double arcStartAngle = GetVectorAngle(arcStart - arcCenter);
	double arcEndAngle = GetVectorAngle(arcEnd - arcCenter);

	


	double gtol = AcGeContext::gTol.equalPoint();
	if ( !IsArcAnticlockwise(arc))
	{		
		//修正弧线的开始和结束点,改成逆时针开始和结束
		arcStart = arc.endPoint();
		arcEnd = arc.startPoint();
		 arcStartAngle = GetVectorAngle(arcStart - arcCenter);
		 arcEndAngle = GetVectorAngle(arcEnd - arcCenter);
	}
	double segStartAngle = GetVectorAngle(segStart - arcCenter);
	double segEndAngle = GetVectorAngle(segEnd - arcCenter);

	//线段所在直线是否与圆弧所在圆有交点
	AcGePoint2d lineCircleCrossPt1, lineCircleCrossPt2;
	int crossNum = getCrossLineAndCircle(segStart, segEnd, arc, lineCircleCrossPt1, lineCircleCrossPt2);


	//是否有一条直线连接圆心，线段任意一点，弧线任意一点，操作方法如下
	//圆心与圆弧端点连线，与线段是否有交点。 圆心与线段交点连线，是否与圆弧有交点. 
	bool existLine_arcStart = false;
	bool existLine_arcEnd = false;
	bool existLine_segStart = false;
	bool existLine_segEnd = false;
	AcGePoint2d tmp;
	std::vector<AcGePoint2d> ardialCrossToLineArray; //射线与线段的交点

	//圆心和弧线开始端点的射线，与线段交点
	if (getLineLineCrossPoint(arcCenter, arcStart, segStart, segEnd, tmp) != 0)
	{
		if (std::abs(GetVectorAngle(tmp - arcCenter) - GetVectorAngle(arcStart - arcCenter)) < gtol)
		{
			if (IsPtInStraightSeg(tmp, segStart, segEnd))
			{
				existLine_arcStart = true;
				ardialCrossToLineArray.push_back(tmp);
			}
		}
	}
	tmp.x = 0;
	tmp.y = 0;
	if (getLineLineCrossPoint(arcCenter, arcEnd, segStart, segEnd, tmp) != 0)
	{
		if (std::abs(GetVectorAngle(tmp - arcCenter) - GetVectorAngle(arcEnd - arcCenter)) < gtol)
		{
			if (IsPtInStraightSeg(tmp, segStart, segEnd))
			{
				existLine_arcEnd = true;
				ardialCrossToLineArray.push_back(tmp);
			}
		}
	}

	tmp.x = 0;
	tmp.y = 0;
	AcGePoint2d tmp2;

	if (getCrossLineAndCircle(arcCenter, segStart, arc, tmp2, tmp) > 0)
	{
		if (IsPtInAngle(tmp, arcCenter, arcStart, arcEnd))
		{
			if (std::abs( GetVectorAngle(tmp - arcCenter) - GetVectorAngle(segStart - arcCenter)) < gtol)
			{
				ardialCrossToLineArray.push_back(segStart);
				existLine_segStart = true;
			}

		}
		if (IsPtInAngle(tmp2, arcCenter, arcStart, arcEnd))
		{
			if (std::abs(GetVectorAngle(tmp2 - arcCenter) - GetVectorAngle(segStart - arcCenter))< gtol)
			{
				ardialCrossToLineArray.push_back(segStart);
				existLine_segStart = true;
			}
		}
	}
	tmp.x = 0;
	tmp.y = 0;
	tmp2.x = 0;
	tmp2.y = 0;
	if (getCrossLineAndCircle(arcCenter, segEnd, arc, tmp2, tmp) != -1)
	{
		if (IsPtInAngle(tmp, arcCenter, arcStart, arcEnd))
		{
			if (std::abs(GetVectorAngle(tmp - arcCenter) - GetVectorAngle(segEnd - arcCenter)) < gtol)
			{
				existLine_segEnd = true;
				ardialCrossToLineArray.push_back(segEnd);
			}


		}
		if (IsPtInAngle(tmp2, arcCenter, arcStart, arcEnd))
		{
			if (std::abs(GetVectorAngle(tmp2 - arcCenter) - GetVectorAngle(segEnd - arcCenter)) <gtol)
			{
				existLine_segEnd = true;
				ardialCrossToLineArray.push_back(segEnd);
			}
		}
	}






	if (existLine_arcStart == false
		&& existLine_arcEnd == false
		&& existLine_segStart == false
		&& existLine_segEnd == false)
	{
		//弧线扇形射线区域，与线段没有交点
		AcGePoint2d arcStartCross;
		double arcStartDis = DistancePointToStraightSeg(arcStart, segStart, segEnd, arcStartCross);
		AcGePoint2d arcEndCross;
		double arcEndDis = DistancePointToStraightSeg(arcEnd, segStart, segEnd, arcEndCross);
		if (arcStartDis < arcEndDis)
		{
			crossArc = arcStart;
			crossSeg = arcStartCross;
			return arcStartDis;
		}
		else
		{
			crossArc = arcEnd;
			crossSeg = arcEndCross;
			return arcEndDis;
		}
	}




	//线段与弧线交点情况
	if (crossNum == 1)
	{
		if (IsPtInStraightSeg(lineCircleCrossPt1, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt1, arcCenter, arcStart, arcEnd))
		{
			//有焦点,最短距离是0
			crossArc = lineCircleCrossPt1;
			crossSeg = lineCircleCrossPt1;
			return 0;
		}
	}
	else if (crossNum == 2)
	{
		//圆与直线相交
		if (IsPtInStraightSeg(lineCircleCrossPt1, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt1, arcCenter, arcStart, arcEnd))
		{
			//有交点,最短距离是0
			crossArc = lineCircleCrossPt1;
			crossSeg = lineCircleCrossPt1;
			return 0;
		}

		if (IsPtInStraightSeg(lineCircleCrossPt2, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt2, arcCenter, arcStart, arcEnd))
		{
			//有交点,最短距离是0
			crossArc = lineCircleCrossPt2;
			crossSeg = lineCircleCrossPt2;
			return 0;
		}

	}


	/*
	* 	bool existLine_arcStart = false;
	bool existLine_arcEnd = false;
	bool existLine_segStart = false;
	bool existLine_segEnd = false;
	这四个值中，肯定有两个为true
	*/
	if (ardialCrossToLineArray.size() != 2)
	{
		//出错了
		return -1;
	}
	double rtnMinDis = 0.0;

	//求圆心与线段两个点距离，判断是不是有一个在圆内部，或者两个都在圆内部
	double disCenterToSelectPt1 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(0));
	double disCenterToSelectPt2 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(1));
	if ( (disCenterToSelectPt1 < arc.radius()) && (disCenterToSelectPt2 < arc.radius()))
	{
		//在内部的，应该选较长的边界线条，这样半径减去它，就是弧线与直线最短距离
		//并且求这个端点与圆心连线与弧线的交点
		double rtnMaxDis = disCenterToSelectPt1 > disCenterToSelectPt2 ? disCenterToSelectPt1 : disCenterToSelectPt2;
		if (disCenterToSelectPt1 > disCenterToSelectPt2 )
		{
			crossSeg = ardialCrossToLineArray.at(0);
		}
		else
		{
			crossSeg = ardialCrossToLineArray.at(1);
		}

		//求这条直线与弧线交点
		AcGePoint2d tmp1, tmp2;
		int count = getCrossLineAndCircle(arcCenter, crossSeg, arc, tmp1, tmp2);
		if (count>= 1)
		{
			if (IsPtInAngle(tmp1, arcCenter, arcStart, arcEnd))
			{
				crossArc = tmp1;
			}
		}
		if(count == 2)
		{
			if (IsPtInAngle(tmp2, arcCenter, arcStart, arcEnd))
			{
				crossArc = tmp2;
			}
		}
		return arc.radius() - rtnMaxDis;	
	}

	//线段与扇形射线区域有交点，但是与线段没有交点，圆心到直线锤点和两个相交区域点这三个点中，肯定有一个最短

	AcGePoint2d  footCenterToLine;
	rtnMinDis = DistancePointToLine(arcCenter, segStart, segEnd, footCenterToLine);
	if (IsPtInAngle(footCenterToLine, arcCenter, arcStart, arcEnd)
		&& IsPtInStraightSeg(footCenterToLine, segStart, segEnd))
	{
		//锤点在扇形区域，也在线段上，肯定最短了
		crossSeg = footCenterToLine;
		//求这条直线与弧线交点
		AcGePoint2d tmp1, tmp2;
		int count = getCrossLineAndCircle(arcCenter, footCenterToLine, arc, tmp1, tmp2);
		if (count >= 1)
		{
			if (IsPtInAngle(tmp1, arcCenter, arcStart, arcEnd))
			{
				crossArc = tmp1;
				return rtnMinDis - arc.radius();
			}
		}
		if (count == 2)
		{
			if (IsPtInAngle(tmp2, arcCenter, arcStart, arcEnd))
			{
				crossArc = tmp2;
				return rtnMinDis - arc.radius();
			}
		}

	}
	//如果出现不在线段上，或者不经过arc，那么两条重合区域射线与arc和seg的交点，肯定有一条最短


	double tmpDis1 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(0));
	double tmpDis2 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(1));

	if (tmpDis1 < tmpDis2)
	{
		crossSeg = ardialCrossToLineArray.at(0);
	}
	else
	{
		crossSeg = ardialCrossToLineArray.at(1);
	}

	//求这个点与圆弧构成的这条直线，与圆弧交点
	AcGePoint2d arcCross1, arcCross2;
	int count = getCrossLineAndCircle(arcCenter, crossSeg, arc, arcCross1, arcCross2);
	if (count >= 1)
	{
		if (IsPtInAngle(arcCross1, arcCenter, arcStart, arcEnd))
		{
			crossArc = arcCross1;
		}
	}
	if (count == 2)
	{
		if (IsPtInAngle(arcCross2, arcCenter, arcStart, arcEnd))
		{
			crossArc = arcCross2;
		}
	}
	
	return DistancePointToPoint(crossArc, crossSeg);

}

int CDistance::getCrossLineAndCircle(AcGePoint2d linePt1, AcGePoint2d linePt2, AcGeCircArc2d arc, AcGePoint2d& cross1, AcGePoint2d& cross2)
{
	AcGePoint2d center = arc.center();
	double radius = arc.radius();
	double gtol = AcGeContext::gTol.equalPoint();
	if (std::abs(linePt2.x - linePt1.x) < gtol)
	{
		//直线到圆的距离
		double dis = std::abs(linePt1.x - arc.center().x);
		if (std::abs(dis - radius)< gtol  )
		{
			//有一个交点
			cross1.x  = linePt1.x;
			cross1.y = center.y;
			return 1;
		}
		else if (dis < arc.radius())
		{
			//有交点
			cross1.x = cross2.x = linePt1.x;
			double tmp = radius* radius - std::pow((cross1.x - center.x), 2);
			cross1.y = center.y + std::sqrt(tmp);
			cross2.y = center.y - std::sqrt(tmp);
			return 2;
		}
		else
		{
			//无交点
			return 0;
		}
	}
	if (std::abs(linePt2.y - linePt1.y) < gtol)
	{
		//直线到圆的距离
		double dis = std::abs(linePt1.y - arc.center().y);
		if (std::abs(dis - radius) < gtol)
		{
			cross1.x = center.x;
			cross1.y = linePt1.y;
			return 1;
		}
		else if (dis < arc.radius()) 
		{
			cross1.y = cross2.y = linePt1.y;
			double tmp = radius * radius - std::pow((cross1.y - center.y), 2);
			cross1.x = center.x + std::sqrt(tmp);
			cross2.x = center.x - std::sqrt(tmp);
			return 2;

		}
		else
		{
			return 0;
		}
	}


	double k = (linePt1.y - linePt2.y) / (linePt1.x - linePt2.x);
	double b = linePt1.y - k * linePt1.x;


	return getCrossLineAndCircle(k, b, arc, cross1, cross2);
}

int CDistance::getCrossLineAndCircle(double k, double b, AcGeCircArc2d arc, AcGePoint2d& cross1, AcGePoint2d& cross2)
{
	

	AcGePoint2d arcCenter = arc.center();

	//double k = (linePt1.y - linePt2.y) / (linePt1.x - linePt2.x);
	//double b = linePt1.y - k * linePt1.x;
	double r = arc.radius();
	double A = 1 + k * k;
	double B = 2 * (k * b - k * arcCenter.y - arcCenter.x);
	double C = arcCenter.x * arcCenter.x + (b - arcCenter.y) * (b - arcCenter.y) - r * r;


	double delta = B * B - 4 * A * C;

	double gtol = AcGeContext::gTol.equalPoint();

	if ( std::abs(delta) <	gtol )
	{
		//只有一个
		cross1.x = -B / (2 * A);
		cross1.y = k * cross1.x + b;
		return 1;
	}
	else if (delta < 0)
	{
		return 0;
	}

	cross1.x = (-B + std::sqrt(delta)) / (2 * A);
	cross2.x = (-B - std::sqrt(delta)) / (2 * A);
	cross1.y = k * cross1.x + b;
	cross2.y = k * cross2.x + b;


	return 2;
}



int CDistance::getLineLineCrossPoint(AcGePoint2d LineStart1, AcGePoint2d LineEnd1, AcGePoint2d LineStart2, AcGePoint2d LineEnd2, AcGePoint2d& crossPt)
{
	bool bLine1_k_invalid = false;
	bool bLine1_k_zero = false;
	bool bLine2_k_invalid = false;
	bool bLine2_k_zero = false;


	double gtol = AcGeContext::gTol.equalPoint();
	if (std::abs(LineEnd1.x - LineStart1.x) < gtol)
	{
		bLine1_k_invalid = true;
	}
	if (std::abs(LineEnd1.y - LineStart1.y) < gtol)
	{
		bLine1_k_zero = true;
	}
	if (std::abs(LineEnd2.x - LineStart2.x) < gtol)
	{
		bLine2_k_invalid = true;
	}
	if (std::abs(LineEnd2.y - LineStart2.y) < gtol)
	{
		bLine2_k_zero = true;
	}
	//两个斜率都不存在，平行没有交点
	if ( (bLine1_k_invalid  == true) && (bLine2_k_invalid == true))
	{
		return 0;
	}
	//line1平行y轴，line2平行于x轴
	else if ( (bLine1_k_invalid == true) && (bLine2_k_zero == true))
	{
		crossPt.x = LineStart1.x;
		crossPt.y = LineStart2.y;
		return 1;
	}
	//line1平行于x轴，line2平行于y轴
	else if ( (bLine1_k_zero == true) && (bLine2_k_invalid == true))
	{
		crossPt.x = LineStart2.x;
		crossPt.y = LineStart1.y;
		return 1;
	}
	//两个斜率都为0，平行没有交点
	else if ( (bLine1_k_zero == true) && (bLine2_k_zero == true))
	{
		return 0;
	}
	
	//line1斜率为0或者不存在
	if ((bLine2_k_invalid == false) && (bLine2_k_zero == false))
	{
		double k2 = (LineEnd2.y - LineStart2.y) / (LineEnd2.x - LineStart2.x);
		double b2 = LineStart2.y - k2 * LineStart2.x;
		if (bLine1_k_invalid == true) 
		{
			crossPt.x = LineStart1.x;
			crossPt.y = k2 * crossPt.x + b2;
			return 1;
		}
		if (bLine1_k_zero == true) 
		{
			crossPt.y = LineStart1.y;
			crossPt.x = (crossPt.y - b2) / k2;
			return 1;
		}
	}
	//line2斜率为0或者不存在
	if ((bLine1_k_invalid == false) && (bLine1_k_zero == false))
	{
		double k1 = (LineEnd1.y - LineStart1.y) / (LineEnd1.x - LineStart1.x);
		double b1 = LineStart1.y - k1 * LineStart1.x;
		if (bLine2_k_invalid == true)
		{
			crossPt.x = LineStart2.x;
			crossPt.y = k1 * crossPt.x + b1;
			return 1;
		}
		if (bLine2_k_zero == true)
		{
			crossPt.y = LineStart2.y;
			crossPt.x = (crossPt.y - b1) / k1;
			return 1;
		}
	}

	//斜率都不为0，都存在

	double k1 = (LineEnd1.y - LineStart1.y) / (LineEnd1.x - LineStart1.x);
	double k2 = (LineEnd2.y - LineStart2.y) / (LineEnd2.x - LineStart2.x);
	double b1 = LineStart1.y - k1 * LineStart1.x;
	double b2 = LineStart2.y - k2 * LineStart2.x;

	if (k1 == k2)
	{
		//平行，
		return 0;
	}

	//定义两条直线的交点

	crossPt.x = (b2 - b1) / (k1 - k2);
	crossPt.y = k1 * (crossPt.x - LineStart1.x) + LineStart1.y;

	return 1;
}

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

double CDistance::DistanceArcToArc(AcGeCircArc2d arc1, AcGeCircArc2d arc2, AcGePoint2d& cross1, AcGePoint2d& cross2)
{
	AcGePoint2d arcStart1 = arc1.startPoint();
	AcGePoint2d arcEnd1 = arc1.endPoint();
	AcGePoint2d arcCenter1 = arc1.center();
	//要从真实角度，推断起始角度和结束角度
	double rellArcAngle1 = arc1.endAng() - arc1.startAng(); //这里返回的是按照参考向量方向旋转的真实角度，也就是弧线夹角角度0-2π

	//分别计算圆心到4个端点在x轴上面的夹角
	double arcStartAngle1 = GetVectorAngle(arcStart1 - arcCenter1);
	double arcEndAngle1 = GetVectorAngle(arcEnd1 - arcCenter1);

	


	double gtol = AcGeContext::gTol.equalPoint();
	if (!IsArcAnticlockwise(arc1))
	{
		//修正弧线的开始和结束点,改成逆时针开始和结束
		arcStart1 = arc1.endPoint();
		arcEnd1 = arc1.startPoint();
		arcStartAngle1 = GetVectorAngle(arcStart1 - arcCenter1);
		arcEndAngle1 = GetVectorAngle(arcEnd1 - arcCenter1);
	}
	//--------------------------------
	AcGePoint2d arcStart2 = arc2.startPoint();
	AcGePoint2d arcEnd2 = arc2.endPoint();
	AcGePoint2d arcCenter2 = arc2.center();
	//要从真实角度，推断起始角度和结束角度
	double rellArcAngle2 = arc2.endAng() - arc2.startAng(); //这里返回的是按照参考向量方向旋转的真实角度，也就是弧线夹角角度0-2π

	//分别计算圆心到4个端点在x轴上面的夹角
	double arcStartAngle2 = GetVectorAngle(arcStart2 - arcCenter2);
	double arcEndAngle2 = GetVectorAngle(arcEnd2 - arcCenter2);

	//修正弧线的开始和结束点,改成逆时针开始和结束

	if (!IsArcAnticlockwise(arc2))
	{
		//修正弧线的开始和结束点,改成逆时针开始和结束
		arcStart2 = arc2.endPoint();
		arcEnd2 = arc2.startPoint();
		arcStartAngle2 = GetVectorAngle(arcStart2 - arcCenter2);
		arcEndAngle2 = GetVectorAngle(arcEnd2 - arcCenter2);
	}
	//--------------------------------
	
	AcGePoint2d crossTmp1, crossTmp2;

	//相交性判断，相交了距离肯定为0，首先获取圆心直线方程，再计算中垂线，最后中垂线与圆相交求交点，最后看交点在不在另一个圆弧上
	AcGePoint2d centerMiddlePoint; //中锤点坐标
	centerMiddlePoint.x = (arcCenter1.x + arcCenter2.x) / 2;
	centerMiddlePoint.y = (arcCenter1.y + arcCenter2.y) / 2;

	double kc = 0.0;
	double bc = 0.0;

	double km = 0.0; //中垂线斜率
	double bm = 0.0;//中垂线常数b

	//找到中垂线上除了中锤点点以外,任取另一点坐标
	AcGePoint2d centerMiddlePoint2;
	//首先检查斜率
	if (std::abs(arcCenter2.x - arcCenter1.x) < gtol)
	{
		//圆心连线斜率不存在，中垂线斜率为0
		centerMiddlePoint2.y = centerMiddlePoint.y;
		centerMiddlePoint2.x = centerMiddlePoint.x +  10.0;

	}
	if (std::abs(arcCenter2.y - arcCenter1.y) < gtol)
	{
		//圆心连线斜率为0, 中垂线斜率不存在
		centerMiddlePoint2.x = centerMiddlePoint.x;
		centerMiddlePoint2.y = centerMiddlePoint.y + 10.0;
	}
	else
	{
		kc = (arcCenter2.y - arcCenter1.y) / (arcCenter2.x - arcCenter1.x);
		bc = arcCenter1.y - kc * arcCenter1.x;

		km = -1 / kc; //中垂线斜率
		bm = centerMiddlePoint.y - km * centerMiddlePoint.x; //中垂线常数b

		centerMiddlePoint2.x = centerMiddlePoint.x + 10;
		centerMiddlePoint2.y = km * centerMiddlePoint2.x + bm;
	}
	
	int count = 0;
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	count = getCrossLineAndCircle(centerMiddlePoint, centerMiddlePoint2, arc1, crossTmp1, crossTmp2);
	if (count >0)
	{
		if(IsPtInAngle(crossTmp1, arcCenter1, arcStart1, arcEnd1)
			&& IsPtInAngle(crossTmp1, arcCenter2, arcStart2, arcEnd2))
		{
			cross1 = crossTmp1;
			cross2 = crossTmp1;
			return 0;
		}
	}
	if (count == 2)
	{
		if (IsPtInAngle(crossTmp2, arcCenter1, arcStart1, arcEnd1) 
			&& IsPtInAngle(crossTmp2, arcCenter2, arcStart2, arcEnd2) )
		{
			cross1 = crossTmp2;
			cross2 = crossTmp2;
			return 0;
		}
	}
	/////////////////////下面就都是没有交点的情况了

	bool bCenterLineCrossArc1 = false;
	bool bCenterLineCrossArc2 = false;
	AcGePoint2d centerLineCrossArcPoint1, centerLineCrossArcPoint2; //圆心直线与弧线交点，每个月都可能同时有两个有效，需要查看哪几个更近
	//圆心直线过两个圆弧，最短肯定是两个交点了
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	count = getCrossLineAndCircle(arcCenter1, arcCenter2, arc1, crossTmp1, crossTmp2);

	//count必定等于2了，过圆心了    
	//检查交点是不是在两个圆心线段上，排除内含圆情况，
	if (IsPtInAngle(crossTmp1, arcCenter1, arcStart1, arcEnd1) && (IsPtInStraightSeg(crossTmp1 ,arcCenter1, arcCenter2)))
	{
		bCenterLineCrossArc1 = true;
		centerLineCrossArcPoint1 = (crossTmp1);
	}
	if (IsPtInAngle(crossTmp2, arcCenter1, arcStart1, arcEnd1) && (IsPtInStraightSeg(crossTmp2, arcCenter1, arcCenter2)))
	{
		bCenterLineCrossArc1 = true;
		centerLineCrossArcPoint1 =(crossTmp2);
	}

	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	count = getCrossLineAndCircle(arcCenter1, arcCenter2, arc2, crossTmp1, crossTmp2);
	if (IsPtInAngle(crossTmp1, arcCenter2, arcStart2, arcEnd2) && (IsPtInStraightSeg(crossTmp1, arcCenter1, arcCenter2)))
	{
		bCenterLineCrossArc2 = true;
		centerLineCrossArcPoint2 = (crossTmp1);
	}
	if (IsPtInAngle(crossTmp2, arcCenter2, arcStart2, arcEnd2) && (IsPtInStraightSeg(crossTmp2, arcCenter1, arcCenter2)))
	{
		bCenterLineCrossArc2 = true;
		centerLineCrossArcPoint2 = (crossTmp2);
	}

	if (bCenterLineCrossArc1 && bCenterLineCrossArc2)
	{
		//圆心线段与两个弧线都有交点 ，朝内和朝外
		double dis_tmp = DistancePointToPoint(cross1, cross2);
		if (dis_tmp < (arc1.radius() + arc2.radius()))
		{
			cross1 = centerLineCrossArcPoint1;
			cross2 = centerLineCrossArcPoint2;
			return dis_tmp;
		}

		//求两个圆弧端点的直线距离
		crossTmp1.x = 0;
		crossTmp1.y = 0;
		crossTmp2.x = 0;
		crossTmp2.y = 0;
		dis_tmp = DistancePointToPoint(arcStart1, arcStart2);
		crossTmp1 = arcStart1;
		crossTmp2 = arcStart2;

		if (dis_tmp > DistancePointToPoint(arcStart1, arcEnd2))
		{
			dis_tmp = DistancePointToPoint(arcStart1, arcEnd2);
			crossTmp1 = arcStart1;
			crossTmp2 = arcEnd2;
		}
		if (dis_tmp > DistancePointToPoint(arcEnd1, arcStart2))
		{
			dis_tmp = DistancePointToPoint(arcEnd1, arcStart2);
			crossTmp1 = arcEnd1;
			crossTmp2 = arcStart2;
		}
		if (dis_tmp > DistancePointToPoint(arcEnd1, arcEnd2))
		{
			dis_tmp = DistancePointToPoint(arcEnd1, arcEnd2);
			crossTmp1 = arcEnd1;
			crossTmp2 = arcEnd2;
		}

		cross1 = crossTmp1;
		cross2 = crossTmp2;

		return dis_tmp;

	}
	//处理一个内含圆特例
	double center1ToCenter2 = DistancePointToPoint(arcCenter1, arcCenter2);
	if (center1ToCenter2 < std::abs(arc1.radius() - arc2.radius()) )
	{		
		bool tmp1Valid = false;
		bool tmp2Valid = false;
		crossTmp1.x = 0;
		crossTmp1.y = 0;
		crossTmp2.x = 0;
		crossTmp2.y = 0;
		count = getCrossLineAndCircle(arcCenter1, arcCenter2, arc1, crossTmp1, crossTmp2);
		

		bool tmp3Valid = false;
		bool tmp4Valid = false;
		AcGePoint2d crossTmp3, crossTmp4;
		crossTmp3.x = 0;
		crossTmp3.y = 0;
		crossTmp4.x = 0;
		crossTmp4.y = 0;
		count = getCrossLineAndCircle(arcCenter1, arcCenter2, arc2, crossTmp3, crossTmp4);

		//可能有效4个点，但是只有两个点最近,而且这两个点必须在弧线上
		AcGePoint2d ptout1, ptout2;
		double disTmp = DistancePointToPoint(crossTmp1, crossTmp3);
		ptout1 = crossTmp1;
		ptout2 = crossTmp3;
		if (disTmp > DistancePointToPoint(crossTmp1, crossTmp4))
		{
			disTmp = DistancePointToPoint(crossTmp1, crossTmp4);
			ptout1 = crossTmp1;
			ptout2 = crossTmp4;
		}
		if (disTmp > DistancePointToPoint(crossTmp2, crossTmp3))
		{
			disTmp = DistancePointToPoint(crossTmp2, crossTmp3);
			ptout1 = crossTmp2;
			ptout2 = crossTmp3;
		}
		if (disTmp > DistancePointToPoint(crossTmp2, crossTmp4))
		{
			disTmp = DistancePointToPoint(crossTmp2, crossTmp4);
			ptout1 = crossTmp2;
			ptout2 = crossTmp4;
		}
		if (IsPtInAngle(ptout1, arcCenter1, arcStart1, arcEnd1) && IsPtInAngle(ptout2, arcCenter2, arcStart2, arcEnd2))
		{
			cross1 = ptout1;
			cross2 = ptout2;
			return DistancePointToPoint(cross1, cross2);
		}
	}



	//圆心线段过了一个弧的情况需要 和 弧线端点与另一个弧圆心连线经过了这个弧一起考虑,
	//先看端点和圆心连线
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	bool bCross_ArcStart1 = false;				//arc1的start端点，与arc2圆心连线是不是与arc2相交
	AcGePoint2d pt_ArcStart1;					//这个交点的坐标
	double dis_arcStart1;						//arc1的start端点，到arc2的最短距离

	bool bCross_ArcEnd1 = false;
	AcGePoint2d pt_ArcEnd1;
	double dis_arcEnd1;

	bool bCross_ArcStart2 = false;
	AcGePoint2d pt_ArcStart2;
	double dis_arcStart2;

	bool bCross_ArcEnd2 = false;
	AcGePoint2d pt_ArcEnd2;
	double dis_arcEnd2;

	double dis_tmp = -1.0; // 存储符合条件的最短距离

	dis_arcStart1 = DistancePointToArc(arcStart1, bCross_ArcStart1, pt_ArcStart1, arc2, arcStart2, arcEnd2);
	dis_arcEnd1 = DistancePointToArc(arcEnd1,	  bCross_ArcEnd1,   pt_ArcEnd1,   arc2, arcStart2, arcEnd2);
	dis_arcStart2 = DistancePointToArc(arcStart2, bCross_ArcStart2, pt_ArcStart2, arc1, arcStart1, arcEnd1);
	dis_arcEnd2 = DistancePointToArc(arcEnd2,   bCross_ArcEnd2,   pt_ArcEnd2, arc1, arcStart1, arcEnd1);

	AcGePoint2d maybeMindisCross1, maybeMindisCross2; 
	bool bMaybeMindisValid = false;

	//找到一个以上的，经过端点，弧线，圆心的情况
	if (bCross_ArcStart1 || bCross_ArcEnd1 || bCross_ArcStart2 || bCross_ArcEnd2)
	{
		//寻找 bCross 为true  里面最小的
		if (bCross_ArcStart1)
		{
			dis_tmp = dis_arcStart1;
			crossTmp1 = pt_ArcStart1;
			crossTmp2 = arcStart1;
		}
		if ( (bCross_ArcEnd1) )
		{
			if (dis_tmp < 0)
			{
				dis_tmp = dis_arcEnd1;
				crossTmp1 = pt_ArcEnd1;
				crossTmp2 = arcEnd1;
			}
			else
			{
				if (dis_tmp > dis_arcEnd1)
				{
					dis_tmp = dis_arcEnd1;
					crossTmp1 = pt_ArcEnd1;
					crossTmp2 = arcEnd1;
				}
			}

		}
		if ( bCross_ArcStart2  )
		{
			if (dis_tmp < 0)
			{
				dis_tmp = dis_arcStart2;
				crossTmp1 = pt_ArcStart2;
				crossTmp2 = arcStart2;
			}
			else
			{
				if (dis_tmp > dis_arcStart2)
				{
					dis_tmp = dis_arcStart2;
					crossTmp1 = pt_ArcStart2;
					crossTmp2 = arcStart2;
				}
			}

		}
		if (bCross_ArcEnd2  )
		{
			if (dis_tmp < 0)
			{
				dis_tmp = dis_arcEnd2;
				crossTmp1 = pt_ArcEnd2;
				crossTmp2 = arcEnd2;
			}
			else
			{
				if ((dis_tmp > dis_arcEnd2))
				{
					dis_tmp = dis_arcEnd2;
					crossTmp1 = pt_ArcEnd2;
					crossTmp2 = arcEnd2;
				}
			}

		}
		//找到最短距离坐标，计算距离返回

		maybeMindisCross1 = crossTmp2;
		maybeMindisCross2 = crossTmp1;	
		bMaybeMindisValid = true;
	}

	//求两个圆弧端点的直线距离
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;

	if (bMaybeMindisValid)
	{
		dis_tmp = DistancePointToPoint(maybeMindisCross1, maybeMindisCross2);
		crossTmp2 =  maybeMindisCross1;
		crossTmp1 = maybeMindisCross2;
		if (dis_tmp > DistancePointToPoint(arcStart1, arcStart2))
		{
			dis_tmp = DistancePointToPoint(arcStart1, arcStart2);
			crossTmp1 = arcStart1;
			crossTmp2 = arcStart2;
		}
	}
	else
	{
		dis_tmp = DistancePointToPoint(arcStart1, arcStart2);
		crossTmp1 = arcStart1;
		crossTmp2 = arcStart2;
	}
	if (dis_tmp > DistancePointToPoint(arcStart1, arcEnd2))
	{
		dis_tmp = DistancePointToPoint(arcStart1, arcEnd2);
		crossTmp1 = arcStart1;
		crossTmp2 = arcEnd2;
	}
	if (dis_tmp > DistancePointToPoint(arcEnd1, arcStart2))
	{
		dis_tmp = DistancePointToPoint(arcEnd1, arcStart2);
		crossTmp1 = arcEnd1;
		crossTmp2 = arcStart2;
	}
	if (dis_tmp > DistancePointToPoint(arcEnd1, arcEnd2))
	{
		dis_tmp = DistancePointToPoint(arcEnd1, arcEnd2);
		crossTmp1 = arcEnd1;
		crossTmp2 = arcEnd2;
	}


	cross1 = crossTmp1;
	cross2 = crossTmp2;

	return dis_tmp;
}


double CDistance::DistancePointToArc(AcGePoint2d pt, bool& bCross, AcGePoint2d& outMinDisPoint,
	AcGeCircArc2d arc, AcGePoint2d arcStart, AcGePoint2d arcEnd)
{
	bCross = false; //初始化
	AcGePoint2d center = arc.center();



	AcGePoint2d crossTmp1 ;
	AcGePoint2d crossTmp2;
	int  count = getCrossLineAndCircle(pt, arc.center(), arc, crossTmp1, crossTmp2);  //圆2的圆心与圆1端点连接，看连线是否过圆2的圆弧 
	//过圆心，必定2个点   
	bool valid1 = false;
	bool valid2 = false;
	if (IsPtInAngle(crossTmp1, center, arcStart, arcEnd))
	{
		valid1 = true;
	}
	if (IsPtInAngle(crossTmp2, center, arcStart, arcEnd) )
	{
		valid2 = true;
	}

	
	
	//同向并且与圆弧相交的,可能有两个，是个大弧线，取较短的那个
	double outDis = 0.0;
	if (valid1)
	{
		bCross = true;
		outDis = DistancePointToPoint(pt, crossTmp1);
		outMinDisPoint = crossTmp1;
		//return outDis;
	}
	if (valid2)
	{
		if (valid1)
		{
			if (outDis > DistancePointToPoint(pt, crossTmp2))
			{
				bCross = true;
				outDis = DistancePointToPoint(pt, crossTmp2);;
				outMinDisPoint = crossTmp2;
			}
		}
		else
		{
			bCross = true;
			outDis = DistancePointToPoint(pt, crossTmp2);;
			outMinDisPoint = crossTmp2;
		}

		//return outDis;
	}
	if (valid1 || valid2)
	{
		return outDis;
	}
	
	 //其他情况就是端点到点的距离了
	
	double tmpDis1 = DistancePointToPoint(pt, arcStart);
	double tmpDis2 = DistancePointToPoint(pt, arcEnd);
	if (tmpDis1 < tmpDis2)
	{
		outMinDisPoint = arcStart;
		return tmpDis1;
	}
	outMinDisPoint = arcEnd;
	return tmpDis2;
}

bool CDistance:: isPtInCircle(AcGePoint2d pt, AcGeCircArc2d arc)
{
	double r = arc.radius();
	AcGePoint2d center = arc.center();
	double tmp = (pt.x - center.x) * (pt.x - center.x) + (pt.y - center.y) * (pt.y - center.y);
	if (tmp < r*r)
	{
		return true;
	}

	return false;
}
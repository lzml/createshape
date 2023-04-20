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

			//������һ�������
			for (int i = 0; i < pLine1->numVerts() - 1; i++)
			{
				AcDbPolyline::SegType seg_line1_type = pLine1->segType(i);

				if (seg_line1_type == AcDbPolyline::SegType::kCoincident
					|| seg_line1_type == AcDbPolyline::SegType::kPoint
					|| seg_line1_type == AcDbPolyline::SegType::kEmpty)
				{
					continue;
				}

				//�����ڶ��������
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


					//���м���
					//�����ζ���ֱ��
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
					//������һ��ֱ��һ������
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
					//�����ζ��ǻ���
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
						//����Ϊ0
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
	//��Ҫ������  �Ѿ���ֱ���ϣ�����ֻ��Ҫ�жϾ����ǲ������߶���
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


	//��������
	//AcGePoint2d footPt;

	//footPt.x = (B * B * pt.x - A * B * pt.y - A * C) / (A * A + B * B);
	//footPt.y = (A * A * pt.y - A * B * pt.x - B * C) / (A * A + B * B);

	//��������
	AcGePoint2d footPt;
	double ptToFootPt = DistancePointToLine(pt, straightSegStart, straightSegEnd, footPt);



	//���㴹���ڲ����߶���,�Ѿ��϶���ֱ���ϣ����������ͺ�
	double segLength = DistancePointToPoint(straightSegStart, straightSegEnd);
	double disToStart = DistancePointToPoint(footPt, straightSegStart);
	double disToEnd = DistancePointToPoint(footPt, straightSegEnd);
	if ((disToStart < segLength) && (disToEnd < segLength))
	{
		//�������߶���
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
	//	//ƽ�У���ȡ����
	//	crossPt1 = straightSegStart1;
	//	double len1 = DistancePointToPoint(crossPt1, straightSegStart2);
	//	double len2 = DistancePointToPoint(crossPt1, straightSegEnd2);
	//	crossPt2 = len1 < len2 ? straightSegStart2 : straightSegEnd2;
	//	return len1 < len2 ? len1 : len2;
	//}

	////��������ֱ�ߵĽ���
	//AcGePoint2d crossCrossPt;
	//crossCrossPt.x = (k1 * straightSegStart1.x - k2 * straightSegStart2.y - straightSegStart1.y + straightSegStart2.y)
	//	/ (k1 - k2);
	//crossCrossPt.y = k1 * (crossCrossPt.x - straightSegStart1.x) + straightSegStart1.y;

	//��������ֱ�ߵĽ���
	AcGePoint2d crossCrossPt;
	int count = getLineLineCrossPoint(straightSegStart1, straightSegEnd1, straightSegStart2, straightSegEnd2, crossCrossPt);
	if (count != 1)
	{
		//ƽ��û�н��㣬Ҫ�����������ֱ��
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

	//����ͬʱ�������߶���
	if (IsPtInStraightSeg(crossCrossPt, straightSegStart1, straightSegEnd1)
		&& IsPtInStraightSeg(crossCrossPt, straightSegStart2, straightSegEnd2))
	{
		crossPt1 = crossCrossPt;
		crossPt2 = crossCrossPt;
		return 0;
	}

	//�����߶�û�н���,�ͷֱ�һ���߶εĵ�����һ���߶εľ���
	AcGePoint2d cross11, cross12, cross21, cross22;
	double dis11 = DistancePointToStraightSeg(straightSegStart1, straightSegStart2, straightSegEnd2, cross11);
	double dis12 = DistancePointToStraightSeg(straightSegEnd1, straightSegStart2, straightSegEnd2, cross12);
	double dis21 = DistancePointToStraightSeg(straightSegStart2, straightSegStart1, straightSegEnd1, cross21);
	double dis22 = DistancePointToStraightSeg(straightSegEnd2, straightSegStart1, straightSegEnd1, cross22);

	//����4������������С��
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
	//����ʱ�뷽���£� �ӿ�ʼ�����������ļн� -�е���
	if (angle < 0)
	{
		angle = angle + 8 * std::atan(1.0);
	}
	//�޸ĵ�0-2��
	return angle;
}

bool CDistance::IsArcAnticlockwise(AcGeCircArc2d arc)
{
	AcGePoint2d arcCenter = arc.center();
	double reallAngle = arc.endAng() - arc.startAng();


	AcGePoint2d arcStart = arc.startPoint();
	AcGePoint2d arcEnd = arc.endPoint();

	double gtol = AcGeContext::gTol.equalPoint();

	//��ʱ���£��ӿ�ʼ�������ļнǽǶȣ����������ʵ�Ƕȣ��Ǿ�������������ǣ����Ƿ���
	double arcStartAngle = GetVectorAngle(AcGeVector2d(arcStart.x - arcCenter.x, arcStart.y - arcCenter.y));
	double arcEndAngle = GetVectorAngle(AcGeVector2d(arcEnd.x - arcCenter.x, arcEnd.y - arcCenter.y));
	double startToEndAngle = arcEndAngle - arcStartAngle;
	if (startToEndAngle < 0)
	{
		//������0-2��
		startToEndAngle = startToEndAngle + 8 * atan(1.0);
	}


	if (std::abs(startToEndAngle - reallAngle) < gtol )
	{
		return true;  //��������ʱ���
	}

	return false;

}

bool CDistance::IsPtInAngle(AcGePoint2d pt, AcGePoint2d arcCenter, AcGePoint2d arcStart, AcGePoint2d arcEnd)
{

	double ptAngle = GetVectorAngle(AcGeVector2d(pt.x - arcCenter.x, pt.y - arcCenter.y));
	double arcStartAngle = GetVectorAngle(AcGeVector2d(arcStart.x - arcCenter.x, arcStart.y - arcCenter.y));
	double arcEndAngle = GetVectorAngle(AcGeVector2d(arcEnd.x - arcCenter.x, arcEnd.y - arcCenter.y));
	
	//��֤�Ƕȷ�����ʱ����ɣ�һ����start��ʱ����ת��end�ģ����������ʼ�Ƕȴ�����ֹ�Ƕ�,Ҫ���ֶԴ�
	double gtol = AcGeContext::gTol.equalPoint();

	if (arcStartAngle > arcEndAngle)
	{
		// ptAngle <2�� �� ptAngle > 0���ǿ϶���
		if ( (std::abs(ptAngle - arcStartAngle) < gtol) || (std::abs(arcEndAngle - ptAngle) < gtol))
		{
			//����������
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
		//ptҪ����arc�ĽǶ�֮��
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

	//Ҫ����ʵ�Ƕȣ��ƶ���ʼ�ǶȺͽ����Ƕ�
	double rellArcAngle = arc.endAng() - arc.startAng(); //���ﷵ�ص��ǰ��ղο�����������ת����ʵ�Ƕȣ�Ҳ���ǻ��߼нǽǶ�0-2��

	//�ֱ����Բ�ĵ�4���˵���x������ļн�
	double arcStartAngle = GetVectorAngle(arcStart - arcCenter);
	double arcEndAngle = GetVectorAngle(arcEnd - arcCenter);

	


	double gtol = AcGeContext::gTol.equalPoint();
	if ( !IsArcAnticlockwise(arc))
	{		
		//�������ߵĿ�ʼ�ͽ�����,�ĳ���ʱ�뿪ʼ�ͽ���
		arcStart = arc.endPoint();
		arcEnd = arc.startPoint();
		 arcStartAngle = GetVectorAngle(arcStart - arcCenter);
		 arcEndAngle = GetVectorAngle(arcEnd - arcCenter);
	}
	double segStartAngle = GetVectorAngle(segStart - arcCenter);
	double segEndAngle = GetVectorAngle(segEnd - arcCenter);

	//�߶�����ֱ���Ƿ���Բ������Բ�н���
	AcGePoint2d lineCircleCrossPt1, lineCircleCrossPt2;
	int crossNum = getCrossLineAndCircle(segStart, segEnd, arc, lineCircleCrossPt1, lineCircleCrossPt2);


	//�Ƿ���һ��ֱ������Բ�ģ��߶�����һ�㣬��������һ�㣬������������
	//Բ����Բ���˵����ߣ����߶��Ƿ��н��㡣 Բ�����߶ν������ߣ��Ƿ���Բ���н���. 
	bool existLine_arcStart = false;
	bool existLine_arcEnd = false;
	bool existLine_segStart = false;
	bool existLine_segEnd = false;
	AcGePoint2d tmp;
	std::vector<AcGePoint2d> ardialCrossToLineArray; //�������߶εĽ���

	//Բ�ĺͻ��߿�ʼ�˵�����ߣ����߶ν���
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
		//�������������������߶�û�н���
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




	//�߶��뻡�߽������
	if (crossNum == 1)
	{
		if (IsPtInStraightSeg(lineCircleCrossPt1, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt1, arcCenter, arcStart, arcEnd))
		{
			//�н���,��̾�����0
			crossArc = lineCircleCrossPt1;
			crossSeg = lineCircleCrossPt1;
			return 0;
		}
	}
	else if (crossNum == 2)
	{
		//Բ��ֱ���ཻ
		if (IsPtInStraightSeg(lineCircleCrossPt1, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt1, arcCenter, arcStart, arcEnd))
		{
			//�н���,��̾�����0
			crossArc = lineCircleCrossPt1;
			crossSeg = lineCircleCrossPt1;
			return 0;
		}

		if (IsPtInStraightSeg(lineCircleCrossPt2, segStart, segEnd)
			&& IsPtInAngle(lineCircleCrossPt2, arcCenter, arcStart, arcEnd))
		{
			//�н���,��̾�����0
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
	���ĸ�ֵ�У��϶�������Ϊtrue
	*/
	if (ardialCrossToLineArray.size() != 2)
	{
		//������
		return -1;
	}
	double rtnMinDis = 0.0;

	//��Բ�����߶���������룬�ж��ǲ�����һ����Բ�ڲ���������������Բ�ڲ�
	double disCenterToSelectPt1 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(0));
	double disCenterToSelectPt2 = DistancePointToPoint(arcCenter, ardialCrossToLineArray.at(1));
	if ( (disCenterToSelectPt1 < arc.radius()) && (disCenterToSelectPt2 < arc.radius()))
	{
		//���ڲ��ģ�Ӧ��ѡ�ϳ��ı߽������������뾶��ȥ�������ǻ�����ֱ����̾���
		//����������˵���Բ�������뻡�ߵĽ���
		double rtnMaxDis = disCenterToSelectPt1 > disCenterToSelectPt2 ? disCenterToSelectPt1 : disCenterToSelectPt2;
		if (disCenterToSelectPt1 > disCenterToSelectPt2 )
		{
			crossSeg = ardialCrossToLineArray.at(0);
		}
		else
		{
			crossSeg = ardialCrossToLineArray.at(1);
		}

		//������ֱ���뻡�߽���
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

	//�߶����������������н��㣬�������߶�û�н��㣬Բ�ĵ�ֱ�ߴ���������ཻ��������������У��϶���һ�����

	AcGePoint2d  footCenterToLine;
	rtnMinDis = DistancePointToLine(arcCenter, segStart, segEnd, footCenterToLine);
	if (IsPtInAngle(footCenterToLine, arcCenter, arcStart, arcEnd)
		&& IsPtInStraightSeg(footCenterToLine, segStart, segEnd))
	{
		//��������������Ҳ���߶��ϣ��϶������
		crossSeg = footCenterToLine;
		//������ֱ���뻡�߽���
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
	//������ֲ����߶��ϣ����߲�����arc����ô�����غ�����������arc��seg�Ľ��㣬�϶���һ�����


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

	//���������Բ�����ɵ�����ֱ�ߣ���Բ������
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
		//ֱ�ߵ�Բ�ľ���
		double dis = std::abs(linePt1.x - arc.center().x);
		if (std::abs(dis - radius)< gtol  )
		{
			//��һ������
			cross1.x  = linePt1.x;
			cross1.y = center.y;
			return 1;
		}
		else if (dis < arc.radius())
		{
			//�н���
			cross1.x = cross2.x = linePt1.x;
			double tmp = radius* radius - std::pow((cross1.x - center.x), 2);
			cross1.y = center.y + std::sqrt(tmp);
			cross2.y = center.y - std::sqrt(tmp);
			return 2;
		}
		else
		{
			//�޽���
			return 0;
		}
	}
	if (std::abs(linePt2.y - linePt1.y) < gtol)
	{
		//ֱ�ߵ�Բ�ľ���
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
		//ֻ��һ��
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
	//����б�ʶ������ڣ�ƽ��û�н���
	if ( (bLine1_k_invalid  == true) && (bLine2_k_invalid == true))
	{
		return 0;
	}
	//line1ƽ��y�ᣬline2ƽ����x��
	else if ( (bLine1_k_invalid == true) && (bLine2_k_zero == true))
	{
		crossPt.x = LineStart1.x;
		crossPt.y = LineStart2.y;
		return 1;
	}
	//line1ƽ����x�ᣬline2ƽ����y��
	else if ( (bLine1_k_zero == true) && (bLine2_k_invalid == true))
	{
		crossPt.x = LineStart2.x;
		crossPt.y = LineStart1.y;
		return 1;
	}
	//����б�ʶ�Ϊ0��ƽ��û�н���
	else if ( (bLine1_k_zero == true) && (bLine2_k_zero == true))
	{
		return 0;
	}
	
	//line1б��Ϊ0���߲�����
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
	//line2б��Ϊ0���߲�����
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

	//б�ʶ���Ϊ0��������

	double k1 = (LineEnd1.y - LineStart1.y) / (LineEnd1.x - LineStart1.x);
	double k2 = (LineEnd2.y - LineStart2.y) / (LineEnd2.x - LineStart2.x);
	double b1 = LineStart1.y - k1 * LineStart1.x;
	double b2 = LineStart2.y - k2 * LineStart2.x;

	if (k1 == k2)
	{
		//ƽ�У�
		return 0;
	}

	//��������ֱ�ߵĽ���

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
	//Ҫ����ʵ�Ƕȣ��ƶ���ʼ�ǶȺͽ����Ƕ�
	double rellArcAngle1 = arc1.endAng() - arc1.startAng(); //���ﷵ�ص��ǰ��ղο�����������ת����ʵ�Ƕȣ�Ҳ���ǻ��߼нǽǶ�0-2��

	//�ֱ����Բ�ĵ�4���˵���x������ļн�
	double arcStartAngle1 = GetVectorAngle(arcStart1 - arcCenter1);
	double arcEndAngle1 = GetVectorAngle(arcEnd1 - arcCenter1);

	


	double gtol = AcGeContext::gTol.equalPoint();
	if (!IsArcAnticlockwise(arc1))
	{
		//�������ߵĿ�ʼ�ͽ�����,�ĳ���ʱ�뿪ʼ�ͽ���
		arcStart1 = arc1.endPoint();
		arcEnd1 = arc1.startPoint();
		arcStartAngle1 = GetVectorAngle(arcStart1 - arcCenter1);
		arcEndAngle1 = GetVectorAngle(arcEnd1 - arcCenter1);
	}
	//--------------------------------
	AcGePoint2d arcStart2 = arc2.startPoint();
	AcGePoint2d arcEnd2 = arc2.endPoint();
	AcGePoint2d arcCenter2 = arc2.center();
	//Ҫ����ʵ�Ƕȣ��ƶ���ʼ�ǶȺͽ����Ƕ�
	double rellArcAngle2 = arc2.endAng() - arc2.startAng(); //���ﷵ�ص��ǰ��ղο�����������ת����ʵ�Ƕȣ�Ҳ���ǻ��߼нǽǶ�0-2��

	//�ֱ����Բ�ĵ�4���˵���x������ļн�
	double arcStartAngle2 = GetVectorAngle(arcStart2 - arcCenter2);
	double arcEndAngle2 = GetVectorAngle(arcEnd2 - arcCenter2);

	//�������ߵĿ�ʼ�ͽ�����,�ĳ���ʱ�뿪ʼ�ͽ���

	if (!IsArcAnticlockwise(arc2))
	{
		//�������ߵĿ�ʼ�ͽ�����,�ĳ���ʱ�뿪ʼ�ͽ���
		arcStart2 = arc2.endPoint();
		arcEnd2 = arc2.startPoint();
		arcStartAngle2 = GetVectorAngle(arcStart2 - arcCenter2);
		arcEndAngle2 = GetVectorAngle(arcEnd2 - arcCenter2);
	}
	//--------------------------------
	
	AcGePoint2d crossTmp1, crossTmp2;

	//�ཻ���жϣ��ཻ�˾���϶�Ϊ0�����Ȼ�ȡԲ��ֱ�߷��̣��ټ����д��ߣ�����д�����Բ�ཻ�󽻵㣬��󿴽����ڲ�����һ��Բ����
	AcGePoint2d centerMiddlePoint; //�д�������
	centerMiddlePoint.x = (arcCenter1.x + arcCenter2.x) / 2;
	centerMiddlePoint.y = (arcCenter1.y + arcCenter2.y) / 2;

	double kc = 0.0;
	double bc = 0.0;

	double km = 0.0; //�д���б��
	double bm = 0.0;//�д��߳���b

	//�ҵ��д����ϳ����д��������,��ȡ��һ������
	AcGePoint2d centerMiddlePoint2;
	//���ȼ��б��
	if (std::abs(arcCenter2.x - arcCenter1.x) < gtol)
	{
		//Բ������б�ʲ����ڣ��д���б��Ϊ0
		centerMiddlePoint2.y = centerMiddlePoint.y;
		centerMiddlePoint2.x = centerMiddlePoint.x +  10.0;

	}
	if (std::abs(arcCenter2.y - arcCenter1.y) < gtol)
	{
		//Բ������б��Ϊ0, �д���б�ʲ�����
		centerMiddlePoint2.x = centerMiddlePoint.x;
		centerMiddlePoint2.y = centerMiddlePoint.y + 10.0;
	}
	else
	{
		kc = (arcCenter2.y - arcCenter1.y) / (arcCenter2.x - arcCenter1.x);
		bc = arcCenter1.y - kc * arcCenter1.x;

		km = -1 / kc; //�д���б��
		bm = centerMiddlePoint.y - km * centerMiddlePoint.x; //�д��߳���b

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
	/////////////////////����Ͷ���û�н���������

	bool bCenterLineCrossArc1 = false;
	bool bCenterLineCrossArc2 = false;
	AcGePoint2d centerLineCrossArcPoint1, centerLineCrossArcPoint2; //Բ��ֱ���뻡�߽��㣬ÿ���¶�����ͬʱ��������Ч����Ҫ�鿴�ļ�������
	//Բ��ֱ�߹�����Բ������̿϶�������������
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	count = getCrossLineAndCircle(arcCenter1, arcCenter2, arc1, crossTmp1, crossTmp2);

	//count�ض�����2�ˣ���Բ����    
	//��齻���ǲ���������Բ���߶��ϣ��ų��ں�Բ�����
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
		//Բ���߶����������߶��н��� �����ںͳ���
		double dis_tmp = DistancePointToPoint(cross1, cross2);
		if (dis_tmp < (arc1.radius() + arc2.radius()))
		{
			cross1 = centerLineCrossArcPoint1;
			cross2 = centerLineCrossArcPoint2;
			return dis_tmp;
		}

		//������Բ���˵��ֱ�߾���
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
	//����һ���ں�Բ����
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

		//������Ч4���㣬����ֻ�����������,����������������ڻ�����
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



	//Բ���߶ι���һ�����������Ҫ �� ���߶˵�����һ����Բ�����߾����������һ����,
	//�ȿ��˵��Բ������
	crossTmp1.x = 0;
	crossTmp1.y = 0;
	crossTmp2.x = 0;
	crossTmp2.y = 0;
	bool bCross_ArcStart1 = false;				//arc1��start�˵㣬��arc2Բ�������ǲ�����arc2�ཻ
	AcGePoint2d pt_ArcStart1;					//������������
	double dis_arcStart1;						//arc1��start�˵㣬��arc2����̾���

	bool bCross_ArcEnd1 = false;
	AcGePoint2d pt_ArcEnd1;
	double dis_arcEnd1;

	bool bCross_ArcStart2 = false;
	AcGePoint2d pt_ArcStart2;
	double dis_arcStart2;

	bool bCross_ArcEnd2 = false;
	AcGePoint2d pt_ArcEnd2;
	double dis_arcEnd2;

	double dis_tmp = -1.0; // �洢������������̾���

	dis_arcStart1 = DistancePointToArc(arcStart1, bCross_ArcStart1, pt_ArcStart1, arc2, arcStart2, arcEnd2);
	dis_arcEnd1 = DistancePointToArc(arcEnd1,	  bCross_ArcEnd1,   pt_ArcEnd1,   arc2, arcStart2, arcEnd2);
	dis_arcStart2 = DistancePointToArc(arcStart2, bCross_ArcStart2, pt_ArcStart2, arc1, arcStart1, arcEnd1);
	dis_arcEnd2 = DistancePointToArc(arcEnd2,   bCross_ArcEnd2,   pt_ArcEnd2, arc1, arcStart1, arcEnd1);

	AcGePoint2d maybeMindisCross1, maybeMindisCross2; 
	bool bMaybeMindisValid = false;

	//�ҵ�һ�����ϵģ������˵㣬���ߣ�Բ�ĵ����
	if (bCross_ArcStart1 || bCross_ArcEnd1 || bCross_ArcStart2 || bCross_ArcEnd2)
	{
		//Ѱ�� bCross Ϊtrue  ������С��
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
		//�ҵ���̾������꣬������뷵��

		maybeMindisCross1 = crossTmp2;
		maybeMindisCross2 = crossTmp1;	
		bMaybeMindisValid = true;
	}

	//������Բ���˵��ֱ�߾���
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
	bCross = false; //��ʼ��
	AcGePoint2d center = arc.center();



	AcGePoint2d crossTmp1 ;
	AcGePoint2d crossTmp2;
	int  count = getCrossLineAndCircle(pt, arc.center(), arc, crossTmp1, crossTmp2);  //Բ2��Բ����Բ1�˵����ӣ��������Ƿ��Բ2��Բ�� 
	//��Բ�ģ��ض�2����   
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

	
	
	//ͬ������Բ���ཻ��,�������������Ǹ����ߣ�ȡ�϶̵��Ǹ�
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
	
	 //����������Ƕ˵㵽��ľ�����
	
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
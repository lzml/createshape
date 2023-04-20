#pragma once

// 距离和相关的两个点

class CDisInfo
{
public:
	CDisInfo();
	CDisInfo(double dis, AcGePoint2d p1, AcGePoint2d p2);
	~CDisInfo(void);

public: 
	double m_distance;
	AcGePoint2d pt1;
	AcGePoint2d pt2;

};


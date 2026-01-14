// GeoMath.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"
#include "framework.h"

// TODO: 라이브러리 함수의 예제입니다.
void fnGeoMathCore()
{
}

#include "stdafx.h"
#include "GeoMath.h"
//#include <gtest/gtest.h>


static LineEq GetLineEquation(PointF pt1, PointF pt2)
{
	LineEq result_Eq;
	double x1 = (double)pt1.x;
	double y1 = (double)pt1.y;
	double x2 = (double)pt2.x;
	double y2 = (double)pt2.y;

	double dx = x2 - x1;
	double dy = y2 - y1;

	if (std::abs(dx) < EPSILON) {
		result_Eq.isVertical = true;
		result_Eq.verticalX = x1;
		result_Eq.m = 0;
		result_Eq.c = y1;
	}
	else {
		result_Eq.isVertical = false;
		result_Eq.m = dy / dx;
		result_Eq.c = y1 - (result_Eq.m * x1);
	}
}

inline void GetPerpendicularLine(const LineEq *baseLine, PointF pt, LineEq *result_Eq)
{
	double x = (double)pt.x;
	double y = (double)pt.y;

	if (baseLine->isVertical) {
		// Case 1: 기준선이 '수직선(Vertical)'인 경우
		// -> 수선은 '수평선(Horizontal)'이 되어야 함 (기울기 0, Y = y)
		result_Eq->isVertical = false;
		result_Eq->m = 0.0;
		result_Eq->c = y;
	}
	else if (std::abs(baseLine->m) < EPSILON) {
		// Case 2: 기준선이 '수평선'인 경우 (기울기 m이 거의 0)
		// -> 수선은 '수직선(Vertical)'이 되어야 함 (X = x)
		result_Eq->isVertical = true;
		result_Eq->verticalX = x;
		result_Eq->m = 0.0; // 의미 없음
		result_Eq->c = 0.0; // 의미 없음
	}
	else {
		// Case 3: 일반적인 경우 (기울기가 존재)
		// -> 수선의 기울기 = -1 / 기준선 기울기
		result_Eq->isVertical = false;
		result_Eq->m = -1.0 / baseLine->m;

		// Y = mX + c  =>  c = Y - mX
		result_Eq->c = y - (result_Eq->m * x);
	}
}

inline void GetPerpendicularFoot(PointF pt1, PointF pt2, PointF pt3, PointF *result_FootPt)
{
	double x1 = (double)pt1.x;
	double y1 = (double)pt1.y;
	double x2 = (double)pt2.x;
	double y2 = (double)pt2.y;
	double x3 = (double)pt3.x;
	double y3 = (double)pt3.y;

	double dx = x2 - x1;
	double dy = y2 - y1;

	// Case 1: pt1-pt2가 수직선인 경우 (dx = 0)
	// 수선은 수평선이 되며, 교점의 X는 x1, Y는 y3
	if (abs(dx) < EPSILON) {
		result_FootPt->SetPoint((int)x1, (int)y3);
		return;
	}

	// Case 2: pt1-pt2가 수평선인 경우 (dy = 0)
	// -> 수선은 수직선이 되며, 교점의 X는 x3, Y는 y1
	if (abs(dy) < EPSILON) {
		result_FootPt->SetPoint((int)x3, (int)y1);
		return;
	}

	// Case 3: 일반적인 경우 (기울기가 존재하는 경우)
	// -> 직선의 방정식 1: Y - y1 = m(X - x1)		-> P1, P2를 지나는 선
	// -> 직선의 방정식 2: Y - y3 = -1/m(X - x3)	-> P3를 지나고 수직인 선

	double m = dy / dx;

	// 두 직선의 교점 (X, Y) 공식 유도 결과:
	// -> X = (m*y3 + x3 - m*y1 + m^2*x1) / (m^2 + 1)
	// -> Y = m * (X - x1) + y1

	result_FootPt->x = (LONG)round((m * y3 + x3 - m * y1 + m * m * x1) / (m * m + 1.0));
	result_FootPt->y = (LONG)round(m * (result_FootPt->x - x1) + y1);
}

// 수직선 방정식 테스트
TEST(GeoMathTest, VerticalLineCheck) {
    PointF p1 = { 10.0, 5.0 };
    PointF p2 = { 10.0, 100.0 }; // X가 같으므로 수직선

    LineEq result = GeoMath::GetLineEquation(p1, p2);

    EXPECT_TRUE(result.isVertical);
    EXPECT_DOUBLE_EQ(result.verticalX, 10.0);
}

// 회전 변환 테스트
TEST(GeoMathTest, RotationCheck) {
    // (10, 0)을 원점(0,0) 기준으로 90도 회전하면 (0, 10)이 되어야 함
    std::vector<PointF> tri = { {10.0, 0.0} };
    PointF origin = { 0.0, 0.0 };

    auto result = GeoMath::RotateTriangle(tri, origin, 90.0);

    EXPECT_NEAR(result[0].x, 0.0, 1e-9);
    EXPECT_NEAR(result[0].y, 10.0, 1e-9);
}

// GeoMathCore.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

//#include "stdafx.h" // 정적 라이브러리 프로젝트의 경우 필요할 수 있습니다. (VS 2017) 
#include "pch.h" // 정적 라이브러리 프로젝트의 경우 필요할 수 있습니다. (VS 2022)
//#include "framework.h"
#include "GeoMath.h"

// TODO: 라이브러리 함수의 예제입니다.
void fnGeoMathCore()
{
}


LineEq GeoMath::GetLineEquation(PointF pt1, PointF pt2)
{
	LineEq result_Eq;
	float x1 = pt1.x;
	float y1 = pt1.y;
	float x2 = pt2.x;
	float y2 = pt2.y;

	float dx = x2 - x1;
	float dy = y2 - y1;

	if (std::abs(dx) < EPSILON) {
		result_Eq.isVertical = true;
		result_Eq.verticalX = x1;
		result_Eq.m = 0.0f;
		result_Eq.c = y1;
	} else {
		result_Eq.isVertical = false;
		result_Eq.m = dy / dx;
		result_Eq.c = y1 - (result_Eq.m * x1);
	}

	return result_Eq;
}

LineEq GeoMath::GetPerpendicularLine(const LineEq *baseLine, PointF pt)
{
	LineEq result_Eq;
	float x = pt.x;
	float y = pt.y;

	if (baseLine->isVertical) {
		// Case 1: 기준선이 '수직선(Vertical)'인 경우
		// -> 수선은 '수평선(Horizontal)'이 되어야 함 (기울기 0, Y = y)
		result_Eq.isVertical = false;
		result_Eq.m = 0.0f;
		result_Eq.c = y;
	} else if (std::abs(baseLine->m) < EPSILON) {
		// Case 2: 기준선이 '수평선'인 경우 (기울기 m이 거의 0)
		// -> 수선은 '수직선(Vertical)'이 되어야 함 (X = x)
		result_Eq.isVertical = true;
		result_Eq.verticalX = x;
		result_Eq.m = 0.0f; // 의미 없음
		result_Eq.c = 0.0f; // 의미 없음
	} else {
		// Case 3: 일반적인 경우 (기울기가 존재)
		// -> 수선의 기울기 = -1 / 기준선 기울기
		result_Eq.isVertical = false;
		result_Eq.m = -1.0f / baseLine->m;

		// Y = mX + c  =>  c = Y - mX
		result_Eq.c = y - (result_Eq.m * x);
	}

	return result_Eq;
}

PointF GeoMath::GetPerpendicularFoot(PointF pt1, PointF pt2, PointF pt3)
{
	PointF result_FootPt;
	float x1 = pt1.x;
	float y1 = pt1.y;
	float x2 = pt2.x;
	float y2 = pt2.y;
	float x3 = pt3.x;
	float y3 = pt3.y;

	float dx = x2 - x1;
	float dy = y2 - y1;

	// Case 1: pt1-pt2가 수직선인 경우 (dx = 0)
	// 수선은 수평선이 되며, 교점의 X는 x1, Y는 y3
	if (abs(dx) < EPSILON) {
		result_FootPt.x = x1;
		result_FootPt.y = y3;
		return result_FootPt;
	}

	// Case 2: pt1-pt2가 수평선인 경우 (dy = 0)
	// -> 수선은 수직선이 되며, 교점의 X는 x3, Y는 y1
	if (abs(dy) < EPSILON) {
		result_FootPt.x = x3;
		result_FootPt.y = y1;
		return result_FootPt;
	}

	// Case 3: 일반적인 경우 (기울기가 존재하는 경우)
	// -> 직선의 방정식 1: Y - y1 = m(X - x1)		-> P1, P2를 지나는 선
	// -> 직선의 방정식 2: Y - y3 = -1/m(X - x3)	-> P3를 지나고 수직인 선

	float m = dy / dx;

	// 두 직선의 교점 (X, Y) 공식 유도 결과:
	// -> X = (m*y3 + x3 - m*y1 + m^2*x1) / (m^2 + 1)
	// -> Y = m * (X - x1) + y1

	result_FootPt.x = (m * y3 + x3 - m * y1 + m * m * x1) / (m * m + 1.0f);
	result_FootPt.y = m * (result_FootPt.x - x1) + y1;
	return result_FootPt;
}

std::vector<PointF> GeoMath::RotateTriangle(const std::vector<PointF> &points, PointF origin, float angleDegree)
{
	std::vector<PointF> rotatedPoints;
	rotatedPoints.reserve(points.size());

	float angleRad = DegreesToRadians(angleDegree);
	float cosA = std::cos(angleRad);
	float sinA = std::sin(angleRad);
	for (const auto& pt : points) {
		// 원점 기준으로 이동
		float translatedX = pt.x - origin.x;
		float translatedY = pt.y - origin.y;
		// 회전 변환
		float rotatedX = translatedX * cosA - translatedY * sinA;
		float rotatedY = translatedX * sinA + translatedY * cosA;
		// 다시 원래 위치로 이동
		PointF rotatedPt;
		rotatedPt.x = rotatedX + origin.x;
		rotatedPt.y = rotatedY + origin.y;
		rotatedPoints.push_back(rotatedPt);
	}
	return rotatedPoints;
}

float GeoMath::DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}

float GeoMath::RadiansToDegrees(float radians)
{
	return radians * (180.0f / PI);
}

float GeoMath::CalculateDistance(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

float GeoMath::CalculateAngle(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return RadiansToDegrees(std::atan2(dy, dx));
}

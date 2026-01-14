#ifndef _GEO_MATH_H_
#define _GEO_MATH_H_

#include <cmath>
#include <vector>

#define EPSILON 1e-9
#define PI      3.14159265358979323846f

// MFC CPoint 대신 순수 구조체 사용
struct PointF {
    float x;
    float y;
};

struct LineEq {
    float m;
    float c;
    bool isVertical;
    float verticalX;
};

class GeoMath {



public:
    static LineEq GetLineEquation(PointF p1, PointF p2);
    static LineEq GetPerpendicularLine(const LineEq *baseLine, PointF p);
    static PointF GetPerpendicularFoot(PointF p1, PointF p2, PointF p3);

    // 상태(멤버변수)를 건드리는 대신, 입력값을 받고 결과값을 리턴하도록 변경 (Pure Function)
    static std::vector<PointF> RotateTriangle(const std::vector<PointF> &points, PointF origin, float angleDegree);

	static float DegreesToRadians(float degrees);
	static float RadiansToDegrees(float radians);
	static float CalculateDistance(float x1, float y1, float x2, float y2);
	static float CalculateAngle(float x1, float y1, float x2, float y2);
};

#endif
#include "pch.h"

// Usage. Visual Studio '프로젝트 속성' 설정을 통한 전처리기 변경
// 1. C/C++ -> 일반 -> 추가 포함 디렉터리: $(SolutionDir)GeoMathCore
// 2. `#include "../GeoMathCore/GeoMath.h"` 를 `#include "GeoMath.h"`로 변경 가능
#include "GeoMath.h"

// 1. 직선의 방정식 테스트 (수직선 케이스)
TEST(GeoMathTest, VerticalLineCheck) {
    // 테스트 데이터 준비
    PointF p1 = { 10.0, 5.0 };
    PointF p2 = { 10.0, 100.0 }; // X 좌표가 같음 -> 수직선

    // 로직 수행
    LineEq result = GeoMath::GetLineEquation(p1, p2);

    // 검증 (Expectation)
    EXPECT_TRUE(result.isVertical);       // 수직선 플래그가 true여야 함
    EXPECT_FLOAT_EQ(result.verticalX, 10.0); // 수직선의 X좌표는 10.0이어야 함
}

// 2. 일반 직선 테스트 (기울기가 있는 경우)
TEST(GeoMathTest, NormalLineCheck) {
    PointF p1 = { 0.0, 0.0 };
    PointF p2 = { 10.0, 10.0 }; // 기울기 1

    LineEq result = GeoMath::GetLineEquation(p1, p2);

    EXPECT_FALSE(result.isVertical);
    EXPECT_FLOAT_EQ(result.m, 1.0); // 기울기 1
    EXPECT_FLOAT_EQ(result.c, 0.0); // y절편 0
}

// 3. 회전 변환 테스트
TEST(GeoMathTest, RotationCheck) {
    // (10, 0)을 원점(0,0) 기준으로 90도 회전하면 (0, 10)이 되어야 함
    std::vector<PointF> tri = { {10.0, 0.0} };
    PointF origin = { 0.0, 0.0 };

    auto result = GeoMath::RotateTriangle(tri, origin, 90.0);

    EXPECT_NEAR(result[0].x, 0.0, 1e-9);
    EXPECT_NEAR(result[0].y, 10.0, 1e-9);
}

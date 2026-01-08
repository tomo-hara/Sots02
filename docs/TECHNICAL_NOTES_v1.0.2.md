## 3. GDI 그리기 정밀도 보정 (Endpoint Exclusion Issue)

### 🔴 문제 상황 (Problem)
`MoveToEx`와 `LineTo`를 사용하여 십자선(+)이나 도형을 그릴 때, **지정한 좌표보다 1픽셀 짧게 그려지는 현상**이 발생함.
특히 `Rectangle` 함수로 배경을 지울 때와 달리, 선(Line)을 그릴 때 시각적으로 끝부분이 누락되어 보임.

### 🟡 원인 (Root Cause)
Windows GDI(Graphics Device Interface)의 대부분의 도형 함수는 **"Right-Bottom Exclusive (우하단 배제)"** 정책을 따릅니다.

* **수학적 정의:** 그리기 구간이 $(Start, End)$ 입니다.
* **동작:** 시작점(Start)은 포함하지만, 끝점(End)은 **포함하지 않습니다.**
* **예시:** `LineTo(x, y)` 호출 시, GDI는 `(x, y)` 바로 전 픽셀까지만 색을 칠합니다.

### 💡 심화 분석: DC 영역 초과(Boundary Overflow)에 대한 검증
보정을 위해 좌표에 `+1`을 더할 때, **"메모리 DC의 할당된 영역을 벗어나서(Overflow) 프로그램이 뻗거나 메모리가 오염되지 않을까?"** 라는 우려가 있었습니다.
- `Rectangle`
1.  **시나리오:** 버퍼 너비가 100(0~99)일 때, 끝까지 선을 긋기 위해 `Rectangle(0, sy, 100, ey)`을 호출한다면?
2.  **GDI 내부 동작 (Clipping):**
    * `Rectangle(0, sy, 100, ey)`은 100번째 픽셀까지 그립니다. (테두리 포함, 안전)
    * 만약 `Rectangle(0, sy, 101, ey)`을 호출하여 101번째(범위 밖) 픽셀을 그리려 시도한다면?
    * **결론:** GDI는 내부적으로 **Clipping Region(클리핑 영역)** 을 가지고 있어, DC 크기를 벗어나는 좌표에 대한 그리기 요청은 **무시(Discard)** 합니다.
3.  **검증 결과:** 따라서 시각적 완전성을 위해 `+1` 보정을 하더라도, 시스템 안정성에는 문제가 없음을 확인했습니다. 더불어 사각형은 보정하지 않은 값이 정확합니다. (버퍼값에 해당하는 좌표를 사용하는 경우에만 보정, 주로 연속된 사각형을 그리는 경우 `-1` 보정)


### 🟢 해결 방법 (Solution)
`LineTo` 등의 선형 그리기 함수 사용 시, 우측(Right)과 하단(Bottom) 좌표에 **1픽셀 보정값**을 적용하는 매크로 상수를 도입했습니다.

> **참고:** `Rectangle` 함수의 경우 테두리(Pen)가 내부로 그려지거나, 배경 채우기 용도(`ClearCanvas`)로 사용 시 영역을 정확히 채우므로 보정값을 적용하지 않습니다.

#### 💻 적용 코드 (Code Snippet)

```cpp
// Sots02Dlg.h
// GDI Endpoint Exclusive 특성 보정을 위한 상수 (+1)
#define GDI_ENDPOINT_ADJUST 1 

// Sots02Dlg.cpp
void CSots02Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // ... 좌표 변환 ...

    // 가로선 그리기 (우측 끝점 보정)
    ::MoveToEx(mh_drawDC, point.x - 3, point.y, NULL);
    ::LineTo(mh_drawDC, point.x + 3 + GDI_ENDPOINT_ADJUST, point.y);

    // 세로선 그리기 (하단 끝점 보정)
    ::MoveToEx(mh_drawDC, point.x, point.y - 3, NULL);
    ::LineTo(mh_drawDC, point.x, point.y + 3 + GDI_ENDPOINT_ADJUST);
    
    // ...
}
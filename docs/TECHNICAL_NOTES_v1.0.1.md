# 🛠️ 기술 구현 및 트러블 슈팅 로그

본 문서는 MFC Dialog 기반 프로젝트 개발 과정에서 발생한 주요 렌더링 이슈(화면 지워짐, 좌표 오차)의 원인과 해결 방법 기술합니다.

## 1. Memory DC 도입을 통한 렌더링 지속성 확보 (Double Buffering)

### 🔴 문제 상황 (Problem)
기존 `CPaintDC`를 이용해 화면에 직접 그리는 방식을 사용할 때 다음과 같은 상황에서 **그려진 도형이 사라지는 현상**이 발생함.
1. **Window Overlay:** 다른 창이 프로그램 위를 덮었다가 치워질 때
2. **Minimize/Restore:** 창을 최소화했다가 다시 복구할 때
3. **Resize:** 윈도우 크기를 조절하여 강제로 다시 그리기(Repaint)가 수행될 때

### 🟡 원인 (Root Cause)
마우스 이벤트(예: `OnLButtonDown`)에서 **화면 DC에 직접 그리기**만 하고,
`WM_PAINT`에서 장면을 “재구성”하지 않으면 Windows는 화면을 복구해주지 않습니다.

즉, 화면은 언제든 **무효화(Invalidated)**될 수 있는데,
“화면에 그린 흔적”은 운영체제가 보장해주지 않습니다.

### 🟢 해결 방법 (Solution)
**Memory DC(더블 버퍼링)** 기법을 도입하여 그래픽 데이터의 지속성을 확보함. `CImage` 클래스를 오프스크린 버퍼(Off-screen Buffer)로 활용.

1.  **초기화 (`OnInitDialog`):**
    * 그리기 영역(Picture Control) 크기에 맞는 `CImage` 객체 생성.
    * `CImage::GetDC()`를 통해 메모리 DC 핸들(`mh_drawDC`) 확보.
    * 초기화 시 `ClearCanvas()`로 흰 배경을 한번 채움
2.  **그리기 (`OnLButtonDown` 등):**
    * 화면 DC가 아닌, 메모리 DC(`mh_drawDC`)에 점과 선을 그림.
    * 이 시점에는 화면에 반영되지 않음.
    * `Invalidate(...)` 또는 `InvalidateRect(...)`로 Paint 트리거
3.  **출력 (`OnPaint`):**
    * `OnPaint`에서는 `CImage::Draw` (내부적으로 `BitBlt` 수행)를 호출하여 메모리에 저장된 완성된 이미지를 화면에 고속 복사.
    * 결과적으로 가려짐/최소화/복원/리사이즈 이후에도 `WM_PAINT`만 호출되면 장면이 재현됨

### 💻 핵심 코드 (Code Snippet)
```cpp
// Sots02Dlg.cpp

// 1. 그리기 로직: 메모리 DC에만 기록
void CSots02Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // ... 좌표 변환 후 ...
    ::MoveToEx(mh_drawDC, point.x, point.y, NULL); // mh_drawDC는 메모리 DC
    ::LineTo(mh_drawDC, point.x + 3, point.y);
    
    // 화면 갱신 요청 (OnPaint 호출 유도)
    InvalidateRect(m_bufOffsetRect, FALSE); 
}

// 2. 출력 로직: 메모리 DC 내용을 화면으로 전송
void CSots02Dlg::OnPaint()
{
    CPaintDC dc(this);
    // 메모리(CImage)에 그려진 내용을 현재 화면 DC로 복사
    m_drawImage.Draw(dc, m_drawRect.left, m_drawRect.top);
}
```

## 2. 좌표계 불일치(Coordinate Mismatch) 해결

### 🔴 문제 상황 (Problem)

> MFC의 이벤트 핸들러와 GDI 그리기 함수 간의 좌표 기준점(Origin) 이 서로 달라, 마우스를 클릭한 위치와 실제 점이 찍히는 위치가 어긋나는 현상 발생.

### 🟡 원인 (Root Cause)
MFC Dialog 기반에서는 좌표계가 여러 개 존재합니다.

- **Dialog 좌표계**: `OnLButtonDown`의 `CPoint point`가 기본적으로 따르는 좌표
- **특정 컨트롤(Client) 및 DC 좌표계**: Picture Control 내부는 $(0,0)$ ~ $(w,h)$ 기준 **(Offset)**
- **Screen 좌표계**: `GetWindowRect()`는 기본이 Screen 기준

즉, “어떤 좌표계를 기준으로 계산/그리기/무효화할지”를 통일하지 않으면 mismatch가 발생합니다.

### 🟢 해결 방법 (Solution)

> 다이얼로그 기준 좌표를 Control(Memory DC) 기준의 로컬 좌표로 변환하는 오프셋(Offset) 로직 구현.
1. 영역 계산 (OnInitDialog):
- GetDlgItem(IDC_DRAW_RECT)로 컨트롤 포인터 획득.
- GetWindowRect로 스크린 기준 좌표 획득 후, ScreenToClient를 통해 다이얼로그 내부 기준 좌표(m_drawRect)로 변환.

2. 좌표 보정 (OnLButtonDown):
- 입력된 마우스 좌표(point)에서 컨트롤의 시작 위치(m_drawRect.left, m_drawRect.top)를 차감.
- point.Offset(-left, -top) 수행.

### 📊 좌표 변환 모식도 (Coordinate Flow) 

Click(100, 100) [Dialog 기준] <br>
➡️ Offset(-30, -30) [Control 시작점 차감] <br>
➡️ DrawAt(70, 70) [Memory DC 기준]

## 💻 핵심 코드 (Code Snippet)

```cpp
C++// Sots02Dlg.cpp

BOOL CSots02Dlg::OnInitDialog()
{
    // ...
    // 컨트롤의 위치 정보를 다이얼로그 클라이언트 기준으로 변환하여 저장
    GetDlgItem(IDC_DRAW_RECT)->GetWindowRect(m_drawRect);
    ScreenToClient(&m_drawRect); 
    // ...
}

void CSots02Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // 클릭 위치가 그리기 영역 내부인지 확인
    if (m_drawRect.PtInRect(point)) {
        // [중요] 다이얼로그 좌표 -> 메모리 DC 좌표로 변환
        point.Offset(-m_drawRect.left, -m_drawRect.top);
        
        // 변환된 좌표(local)를 사용하여 그리기
        ::MoveToEx(mh_drawDC, point.x - 3, point.y, NULL);
        // ...
    }
}
```

## 3. 요약 (Summary)

|구분|변경 전|변경 후 (개선됨)|
|------|---|---|
|렌더링 방식|화면 DC(CPaintDC)에 직접 그림|Memory DC(CImage) 에 그리고 BitBlt로 복사|
|화면 가림/복구|그려진 내용 소실됨|영구 보존 (Repaint 시 복구됨)
|좌표 기준|다이얼로그 좌표 그대로 사용 (오차 발생)|Offset 계산을 통해 로컬 좌표로 변환 (정확함)|

## 권장 체크리스트

- [ ] 화면에 직접 그리지 말고 **버퍼에 그린 뒤 WM_PAINT에서 출력**
- [ ] 입력 좌표가 어디 기준인지 명확히( Dialog / Control / Screen )
- [ ] Screen 좌표는 반드시 `ScreenToClient`로 변환 후 사용
- [ ] `SelectObject`한 GDI 객체는 old 객체로 복구 후 `DeleteObject`로 해제
- [ ] 리사이즈로 컨트롤 크기가 바뀌는 경우 `OnSize`에서 버퍼 재생성 고려
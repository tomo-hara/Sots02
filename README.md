# 📐 MFC Geometric Relation Visualizer

Windows MFC(Microsoft Foundation Class)를 사용하여 개발한 기하학적 관계 시각화 도구입니다. 사용자가 마우스로 입력한 점들을 기반으로 직선, 수선, 그리고 교점을 실시간으로 계산하여 시각적으로 구현합니다.

## Demo
### 📷GIF/스크린샷: `docs/demo.gif` (추가 예정)


## 📋 프로젝트 개요 (Overview)

이 프로젝트는 Windows GUI 프로그래밍의 기초인 GDI(Graphics Device Interface)와 이벤트 처리를 학습하고 실습하기 위해 제작되었습니다. 마우스 클릭 이벤트를 통해 좌표를 획득하고, 이를 바탕으로 수학적 계산(직선의 방정식, 수직 조건, 연립방정식)을 수행하여 그래픽으로 표현합니다.

## ✨ 주요 기능 (Features)

## 기술 스택
- **MFC**: CDialogEx, Message Map, OnInitDialog, OnLButtonDown, OnPaint, OnEraseBkgnd
- **WinAPI GDI (Native)**: HDC, HPEN/HBRUSH, SelectObject, MoveToEx/LineTo, Ellipse, TextOut/DrawText, (옵션) Memory DC + BitBlt
- **C++**: 구조체/클래스 기반 모델링, 예외/경계 케이스 처리

## 📝 구현 핵심 (Implementation Details)

## 🛠 개발 환경 (Environment)

* **OS:** Windows 10 / 11
* **IDE:** Visual Studio 2019 or 2022
* **Language:** C++
* **Framework:** MFC (Microsoft Foundation Class)
* **Architecture:** Document/View Architecture (또는 Dialog Based)

## 🚀 사용 방법 (Usage)

1.  프로그램을 실행합니다.
2.  클라이언트 영역의 임의의 위치를 **클릭**하여 첫 번째 점($P_1$)을 찍습니다.
3.  다른 위치를 **클릭**하여 두 번째 점($P_2$)을 찍으면, 두 점을 잇는 **직선**이 나타납니다.
4.  또 다른 위치를 **클릭**하여 세 번째 점($P_3$)을 찍으면, $P_3$에서 직선으로 내린 **수선**과 **교점**이 나타납니다.
5.  (옵션) 마우스 오른쪽 클릭 또는 리셋 버튼을 통해 화면을 초기화할 수 있습니다.

## 🧮 수학적 알고리즘 (Math Logic)

이 프로그램은 다음과 같은 수학적 원리를 코드로 구현했습니다.

### 1. 직선의 방정식 (Line Equation)
두 점 $P_1(x_1, y_1)$, $P_2(x_2, y_2)$를 지나는 직선의 기울기 $m$은 다음과 같습니다.
$$m = \frac{y_2 - y_1}{x_2 - x_1}$$

이를 이용한 직선의 방정식:
$$y - y_1 = m(x - x_1)$$

### 2. 수선의 방정식 (Perpendicular Equation)
직선의 기울기가 $m$일 때, 이에 수직인 직선(수선)의 기울기 $m_{\perp}$는 다음과 같습니다.
$$m_{\perp} = -\frac{1}{m}$$

점 $P_3(x_3, y_3)$를 지나고 기울기가 $m_{\perp}$인 수선의 방정식:
$$y - y_3 = -\frac{1}{m}(x - x_3)$$

### 3. 교점 구하기 (Intersection)
두 직선의 방정식을 연립하여 $x, y$를 구합니다.
* **일반적인 경우:** $ax + b = cx + d$ 꼴로 놓고 $x$를 구한 뒤 $y$를 대입.
* **수직선의 경우:** 분모가 0이 되는 경우($x_1 = x_2$)를 별도로 분기 처리.

## 📂 파일 구조 (File Structure)

```text
Sots02/ (Project Root)
├── Sots02.h                // 애플리케이션 클래스 선언부 (CWinApp 상속)
├── Sots02.cpp              // 애플리케이션의 진입점(InitInstance) 및 실행 로직 구현
│
├── Sots02Dlg.h             // 메인 UI 대화상자 클래스 선언 (CDialogEx 상속)
├── Sots02Dlg.cpp           // UI 이벤트 처리(버튼 클릭, 데이터 교환 등) 및 비즈니스 로직
│
├── Resource.h              // 컨트롤 ID(IDC_BUTTON1 등) 및 리소스 식별자 정의 헤더
├── Sots02.rc               // 메뉴, 아이콘, 다이얼로그 디자인 등 리소스 스크립트
│
├── stdafx.h                // 미리 컴파일된 헤더 (자주 변경되지 않는 시스템 헤더 포함)
├── stdafx.cpp              // 미리 컴파일된 헤더 생성을 위한 소스 파일
│
├── targetver.h             // 지원할 Windows 플랫폼 버전을 정의하는 헤더
└── res/                    // 리소스 바이너리 폴더 (아이콘, 비트맵 등 저장)
    └── Sots02.ico          // 응용 프로그램 아이콘 파일
```

## UI Flow
1. 사용자 클릭 이벤트 수신 → 클릭 좌표 저장(점 배열/벡터)
2. 점 개수에 따라 렌더링 대상 결정
   - 0~1개: 점만 표시
   - 2개: 점 + 직선 + (선택) 방정식 표시
   - 3개: 점 + 직선 + 수선 + 교점 표시
3. `Invalidate()` 등을 통해 Redraw 트리거 → `OnPaint()`에서 도형 렌더링

## Edge Cases

## Build & Run
- IDE: Visual Studio 2017
- OS: Windows 11
- 프로젝트: MFC Dialog 기반

## License

- None
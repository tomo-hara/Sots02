## WSL(Ubuntu) 환경에서 `cmake . && make` 명령어로 .so 파일이 생성되는지 확인합니다.
# Usage : 
# 1. Create a build directory: mkdir build && cd build
# 2. Run CMake to configure the project: cmake ..
# 3. Build the project: cmake --build .
# 4. The shared library will be generated in the build directory.

# 5. (Optional) To install the library system-wide, run: sudo make install
# 6. (Optional) To use the library in other projects, link against GeoMath.
# 7. (Optional) Add include directories and link libraries in your CMakeLists.txt as needed.
# 8. (Optional) For Windows, ensure you have the necessary build tools installed (e.g., Visual Studio with CMake support).
# 9. (Optional) For Linux, ensure you have the necessary build tools installed (e.g., GCC, Make).

### 각 플랫폼에 따라 GeoMath 라이브러리를 가져와서 사용하는 간단한 코드를 추가합니다.
## Linux/macOS용 main.cpp 예시 
# Linux에서는 MFC DoModal() 대신 main() 함수가 진입점이 됩니다. 
#
# // main.cpp
# #include <iostream>
# #include "GeoMath.h" // 우리가 분리할 핵심 로직 헤더
# 
# int main(int argc, char* argv[]) {
#     std::cout << "[Linux/macOS Geometry Tool]" << std::endl;
# 
#     // 예: 좌표 입력 (실제로는 argv로 받을 수 있음)
#     PointF p1 = {10, 10};
#     PointF p2 = {20, 20};
# 
#     // 분리해 둔 Core 로직 호출
#     LineEq eq = GeoMath::GetLineEquation(p1, p2);
# 
#     // 결과 텍스트 출력
#     if (eq.isVertical) {
#         std::cout << "Result: Vertical Line at X=" << eq.verticalX << std::endl;
#     } else {
#         std::cout << "Result: Y = " << eq.m << "X + " << eq.c << std::endl;
#     }
# 
#     return 0;
# }

## Windows용 main.cpp 예시
# Windows에서는 MFC 애플리케이션의 진입점이 되는 WinMain() 함수가 필요합니다.
#
# // main.cpp
# #include <afxwin.h>
# #include "GeoMath.h" // 우리가 분리할 핵심 로직 헤더
# 
#class CGeoMathApp : public CWinApp {
#    public:
#        virtual BOOL InitInstance() {
#                AfxMessageBox(_T("[Windows Geometry Tool]"));
#                // 예: 좌표 입력 (실제로는 사용자 입력을 받을 수 있음)
#                PointF p1 = {10, 10};
#                PointF p2 = {20, 20};
#                // 분리해 둔 Core 로직 호출
#                LineEq eq = GeoMath::GetLineEquation(p1, p2);
#                // 결과 메시지 박스 출력
#                CString result;
#                if (eq.isVertical) {
#                        result.Format(_T("Result: Vertical Line at X=%f"), eq.verticalX);
#                    } else {
#                        result.Format(_T("Result: Y = %fX + %f"), eq.m, eq.c);
#                    }
#                    AfxMessageBox(result);
#                    return FALSE; // 애플리케이션 종료
#                }
#        };
#        CGeoMathApp theApp;


cmake_minimum_required(VERSION 3.10)
project(GeoMathEngine)

set(CMAKE_CXX_STANDARD 14)

# 소스 파일 지정
set(SOURCE_FILES GeoMath.cpp GeoMath.h)

# 라이브러리 생성 (Shared Library -> .dll / .so)
add_library(GeoMath SHARED ${SOURCE_FILES})

# Linux인 경우 컴파일 옵션 추가
if(UNIX)
    target_compile_options(GeoMath PRIVATE -Wall -Wextra)
endif()
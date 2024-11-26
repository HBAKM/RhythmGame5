#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>

// 노트 벡터 정의
std::vector<Note> notes;

extern std::chrono::steady_clock::time_point startTime;

// std::chrono::duration_cast: 시간 단위 변환 함수
// current - startTime: 현재 시간과 게임 시작 시간(startTime)의 차이
// std::chrono::duration_cast<std::chrono::milliseconds>():
//      std::chrono::duration 객체를 밀리초(milliseconds) 단위로 변환. (duration_cast로 명시적 변환)
// .count() :td::chrono::duration 객체에서 실제 값을 꺼냄 -> currentTime이 됨

int currentTime = 0;  // 게임이 시작된 이후 경과한 시간을 밀리초 단위로 저장

// 게임 시작 시점
std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

// 경과 시간을 계산하는 함수
int GetElapsedTime() {
    auto current = std::chrono::steady_clock::now(); // 현재 시간 가져옴
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - startTime).count();
    currentTime = static_cast<int>(elapsed); // currentTime을 갱신
    return currentTime;
}

// 노트 로드 함수
void LoadNotes(const std::string& filename) {
    ParseOsuFile(filename);  // osu! 파일을 파싱하여 노트를 로드
}

void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness) {
    // 테두리 펜 설정 (두께와 색상)
    HPEN hPen = CreatePen(PS_SOLID, thickness, RGB(0, 0, 0));  // 검정색 테두리
    HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);      // 투명 내부 브러시

    // 이전 GDI 객체 저장
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // 원 그리기 (테두리만 표시)
    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

    // 이전 GDI 객체 복원
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // GDI 객체 삭제
    DeleteObject(hPen);
}

// 노트 그리기 함수 (화면 좌표 사용)
// screenX, screenY: 화면 좌표. 이 좌표에 노트를 그림
// scaleX, scaleY: 스케일을 조정할 비율. 게임 화면 크기에 따라 노트의 크기를 조절
void Note::Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const {
    if (type == 1) {
        // 원 그리기 (크기 및 위치 스케일링 적용)
        int radius = static_cast<int>(20 * scaleX);  // 반지름 크기 조정
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {
        // 슬라이더 (궤적 따라 테두리 원 배치)
        if (sliderPoints.size() > 1) { // sliderPoints 배열에 점이 두 개 이상 있는지 확인
            const int bubbleRadius = static_cast<int>(25 * scaleX);  // 슬라이더 크기
            const int borderThickness = 2;                           // 테두리 두께
            const float bubbleSpacing = 20.0f;                       //  간격

            // 궤적을 따라 이동하며 원 배치
            for (size_t i = 0; i < sliderPoints.size() - 1; ++i) {
                POINT start = sliderPoints[i];
                POINT end = sliderPoints[i + 1];

                // 화면 좌표로 변환 (스케일 적용)
                int startX = static_cast<int>(start.x * scaleX);
                int startY = static_cast<int>(start.y * scaleY);
                int endX = static_cast<int>(end.x * scaleX);
                int endY = static_cast<int>(end.y * scaleY);

                // 시작점과 끝점 사이의 거리 계산
                float dx = endX - startX;
                float dy = endY - startY;
                float distance = sqrt(dx * dx + dy * dy); // 피타고라스의 정리

                // 일정 간격마다 원 배치
                for (float t = 0; t < distance; t += bubbleSpacing) {
                    float ratio = t / distance;  // 간격 비율
                    int bubbleX = static_cast<int>(startX + ratio * dx);
                    int bubbleY = static_cast<int>(startY + ratio * dy);

                    // 테두리 그리기
                    DrawCircleOutline(hdc, bubbleX, bubbleY, bubbleRadius, borderThickness);
                }
            }
        }
    }
}

void DrawNotes(HDC hdc, RECT rect, HWND hWnd, int currentTime){
    for (const auto& note : notes) {
        if (currentTime >= note.startTime) { 
            // 노트의 좌표를 계산하고 그리기
            int screenX = note.x;
            int screenY = note.y; 
            note.Draw(hdc, screenX, screenY, 1.0f, 1.0f);  // Draw 호출
        }
    }
}

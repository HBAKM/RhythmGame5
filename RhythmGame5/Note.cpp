#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>

// 노트 벡터 정의
std::vector<Note> notes;

extern std::chrono::steady_clock::time_point startTime;

// 게임이 시작된 후 경과 시간을 밀리초 단위로 반환하는 함수
int GetElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return static_cast<int>(duration.count());
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
void Note::Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const {
    OutputDebugString(L"Note::Draw 함수가 호출됨\n");// 디버깅용
    if (type == 1) {
        // 원 그리기 (크기 및 위치 스케일링 적용)
        int radius = static_cast<int>(20 * scaleX);  // 반지름 크기 조정
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {
        // 슬라이더 (비눗방울처럼 궤적 따라 테두리 원 배치)
        if (sliderPoints.size() > 1) {
            const int bubbleRadius = static_cast<int>(25 * scaleX);  // 슬라이더 비눗방울 크기
            const int borderThickness = 2;                           // 테두리 두께
            const float bubbleSpacing = 20.0f;                       // 비눗방울 간의 간격

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
                float distance = sqrt(dx * dx + dy * dy);

                // 일정 간격마다 원 배치
                for (float t = 0; t < distance; t += bubbleSpacing) {
                    float ratio = t / distance;  // 간격 비율
                    int bubbleX = static_cast<int>(startX + ratio * dx);
                    int bubbleY = static_cast<int>(startY + ratio * dy);

                    // 테두리 비눗방울 그리기
                    DrawCircleOutline(hdc, bubbleX, bubbleY, bubbleRadius, borderThickness);
                }
            }
        }
    }
}

void DrawNotes(HDC hdc, RECT rect, HWND hWnd) {
    int currentTime = GetElapsedTime();  // 밀리초 단위의 게임 경과 시간

    for (const auto& note : notes) {
        if (currentTime >= note.startTime) {  // note.time을 note.startTime으로 수정
            // 노트의 좌표를 계산하고 그리기
            int screenX = note.x;
            int screenY = note.y + (currentTime - note.startTime) * 0.1f;  // Y좌표는 시간에 따라 이동
            note.Draw(hdc, screenX, screenY, 1.0f, 1.0f);  // Draw 호출
        }
    }
}
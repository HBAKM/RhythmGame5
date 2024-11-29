#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <cmath>

// 노트 벡터 정의
std::vector<Note> notes;

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
void Note::Draw(HDC hdc, int screenX, int screenY) const {
    if (type == 1) {
        // 원 그리기 (크기 고정)
        int radius = 20;  // 고정된 반지름 크기
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {  // 슬라이더 노트
        // 슬라이더의 지속 시간 계산
        int sliderDuration = endTime - startTime; // 슬라이더의 지속 시간

        extern int gameStartTimeMillis;

        // 슬라이더가 지나간 비율(progress)을 계산 (0~100)
        int progress = 0;
        if (gameStartTimeMillis >= startTime && gameStartTimeMillis <= endTime) {
            progress = (gameStartTimeMillis - startTime) * 100 / sliderDuration;
        }

        // 슬라이더 진행률이 100% 이상이면 더 이상 진행하지 않음
        if (progress > 100) progress = 100;

        // 현재 위치 계산: 첫 번째 점과 마지막 점 사이를 보간
        size_t numPoints = sliderPoints.size();
        size_t currentPointIndex = progress * (numPoints - 1) / 100;  // 현재 점( 진행률에 맞는 점)의 인덱스 계산

        // 두 점 사이를 보간하여 원의 위치를 계산
        if (currentPointIndex < numPoints - 1) {
            POINT p1 = sliderPoints[currentPointIndex];
            POINT p2 = sliderPoints[currentPointIndex + 1];

            // 선형 보간 (progress에 따른 점 위치 계산)
            int interpX = p1.x + (p2.x - p1.x) * (progress - currentPointIndex * 100) / 100;
            int interpY = p1.y + (p2.y - p1.y) * (progress - currentPointIndex * 100) / 100;

            // 이동하는 원 그리기
            int radius = 20;
            Ellipse(hdc, interpX - radius, interpY - radius, interpX + radius, interpY + radius);
        }

        // 슬라이더 경로 그리기 (선으로 그리기)
        for (size_t i = 0; i < sliderPoints.size() - 1; ++i) {
            POINT p1 = sliderPoints[i];
            POINT p2 = sliderPoints[i + 1];
            MoveToEx(hdc, p1.x, p1.y, NULL);
            LineTo(hdc, p2.x, p2.y);  // 선을 그리기
        }
    }
}

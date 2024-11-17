#pragma once

#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <string>
#include <windows.h>

// 노트 구조체 정의
struct Note {
    int startTime;
    int x;
    int y;
    int type;  // 1: 원, 2: 슬라이드
    std::vector<POINT> sliderPoints;  // 슬라이드의 궤적을 나타내는 점들

    void Draw(HDC hdc) const {
        if (type == 1) {
            // 원 그리기
            Ellipse(hdc, x - 50, y - 50, x + 50, y + 50);
        }
        else if (type == 2) {
            // 슬라이드 곡선 그리기
            if (!sliderPoints.empty()) {
                MoveToEx(hdc, sliderPoints[0].x, sliderPoints[0].y, NULL);
                for (size_t i = 1; i < sliderPoints.size(); i++) {
                    LineTo(hdc, sliderPoints[i].x, sliderPoints[i].y);
                }
            }
        }
    }
};

extern std::vector<Note> notes;

void LoadNotes(const std::string& filename);
void UpdateNotes(int currentTime);
void DrawNotes(HDC hdc, int currentTime);

#endif // NOTE_H

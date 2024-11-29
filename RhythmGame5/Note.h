#pragma once

#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <string>
#include <windows.h>
#include <chrono>

// 노트 구조체 정의
struct Note {
    int startTime; // 노트가 등장하는 시간 (밀리초 단위)
    int endTime;    // 노트 종료 시간 (밀리초 단위)
    int x;  // x 좌표 (화면 좌표)
    int y;  // y 좌표 (화면 좌표)
    int type;  // 1: 원형 노트, 2: 슬라이더 노트
    std::vector<POINT> sliderPoints;  // 슬라이더의 궤적을 나타내는 점들

    // 기본 생성자
    Note(int startTime, int x, int y, int type)
        : startTime(startTime), x(x), y(y), type(type) {
    }

    // 슬라이더 노트를 위한 생성자
    Note(int startTime, int x, int y, int type, const std::vector<POINT>& sliderPoints, int endTime)
        : startTime(startTime), x(x), y(y), type(type), sliderPoints(sliderPoints), endTime(endTime) {
    }

    // 노트 그리기 함수 (화면 좌표 사용)
    void Draw(HDC hdc, int screenX, int screenY) const;

};

// 전역 노트 벡터
extern std::vector<Note> notes;

// 게임 시작 시간 기록
extern std::chrono::steady_clock::time_point gameStartTime;

// 함수 선언
void LoadNotes(const std::string& filename);
void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness);

#endif // NOTE_H

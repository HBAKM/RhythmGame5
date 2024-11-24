#pragma once

#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <string>
#include <windows.h>
#include <chrono>

// 노트 구조체 정의
struct Note {
    enum class Type {
        HitX, HitZ
    };

    int startTime; // 노트가 등장하는 시간 (밀리초 단위)
    int x;
    int y;
    int type;  // 1: 원, 2: 슬라이드
    std::vector<POINT> sliderPoints;  // 슬라이드의 궤적을 나타내는 점들
    bool isHit;  // 노트가 맞았는지 여부

    // 기본 생성자
    Note(int startTime, int x, int y, int type)
        : startTime(startTime), x(x), y(y), type(type) {
    }

    // 슬라이더 노트를 위한 생성자
    Note(int startTime, int x, int y, int type, const std::vector<POINT>& sliderPoints)
        : startTime(startTime), x(x), y(y), type(type), sliderPoints(sliderPoints) {
    }

    // 노트 그리기 함수 (화면 좌표 사용)
    void Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const;

};

// == 연산자 오버로드 추가
inline bool operator==(const Note& lhs, const Note& rhs) {
    return lhs.startTime == rhs.startTime &&
        lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.type == rhs.type;
}

// 전역 노트 벡터
extern std::vector<Note> notes;

// 게임 시작 시간 기록
extern std::chrono::steady_clock::time_point startTime;

// 함수 선언
void LoadNotes(const std::string& filename);
void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness);
int GetElapsedTime();

#endif // NOTE_H

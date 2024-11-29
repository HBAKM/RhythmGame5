#pragma once
#include <windows.h>
#include "Note.h" 

// 게임 화면을 그리는 함수
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd);

// 게임 초기화 함수
void InitializeGame();

// 노트를 맞췄는지 확인하는 함수
bool IsNoteHit(const Note& note, const POINT& mousePos);

// 원형 점에 가까운지 확인하는 함수
bool IsPointNearCircle(const POINT& mousePos, const POINT& circlePos, int radius=20);

// 히트 관련 처리 함수
void HandleHit(Note& note, POINT mousePos);

// 마우스 위치 받아오는 함수
void ProcessMouseHit(POINT mousePos);

// 게임 시작 시간
extern std::chrono::steady_clock::time_point gameStartTime;

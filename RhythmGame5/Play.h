#pragma once
#include <windows.h>
#include "Note.h" 

// 게임 화면을 그리는 함수
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd, int currentTime);

// 게임 초기화 함수
void InitializeGame();

// 노트를 맞췄는지 확인하는 함수
bool IsNoteHit(const Note& note, WPARAM keyPressed, POINT mousePos);

// 히트 관련 처리 함수
void HandleHit(Note& note);
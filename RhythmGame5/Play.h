#pragma once
#include <windows.h>
#include "Note.h"  // Note 클래스가 정의되어 있어야 합니다.

// 게임 화면을 그리는 함수
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd);

// 게임 초기화 함수
void InitializeGame();

// 노트를 맞췄는지 확인하는 함수
bool IsNoteHit(const Note& note, WPARAM keyPressed);

void HandleHit(Note& note);
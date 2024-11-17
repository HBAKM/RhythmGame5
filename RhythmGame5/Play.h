#pragma once
#include <windows.h>

// 게임 화면을 그리는 함수 
void DrawGame(HDC hdc, RECT& rcPaint);

// 게임 초기화 함수
void InitializeGame();
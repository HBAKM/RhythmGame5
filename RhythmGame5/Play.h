#pragma once
#include <windows.h>
#include "Note.h" 

// ���� ȭ���� �׸��� �Լ�
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd);

// ���� �ʱ�ȭ �Լ�
void InitializeGame();

// ��Ʈ�� ������� Ȯ���ϴ� �Լ�
bool IsNoteHit(const Note& note, const POINT& mousePos);

// ���� ���� ������� Ȯ���ϴ� �Լ�
bool IsPointNearCircle(const POINT& mousePos, const POINT& circlePos, int radius=20);

// ��Ʈ ���� ó�� �Լ�
void HandleHit(Note& note, POINT mousePos);

// ���콺 ��ġ �޾ƿ��� �Լ�
void ProcessMouseHit(POINT mousePos);

// ���� ���� �ð�
extern std::chrono::steady_clock::time_point gameStartTime;

#pragma once
#include <windows.h>
#include "Note.h" 

// ���� ȭ���� �׸��� �Լ�
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd, int currentTime);

// ���� �ʱ�ȭ �Լ�
void InitializeGame();

// ��Ʈ�� ������� Ȯ���ϴ� �Լ�
bool IsNoteHit(const Note& note, WPARAM keyPressed, POINT mousePos);

// ��Ʈ ���� ó�� �Լ�
void HandleHit(Note& note);
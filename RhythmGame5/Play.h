#pragma once
#include <windows.h>
#include "Note.h"  // Note Ŭ������ ���ǵǾ� �־�� �մϴ�.

// ���� ȭ���� �׸��� �Լ�
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd);

// ���� �ʱ�ȭ �Լ�
void InitializeGame();

// ��Ʈ�� ������� Ȯ���ϴ� �Լ�
bool IsNoteHit(const Note& note, WPARAM keyPressed);

void HandleHit(Note& note);
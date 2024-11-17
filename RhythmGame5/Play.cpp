#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include "play.h"
#include "Note.h"
#include "Parser.h"

#pragma comment(lib, "winmm.lib") // Windows ��Ƽ�̵�� ���̺귯�� ��ũ

extern std::vector<Note> notes; // �ܺο��� ���ǵ� Note ����

int life = 10; // �ʱ� ����
bool isGameOver = false; // ���� ���� ����
bool isGameClear = false; // ���� Ŭ���� ����

std::chrono::steady_clock::time_point startTime; // ���� ���� �ð�

// ���� �ʱ�ȭ �Լ�
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! ���Ͽ��� ��Ʈ �ε�
    PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // ��� ���� ���
    startTime = std::chrono::steady_clock::now(); // ���� �ð� ���
}

// ������ ���۵� �� ��� �ð��� �и��� ������ ��ȯ�ϴ� �Լ�
int GetElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return static_cast<int>(duration.count());
}

// ���� ������Ʈ �Լ� (��Ʈ ������Ʈ �� ���� ���� üũ)
void UpdateGame(HWND hWnd) {
    int currentTime = GetElapsedTime();
    UpdateNotes(currentTime);

    // ������ �� �Ǿ��� �� ���� ����
    if (life <= 0) {
        isGameOver = true;
        std::cout << "���� ����!" << std::endl;
    }

    InvalidateRect(hWnd, NULL, TRUE); // ȭ�� ����
}

// ���� ȭ���� �׸��� �Լ� ����
void DrawGame(HDC hdc, RECT& rcPaint) {
    // ��Ʈ �׸���
    for (Note& note : notes) {
        note.Draw(hdc);
    }

    // ����� �׸���
    int maxLife = 10;
    int lifeWidth = 200;
    int lifeHeight = 20;
    int currentLifeWidth = static_cast<int>((static_cast<float>(life) / maxLife) * lifeWidth);

    // ��� ����
    RECT bgRect = { 10, 10, 10 + lifeWidth, 10 + lifeHeight };
    HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
    FillRect(hdc, &bgRect, hBrush);
    DeleteObject(hBrush);

    // ���� ���� ����
    RECT lifeRect = { 10, 10, 10 + currentLifeWidth, 10 + lifeHeight };
    hBrush = CreateSolidBrush(RGB(0, 255, 0));
    FillRect(hdc, &lifeRect, hBrush);
    DeleteObject(hBrush);

    if (isGameOver) {
        std::wstring gameOverText = L"���� ����!";
        TextOut(hdc, 250, 250, gameOverText.c_str(), gameOverText.length());
    }
    else if (isGameClear) {
        std::wstring gameClearText = L"���� Ŭ����!";
        TextOut(hdc, 250, 250, gameClearText.c_str(), gameClearText.length());
    }

    // Ȯ�ο� �ڵ�
    if (!PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC)) {
        std::cerr << "����� ���� ��� ����!" << std::endl;
    }
}
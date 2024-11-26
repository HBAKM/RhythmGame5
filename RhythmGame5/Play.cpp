#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include "play.h"
#include "Note.h"
#include "Parser.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")  // GDI+ ���̺귯�� ��ũ
#pragma comment(lib, "winmm.lib") // Windows ��Ƽ�̵�� ���̺귯�� ��ũ

extern std::vector<Note> notes; // �ܺο��� ���ǵ� Note ����

int life = 10; // �ʱ� ����
bool isGameOver = false; // ���� ���� ����
bool isGameClear = false; // ���� Ŭ���� ����

// std::chrono: ����ð� ��������
// steady_clock: ��� �ð�(Elapsed Time)�� ���
// time_point: Ư������
extern std::chrono::steady_clock::time_point startTime; // ���� ���� �ð�

// ���� �ʱ�ȭ �Լ�
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! ���Ͽ��� ��Ʈ �ε�
   // PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // ��� ���� ���
    startTime = std::chrono::steady_clock::now(); // ���� �ð��� ���۽ð����� (���� ���۽ð�)
}

// ���� ������Ʈ �Լ� (��Ʈ ������Ʈ �� ���� ���� üũ)
void UpdateGame(HWND hWnd) {
    int currentTime = GetElapsedTime();

    // Ÿ�ֿ̹� ���� ������ ��Ʈ ����
    notes.erase(std::remove_if(notes.begin(), notes.end(),
        [currentTime](const Note& note) {
            return currentTime > note.startTime + 500; // Ÿ�̹� �ʰ�
        }),
        notes.end());

    // ������ �� �Ǿ��� �� ���� ����
    if (life <= 0) {
        isGameOver = true;
    }

    InvalidateRect(hWnd, NULL, TRUE); // ȭ�� ����
}

void HandleHit(Note& note) {
    // ��Ʈ�� ��Ȯ�� ������ ��
    if (!note.isHit) {  // �̹� ���� ��Ʈ�� ó������ ����
        note.isHit = true;  // ��Ʈ�� ����ٰ� ǥ��
        notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());  // ���Ϳ��� ����

        if (life < 10) {
            life++;  // ������ �� ���� ����
        }
    }
    else {
        if (life > 0) {
            life--;  // ���� 1 ����
        }
    }
}

// Ÿ�ֿ̹� �´��� Ȯ���ϴ� �Լ�
bool IsNoteHit(const Note& note, WPARAM keyPressed, POINT mousePos) {
    // ��Ʈ�� ��Ʈ ������ Ÿ�ֿ̹� �ִ��� Ȯ��
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    return false;
}

// ���� ȭ���� �׸��� �Լ� ����
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd, int currentTime) {

    /// GDI+ �ʱ�ȭ
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;  // GDI+ ��ū ����
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // �̹��� �ε�
    Gdiplus::Image image(L"background_dark.bmp");
    /*if (image.GetLastStatus() != Gdiplus::Ok) {
        MessageBox(hWnd, L"�̹����� �ε��ϴ� �� �����߽��ϴ�.", L"����", MB_OK | MB_ICONERROR);
    }*/

    // â ũ�� ��������
    RECT rect;
    GetClientRect(hWnd, &rect);

    // ȭ�� ũ�⿡ ���� ������ ���
    // scaleX, scaleY: �������� ������ ����. ���� ȭ�� ũ�⿡ ���� ��Ʈ�� ũ�⸦ ���� 
    float scaleX = static_cast<float>(rect.right) / 512.0f;  // osu! �⺻ �ʺ�
    float scaleY = static_cast<float>(rect.bottom) / 384.0f; // osu! �⺻ ����

    // GDI+�� ��� �̹��� �׸���
    Gdiplus::Graphics graphics(hdc);
    graphics.DrawImage(&image, 0, 0, rect.right, rect.bottom);

    // ����� �׸���
    int maxLife = 10;
    int lifeWidth = 400;
    int lifeHeight = 20;
    int currentLifeWidth = static_cast<int>((static_cast<float>(life) / maxLife) * lifeWidth);

    // ���� ���� ����
    RECT lifeRect = { 10, 10, 10 + currentLifeWidth, 10 + lifeHeight };
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    FillRect(hdc, &lifeRect, hBrush);
    DeleteObject(hBrush);

  /*  std::wstring msg = L"��Ʈ ���� ũ��: " + std::to_wstring(notes.size());
    MessageBox(hWnd, msg.c_str(), L"����� �޽���", MB_OK);*/
    
    if (notes.empty()) {
        OutputDebugString(L"��Ʈ�� �����ϴ�.\n"); // ������
    }

    // ��Ʈ �׸���
    for (auto& note : notes) {
        // ��Ʈ�� ���� �ð��� ���� �ð����� �۰ų� ������ ȭ�鿡 �׸���
        if (note.startTime <= currentTime) {
            // ��Ʈ �׸���
            void DrawGame(HDC hdc, RECT & rcPaint, HWND hWnd, int currentTime);
        }

        // �ؽ�Ʈ ����, ũ�� �� ��Ÿ���� ������ �� �ֽ��ϴ�.
        if (isGameOver) {
            std::wstring gameOverText = L"���� ����!";
            SetTextColor(hdc, RGB(255, 0, 0));  // ������ �ؽ�Ʈ
            SetBkMode(hdc, TRANSPARENT);  // ��� ���� �ؽ�Ʈ�� �׸���
            TextOut(hdc, 250, 250, gameOverText.c_str(), gameOverText.length());
        }
        else if (isGameClear) {
            std::wstring gameClearText = L"���� Ŭ����!";
            SetTextColor(hdc, RGB(0, 255, 0));  // �ʷϻ� �ؽ�Ʈ
            SetBkMode(hdc, TRANSPARENT);  // ��� ���� �ؽ�Ʈ�� �׸���
            TextOut(hdc, 250, 250, gameClearText.c_str(), gameClearText.length());
        }
    }
}
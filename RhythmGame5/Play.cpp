#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <cmath> 
#include "play.h"
#include "Note.h"
#include "Parser.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")  // GDI+ ���̺귯�� ��ũ
#pragma comment(lib, "winmm.lib") // Windows ��Ƽ�̵�� ���̺귯�� ��ũ

extern std::vector<std::pair<int, double>> timingPoints; // Ÿ�̹� ����Ʈ
extern std::vector<Note> notes; // �ܺο��� ���ǵ� Note ����

int life = 10; // �ʱ� ����
bool isGameOver = false; // ���� ���� ����
bool isGameClear = false; // ���� Ŭ���� ����

// std::chrono: ����ð� ��������
// steady_clock: ��� �ð�(Elapsed Time)�� ���
// time_point: Ư������
std::chrono::steady_clock::time_point gameStartTime;
int gameStartTimeMillis;

// ���� �ʱ�ȭ �Լ�
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! ���Ͽ��� ��Ʈ �ε�
    //PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // ��� ���� ���
    gameStartTime = std::chrono::steady_clock::now(); // ���ӽð� �ð� ���� ����
    // ����� �ð��� �и��� ������ ��ȯ
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(gameStartTime.time_since_epoch());
    // �и��� ���� ��Ƽ���� ����
    int gameStartTimeMillis = duration.count();
}

// ���콺 ��ġ �޾ƿ��� �Լ�
void ProcessMouseHit(POINT mousePos) {
    // ���콺 ��ġ���� ��Ʈ�� ó���ϴ� �ڵ�
    for (auto& note : notes) {
        HandleHit(note, mousePos); // ��Ʈ ���θ� ó��
    }
}

void HandleHit(Note& note, POINT mousePos) {
    // �����̴� ��Ʈ�� ���
    if (note.type == 2) {
        // �����̴��� ��� ���� ���� Ȯ��
        for (size_t i = 0; i < note.sliderPoints.size(); ++i) {
            if (IsPointNearCircle(mousePos, note.sliderPoints[i])) {
                // ������ ���� ����
                if (life < 10) {
                    life++;  // ������ �� ���� ����
                }
            }
            else {
                // Ʋ������ ���� ����
                if (life > 0) {
                    life--;  // Ʋ���� ���� 1 ����
                }
            }
        }
    }
    else { // �Ϲ� ��Ʈ�� ���
        if (IsNoteHit(note, mousePos)) {
            // ������ ���� ����
            if (life < 10) {
                life++;  // ������ �� ���� ����
            }
        }
        else {
            // Ʋ������ ���� ����
            if (life > 0) {
                life--;  // Ʋ���� ���� 1 ����
            }
        }
    }
}

// ���� ��Ʈ�� ���콺 ��ġ�� ��Ʈ�ߴ��� Ȯ���ϴ� �Լ�
bool IsNoteHit(const Note& note, const POINT& mousePos) {
    if (note.type == 1) {  // ���� ��Ʈ
        // ���� �������� ����
        int radius = 20;

        // ���콺 ��ġ�� ��Ʈ�� ��ġ ���� �Ÿ� ���
        int dx = mousePos.x - note.x;
        int dy = mousePos.y - note.y;

        // �Ÿ� ���
        int distanceSquared = dx * dx + dy * dy;
        int radiusSquared = radius * radius;

        // ���콺 ��ġ�� �� �ȿ� ������ ��Ʈ
        return distanceSquared <= radiusSquared;
    }
    else if (note.type == 2) {  // �����̴� ��Ʈ
        // �����̴��� ���� ���� �����Ƿ�, �� ���� ���� �Ÿ� ���
        for (size_t i = 0; i < note.sliderPoints.size() - 1; ++i) {
            // �� ���� �մ� ���п� ���� �Ÿ� ���
            POINT sliderPoint = note.sliderPoints[i];

            // �����̴� ���� ���� ���� ���콺�� �´��� Ȯ��
            if (IsPointNearCircle(mousePos, sliderPoint)) {
                return true;
            }
        }
    }

    return false;  // �⺻������ ��Ʈ���� ����
}

// ���� ���� ������� Ȯ���ϴ� �Լ�
bool IsPointNearCircle(const POINT& mousePos, const POINT& circlePos, int radius) {
    // ���콺�� �� ������ �Ÿ��� ������ ������ �̳����� Ȯ��
    int dx = mousePos.x - circlePos.x;
    int dy = mousePos.y - circlePos.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// ���� ȭ���� �׸��� �Լ� ����
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd) {

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

    //// ȭ�� ũ�⿡ ���� ������ ���
    //// scaleX, scaleY: �������� ������ ����. ���� ȭ�� ũ�⿡ ���� ��Ʈ�� ũ�⸦ ���� 
    //float scaleX = static_cast<float>(rect.right) / 512.0f;  // osu! �⺻ �ʺ�
    //float scaleY = static_cast<float>(rect.bottom) / 384.0f; // osu! �⺻ ����

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
        // ��Ʈ�� ��ǥ ���
        int screenX = note.x;  // ��Ʈ�� X ��ǥ
        int screenY = note.y;  // ��Ʈ�� Y ��ǥ

        // Ÿ�̹� ����Ʈ�� ��Ʈ ����Ʈ�� ���Ͽ� ��Ʈ�� ���� �ð��� ����
        for (const auto& timingPoint : timingPoints) {
            int time = timingPoint.first;
            double bpm = timingPoint.second;

            if (note.startTime >= time) {
                note.startTime = note.startTime + (60000.0 / bpm) * (note.startTime - time);
            }
        }

        if (gameStartTimeMillis == note.startTime) {
            note.Draw(hdc, screenX, screenY);  // ��Ʈ �׸���
        }

    }

    if (life <= 0) {
        isGameOver = true;
    }
    
    // ���ӿ���, ����Ŭ���� �޽����ڽ�
    if (isGameOver) {
        int msgBoxResult = MessageBox(NULL, L"���� ����!", L"���� ����", MB_OK | MB_ICONERROR);  // ���� ���� �޽��� �ڽ�

        // Ȯ�� ��ư�� ������ ���� ����
        if (msgBoxResult == IDOK) {
            PostQuitMessage(0);  // ������ �޽��� ť�� ���� �޽����� ���� ���� ����
        }
    }
    else if (isGameClear) {
        int msgBoxResult = MessageBox(NULL, L"���� Ŭ����!", L"�����մϴ�!", MB_OK | MB_ICONINFORMATION);  // ���� Ŭ���� �޽��� �ڽ�

        // Ȯ�� ��ư�� ������ ���� ����
        if (msgBoxResult == IDOK) {
            PostQuitMessage(0);  // ������ �޽��� ť�� ���� �޽����� ���� ���� ����
        }
    }
}
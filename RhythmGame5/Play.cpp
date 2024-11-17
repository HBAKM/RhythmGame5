#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include "play.h"
#include "Note.h"
#include "Parser.h"

#pragma comment(lib, "winmm.lib") // Windows 멀티미디어 라이브러리 링크

extern std::vector<Note> notes; // 외부에서 정의된 Note 벡터

int life = 10; // 초기 생명
bool isGameOver = false; // 게임 종료 상태
bool isGameClear = false; // 게임 클리어 상태

std::chrono::steady_clock::time_point startTime; // 게임 시작 시간

// 게임 초기화 함수
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! 파일에서 노트 로드
    PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // 배경 음악 재생
    startTime = std::chrono::steady_clock::now(); // 시작 시간 기록
}

// 게임이 시작된 후 경과 시간을 밀리초 단위로 반환하는 함수
int GetElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return static_cast<int>(duration.count());
}

// 게임 업데이트 함수 (노트 업데이트 및 게임 상태 체크)
void UpdateGame(HWND hWnd) {
    int currentTime = GetElapsedTime();
    UpdateNotes(currentTime);

    // 생명이 다 되었을 때 게임 오버
    if (life <= 0) {
        isGameOver = true;
        std::cout << "게임 오버!" << std::endl;
    }

    InvalidateRect(hWnd, NULL, TRUE); // 화면 갱신
}

// 게임 화면을 그리는 함수 정의
void DrawGame(HDC hdc, RECT& rcPaint) {
    // 노트 그리기
    for (Note& note : notes) {
        note.Draw(hdc);
    }

    // 생명바 그리기
    int maxLife = 10;
    int lifeWidth = 200;
    int lifeHeight = 20;
    int currentLifeWidth = static_cast<int>((static_cast<float>(life) / maxLife) * lifeWidth);

    // 배경 색상
    RECT bgRect = { 10, 10, 10 + lifeWidth, 10 + lifeHeight };
    HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
    FillRect(hdc, &bgRect, hBrush);
    DeleteObject(hBrush);

    // 현재 생명 색상
    RECT lifeRect = { 10, 10, 10 + currentLifeWidth, 10 + lifeHeight };
    hBrush = CreateSolidBrush(RGB(0, 255, 0));
    FillRect(hdc, &lifeRect, hBrush);
    DeleteObject(hBrush);

    if (isGameOver) {
        std::wstring gameOverText = L"게임 오버!";
        TextOut(hdc, 250, 250, gameOverText.c_str(), gameOverText.length());
    }
    else if (isGameClear) {
        std::wstring gameClearText = L"게임 클리어!";
        TextOut(hdc, 250, 250, gameClearText.c_str(), gameClearText.length());
    }

    // 확인용 코드
    if (!PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC)) {
        std::cerr << "오디오 파일 재생 실패!" << std::endl;
    }
}
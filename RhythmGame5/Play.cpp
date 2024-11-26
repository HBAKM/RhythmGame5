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
#pragma comment(lib, "gdiplus.lib")  // GDI+ 라이브러리 링크
#pragma comment(lib, "winmm.lib") // Windows 멀티미디어 라이브러리 링크

extern std::vector<Note> notes; // 외부에서 정의된 Note 벡터

int life = 10; // 초기 생명
bool isGameOver = false; // 게임 종료 상태
bool isGameClear = false; // 게임 클리어 상태

// std::chrono: 현재시간 가져오기
// steady_clock: 경과 시간(Elapsed Time)을 계산
// time_point: 특정시점
extern std::chrono::steady_clock::time_point startTime; // 게임 시작 시간

// 게임 초기화 함수
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! 파일에서 노트 로드
   // PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // 배경 음악 재생
    startTime = std::chrono::steady_clock::now(); // 현재 시간을 시작시간으로 (게임 시작시간)
}

// 게임 업데이트 함수 (노트 업데이트 및 게임 상태 체크)
void UpdateGame(HWND hWnd) {
    int currentTime = GetElapsedTime();

    // 타이밍에 맞지 않으면 노트 제거
    notes.erase(std::remove_if(notes.begin(), notes.end(),
        [currentTime](const Note& note) {
            return currentTime > note.startTime + 500; // 타이밍 초과
        }),
        notes.end());

    // 생명이 다 되었을 때 게임 오버
    if (life <= 0) {
        isGameOver = true;
    }

    InvalidateRect(hWnd, NULL, TRUE); // 화면 갱신
}

void HandleHit(Note& note) {
    // 노트를 정확히 맞췄을 때
    if (!note.isHit) {  // 이미 맞은 노트는 처리하지 않음
        note.isHit = true;  // 노트를 맞췄다고 표시
        notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());  // 벡터에서 제거

        if (life < 10) {
            life++;  // 맞췄을 때 생명 증가
        }
    }
    else {
        if (life > 0) {
            life--;  // 생명 1 감소
        }
    }
}

// 타이밍에 맞는지 확인하는 함수
bool IsNoteHit(const Note& note, WPARAM keyPressed, POINT mousePos) {
    // 노트가 히트 가능한 타이밍에 있는지 확인
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    return false;
}

// 게임 화면을 그리는 함수 정의
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd, int currentTime) {

    /// GDI+ 초기화
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;  // GDI+ 토큰 변수
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 이미지 로드
    Gdiplus::Image image(L"background_dark.bmp");
    /*if (image.GetLastStatus() != Gdiplus::Ok) {
        MessageBox(hWnd, L"이미지를 로드하는 데 실패했습니다.", L"오류", MB_OK | MB_ICONERROR);
    }*/

    // 창 크기 가져오기
    RECT rect;
    GetClientRect(hWnd, &rect);

    // 화면 크기에 따른 스케일 계산
    // scaleX, scaleY: 스케일을 조정할 비율. 게임 화면 크기에 따라 노트의 크기를 조절 
    float scaleX = static_cast<float>(rect.right) / 512.0f;  // osu! 기본 너비
    float scaleY = static_cast<float>(rect.bottom) / 384.0f; // osu! 기본 높이

    // GDI+로 배경 이미지 그리기
    Gdiplus::Graphics graphics(hdc);
    graphics.DrawImage(&image, 0, 0, rect.right, rect.bottom);

    // 생명바 그리기
    int maxLife = 10;
    int lifeWidth = 400;
    int lifeHeight = 20;
    int currentLifeWidth = static_cast<int>((static_cast<float>(life) / maxLife) * lifeWidth);

    // 현재 생명 색상
    RECT lifeRect = { 10, 10, 10 + currentLifeWidth, 10 + lifeHeight };
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    FillRect(hdc, &lifeRect, hBrush);
    DeleteObject(hBrush);

  /*  std::wstring msg = L"노트 벡터 크기: " + std::to_wstring(notes.size());
    MessageBox(hWnd, msg.c_str(), L"디버깅 메시지", MB_OK);*/
    
    if (notes.empty()) {
        OutputDebugString(L"노트가 없습니다.\n"); // 디버깅용
    }

    // 노트 그리기
    for (auto& note : notes) {
        // 노트의 등장 시간이 현재 시간보다 작거나 같으면 화면에 그린다
        if (note.startTime <= currentTime) {
            // 노트 그리기
            void DrawGame(HDC hdc, RECT & rcPaint, HWND hWnd, int currentTime);
        }

        // 텍스트 색상, 크기 등 스타일을 설정할 수 있습니다.
        if (isGameOver) {
            std::wstring gameOverText = L"게임 오버!";
            SetTextColor(hdc, RGB(255, 0, 0));  // 빨간색 텍스트
            SetBkMode(hdc, TRANSPARENT);  // 배경 없이 텍스트만 그리기
            TextOut(hdc, 250, 250, gameOverText.c_str(), gameOverText.length());
        }
        else if (isGameClear) {
            std::wstring gameClearText = L"게임 클리어!";
            SetTextColor(hdc, RGB(0, 255, 0));  // 초록색 텍스트
            SetBkMode(hdc, TRANSPARENT);  // 배경 없이 텍스트만 그리기
            TextOut(hdc, 250, 250, gameClearText.c_str(), gameClearText.length());
        }
    }
}
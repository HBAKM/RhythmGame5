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
#pragma comment(lib, "gdiplus.lib")  // GDI+ 라이브러리 링크
#pragma comment(lib, "winmm.lib") // Windows 멀티미디어 라이브러리 링크

extern std::vector<std::pair<int, double>> timingPoints; // 타이밍 포인트
extern std::vector<Note> notes; // 외부에서 정의된 Note 벡터

int life = 10; // 초기 생명
bool isGameOver = false; // 게임 종료 상태
bool isGameClear = false; // 게임 클리어 상태

// std::chrono: 현재시간 가져오기
// steady_clock: 경과 시간(Elapsed Time)을 계산
// time_point: 특정시점
std::chrono::steady_clock::time_point gameStartTime;
int gameStartTimeMillis;

// 게임 초기화 함수
void InitializeGame() {
    LoadNotes("Stellar.osu"); // osu! 파일에서 노트 로드
    //PlaySound(TEXT("audio.wav"), 0, SND_FILENAME | SND_ASYNC); // 배경 음악 재생
    gameStartTime = std::chrono::steady_clock::now(); // 게임시간 시간 측정 시작
    // 경과된 시간을 밀리초 단위로 변환
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(gameStartTime.time_since_epoch());
    // 밀리초 값을 인티저로 저장
    int gameStartTimeMillis = duration.count();
}

// 마우스 위치 받아오는 함수
void ProcessMouseHit(POINT mousePos) {
    // 마우스 위치에서 히트를 처리하는 코드
    for (auto& note : notes) {
        HandleHit(note, mousePos); // 히트 여부를 처리
    }
}

void HandleHit(Note& note, POINT mousePos) {
    // 슬라이더 노트일 경우
    if (note.type == 2) {
        // 슬라이더의 모든 점에 대해 확인
        for (size_t i = 0; i < note.sliderPoints.size(); ++i) {
            if (IsPointNearCircle(mousePos, note.sliderPoints[i])) {
                // 맞으면 생명 증가
                if (life < 10) {
                    life++;  // 맞췄을 때 생명 증가
                }
            }
            else {
                // 틀렸으면 생명 감소
                if (life > 0) {
                    life--;  // 틀리면 생명 1 감소
                }
            }
        }
    }
    else { // 일반 노트일 경우
        if (IsNoteHit(note, mousePos)) {
            // 맞으면 생명 증가
            if (life < 10) {
                life++;  // 맞췄을 때 생명 증가
            }
        }
        else {
            // 틀렸으면 생명 감소
            if (life > 0) {
                life--;  // 틀리면 생명 1 감소
            }
        }
    }
}

// 원형 노트가 마우스 위치를 히트했는지 확인하는 함수
bool IsNoteHit(const Note& note, const POINT& mousePos) {
    if (note.type == 1) {  // 원형 노트
        // 원의 반지름을 지정
        int radius = 20;

        // 마우스 위치와 노트의 위치 간의 거리 계산
        int dx = mousePos.x - note.x;
        int dy = mousePos.y - note.y;

        // 거리 계산
        int distanceSquared = dx * dx + dy * dy;
        int radiusSquared = radius * radius;

        // 마우스 위치가 원 안에 있으면 히트
        return distanceSquared <= radiusSquared;
    }
    else if (note.type == 2) {  // 슬라이더 노트
        // 슬라이더는 여러 점을 가지므로, 각 점에 대해 거리 계산
        for (size_t i = 0; i < note.sliderPoints.size() - 1; ++i) {
            // 두 점을 잇는 선분에 대한 거리 계산
            POINT sliderPoint = note.sliderPoints[i];

            // 슬라이더 점에 대한 원이 마우스와 맞는지 확인
            if (IsPointNearCircle(mousePos, sliderPoint)) {
                return true;
            }
        }
    }

    return false;  // 기본적으로 히트하지 않음
}

// 원형 점에 가까운지 확인하는 함수
bool IsPointNearCircle(const POINT& mousePos, const POINT& circlePos, int radius) {
    // 마우스와 원 사이의 거리가 지정된 반지름 이내인지 확인
    int dx = mousePos.x - circlePos.x;
    int dy = mousePos.y - circlePos.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// 게임 화면을 그리는 함수 정의
void DrawGame(HDC hdc, RECT& rcPaint, HWND hWnd) {

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

    //// 화면 크기에 따른 스케일 계산
    //// scaleX, scaleY: 스케일을 조정할 비율. 게임 화면 크기에 따라 노트의 크기를 조절 
    //float scaleX = static_cast<float>(rect.right) / 512.0f;  // osu! 기본 너비
    //float scaleY = static_cast<float>(rect.bottom) / 384.0f; // osu! 기본 높이

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
        // 노트의 좌표 계산
        int screenX = note.x;  // 노트의 X 좌표
        int screenY = note.y;  // 노트의 Y 좌표

        // 타이밍 포인트와 노트 리스트를 비교하여 노트의 시작 시간을 맞추
        for (const auto& timingPoint : timingPoints) {
            int time = timingPoint.first;
            double bpm = timingPoint.second;

            if (note.startTime >= time) {
                note.startTime = note.startTime + (60000.0 / bpm) * (note.startTime - time);
            }
        }

        if (gameStartTimeMillis == note.startTime) {
            note.Draw(hdc, screenX, screenY);  // 노트 그리기
        }

    }

    if (life <= 0) {
        isGameOver = true;
    }
    
    // 게임오버, 게임클리어 메시지박스
    if (isGameOver) {
        int msgBoxResult = MessageBox(NULL, L"게임 오버!", L"게임 종료", MB_OK | MB_ICONERROR);  // 게임 오버 메시지 박스

        // 확인 버튼을 누르면 게임 종료
        if (msgBoxResult == IDOK) {
            PostQuitMessage(0);  // 윈도우 메시지 큐에 종료 메시지를 보내 게임 종료
        }
    }
    else if (isGameClear) {
        int msgBoxResult = MessageBox(NULL, L"게임 클리어!", L"축하합니다!", MB_OK | MB_ICONINFORMATION);  // 게임 클리어 메시지 박스

        // 확인 버튼을 누르면 게임 종료
        if (msgBoxResult == IDOK) {
            PostQuitMessage(0);  // 윈도우 메시지 큐에 종료 메시지를 보내 게임 종료
        }
    }
}
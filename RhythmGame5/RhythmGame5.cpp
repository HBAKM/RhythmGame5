// RhythmGame5.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "RhythmGame5.h"
#include "Play.h"
#include "Note.h"
#include "Parser.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RHYTHMGAME5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RHYTHMGAME5));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RHYTHMGAME5));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RHYTHMGAME5);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// 노트 히트 여부를 확인하는 함수
bool IsMouseOverNote(const POINT& mousePos, const Note& note) {
    // 노트가 원 형태일 경우 (타입이 1인 경우)
    if (note.type == 1) {
        int radius = 20;  // 원 반지름 (기본 값)
        int dx = mousePos.x - note.x;
        int dy = mousePos.y - note.y;
        return (dx * dx + dy * dy <= radius * radius);  // 원 안에 있으면 true 반환
    }
    // 슬라이더 형태일 경우 (타입이 2인 경우)
    else if (note.type == 2) {
        // 슬라이더는 여러 개의 점으로 정의되므로, 슬라이더 경로를 따라 마우스가 있으면 히트 처리
        for (size_t i = 0; i < note.sliderPoints.size() - 1; ++i) {
            POINT start = note.sliderPoints[i];
            POINT end = note.sliderPoints[i + 1];

            // 선분에 대해 가까운 지점인지 확인
            float dx = end.x - start.x;
            float dy = end.y - start.y;
            float length = sqrt(dx * dx + dy * dy);
            float t = ((mousePos.x - start.x) * dx + (mousePos.y - start.y) * dy) / (length * length);

            // t 값이 0과 1 사이일 때는 선분 위에 가까운 점이 있다는 의미
            if (t >= 0 && t <= 1) {
                float closestX = start.x + t * dx;
                float closestY = start.y + t * dy;
                int distX = mousePos.x - closestX;
                int distY = mousePos.y - closestY;
                int distanceSquared = distX * distX + distY * distY;
                if (distanceSquared <= 25 * 25) { // 일정 거리 내에 있으면 히트
                    return true;
                }
            }
        }
    }
    return false;  // 그 외에는 히트하지 않음
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

HWND hButton;         // '시작하기' 버튼 핸들
bool isPlaying = false; // 게임 진행 여부 플래그
static bool isHomeScreen = true; // 홈 화면 표시 여부를 나타내는 플래그

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            // 버튼의 크기 구하기 (너비 200px, 높이 50px)
            int buttonWidth = 200;
            int buttonHeight = 50;

            // 창의 크기 구하기
            RECT rect;
            GetClientRect(hWnd, &rect); // 클라이언트 영역의 크기 얻기

            // 창의 가운데 좌표 계산
            int x = (rect.right - rect.left - buttonWidth) / 2; // 수평 가운데
            int y = (rect.bottom - rect.top - buttonHeight) / 2 + 200; // 수직 가운데보다 아래로 이동

            // '시작하기' 버튼 만들기
            hButton = CreateWindow(L"BUTTON", L"시작하기", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                x, y, buttonWidth, buttonHeight, hWnd, (HMENU)1, (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE), NULL);
        }
        break;
    case WM_COMMAND:
        {
            if (LOWORD(wParam) == 1) { // '시작하기' 버튼 클릭
                ShowWindow(hButton, SW_HIDE); // 버튼 숨기기
                isPlaying = true; // 게임 시작
                isHomeScreen = false; // 홈 화면 비활성화
                InvalidateRect(hWnd, NULL, TRUE); // 화면 무효화
                InitializeGame();
            }

        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (isPlaying) {
                DrawGame(hdc, ps.rcPaint, hWnd);
            }

            if (isHomeScreen) {
                // 배경 이미지 로드
                HBITMAP hBackground = (HBITMAP)LoadImage(NULL, L"background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBackground == NULL) {
                    MessageBox(hWnd, L"배경 이미지를 로드하는데 실패했습니다.", L"오류", MB_OK | MB_ICONERROR);
                }
                // 배경 그리기
                HDC hMemDC = CreateCompatibleDC(hdc);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackground);

                BITMAP bmp;
                GetObject(hBackground, sizeof(BITMAP), &bmp);

                // 클라이언트 영역 크기를 가져와 창 크기에 맞게 설정
                RECT rect;
                GetClientRect(hWnd, &rect);
                int windowWidth = rect.right;
                int windowHeight = rect.bottom;

                // StretchBlt를 사용하여 배경을 창 크기에 맞게 그리기
                StretchBlt(hdc, 0, 0, windowWidth, windowHeight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

                // 정리
                SelectObject(hMemDC, hOldBitmap);
                DeleteDC(hMemDC);
                DeleteObject(hBackground);
                // 
                // 게임 제목 및 설명 텍스트 그리기
                // 배경을 투명하게 설정
                SetBkMode(hdc, TRANSPARENT); // 투명 배경으로 설정

                HFONT hFont = CreateFont(100, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
                HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

                // 텍스트를 중앙에 위치시키기 위한 좌표 계산
                int textX = (windowWidth - 700) / 2;
                int textY = (windowHeight - 100) / 2 - 200;  // 수직 중앙보다 위로 이동

                RECT textRect = { textX, textY, textX + 700, textY + 100 };

                // 그림자 텍스트 그리기 (폰트 크기를 키운 뒤, 위치를 이동시켜 그리기)
                RECT shadowRect = textRect;
                OffsetRect(&shadowRect, 3, 3);  // 약간 오른쪽 아래로 이동
                SetTextColor(hdc, RGB(1, 45, 106)); // 그림자 색상 설정
                DrawText(hdc, L"RhythmGame", -1, &shadowRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                // 텍스트 그리기
                SetTextColor(hdc, RGB(255, 250, 240)); // 텍스트 색상 설정
                DrawText(hdc, L"RhythmGame", -1, &textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                // 정리
                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);

                // 게임 방법 텍스트 넣기
                HFONT instructionFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
                HFONT oldFont = (HFONT)SelectObject(hdc, instructionFont);

                // 게임 방법 텍스트 위치 설정
                int instructionTextX = textX;
                int instructionTextY = textY + 200; // 제목 아래에 위치 조정
                RECT instructionRect = { instructionTextX, instructionTextY, instructionTextX + 700, instructionTextY + 50 };

                // 그림자 텍스트 그리기 (폰트 크기를 키운 뒤, 위치를 이동시켜 그리기)
                RECT instructionShadowRect = instructionRect;
                OffsetRect(&instructionShadowRect, 1, 1);  // 약간 오른쪽 아래로 이동
                SetTextColor(hdc, RGB(1, 45, 106)); // 그림자 색상 설정
                DrawText(hdc, L"< 'x' or 'z' + 마우스 > 로 적절한 타이밍에 맞춰 플레이하세요! 사운드 있음", -1, &instructionShadowRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                // 게임 방법 텍스트 그리기
                SetTextColor(hdc, RGB(255, 250, 240)); // 텍스트 색상 설정
                DrawText(hdc, L"< 'x' or 'z' + 마우스 > 로 적절한 타이밍에 맞춰 플레이하세요! 사운드 있음", -1, &instructionRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

                // 정리
                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);
                SelectObject(hdc, oldFont);
                DeleteObject(instructionFont);
                DeleteObject(instructionFont);
            }
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE) {
                SendMessage(hWnd, WM_CLOSE, 0, 0); // ESC 키로 게임 종료
            }
            else if (wParam == 'X' || wParam == 'Z') {
                // 키 입력에 따라 노트를 확인
                for (auto& note : notes) {
                    // 이 부분에서 노트의 타이밍을 확인하여 맞는 노트를 찾음
                    if (IsNoteHit(note, wParam)) {
                        HandleHit(note);  // 맞은 노트 처리
                        break; // 하나의 노트만 처리하면 되므로 바로 종료
                    }
                }
            }
        }
        break;       
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>

// 노트 벡터 정의
std::vector<Note> notes;

// 노트 로드 함수
void LoadNotes(const std::string& filename) {
    ParseOsuFile(filename);  // osu! 파일을 파싱하여 노트를 로드
}

// 노트 업데이트 함수
void UpdateNotes(int currentTime) {
    for (auto& note : notes) {
        if (currentTime >= note.startTime) {
            // 노트가 지정된 시작 시간에 도달했을 때 처리
            std::cout << "노트 시간: " << note.startTime << " 현재 시간: " << currentTime << std::endl;
            // 추가적인 노트 처리 로직을 여기에 작성 (예: 노트 점수 처리 등)
        }
    }
}

// 노트 그리기 함수
void DrawNotes(HDC hdc, int currentTime) {
    for (const auto& note : notes) {
        note.Draw(hdc);  // 전달된 HDC를 사용하여 노트 그리기
    }
}
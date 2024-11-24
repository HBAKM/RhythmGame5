#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

extern std::vector<Note> notes; 

// osu! 파일을 파싱하여 노트를 로드하는 함수
void ParseOsuFile(const std::string& filename) {
    // 파일 열기
    std::ifstream file(filename);
    if (!file.is_open()) {
        MessageBox(NULL, L"파일을 열 수 없습니다.", L"파일 열기 오류", MB_OK | MB_ICONERROR);
        return;  // 파일 열기 실패 시 종료
    }

    std::string line;

    // 파일을 한 줄씩 읽어서 파싱
    bool inHitObjectsSection = false;

    while (std::getline(file, line)) {
        // 빈 줄 무시
        if (line.empty() || line[0] == '\n') {
            continue;
        }

        // '[HitObjects]' 섹션을 찾으면, 노트 데이터를 읽기 시작
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;  // '[HitObjects]' 건너뛰기
        }

        // '[HitObjects]' 섹션에 들어갔으면 노트 정보 파싱
        if (inHitObjectsSection) {
            // 각 라인에서 쉼표로 구분된 값 추출
            std::stringstream ss(line);
            std::string segment;
            int x, y, startTime, type, hitSound;

            // 쉼표로 구분된 값을 읽음
            std::getline(ss, segment, ',');
            x = std::stoi(segment); // x 좌표

            std::getline(ss, segment, ',');
            y = std::stoi(segment); // y 좌표

            std::getline(ss, segment, ',');
            startTime = std::stoi(segment); // 시작 시간

            std::getline(ss, segment, ',');
            type = std::stoi(segment); // 타입 (1: 클릭, 2: 슬라이드 등)

            std::getline(ss, segment, ',');
            hitSound = std::stoi(segment); // 히트 사운드

            // 슬라이더 노트 파싱
            if (type == 2) {
                // 슬라이더 데이터는 쉼표 뒤의 슬라이더 궤적 정보를 포함
                std::getline(ss, segment, ',');  // 슬라이더 궤적 데이터 읽기
                std::stringstream sliderStream(segment);  // 슬라이더 데이터를 파싱하기 위한 스트림 생성
                std::vector<POINT> sliderPoints;  // 슬라이더 궤적을 저장할 벡터
                std::string point;  // 슬라이더 궤적의 각 점을 나타내는 문자열
                while (std::getline(sliderStream, point, '|')) {
                    size_t sep = point.find(':');
                    if (sep != std::string::npos) {
                        // x와 y 좌표를 각각 파싱
                        int px = std::stoi(point.substr(0, sep));
                        int py = std::stoi(point.substr(sep + 1));

                        // 파싱한 좌표를 POINT 구조체로 저장
                        sliderPoints.push_back({ px, py });
                    }
                }
                // 슬라이더 노트를 벡터에 추가, 슬라이더 궤적 포함
                notes.push_back({ startTime, x, y, type, sliderPoints });
            }
            else {
                // 슬라이더가 아닌 일반 노트는 기본 정보를 저장
                notes.push_back({ startTime, x, y, type });
            }
        }

    }

    file.close();
}

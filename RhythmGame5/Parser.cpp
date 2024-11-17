#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

// osu! 파일을 파싱하여 노트를 로드하는 함수
void ParseOsuFile(const std::string& filename) {
    // 파일 열기
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "파일 열기 실패 " << filename << std::endl;
        return;
    }

    // 파일을 한 줄씩 읽어서 파싱
    bool inHitObjectsSection = false;

    while (std::getline(file, line)) {
        // '[HitObjects]' 섹션을 찾으면, 노트 데이터를 읽기 시작
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;  // '[HitObjects]' 섹션은 건너뛰기
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

            // 노트를 벡터에 추가
            notes.push_back({ startTime, x, y, type });
        }
    }

    file.close();
}

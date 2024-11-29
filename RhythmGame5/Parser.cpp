#include "Parser.h"
#include "Note.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility> // pair : 두 개의 값을 보관하는 클래스

extern std::vector<Note> notes;

//int: 타이밍 포인트의 시간(ms 단위).
//double: 해당 타이밍 포인트에서 적용되는 BPM 정보.
std::vector<std::pair<int, double>> timingPoints; // 타이밍 포인트를 저장할 벡터

// if (_Ptr == _Eptr) {_Xinvalid_argument("invalid stoi argument");} ????
int safe_stoi(const std::string& str) {
    try {
        // 공백이나 비어있는 문자열을 처리
        if (str.empty()) {
            OutputDebugString(L"공백이나 비어있는 문자열");
            return 0;
        }
        return std::stoi(str);
    }
    catch (const std::invalid_argument& e) {
        // 예외 발생 시 기본값을 반환하거나 에러 메시지를 출력할 수 있습니다.
        OutputDebugString(L"예외 발생");
        return 0;  // 기본값 0을 반환
    }
    catch (const std::out_of_range& e) {
        // 숫자가 너무 크거나 작아서 범위를 벗어나는 경우
        OutputDebugString(L"숫자가 너무 크거나 작아서 범위를 벗어남");
        return 0;
    }
}

// osu! 파일을 파싱하여 노트를 로드하는 함수
void ParseOsuFile(const std::string& filename) {
    // 파일 열기
    std::ifstream file(filename);
    if (!file.is_open()) {
        MessageBox(NULL, L"파일을 열 수 없습니다.", L"파일 열기 오류", MB_OK | MB_ICONERROR);
        return;  // 파일 열기 실패 시 종료
    }

    std::string line;
    bool inHitObjectsSection = false;
    bool inTimingPointsSection = false;

    // 파일을 한 줄씩 읽어서 파싱
    while (std::getline(file, line)) {
        // 빈 줄 무시
        if (line.empty() || line[0] == '\n') {
            continue;
        }

        // [TimingPoints] 섹션
        if (line.find("[TimingPoints]") != std::string::npos) {
            inTimingPointsSection = true;
            continue;
        }

        // [HitObjects] 섹션
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;
        }

        // [TimingPoints] 섹션에서 타이밍 포인트 읽기
        if (inTimingPointsSection && !line.empty()) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> values;

            // ',' 구분자로 각 값을 추출
            while (std::getline(ss, segment, ',')) {
                values.push_back(segment);
            }

            if (values.size() < 2) {
                continue;
            }

            int time = safe_stoi(values[0]);     // 타이밍 포인트 시간
            double bpm = std::stod(values[1]);   // BPM (부호가 있으면 BPM 변경)

            // BPM 값이 음수이면 속도 변경
            if (bpm < 0) {
                bpm = 60000.0 / (-bpm);  // 음수일 경우 BPM으로 변환
            }

            timingPoints.push_back({ time, bpm });

            if (values.size() < 8 || values[7] == "0") {
                continue;
            }
        }

        // [HitObjects] 섹션에서 노트 정보 읽기
        if (inHitObjectsSection && !line.empty()) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> values;

            // ',' 구분자로 각 값을 추출
            while (std::getline(ss, segment, ',')) {
                values.push_back(segment);
            }

            // 값이 부족한 경우 건너뛰기
            if (values.size() < 6) {
                continue;
            }

            // 안전하게 stoi 호출
            int x = safe_stoi(values[0]);       // x 좌표
            int y = safe_stoi(values[1]);       // y 좌표
            int startTime = safe_stoi(values[2]);    // 시작 시간
            int type = safe_stoi(values[3]);    // 타입 (1: 원형, 2: 슬라이더 등)

            // 슬라이더 길이
            double sliderLength = (type & 0b10) ? std::stod(values[5]) : 0.0;

            // 타이밍 포인트에 맞게 startTime을 계산
            double currentBPM = 0.0;
            for (const auto& tp : timingPoints) {
                if (startTime >= tp.first) {
                    currentBPM = tp.second;
                }
                else {
                    break;
                }
            }

            // 슬라이더 속도 계산
            double sliderVelocity = 1.0; // 기본 속도
            if (!timingPoints.empty() && currentBPM > 0.0) {
                sliderVelocity = currentBPM; // 현재 BPM 사용
            }

            // 슬라이더의 종료 시간 계산
            int endTime = startTime;
            if (type & 0b10) { // 슬라이더일 경우 (비트 플래그라서 0b10)
                double beatDuration = 60000.0 / currentBPM;
                double sliderDuration = (sliderLength / sliderVelocity) * beatDuration;
                endTime = static_cast<int>(startTime + sliderDuration);
            }

            // 슬라이더 노트일 경우
            if (type & 0b10) {
                std::vector<POINT> sliderPoints;

                // 슬라이더 데이터 처리 ('|' 구분자 기준)
                std::stringstream sliderStream(values[5]);
                std::string sliderPoint;
                while (std::getline(sliderStream, sliderPoint, '|')) {
                    // ':' 구분자 기준으로 각 점의 좌표 추출
                    std::stringstream pointStream(sliderPoint);
                    std::string coord;
                    std::vector<std::string> coords;
                    while (std::getline(pointStream, coord, ':')) {
                        coords.push_back(coord);
                    }

                    if (coords.size() == 2) {
                        POINT p = { std::stoi(coords[0]), std::stoi(coords[1]) };
                        sliderPoints.push_back(p);
                    }
                }

                // 슬라이더 노트 추가
                notes.emplace_back(startTime, x, y, 2, sliderPoints, endTime);
            }
            else {
                // 원형 노트 추가
                notes.emplace_back(startTime, x, y, 1);
            }
        }
    }

    file.close();
}
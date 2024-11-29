#include "Parser.h"
#include "Note.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <utility> // pair : �� ���� ���� �����ϴ� Ŭ����

extern std::vector<Note> notes;

//int: Ÿ�̹� ����Ʈ�� �ð�(ms ����).
//double: �ش� Ÿ�̹� ����Ʈ���� ����Ǵ� BPM ����.
std::vector<std::pair<int, double>> timingPoints; // Ÿ�̹� ����Ʈ�� ������ ����

// if (_Ptr == _Eptr) {_Xinvalid_argument("invalid stoi argument");} ????
int safe_stoi(const std::string& str) {
    try {
        // �����̳� ����ִ� ���ڿ��� ó��
        if (str.empty()) {
            OutputDebugString(L"�����̳� ����ִ� ���ڿ�");
            return 0;
        }
        return std::stoi(str);
    }
    catch (const std::invalid_argument& e) {
        // ���� �߻� �� �⺻���� ��ȯ�ϰų� ���� �޽����� ����� �� �ֽ��ϴ�.
        OutputDebugString(L"���� �߻�");
        return 0;  // �⺻�� 0�� ��ȯ
    }
    catch (const std::out_of_range& e) {
        // ���ڰ� �ʹ� ũ�ų� �۾Ƽ� ������ ����� ���
        OutputDebugString(L"���ڰ� �ʹ� ũ�ų� �۾Ƽ� ������ ���");
        return 0;
    }
}

// osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε��ϴ� �Լ�
void ParseOsuFile(const std::string& filename) {
    // ���� ����
    std::ifstream file(filename);
    if (!file.is_open()) {
        MessageBox(NULL, L"������ �� �� �����ϴ�.", L"���� ���� ����", MB_OK | MB_ICONERROR);
        return;  // ���� ���� ���� �� ����
    }

    std::string line;
    bool inHitObjectsSection = false;
    bool inTimingPointsSection = false;

    // ������ �� �پ� �о �Ľ�
    while (std::getline(file, line)) {
        // �� �� ����
        if (line.empty() || line[0] == '\n') {
            continue;
        }

        // [TimingPoints] ����
        if (line.find("[TimingPoints]") != std::string::npos) {
            inTimingPointsSection = true;
            continue;
        }

        // [HitObjects] ����
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;
        }

        // [TimingPoints] ���ǿ��� Ÿ�̹� ����Ʈ �б�
        if (inTimingPointsSection && !line.empty()) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> values;

            // ',' �����ڷ� �� ���� ����
            while (std::getline(ss, segment, ',')) {
                values.push_back(segment);
            }

            if (values.size() < 2) {
                continue;
            }

            int time = safe_stoi(values[0]);     // Ÿ�̹� ����Ʈ �ð�
            double bpm = std::stod(values[1]);   // BPM (��ȣ�� ������ BPM ����)

            // BPM ���� �����̸� �ӵ� ����
            if (bpm < 0) {
                bpm = 60000.0 / (-bpm);  // ������ ��� BPM���� ��ȯ
            }

            timingPoints.push_back({ time, bpm });

            if (values.size() < 8 || values[7] == "0") {
                continue;
            }
        }

        // [HitObjects] ���ǿ��� ��Ʈ ���� �б�
        if (inHitObjectsSection && !line.empty()) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> values;

            // ',' �����ڷ� �� ���� ����
            while (std::getline(ss, segment, ',')) {
                values.push_back(segment);
            }

            // ���� ������ ��� �ǳʶٱ�
            if (values.size() < 6) {
                continue;
            }

            // �����ϰ� stoi ȣ��
            int x = safe_stoi(values[0]);       // x ��ǥ
            int y = safe_stoi(values[1]);       // y ��ǥ
            int startTime = safe_stoi(values[2]);    // ���� �ð�
            int type = safe_stoi(values[3]);    // Ÿ�� (1: ����, 2: �����̴� ��)

            // �����̴� ����
            double sliderLength = (type & 0b10) ? std::stod(values[5]) : 0.0;

            // Ÿ�̹� ����Ʈ�� �°� startTime�� ���
            double currentBPM = 0.0;
            for (const auto& tp : timingPoints) {
                if (startTime >= tp.first) {
                    currentBPM = tp.second;
                }
                else {
                    break;
                }
            }

            // �����̴� �ӵ� ���
            double sliderVelocity = 1.0; // �⺻ �ӵ�
            if (!timingPoints.empty() && currentBPM > 0.0) {
                sliderVelocity = currentBPM; // ���� BPM ���
            }

            // �����̴��� ���� �ð� ���
            int endTime = startTime;
            if (type & 0b10) { // �����̴��� ��� (��Ʈ �÷��׶� 0b10)
                double beatDuration = 60000.0 / currentBPM;
                double sliderDuration = (sliderLength / sliderVelocity) * beatDuration;
                endTime = static_cast<int>(startTime + sliderDuration);
            }

            // �����̴� ��Ʈ�� ���
            if (type & 0b10) {
                std::vector<POINT> sliderPoints;

                // �����̴� ������ ó�� ('|' ������ ����)
                std::stringstream sliderStream(values[5]);
                std::string sliderPoint;
                while (std::getline(sliderStream, sliderPoint, '|')) {
                    // ':' ������ �������� �� ���� ��ǥ ����
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

                // �����̴� ��Ʈ �߰�
                notes.emplace_back(startTime, x, y, 2, sliderPoints, endTime);
            }
            else {
                // ���� ��Ʈ �߰�
                notes.emplace_back(startTime, x, y, 1);
            }
        }
    }

    file.close();
}
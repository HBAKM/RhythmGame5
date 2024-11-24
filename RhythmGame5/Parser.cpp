#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

extern std::vector<Note> notes; 

// osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε��ϴ� �Լ�
void ParseOsuFile(const std::string& filename) {
    // ���� ����
    std::ifstream file(filename);
    if (!file.is_open()) {
        MessageBox(NULL, L"������ �� �� �����ϴ�.", L"���� ���� ����", MB_OK | MB_ICONERROR);
        return;  // ���� ���� ���� �� ����
    }

    std::string line;

    // ������ �� �پ� �о �Ľ�
    bool inHitObjectsSection = false;

    while (std::getline(file, line)) {
        // �� �� ����
        if (line.empty() || line[0] == '\n') {
            continue;
        }

        // '[HitObjects]' ������ ã����, ��Ʈ �����͸� �б� ����
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;  // '[HitObjects]' �ǳʶٱ�
        }

        // '[HitObjects]' ���ǿ� ������ ��Ʈ ���� �Ľ�
        if (inHitObjectsSection) {
            // �� ���ο��� ��ǥ�� ���е� �� ����
            std::stringstream ss(line);
            std::string segment;
            int x, y, startTime, type, hitSound;

            // ��ǥ�� ���е� ���� ����
            std::getline(ss, segment, ',');
            x = std::stoi(segment); // x ��ǥ

            std::getline(ss, segment, ',');
            y = std::stoi(segment); // y ��ǥ

            std::getline(ss, segment, ',');
            startTime = std::stoi(segment); // ���� �ð�

            std::getline(ss, segment, ',');
            type = std::stoi(segment); // Ÿ�� (1: Ŭ��, 2: �����̵� ��)

            std::getline(ss, segment, ',');
            hitSound = std::stoi(segment); // ��Ʈ ����

            // �����̴� ��Ʈ �Ľ�
            if (type == 2) {
                // �����̴� �����ʹ� ��ǥ ���� �����̴� ���� ������ ����
                std::getline(ss, segment, ',');  // �����̴� ���� ������ �б�
                std::stringstream sliderStream(segment);  // �����̴� �����͸� �Ľ��ϱ� ���� ��Ʈ�� ����
                std::vector<POINT> sliderPoints;  // �����̴� ������ ������ ����
                std::string point;  // �����̴� ������ �� ���� ��Ÿ���� ���ڿ�
                while (std::getline(sliderStream, point, '|')) {
                    size_t sep = point.find(':');
                    if (sep != std::string::npos) {
                        // x�� y ��ǥ�� ���� �Ľ�
                        int px = std::stoi(point.substr(0, sep));
                        int py = std::stoi(point.substr(sep + 1));

                        // �Ľ��� ��ǥ�� POINT ����ü�� ����
                        sliderPoints.push_back({ px, py });
                    }
                }
                // �����̴� ��Ʈ�� ���Ϳ� �߰�, �����̴� ���� ����
                notes.push_back({ startTime, x, y, type, sliderPoints });
            }
            else {
                // �����̴��� �ƴ� �Ϲ� ��Ʈ�� �⺻ ������ ����
                notes.push_back({ startTime, x, y, type });
            }
        }

    }

    file.close();
}

#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

// osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε��ϴ� �Լ�
void ParseOsuFile(const std::string& filename) {
    // ���� ����
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "���� ���� ���� " << filename << std::endl;
        return;
    }

    // ������ �� �پ� �о �Ľ�
    bool inHitObjectsSection = false;

    while (std::getline(file, line)) {
        // '[HitObjects]' ������ ã����, ��Ʈ �����͸� �б� ����
        if (line.find("[HitObjects]") != std::string::npos) {
            inHitObjectsSection = true;
            continue;  // '[HitObjects]' ������ �ǳʶٱ�
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

            // ��Ʈ�� ���Ϳ� �߰�
            notes.push_back({ startTime, x, y, type });
        }
    }

    file.close();
}

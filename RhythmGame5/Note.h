#pragma once

#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <string>
#include <windows.h>
#include <chrono>

// ��Ʈ ����ü ����
struct Note {
    enum class Type {
        HitX, HitZ
    };

    int startTime; // ��Ʈ�� �����ϴ� �ð� (�и��� ����)
    int x;
    int y;
    int type;  // 1: ��, 2: �����̵�
    std::vector<POINT> sliderPoints;  // �����̵��� ������ ��Ÿ���� ����
    bool isHit;  // ��Ʈ�� �¾Ҵ��� ����

    // �⺻ ������
    Note(int startTime, int x, int y, int type)
        : startTime(startTime), x(x), y(y), type(type) {
    }

    // �����̴� ��Ʈ�� ���� ������
    Note(int startTime, int x, int y, int type, const std::vector<POINT>& sliderPoints)
        : startTime(startTime), x(x), y(y), type(type), sliderPoints(sliderPoints) {
    }

    // ��Ʈ �׸��� �Լ� (ȭ�� ��ǥ ���)
    void Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const;

};

// == ������ �����ε� �߰�
inline bool operator==(const Note& lhs, const Note& rhs) {
    return lhs.startTime == rhs.startTime &&
        lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.type == rhs.type;
}

// ���� ��Ʈ ����
extern std::vector<Note> notes;

// ���� ���� �ð� ���
extern std::chrono::steady_clock::time_point startTime;

// �Լ� ����
void LoadNotes(const std::string& filename);
void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness);
int GetElapsedTime();

#endif // NOTE_H

#pragma once

#ifndef NOTE_H
#define NOTE_H

#include <vector>
#include <string>
#include <windows.h>
#include <chrono>

// ��Ʈ ����ü ����
struct Note {
    int startTime; // ��Ʈ�� �����ϴ� �ð� (�и��� ����)
    int endTime;    // ��Ʈ ���� �ð� (�и��� ����)
    int x;  // x ��ǥ (ȭ�� ��ǥ)
    int y;  // y ��ǥ (ȭ�� ��ǥ)
    int type;  // 1: ���� ��Ʈ, 2: �����̴� ��Ʈ
    std::vector<POINT> sliderPoints;  // �����̴��� ������ ��Ÿ���� ����

    // �⺻ ������
    Note(int startTime, int x, int y, int type)
        : startTime(startTime), x(x), y(y), type(type) {
    }

    // �����̴� ��Ʈ�� ���� ������
    Note(int startTime, int x, int y, int type, const std::vector<POINT>& sliderPoints, int endTime)
        : startTime(startTime), x(x), y(y), type(type), sliderPoints(sliderPoints), endTime(endTime) {
    }

    // ��Ʈ �׸��� �Լ� (ȭ�� ��ǥ ���)
    void Draw(HDC hdc, int screenX, int screenY) const;

};

// ���� ��Ʈ ����
extern std::vector<Note> notes;

// ���� ���� �ð� ���
extern std::chrono::steady_clock::time_point gameStartTime;

// �Լ� ����
void LoadNotes(const std::string& filename);
void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness);

#endif // NOTE_H

#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <cmath>

// ��Ʈ ���� ����
std::vector<Note> notes;

// ��Ʈ �ε� �Լ�
void LoadNotes(const std::string& filename) {
    ParseOsuFile(filename);  // osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε�
}

void DrawCircleOutline(HDC hdc, int centerX, int centerY, int radius, int thickness) {
    // �׵θ� �� ���� (�β��� ����)
    HPEN hPen = CreatePen(PS_SOLID, thickness, RGB(0, 0, 0));  // ������ �׵θ�
    HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);      // ���� ���� �귯��

    // ���� GDI ��ü ����
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // �� �׸��� (�׵θ��� ǥ��)
    Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);

    // ���� GDI ��ü ����
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);

    // GDI ��ü ����
    DeleteObject(hPen);
}


// ��Ʈ �׸��� �Լ� (ȭ�� ��ǥ ���)
// screenX, screenY: ȭ�� ��ǥ. �� ��ǥ�� ��Ʈ�� �׸�
void Note::Draw(HDC hdc, int screenX, int screenY) const {
    if (type == 1) {
        // �� �׸��� (ũ�� ����)
        int radius = 20;  // ������ ������ ũ��
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {  // �����̴� ��Ʈ
        // �����̴��� ���� �ð� ���
        int sliderDuration = endTime - startTime; // �����̴��� ���� �ð�

        extern int gameStartTimeMillis;

        // �����̴��� ������ ����(progress)�� ��� (0~100)
        int progress = 0;
        if (gameStartTimeMillis >= startTime && gameStartTimeMillis <= endTime) {
            progress = (gameStartTimeMillis - startTime) * 100 / sliderDuration;
        }

        // �����̴� ������� 100% �̻��̸� �� �̻� �������� ����
        if (progress > 100) progress = 100;

        // ���� ��ġ ���: ù ��° ���� ������ �� ���̸� ����
        size_t numPoints = sliderPoints.size();
        size_t currentPointIndex = progress * (numPoints - 1) / 100;  // ���� ��( ������� �´� ��)�� �ε��� ���

        // �� �� ���̸� �����Ͽ� ���� ��ġ�� ���
        if (currentPointIndex < numPoints - 1) {
            POINT p1 = sliderPoints[currentPointIndex];
            POINT p2 = sliderPoints[currentPointIndex + 1];

            // ���� ���� (progress�� ���� �� ��ġ ���)
            int interpX = p1.x + (p2.x - p1.x) * (progress - currentPointIndex * 100) / 100;
            int interpY = p1.y + (p2.y - p1.y) * (progress - currentPointIndex * 100) / 100;

            // �̵��ϴ� �� �׸���
            int radius = 20;
            Ellipse(hdc, interpX - radius, interpY - radius, interpX + radius, interpY + radius);
        }

        // �����̴� ��� �׸��� (������ �׸���)
        for (size_t i = 0; i < sliderPoints.size() - 1; ++i) {
            POINT p1 = sliderPoints[i];
            POINT p2 = sliderPoints[i + 1];
            MoveToEx(hdc, p1.x, p1.y, NULL);
            LineTo(hdc, p2.x, p2.y);  // ���� �׸���
        }
    }
}

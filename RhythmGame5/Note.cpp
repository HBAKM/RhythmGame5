#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>

// ��Ʈ ���� ����
std::vector<Note> notes;

extern std::chrono::steady_clock::time_point startTime;

// ������ ���۵� �� ��� �ð��� �и��� ������ ��ȯ�ϴ� �Լ�
int GetElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return static_cast<int>(duration.count());
}

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
void Note::Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const {
    OutputDebugString(L"Note::Draw �Լ��� ȣ���\n");// ������
    if (type == 1) {
        // �� �׸��� (ũ�� �� ��ġ �����ϸ� ����)
        int radius = static_cast<int>(20 * scaleX);  // ������ ũ�� ����
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {
        // �����̴� (�񴰹��ó�� ���� ���� �׵θ� �� ��ġ)
        if (sliderPoints.size() > 1) {
            const int bubbleRadius = static_cast<int>(25 * scaleX);  // �����̴� �񴰹�� ũ��
            const int borderThickness = 2;                           // �׵θ� �β�
            const float bubbleSpacing = 20.0f;                       // �񴰹�� ���� ����

            // ������ ���� �̵��ϸ� �� ��ġ
            for (size_t i = 0; i < sliderPoints.size() - 1; ++i) {
                POINT start = sliderPoints[i];
                POINT end = sliderPoints[i + 1];

                // ȭ�� ��ǥ�� ��ȯ (������ ����)
                int startX = static_cast<int>(start.x * scaleX);
                int startY = static_cast<int>(start.y * scaleY);
                int endX = static_cast<int>(end.x * scaleX);
                int endY = static_cast<int>(end.y * scaleY);

                // �������� ���� ������ �Ÿ� ���
                float dx = endX - startX;
                float dy = endY - startY;
                float distance = sqrt(dx * dx + dy * dy);

                // ���� ���ݸ��� �� ��ġ
                for (float t = 0; t < distance; t += bubbleSpacing) {
                    float ratio = t / distance;  // ���� ����
                    int bubbleX = static_cast<int>(startX + ratio * dx);
                    int bubbleY = static_cast<int>(startY + ratio * dy);

                    // �׵θ� �񴰹�� �׸���
                    DrawCircleOutline(hdc, bubbleX, bubbleY, bubbleRadius, borderThickness);
                }
            }
        }
    }
}

void DrawNotes(HDC hdc, RECT rect, HWND hWnd) {
    int currentTime = GetElapsedTime();  // �и��� ������ ���� ��� �ð�

    for (const auto& note : notes) {
        if (currentTime >= note.startTime) {  // note.time�� note.startTime���� ����
            // ��Ʈ�� ��ǥ�� ����ϰ� �׸���
            int screenX = note.x;
            int screenY = note.y + (currentTime - note.startTime) * 0.1f;  // Y��ǥ�� �ð��� ���� �̵�
            note.Draw(hdc, screenX, screenY, 1.0f, 1.0f);  // Draw ȣ��
        }
    }
}
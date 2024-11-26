#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <chrono>

// ��Ʈ ���� ����
std::vector<Note> notes;

extern std::chrono::steady_clock::time_point startTime;

// std::chrono::duration_cast: �ð� ���� ��ȯ �Լ�
// current - startTime: ���� �ð��� ���� ���� �ð�(startTime)�� ����
// std::chrono::duration_cast<std::chrono::milliseconds>():
//      std::chrono::duration ��ü�� �и���(milliseconds) ������ ��ȯ. (duration_cast�� ����� ��ȯ)
// .count() :td::chrono::duration ��ü���� ���� ���� ���� -> currentTime�� ��

int currentTime = 0;  // ������ ���۵� ���� ����� �ð��� �и��� ������ ����

// ���� ���� ����
std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

// ��� �ð��� ����ϴ� �Լ�
int GetElapsedTime() {
    auto current = std::chrono::steady_clock::now(); // ���� �ð� ������
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - startTime).count();
    currentTime = static_cast<int>(elapsed); // currentTime�� ����
    return currentTime;
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
// screenX, screenY: ȭ�� ��ǥ. �� ��ǥ�� ��Ʈ�� �׸�
// scaleX, scaleY: �������� ������ ����. ���� ȭ�� ũ�⿡ ���� ��Ʈ�� ũ�⸦ ����
void Note::Draw(HDC hdc, int screenX, int screenY, float scaleX, float scaleY) const {
    if (type == 1) {
        // �� �׸��� (ũ�� �� ��ġ �����ϸ� ����)
        int radius = static_cast<int>(20 * scaleX);  // ������ ũ�� ����
        Ellipse(hdc, screenX - radius, screenY - radius, screenX + radius, screenY + radius);
    }
    else if (type == 2) {
        // �����̴� (���� ���� �׵θ� �� ��ġ)
        if (sliderPoints.size() > 1) { // sliderPoints �迭�� ���� �� �� �̻� �ִ��� Ȯ��
            const int bubbleRadius = static_cast<int>(25 * scaleX);  // �����̴� ũ��
            const int borderThickness = 2;                           // �׵θ� �β�
            const float bubbleSpacing = 20.0f;                       //  ����

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
                float distance = sqrt(dx * dx + dy * dy); // ��Ÿ����� ����

                // ���� ���ݸ��� �� ��ġ
                for (float t = 0; t < distance; t += bubbleSpacing) {
                    float ratio = t / distance;  // ���� ����
                    int bubbleX = static_cast<int>(startX + ratio * dx);
                    int bubbleY = static_cast<int>(startY + ratio * dy);

                    // �׵θ� �׸���
                    DrawCircleOutline(hdc, bubbleX, bubbleY, bubbleRadius, borderThickness);
                }
            }
        }
    }
}

void DrawNotes(HDC hdc, RECT rect, HWND hWnd, int currentTime){
    for (const auto& note : notes) {
        if (currentTime >= note.startTime) { 
            // ��Ʈ�� ��ǥ�� ����ϰ� �׸���
            int screenX = note.x;
            int screenY = note.y; 
            note.Draw(hdc, screenX, screenY, 1.0f, 1.0f);  // Draw ȣ��
        }
    }
}

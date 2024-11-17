#include "Note.h"
#include "Parser.h"
#include <windows.h>
#include <vector>
#include <iostream>

// ��Ʈ ���� ����
std::vector<Note> notes;

// ��Ʈ �ε� �Լ�
void LoadNotes(const std::string& filename) {
    ParseOsuFile(filename);  // osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε�
}

// ��Ʈ ������Ʈ �Լ�
void UpdateNotes(int currentTime) {
    for (auto& note : notes) {
        if (currentTime >= note.startTime) {
            // ��Ʈ�� ������ ���� �ð��� �������� �� ó��
            std::cout << "��Ʈ �ð�: " << note.startTime << " ���� �ð�: " << currentTime << std::endl;
            // �߰����� ��Ʈ ó�� ������ ���⿡ �ۼ� (��: ��Ʈ ���� ó�� ��)
        }
    }
}

// ��Ʈ �׸��� �Լ�
void DrawNotes(HDC hdc, int currentTime) {
    for (const auto& note : notes) {
        note.Draw(hdc);  // ���޵� HDC�� ����Ͽ� ��Ʈ �׸���
    }
}
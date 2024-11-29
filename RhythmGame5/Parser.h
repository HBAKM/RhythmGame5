#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Note.h"
#include <utility>

// ��Ʈ ����
extern std::vector<Note> notes;  

// �����̴��� ������ ��Ÿ���� ����
extern std::vector<POINT> sliderPoints;

// osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε��ϴ� �Լ� ����
void ParseOsuFile(const std::string& filename);

// Ÿ�̹� ����Ʈ�� ������ ����
extern std::vector<std::pair<int, double>> timingPoints;

#endif 

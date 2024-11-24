#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Note.h"

extern std::vector<Note> notes;  // ��Ʈ ����

// osu! ������ �Ľ��Ͽ� ��Ʈ�� �ε��ϴ� �Լ� ����
void ParseOsuFile(const std::string& filename);

#endif // PARSER_H

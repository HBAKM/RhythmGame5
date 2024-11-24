#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Note.h"

extern std::vector<Note> notes;  // 노트 벡터

// osu! 파일을 파싱하여 노트를 로드하는 함수 선언
void ParseOsuFile(const std::string& filename);

#endif // PARSER_H

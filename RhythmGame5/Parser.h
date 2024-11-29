#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Note.h"
#include <utility>

// 노트 벡터
extern std::vector<Note> notes;  

// 슬라이더의 궤적을 나타내는 점들
extern std::vector<POINT> sliderPoints;

// osu! 파일을 파싱하여 노트를 로드하는 함수 선언
void ParseOsuFile(const std::string& filename);

// 타이밍 포인트를 저장할 벡터
extern std::vector<std::pair<int, double>> timingPoints;

#endif 

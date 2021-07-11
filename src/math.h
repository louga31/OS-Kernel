#pragma once

#include <types.h>

struct Point64;

struct Point {
    uint32_t x = 0;
	uint32_t y = 0;
	Point() : x(0), y(0) {};
	Point(uint32_t x, uint32_t y) : x(x), y(y) {};
};

struct Point64 {
	int64_t x = 0;
	int64_t y = 0;
	Point64() : x(0), y(0) {};
	Point64(int64_t x, int64_t y) : x(x), y(y) {};
};
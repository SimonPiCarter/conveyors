#pragma once

#include "Line.hh"
#include "utils/line_handler.hh"

struct Sorter : public line_handler
{
	Sorter(size_t sorted_p) : line_handler(1, 2), innerLine(2), type(sorted_p) {}

	Line innerLine;

	godot::Vector2i position;
	size_t type = 0;

	bool toggled = true;
};

void load(Sorter & Sorter_p);
void unload(Sorter & Sorter_p);

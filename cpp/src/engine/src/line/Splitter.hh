#pragma once

#include "Line.hh"
#include "utils/line_handler.hh"

struct Splitter : public line_handler
{
	Splitter() : line_handler(1, 2), innerLine(2) {}

	/// @brief priority output
	size_t priority = 0;
	size_t non_priority = 1;

	Line innerLine;

	godot::Vector2i position;

	bool toggled = true;
};

void load(Splitter & splitter_p);
void unload(Splitter & splitter_p);

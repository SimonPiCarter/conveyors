#pragma once

#include "Line.hh"

struct Merger : public line_handler
{
	Merger() : line_handler(2, 1), innerLine(2) {}

	/// @brief priority input
	size_t priority = 0;
	size_t non_priority = 1;

	Line innerLine;

	godot::Vector2i position;

	bool toggled = true;
};

void load(Merger & merger_p);
void unload(Merger & merger_p);

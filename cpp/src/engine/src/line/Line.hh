#pragma once

#include <vector>
#include <list>
#include "grid/Position.hh"

struct ItemOnLine
{
	size_t idx = 0;
	/// @brief index to next element in the list
	size_t next = 0;
	unsigned long dist_to_next = 0;
};

struct Line
{
	Line(size_t capacity_p);

	std::vector<PositionLine> positions;
	Position const &get_start() const { return positions.begin()->start; }
	Position const &get_end() const { return positions.rbegin()->end; }

	std::vector<ItemOnLine> items;
	/// @brief list of free idx in the vector
	std::list<size_t> free_idx;
	/// @brief index of first item in the list
	size_t first = 0;
	/// @brief index of last item in the list
	size_t last = 0;
	unsigned long dist_start = 0;
	unsigned long dist_end = 0;
	unsigned long full_dist = 0;

	unsigned long speed = 50;
};

/// @brief performs a step on a given line
void step(Line &line_p);

bool can_add(Line const &line_p);
size_t can_consume(Line const &line_p);
bool add_to_start(Line &line_p, size_t item_p);
size_t consume(Line &line_p);

size_t get_content_size(Line const &line_p);

bool is_empty(Line const &line_p);

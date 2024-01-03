#pragma once

#include "line/Line.hh"
#include <list>
#include <vector>
#include <functional>

struct Factory;
struct Statistics;

void noop_factory(Factory &, size_t);
void score_factory(Statistics &stats_p, size_t good_p, Factory &, size_t idx_p);

struct Factory
{
	Factory(size_t duration_p) : duration(duration_p), max_buffer_size(5) {}

	std::vector<Line *> entries;
	std::vector<Line *> outputs;

	/// @brief buffered items
	std::list<size_t> buffer;

	/// @brief duration of the process
	size_t const duration = 0;
	size_t const max_buffer_size = 0;
	/// @brief current step into the process
	size_t step_count = 0;

	Position position;

	std::function<void(Factory &, size_t)> output_handler = noop_factory;
};

void load(Factory & factory_p);
void process(Factory & factory_p);

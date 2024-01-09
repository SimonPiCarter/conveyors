#pragma once

#include <vector>

struct Line;

struct line_handler
{
	line_handler(size_t in_p, size_t out_p);

	std::vector<Line *> in;
	std::vector<Line *> out;
	std::vector<size_t> connectors;
};

bool connect_input(line_handler & handler_p, Line & line_p);
bool connect_output(line_handler & handler_p, Line & line_p);

void disconnect(line_handler & handler_p, Line & line_p);

bool has_input_free(line_handler const & handler_p);
bool has_output_free(line_handler const & handler_p);

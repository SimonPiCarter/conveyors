#pragma once

#include <vector>

struct Line;

struct line_handler
{
	line_handler(size_t in_p, size_t out_p);

	std::vector<Line *> in;
	std::vector<Line *> out;
};

bool connect_input(line_handler & handler_p, Line & line_p);
bool connect_output(line_handler & handler_p, Line & line_p);

void disconnect(line_handler & handler_p, Line & line_p);
void disconnect_all(Line & line_p);

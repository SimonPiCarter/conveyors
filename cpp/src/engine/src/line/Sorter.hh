#pragma once

#include "Line.hh"

struct Sorter
{
	Sorter(size_t sorted_p) : innerLine(2), type(sorted_p) {}

	Line * entry = nullptr;
	Line * first = nullptr;
	Line * second = nullptr;

	Line innerLine;
	size_t type = 0;
};

void load(Sorter & Sorter_p);
void unload(Sorter & Sorter_p);

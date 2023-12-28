#pragma once

#include "Line.hh"

struct Splitter
{
	Splitter(size_t capacity_p) : innerLine(capacity_p) {}

	Line * entry = nullptr;
	Line * first = nullptr;
	Line * second = nullptr;

	Line innerLine;
};

void load(Splitter & splitter_p);
void unload(Splitter & splitter_p);

#pragma once

#include "Line.hh"

struct Splitter
{
	Splitter() : innerLine(2) {}

	Line * entry = nullptr;
	Line * first = nullptr;
	Line * second = nullptr;

	Line innerLine;
};

void load(Splitter & splitter_p);
void unload(Splitter & splitter_p);

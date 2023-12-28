#pragma once

#include "Line.hh"

struct Merger
{
	Merger() : innerLine(2) {}

	Line * output = nullptr;
	Line * first = nullptr;
	Line * second = nullptr;

	Line innerLine;
};

void load(Merger & merger_p);
void unload(Merger & merger_p);

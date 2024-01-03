#pragma once

#include "Line.hh"

struct Bridge
{
	Bridge(size_t capacity_p) : innerLine(capacity_p) {}

	Line * input = nullptr;
	Line * output = nullptr;

	Line innerLine;
};

void load(Bridge & bridge_p);
void unload(Bridge & bridge_p);

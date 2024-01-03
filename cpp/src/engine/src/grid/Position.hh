#pragma once
#include <godot_cpp/variant/vector2i.hpp>

struct Position
{
	Position() : x(0), y(0) {}
	Position(godot::Vector2i const &pos_p) : x(pos_p.x), y(pos_p.y) {}
	size_t x = 0;
	size_t y = 0;
};

struct PositionLine
{
	Position start;
	Position end;
};

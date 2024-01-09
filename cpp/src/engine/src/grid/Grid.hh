#pragma once

#include <vector>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/typed_array.hpp>

enum class CaseType
{
	OUT_OF_BOUND,
	FREE,
	BELT,
	SPLITTER,
	MERGER,
	SORTER,
	ENTRY,
	OUTPUT,
	FACTORY
};

struct Grid
{
	size_t width = 0;
	size_t height = 0;
	std::vector<CaseType> type;
	std::vector<size_t> indexes;

	void init_grid(size_t witdth_p, size_t height_p);

	CaseType get_case_type(size_t x, size_t y) const;
	size_t get_case_index(size_t x, size_t y) const;

	void set_case_type(size_t x, size_t y, CaseType type_p);
	void set_case_index(size_t x, size_t y, size_t idx_p);
};

bool check_line_in_grid(Grid const &grid_p, godot::TypedArray<godot::Vector2i> const &points_p);
void set_line_in_grid(Grid &grid_p, std::vector<godot::Vector2i> const &points_p, size_t idx_p);
void unset_line_in_grid(Grid &grid_p, std::vector<godot::Vector2i> const &points_p);
bool is_point_free_or_belt(Grid &grid_p, size_t x, size_t y);
bool is_point_free(Grid &grid_p, size_t x, size_t y);

bool is_handler(CaseType type_p);
#include "Grid.hh"
#include <godot_cpp/variant/utility_functions.hpp>


void Grid::init_grid(size_t width_p, size_t height_p)
{
	type.clear();
	indexes.clear();
	width = width_p;
	height = height_p;
	type.resize(width_p*height_p, CaseType::FREE);
	indexes.resize(width_p*height_p, 0);
}

CaseType Grid::get_case_type(size_t x, size_t y) const
{
	if(x < width && y < height)
	{
		return type[x*height+y];
	}
	return CaseType::OUT_OF_BOUND;
}

size_t Grid::get_case_index(size_t x, size_t y) const
{
	if(x < width && y < height)
	{
		return indexes[x*height+y];
	}
	return 0;
}

void Grid::set_case_type(size_t x, size_t y, CaseType type_p)
{
	godot::UtilityFunctions::print("setting not free : ", x,",", y);
	type[x*height+y] = type_p;
}

void Grid::set_case_index(size_t x, size_t y, size_t idx_p)
{
	indexes[x*height+y] = idx_p;
}

std::vector<std::pair<int, int> > getAllCoord(godot::TypedArray<godot::Vector2i> const &points_p)
{
	std::vector<std::pair<int, int> > result_l;
	godot::Vector2i old_point;
	bool first = true;
	for(size_t p = 0 ; p < points_p.size() ; ++p)
	{
		godot::Vector2i const &point = points_p[p];
		if(first)
		{
			first = false;
			old_point = point;
			continue;
		}
		int minimal = std::min<int>(point.y, old_point.y);
		int maximal = std::max<int>(point.y, old_point.y);
		bool x_mode = false;
		if(point.y == old_point.y)
		{
			minimal = std::min<int>(point.x, old_point.x);
			maximal = std::max<int>(point.x, old_point.x);
			x_mode = true;
		}
		for(int i = minimal ; i <= maximal ; ++i)
		{
			if(x_mode)
			{
				result_l.push_back({i, point.y});
			}
			else
			{
				result_l.push_back({point.x, i});
			}
		}
		old_point = point;
	}

	return result_l;
}

bool check_line_in_grid(Grid const &grid_p, godot::TypedArray<godot::Vector2i> const &points_p)
{
	godot::UtilityFunctions::print("check_line_in_grid : ");
	std::vector<std::pair<int, int> > result_l = getAllCoord(points_p);
	for(auto &&pair_l : result_l)
	{
		if(grid_p.get_case_type(pair_l.first, pair_l.second) != CaseType::FREE)
		{
			godot::UtilityFunctions::print("\tcase not free : ", pair_l.first,",", pair_l.second);
			return false;
		}
		else
		{
			godot::UtilityFunctions::print("\tcase IS free : ", pair_l.first,",", pair_l.second);
		}
	}
	return true;
}

void set_line_in_grid(Grid &grid_p, godot::TypedArray<godot::Vector2i> const &points_p, size_t idx_p)
{
	godot::UtilityFunctions::print("set_line_in_grid : ", points_p);
	std::vector<std::pair<int, int> > result_l = getAllCoord(points_p);
	for(auto &&pair_l : result_l)
	{
		int x = pair_l.first;
		int y = pair_l.second;
		godot::UtilityFunctions::print("setting not free : ", x,",", y);
		grid_p.set_case_type(x, y, CaseType::BELT);
		grid_p.set_case_index(x, y, idx_p);
	}
}

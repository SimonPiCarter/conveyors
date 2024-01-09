#include "Grid.hh"
#include <godot_cpp/variant/utility_functions.hpp>

#include <unordered_set>

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
	type[x*height+y] = type_p;
}

void Grid::set_case_index(size_t x, size_t y, size_t idx_p)
{
	indexes[x*height+y] = idx_p;
}

template<typename container_t>
std::vector<std::pair<int, int> > getAllCoord(container_t const &points_p)
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
			std::pair<int, int> pair_l;
			if(x_mode)
			{
				pair_l = {i, point.y};
			}
			else
			{
				pair_l = {point.x, i};
			}
			// except for very first point we do not add the last point of the precedent line
			// to avoid dobule checking
			if(pair_l.first == old_point.x && pair_l.second == old_point.y && old_point != points_p[0])
			{
				continue;
			}
			result_l.push_back(pair_l);
		}
		old_point = point;
	}

	return result_l;
}

bool check_line_in_grid(Grid const &grid_p, godot::TypedArray<godot::Vector2i> const &points_p)
{
	std::vector<std::pair<int, int> > result_l = getAllCoord(points_p);
	std::unordered_set<int> already_l;
	for(auto &&pair_l : result_l)
	{
		if(already_l.find(pair_l.first*1e6+pair_l.second) != already_l.end())
		{
			return false;
		}
		already_l.insert(pair_l.first*1e6+pair_l.second);

		if(grid_p.get_case_type(pair_l.first, pair_l.second) != CaseType::FREE)
		{
			return false;
		}
	}
	return true;
}

void set_line_in_grid(Grid &grid_p, std::vector<godot::Vector2i> const &points_p, size_t idx_p)
{
	std::vector<std::pair<int, int> > result_l = getAllCoord(points_p);
	for(auto &&pair_l : result_l)
	{
		int x = pair_l.first;
		int y = pair_l.second;
		if(grid_p.get_case_type(x, y) == CaseType::FREE)
		{
			grid_p.set_case_type(x, y, CaseType::BELT);
			grid_p.set_case_index(x, y, idx_p);
		}
	}
}

void unset_line_in_grid(Grid &grid_p, std::vector<godot::Vector2i> const &points_p)
{
	std::vector<std::pair<int, int> > result_l = getAllCoord(points_p);
	for(auto &&pair_l : result_l)
	{
		int x = pair_l.first;
		int y = pair_l.second;

		if(grid_p.get_case_type(x, y) == CaseType::BELT)
		{
			grid_p.set_case_type(x, y, CaseType::FREE);
			grid_p.set_case_index(x, y, 0);
		}
	}
}

bool is_point_free_or_belt(Grid &grid_p, size_t x, size_t y)
{
	CaseType type_l = grid_p.get_case_type(x, y);
	return type_l == CaseType::FREE || type_l == CaseType::BELT;
}

bool is_point_free(Grid &grid_p, size_t x, size_t y)
{
	CaseType type_l = grid_p.get_case_type(x, y);
	return type_l == CaseType::FREE;
}

bool is_handler(CaseType type_p)
{
	return type_p == CaseType::SPLITTER
		|| type_p == CaseType::MERGER
		|| type_p == CaseType::SORTER
		|| type_p == CaseType::FACTORY;
}

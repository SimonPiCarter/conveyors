#include "ManagerGodot.hh"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

ManagerGodot::~ManagerGodot()
{}

template<typename line_handler>
void step_and_load(std::vector<line_handler> &vector_p)
{
	for(line_handler &line_handler_l : vector_p)
	{
		step(line_handler_l.innerLine);
		load(line_handler_l);
	}
}
template<typename line_handler>
void step_and_load(smart_list_togglable<line_handler> &list_p)
{
	for(line_handler &line_handler_l : list_p.vector)
	{
		if(line_handler_l.toggled)
		{
			step(line_handler_l.innerLine);
			load(line_handler_l);
		}
	}
}

template<typename line_handler>
void unload(std::vector<line_handler> &vector_p)
{
	for(line_handler &line_handler_l : vector_p)
	{
		unload(line_handler_l);
	}
}
template<typename line_handler>
void unload(smart_list_togglable<line_handler> &list_p)
{
	for(line_handler &line_handler_l : list_p.vector)
	{
		if(line_handler_l.toggled)
		{
			unload(line_handler_l);
		}
	}
}

void ManagerGodot::_process(double delta_p)
{
	_elapsed += delta_p;

	if(_elapsed >= 0.1)
	{
		for(Factory &facto_l : _factories)
		{
			load(facto_l);
			process(facto_l);
		}
		step_and_load<Splitter>(_splitters);
		step_and_load<Merger>(_mergers);
		step_and_load<Sorter>(_sorters);
		step_and_load<Bridge>(_bridges);

		for(LineGodot *line_l : _lines.vector)
		{
			if(line_l)
			{
				line_l->update();
			}
		}

		unload<Splitter>(_splitters);
		unload<Merger>(_mergers);
		unload<Sorter>(_sorters);
		unload<Bridge>(_bridges);

		_elapsed = 0;
	}
}

void ManagerGodot::init_grid(size_t width_p, size_t height_p)
{
	grid.init_grid(width_p, height_p);
}

int ManagerGodot::add_line(godot::TypedArray<godot::Vector2i> const &points_p, double speed_p, godot::Ref<Mesh> const &mesh_p)
{
	godot::TypedArray<Vector2i> newPoints_l;
	Vector2i old_point;
	bool first_l = true;
	int length_l = 0;
	for(size_t i = 0 ; i < points_p.size() ; ++ i)
	{
		Vector2i const &point_l = points_p[i];
		if(first_l)
		{
			first_l = false;
			old_point = point_l;
			newPoints_l.push_back(point_l);
			continue;
		}
		if(old_point.x != point_l.x && old_point.y != point_l.y)
		{
			newPoints_l.push_back(Vector2i(old_point.x, point_l.y));
			newPoints_l.push_back(Vector2i(point_l.x, point_l.y));
		}
		else
		{
			newPoints_l.push_back(point_l);
		}
		length_l += std::abs(old_point.x - point_l.x) + std::abs(old_point.y - point_l.y);

		old_point = point_l;
	}

	LineGodot * line_l = memnew(LineGodot);
	line_l->set_mesh(mesh_p);
	line_l->set_up_line(2*length_l, speed_p);

	int cur_length_l = 0;
	for(size_t p = 0 ; p < newPoints_l.size() ; ++p)
	{
		godot::Vector2i const &point_l = newPoints_l[p];
		line_l->getLine()->positions.push_back(point_l);
		if(p==0)
		{
			old_point = point_l;
			continue;
		}

		length_l = std::abs(old_point.x - point_l.x) + std::abs(old_point.y - point_l.y);
		cur_length_l += length_l * 200;

		line_l->add_segment_to_line(
				Vector3(old_point.x*1.2,0,old_point.y*1.2),
				Vector3(point_l.x*1.2,0,point_l.y*1.2),
				cur_length_l);

		old_point = point_l;
	}

	int idx_l = _lines.add(line_l);
	set_line_in_grid(grid, line_l->getLine()->positions, idx_l);
	add_child(line_l);
	return idx_l;
}

LineGodot * ManagerGodot::get_line(int idx_p)
{
	return _lines.vector[idx_p];
}

void ManagerGodot::remove_line(int idx_p)
{
	if(_lines.vector[idx_p])
	{
		_lines.vector[idx_p]->queue_free();
		disconnect_all(*_lines.vector[idx_p]->getLine());
		unset_line_in_grid(grid, _lines.vector[idx_p]->getLine()->positions);
		_lines.free(idx_p);
	}
}

bool ManagerGodot::check_line(godot::TypedArray<godot::Vector2i> const &points_p)
{
	return check_line_in_grid(grid, points_p);
}

bool ManagerGodot::check_point(godot::Vector2i const &point_p)
{
	return grid.get_case_type(point_p.x , point_p.y) == CaseType::FREE;
}

int ManagerGodot::get_point_index(godot::Vector2i const &point_p)
{
	return grid.get_case_index(point_p.x , point_p.y);
}

int ManagerGodot::get_point_type(godot::Vector2i const &point_p)
{
	return int(grid.get_case_type(point_p.x , point_p.y));
}

int ManagerGodot::get_line_from_point(godot::Vector2i const &point_p)
{
	if(grid.get_case_type(point_p.x , point_p.y) == CaseType::BELT)
	{
		return grid.get_case_index(point_p.x , point_p.y);
	}
	return -1;
}

int ManagerGodot::add_splitter(godot::Vector2i const &pos_p, LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p)
{
	Splitter splitter_l;
	splitter_l.innerLine.speed = entry_p->getLine()->speed;

	if(entry_p)
	{
		connect_input(splitter_l, *entry_p->getLine());
	}
	if(first_p)
	{
		connect_output(splitter_l, *first_p->getLine());
	}
	if(second_p)
	{
		connect_output(splitter_l, *second_p->getLine());
	}

	splitter_l.position = pos_p;
	size_t idx_l = _splitters.add(std::move(splitter_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::SPLITTER);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

int ManagerGodot::add_splitter_from_line(int line_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	godot::Vector2i end_l = data_line_l->get_end();
	if(!is_point_free_or_belt(grid, end_l.x, end_l.y))
	{
		return -1;
	}
	return add_splitter(end_l, line_l, nullptr, nullptr);
}

bool ManagerGodot::connect_line_to_splitter_output(int line_p, int splitter_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	Splitter & splitter_l = _splitters.vector[splitter_p];

	return connect_output(splitter_l, *data_line_l);
}

bool ManagerGodot::connect_line_to_splitter_input(int line_p, int splitter_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	Splitter & splitter_l = _splitters.vector[splitter_p];

	return connect_input(splitter_l, *data_line_l);
}

godot::Vector2i const &ManagerGodot::get_splitter_pos(int splitter_p) const
{
	Splitter const & splitter_l = _splitters.vector[splitter_p];
	return splitter_l.position;
}

void ManagerGodot::add_merger(godot::Vector2i const &pos_p, LineGodot * output_p, LineGodot * first_p, LineGodot * second_p)
{
	Merger merger_l;
	merger_l.innerLine.speed = std::max(first_p->getLine()->speed, second_p->getLine()->speed);

	merger_l.output = output_p->getLine();
	merger_l.first = first_p->getLine();
	merger_l.second = second_p->getLine();

	merger_l.position = pos_p;
	grid.set_case_type(merger_l.position.x, merger_l.position.y, CaseType::MERGER);
	grid.set_case_index(merger_l.position.x, merger_l.position.y, _mergers.size());

	_mergers.push_back(merger_l);
}


int ManagerGodot::add_sorter(godot::Vector2i const &pos_p, LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p, int type_p)
{
	if(type_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_sorter with type < 0");
	}
	Sorter sorter_l(type_p);
	sorter_l.innerLine.speed = entry_p->getLine()->speed;

	if(entry_p)
	{
		connect_input(sorter_l, *entry_p->getLine());
	}
	if(first_p)
	{
		connect_output(sorter_l, *first_p->getLine());
	}
	if(second_p)
	{
		connect_output(sorter_l, *second_p->getLine());
	}

	sorter_l.position = pos_p;
	size_t idx_l = _sorters.add(std::move(sorter_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::SORTER);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

int ManagerGodot::add_sorter_from_line(int line_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	godot::Vector2i end_l = data_line_l->get_end();
	if(!is_point_free_or_belt(grid, end_l.x, end_l.y))
	{
		return -1;
	}
	return add_sorter(end_l, line_l, nullptr, nullptr, 0);
}

bool ManagerGodot::connect_line_to_sorter_output(int line_p, int sorter_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	Sorter & sorter_l = _sorters.vector[sorter_p];

	return connect_output(sorter_l, *data_line_l);
}

bool ManagerGodot::connect_line_to_sorter_input(int line_p, int sorter_p)
{
	LineGodot * line_l = _lines.vector[line_p];
	Line * data_line_l = line_l->getLine();
	Sorter & sorter_l = _sorters.vector[sorter_p];

	return connect_input(sorter_l, *data_line_l);
}

godot::Vector2i const &ManagerGodot::get_sorter_pos(int sorter_p) const
{
	Sorter const & sorter_l = _sorters.vector[sorter_p];
	return sorter_l.position;
}

void ManagerGodot::set_sorter_type(int sorter_p, int type_p)
{
	Sorter & sorter_l = _sorters.vector[sorter_p];
	sorter_l.type = type_p;
}

int ManagerGodot::get_sorter_type(int sorter_p) const
{
	Sorter const & sorter_l = _sorters.vector[sorter_p];
	return sorter_l.type;
}

void ManagerGodot::add_bridge(LineGodot * entry_p, LineGodot * output_p, int length_p)
{
	Bridge bridge_l(2*length_p);
	bridge_l.input = entry_p->getLine();
	bridge_l.output = output_p->getLine();
	bridge_l.innerLine.speed = entry_p->getLine()->speed;

	_bridges.push_back(bridge_l);
}

void ManagerGodot::add_score_factory(godot::Vector2i const &pos_p, LineGodot * entry_p, int duration_p, int type_p)
{
	using namespace std::placeholders;
	if(duration_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_score_factory with duration < 0");
	}
	if(type_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_score_factory with type < 0");
	}
	Factory facto_l(duration_p);

	facto_l.output_handler = std::bind(score_factory, std::ref(stats), size_t(type_p), _1, _2);

	facto_l.entries.push_back(entry_p->getLine());

	facto_l.position = pos_p;
	grid.set_case_type(facto_l.position.x, facto_l.position.y, CaseType::FACTORY);
	grid.set_case_index(facto_l.position.x, facto_l.position.y, _factories.size());

	_factories.push_back(facto_l);
}

void ManagerGodot::_bind_methods()
{

	ClassDB::bind_method(D_METHOD("init_grid", "width", "height"), &ManagerGodot::init_grid);
	ClassDB::bind_method(D_METHOD("check_line", "points"), &ManagerGodot::check_line);
	ClassDB::bind_method(D_METHOD("check_point", "point"), &ManagerGodot::check_point);
	ClassDB::bind_method(D_METHOD("get_point_index", "point"), &ManagerGodot::get_point_index);
	ClassDB::bind_method(D_METHOD("get_point_type", "point"), &ManagerGodot::get_point_type);
	ClassDB::bind_method(D_METHOD("get_line_from_point", "point"), &ManagerGodot::get_line_from_point);

	ClassDB::bind_method(D_METHOD("add_splitter_from_line", "line"), &ManagerGodot::add_splitter_from_line);
	ClassDB::bind_method(D_METHOD("connect_line_to_splitter_output", "line", "splitter"), &ManagerGodot::connect_line_to_splitter_output);
	ClassDB::bind_method(D_METHOD("connect_line_to_splitter_input", "line", "splitter"), &ManagerGodot::connect_line_to_splitter_input);
	ClassDB::bind_method(D_METHOD("get_splitter_pos", "splitter"), &ManagerGodot::get_splitter_pos);

	ClassDB::bind_method(D_METHOD("add_sorter_from_line", "line"), &ManagerGodot::add_sorter_from_line);
	ClassDB::bind_method(D_METHOD("connect_line_to_sorter_output", "line", "sorter"), &ManagerGodot::connect_line_to_sorter_output);
	ClassDB::bind_method(D_METHOD("connect_line_to_sorter_input", "line", "sorter"), &ManagerGodot::connect_line_to_sorter_input);
	ClassDB::bind_method(D_METHOD("get_sorter_pos", "sorter"), &ManagerGodot::get_sorter_pos);
	ClassDB::bind_method(D_METHOD("set_sorter_type", "sorter", "type"), &ManagerGodot::set_sorter_type);
	ClassDB::bind_method(D_METHOD("get_sorter_type", "sorter"), &ManagerGodot::get_sorter_type);

	ClassDB::bind_method(D_METHOD("add_line", "points", "speed"), &ManagerGodot::add_line);
	ClassDB::bind_method(D_METHOD("get_line", "index"), &ManagerGodot::get_line);
	ClassDB::bind_method(D_METHOD("remove_line", "index"), &ManagerGodot::remove_line);
	ClassDB::bind_method(D_METHOD("add_splitter", "position", "entry", "first", "second"), &ManagerGodot::add_splitter);
	ClassDB::bind_method(D_METHOD("add_merger", "position", "output", "first", "second"), &ManagerGodot::add_merger);
	ClassDB::bind_method(D_METHOD("add_sorter", "position", "entry", "first", "second", "type"), &ManagerGodot::add_sorter);
	ClassDB::bind_method(D_METHOD("add_bridge", "entry", "output", "length"), &ManagerGodot::add_bridge);


	ClassDB::bind_method(D_METHOD("add_score_factory", "position", "entry", "duration", "type"), &ManagerGodot::add_score_factory);
	ClassDB::bind_method(D_METHOD("get_score"), &ManagerGodot::get_score);

	ADD_GROUP("ManagerGodot", "ManagerGodot_");
}

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
		for(Factory &facto_l : _factories.vector)
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

void ManagerGodot::init_grid(int width_p, int height_p)
{
	grid.init_grid(width_p, height_p);
	connectorGrid = std::vector<std::vector<Connector *>>(width_p, std::vector<Connector *>(height_p, nullptr));
}

bool ManagerGodot::check_connector_pos(godot::Vector2i const &pos_p, godot::Vector2i const &pos_handler_p, bool input)
{
	CaseType type_l = grid.get_case_type(pos_handler_p.x, pos_handler_p.y);
	size_t idx_l = grid.get_case_index(pos_handler_p.x, pos_handler_p.y);

	if(connectorGrid[pos_p.x][pos_p.y])
	{
		return false;
	}

	// check handler position and type
	if(!is_handler(type_l))
	{
		return false;
	}

	if(!is_point_free_or_belt(grid, pos_p.x, pos_p.y))
	{
		return false;
	}

	if(grid.get_case_type(pos_p.x, pos_p.y) == CaseType::BELT)
	{
		size_t idxBelt_l = grid.get_case_index(pos_p.x, pos_p.y);
		Line * line_data_l = _lines.vector[idxBelt_l]->getLine();
		if(input
		&& line_data_l->get_end() != pos_p)
		{
			return false;
		}
		if(!input
		&& line_data_l->get_start() != pos_p)
		{
			return false;
		}
	}

	// check distance
	int dist_l = std::abs(pos_p.x - pos_handler_p.x) + std::abs(pos_p.y - pos_handler_p.y);
	if(dist_l != 1)
	{
		return false;
	}

	line_handler const &handler_l = get_handler(type_l, idx_l);

	if(input && !has_input_free(handler_l))
	{
		return false;
	}
	if(!input && !has_output_free(handler_l))
	{
		return false;
	}

	return true;
}

int ManagerGodot::add_connector(godot::Vector2i const &pos_p, godot::Vector2i const &pos_handler_p, bool input)
{
	if(!check_connector_pos(pos_p, pos_handler_p, input))
	{
		return -1;
	}
	CaseType type_l = grid.get_case_type(pos_handler_p.x, pos_handler_p.y);
	size_t idx_l = grid.get_case_index(pos_handler_p.x, pos_handler_p.y);
	size_t idxBelt_l = grid.get_case_index(pos_p.x, pos_p.y);
	Line * line_data_l = _lines.vector[idxBelt_l]->getLine();
	line_handler &handler_l = get_handler(type_l, idx_l);

	Connector *cxor_l = new Connector{line_data_l, type_l, idx_l, input, pos_p};
	size_t idxCxor_l = _connectors.add(cxor_l);
	::connect(*this, *cxor_l);

	handler_l.connectors.push_back(idxCxor_l);
	connectorGrid[pos_p.x][pos_p.y] = cxor_l;

	return idxCxor_l;
}

void ManagerGodot::del_connector(int index_p)
{
	if(index_p < 0 || !_connectors.vector[index_p])
	{
		return;
	}
	Connector *cxor_l = _connectors.vector[index_p];
	::disconnect(*this, *cxor_l);

	CaseType type_l = cxor_l->handler_type;
	size_t idx_l = cxor_l->handler_idx;
	line_handler &handler_l = get_handler(type_l, idx_l);
	handler_l.connectors.erase(std::remove(handler_l.connectors.begin(), handler_l.connectors.end(), size_t(index_p)), handler_l.connectors.end());

	connectorGrid[cxor_l->pos.x][cxor_l->pos.y] = nullptr;
	_connectors.free(index_p);
	delete cxor_l;
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
	line_l->set_up_line(2*length_l+1, speed_p);

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
		disconnect_all(*this, *_lines.vector[idx_p]->getLine());
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

void ManagerGodot::del_line_handler(line_handler &handler_p)
{
	// mandatory copy because vector will be modified
	std::vector<size_t> ctor_l = handler_p.connectors;
	for(size_t i : ctor_l)
	{
		// delete all connector
		del_connector(i);
	}
}

int ManagerGodot::add_splitter(godot::Vector2i const &pos_p)
{
	if(!is_point_free(grid, pos_p.x, pos_p.y))
	{
		return -1;
	}
	Splitter splitter_l;
	splitter_l.innerLine.speed = 100;
	splitter_l.position = pos_p;
	size_t idx_l = _splitters.add(std::move(splitter_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::SPLITTER);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

void ManagerGodot::del_splitter(int idx_p)
{
	if(idx_p < 0) { return; }
	line_handler &handler_l = _splitters.vector[idx_p];
	_splitters.free(size_t(idx_p));
	del_line_handler(handler_l);
}

godot::Vector2i const &ManagerGodot::get_splitter_pos(int splitter_p) const
{
	Splitter const & splitter_l = _splitters.vector[splitter_p];
	return splitter_l.position;
}

int ManagerGodot::add_merger(godot::Vector2i const &pos_p)
{
	if(!is_point_free(grid, pos_p.x, pos_p.y))
	{
		return -1;
	}
	Merger merger_l;
	merger_l.innerLine.speed = 100;
	merger_l.position = pos_p;
	size_t idx_l = _mergers.add(std::move(merger_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::MERGER);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

void ManagerGodot::del_merger(int idx_p)
{
	if(idx_p < 0) { return; }
	line_handler &handler_l = _mergers.vector[idx_p];
	_mergers.free(size_t(idx_p));
	del_line_handler(handler_l);
}

godot::Vector2i const &ManagerGodot::get_merger_pos(int merger_p) const
{
	Merger const & merger_l = _mergers.vector[merger_p];
	return merger_l.position;
}

int ManagerGodot::add_sorter(godot::Vector2i const &pos_p, int type_p)
{
	if(type_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_sorter with type < 0");
	}
	if(!is_point_free(grid, pos_p.x, pos_p.y))
	{
		return -1;
	}
	Sorter sorter_l(type_p);
	sorter_l.innerLine.speed = 100;
	sorter_l.position = pos_p;
	size_t idx_l = _sorters.add(std::move(sorter_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::SORTER);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

void ManagerGodot::del_sorter(int idx_p)
{
	if(idx_p < 0) { return; }
	line_handler &handler_l = _sorters.vector[idx_p];
	_sorters.free(size_t(idx_p));
	del_line_handler(handler_l);
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
	bridge_l.innerLine.speed = 100;

	_bridges.push_back(bridge_l);
}

int ManagerGodot::add_score_factory(godot::Vector2i const &pos_p, int duration_p, int type_p)
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
	if(!is_point_free(grid, pos_p.x, pos_p.y))
	{
		return -1;
	}
	Factory facto_l(duration_p);

	facto_l.output_handler = std::bind(score_factory, std::ref(stats), size_t(type_p), _1, _2);

	facto_l.position = pos_p;
	size_t idx_l = _factories.add(std::move(facto_l));
	grid.set_case_type(pos_p.x, pos_p.y, CaseType::FACTORY);
	grid.set_case_index(pos_p.x, pos_p.y, idx_l);

	return idx_l;
}

void ManagerGodot::del_factory(int idx_p)
{
	if(idx_p < 0) { return; }
	line_handler &handler_l = _factories.vector[idx_p];
	_factories.free(size_t(idx_p));
	del_line_handler(handler_l);
}

void ManagerGodot::_bind_methods()
{

	ClassDB::bind_method(D_METHOD("init_grid", "width", "height"), &ManagerGodot::init_grid);
	ClassDB::bind_method(D_METHOD("check_line", "points"), &ManagerGodot::check_line);
	ClassDB::bind_method(D_METHOD("check_point", "point"), &ManagerGodot::check_point);
	ClassDB::bind_method(D_METHOD("get_point_index", "point"), &ManagerGodot::get_point_index);
	ClassDB::bind_method(D_METHOD("get_point_type", "point"), &ManagerGodot::get_point_type);
	ClassDB::bind_method(D_METHOD("get_line_from_point", "point"), &ManagerGodot::get_line_from_point);

	ClassDB::bind_method(D_METHOD("add_splitter", "position"), &ManagerGodot::add_splitter);
	ClassDB::bind_method(D_METHOD("get_splitter_pos", "splitter"), &ManagerGodot::get_splitter_pos);
	ClassDB::bind_method(D_METHOD("add_merger", "position"), &ManagerGodot::add_merger);
	ClassDB::bind_method(D_METHOD("get_merger_pos", "merger"), &ManagerGodot::get_merger_pos);
	ClassDB::bind_method(D_METHOD("add_sorter", "position", "type"), &ManagerGodot::add_sorter);
	ClassDB::bind_method(D_METHOD("get_sorter_pos", "sorter"), &ManagerGodot::get_sorter_pos);
	ClassDB::bind_method(D_METHOD("set_sorter_type", "sorter", "type"), &ManagerGodot::set_sorter_type);
	ClassDB::bind_method(D_METHOD("get_sorter_type", "sorter"), &ManagerGodot::get_sorter_type);

	ClassDB::bind_method(D_METHOD("add_line", "points", "speed"), &ManagerGodot::add_line);
	ClassDB::bind_method(D_METHOD("get_line", "index"), &ManagerGodot::get_line);
	ClassDB::bind_method(D_METHOD("remove_line", "index"), &ManagerGodot::remove_line);
	ClassDB::bind_method(D_METHOD("add_bridge", "entry", "output", "length"), &ManagerGodot::add_bridge);

	ClassDB::bind_method(D_METHOD("add_connector", "pos", "pos_handler", "input"), &ManagerGodot::add_connector);
	ClassDB::bind_method(D_METHOD("del_connector", "index"), &ManagerGodot::del_connector);
	ClassDB::bind_method(D_METHOD("del_splitter", "idx"), &ManagerGodot::del_splitter);
	ClassDB::bind_method(D_METHOD("del_merger", "idx"), &ManagerGodot::del_merger);
	ClassDB::bind_method(D_METHOD("del_sorter", "idx"), &ManagerGodot::del_sorter);
	ClassDB::bind_method(D_METHOD("del_factory", "idx"), &ManagerGodot::del_factory);

	ClassDB::bind_method(D_METHOD("add_score_factory", "position", "duration", "type"), &ManagerGodot::add_score_factory);

	ClassDB::bind_method(D_METHOD("get_score"), &ManagerGodot::get_score);

	ADD_GROUP("ManagerGodot", "ManagerGodot_");
}

line_handler & ManagerGodot::get_handler(CaseType type_p, size_t idx_p)
{
	switch(type_p)
	{
		case CaseType::SPLITTER:
			return _splitters.vector[idx_p];
		case CaseType::MERGER:
			return _mergers.vector[idx_p];
		case CaseType::SORTER:
			return _sorters.vector[idx_p];
		case CaseType::FACTORY:
			return _factories.vector[idx_p];
		default:
			throw std::logic_error("Cannot get_handler");
	}
}

line_handler const & ManagerGodot::get_handler(CaseType type_p, size_t idx_p) const
{
	switch(type_p)
	{
		case CaseType::SPLITTER:
			return _splitters.vector[idx_p];
		case CaseType::MERGER:
			return _mergers.vector[idx_p];
		case CaseType::SORTER:
			return _sorters.vector[idx_p];
		case CaseType::FACTORY:
			return _factories.vector[idx_p];
		default:
			throw std::logic_error("Cannot get_handler");
	}
}


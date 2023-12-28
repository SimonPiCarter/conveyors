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
void unload(std::vector<line_handler> &vector_p)
{
	for(line_handler &line_handler_l : vector_p)
	{
		unload(line_handler_l);
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

		for(LineGodot *line_l : _lines)
		{
			line_l->update();
		}

		unload<Splitter>(_splitters);
		unload<Merger>(_mergers);
		unload<Sorter>(_sorters);

		_elapsed = 0;
	}
}

LineGodot* ManagerGodot::add_line(godot::TypedArray<godot::Vector2i> const &points_p, double speed_p, godot::Ref<Mesh> const &mesh_p)
{
	std::vector<Vector2i> newPoints_l;
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

	first_l = true;
	int cur_length_l = 0;
	for(Vector2i const &point_l : newPoints_l)
	{
		if(first_l)
		{
			first_l = false;
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

	_lines.push_back(line_l);
	return line_l;
}

void ManagerGodot::add_splitter(LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p)
{
	Splitter splitter_l;
	splitter_l.innerLine.speed = entry_p->getLine()->speed;

	splitter_l.entry = entry_p->getLine();
	splitter_l.first = first_p->getLine();
	splitter_l.second = second_p->getLine();

	_splitters.push_back(splitter_l);
}

void ManagerGodot::add_merger(LineGodot * output_p, LineGodot * first_p, LineGodot * second_p)
{
	Merger merger_l;
	merger_l.innerLine.speed = std::max(first_p->getLine()->speed, second_p->getLine()->speed);

	merger_l.output = output_p->getLine();
	merger_l.first = first_p->getLine();
	merger_l.second = second_p->getLine();

	_mergers.push_back(merger_l);
}


void ManagerGodot::add_sorter(LineGodot * entry_p, LineGodot * first_p, LineGodot * second_p, int type_p)
{
	if(type_p < 0)
	{
		UtilityFunctions::push_error("Cannot add_sorter with type < 0");
	}
	Sorter sorter_l(type_p);
	sorter_l.innerLine.speed = entry_p->getLine()->speed;

	sorter_l.entry = entry_p->getLine();
	sorter_l.first = first_p->getLine();
	sorter_l.second = second_p->getLine();

	_sorters.push_back(sorter_l);
}


void ManagerGodot::add_score_factory(LineGodot * entry_p, int duration_p, int type_p)
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

	_factories.push_back(facto_l);
}

void ManagerGodot::_bind_methods()
{

	ClassDB::bind_method(D_METHOD("add_line", "points", "speed"), &ManagerGodot::add_line);
	ClassDB::bind_method(D_METHOD("add_splitter", "entry", "first", "second"), &ManagerGodot::add_splitter);
	ClassDB::bind_method(D_METHOD("add_merger", "output", "first", "second"), &ManagerGodot::add_merger);
	ClassDB::bind_method(D_METHOD("add_sorter", "entry", "first", "second", "type"), &ManagerGodot::add_sorter);

	ClassDB::bind_method(D_METHOD("add_score_factory", "entry", "duration", "type"), &ManagerGodot::add_score_factory);
	ClassDB::bind_method(D_METHOD("get_score"), &ManagerGodot::get_score);

	ADD_GROUP("ManagerGodot", "ManagerGodot_");
}
